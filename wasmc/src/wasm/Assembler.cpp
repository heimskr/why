#include <cctype>
#include <climits>
#include <cstdlib>
#include <deque>
#include <iomanip>
#include <iostream>
#include <variant>

#include "lib/picosha2.h"
#include "parser/Lexer.h"
#include "parser/StringSet.h"
#include "wasm/Assembler.h"
#include "wasm/Directives.h"
#include "wasm/Nodes.h"
#include "wasm/Registers.h"
#include "wasm/SymbolTable.h"
#include "wasm/Why.h"

namespace Wasmc {
	size_t Assembler::assemblerCount = 0;

	Assembler::Assembler(const ASTNode *root_): root(root_) {}

	std::string Assembler::assemble(bool can_warn) {
		wasmParser.errorCount = 0;
		validateSectionCounts();
		findAllLabels();
		processMetadata();
		processText();
		metaOffsetData() = metaOffsetCode() + code.size();
		metaOffsetSymbols() = metaOffsetData() + data.size();
		createSymbolTableSkeleton(allLabels);
		processRelocation();
		evaluateExpressions();
		expandLabels();
		expandCode();
		metaOffsetDebug() = metaOffsetSymbols() + symbols.size();
		metaOffsetRelocation() = metaOffsetDebug() + debug.size();
		encodeRelocation();
		offsets[StringSet::intern(".end")] = metaOffsetEnd() = metaOffsetRelocation() + relocation.size();
		updateSymbolTable(allLabels);
		relocateCode();
		createDebugData(debugNode);
		concatenated = Section::combine({meta, code, data, symbols, debug, relocation});

		std::cerr << "relocation|" << relocation.size() << "|\n";

		if (can_warn && 0 < unknownSymbols.size()) {
			std::cerr << "\e[2m[\e[22;1;33m?\e[22;39;2m]\e[22m Unknown symbol" << (unknownSymbols.size() == 1? "" : "s")
			          << ": ";
			bool first = true;
			for (const std::string *symbol: unknownSymbols) {
				if (first)
					first = false;
				else
					std::cerr << ", ";
				std::cerr << "\e[1m" << *symbol << "\e[22m";
			}
			std::cerr << '\n';
		}

		return stringify(concatenated);
	}

	void Assembler::processText() {
		if (!textNode)
			throw std::runtime_error("textNode is null in Assembler::processText");

		for (ASTNode *node: *textNode)
			switch (node->symbol) {
				case WASM_DATADIR:
					currentSection = &data;
					break;
				case WASM_CODEDIR:
					currentSection = &code;
					break;
				case WASM_LABEL: {
					const std::string *label = dynamic_cast<WASMLabelNode *>(node)->label;
					*currentSection += label;
					symbolTypes.emplace(label, currentSection == &data? SymbolType::Object : SymbolType::Instruction);
					offsets.emplace(label, currentSection->counter);
					break;
				}
				case WASM_STRINGDIR: {
					auto *directive = dynamic_cast<StringDirective *>(node);
					if (directive->nullTerminate)
						currentSection->append(*directive->string + '\0');
					else
						currentSection->append(*directive->string);
					break;
				}
				case WASM_TYPEDIR: {
					auto *directive = dynamic_cast<TypeDirective *>(node);
					symbolTypes[directive->symbolName] = directive->type;
					break;
				}
				case WASM_SIZEDIR: {
					auto *directive = dynamic_cast<SizeDirective *>(node);
					directive->expression->setCounter(*currentSection);
					if (directive->expression->validate() == Expression::ValidationResult::Invalid) {
						error() << std::string(*directive->expression) << '\n';
						throw std::runtime_error("Invalid expression");
					}
					symbolSizeExpressions[directive->symbolName] = directive->expression;
					break;
				}
				case WASM_VALUEDIR: {
					auto *directive = dynamic_cast<ValueDirective *>(node);
					auto labels = directive->expression->findLabels();
					directive->expression->setCounter(*currentSection);
					long index = -1;
					const std::string *label;
					const auto validation = directive->expression->validate(&label);
					switch (validation) {
						case Expression::ValidationResult::Invalid:
							error() << std::string(*directive->expression) << '\n';
							throw std::runtime_error("Invalid expression");
						case Expression::ValidationResult::LabelNumberDifference:
						case Expression::ValidationResult::LabelNumberSum:
							// If it's a relocation based on a label, try to find the index of the label in the symbol
							// table. If it's not present, that's fine; we can try to find it later.
							if (symbolTableIndices.count(label) != 0)
								index = symbolTableIndices.at(label);
							break;
						case Expression::ValidationResult::DotLabelDifference:
						case Expression::ValidationResult::DoubleLabelDifference:
							// We don't want to insert relocation data for what evaluates to a constant, so we clear
							// label's value.
							label = nullptr;
							break;
						default:
							break;
					}
					valueExpressionLabels.insert(labels.begin(), labels.end());
					*currentSection += {directive->valueSize, directive->expression};
					if (label)
						relocationMap.try_emplace(directive, currentSection == &data, directive->valueSize == 4?
							RelocationType::Lower4 : RelocationType::Full, index, 0, currentSection->counter,
							currentSection, label);
					*currentSection += directive->valueSize;
					break;
				}
				case WASM_ALIGNDIR:
					currentSection->alignUp(dynamic_cast<AlignDirective *>(node)->alignment);
					break;
				case WASM_FILLDIR: {
					auto *directive = dynamic_cast<FillDirective *>(node);
					currentSection->extend<uint8_t>(directive->count, uint8_t(directive->value));
					*currentSection += directive->count;
					break;
				}
				default: {
					if (auto *instruction = dynamic_cast<WASMInstructionNode *>(node)) {
						// Because we can't yet convert the instruction to a Long (probably),
						// we stash it in a map and append one or more zeros.
						instructionMap[currentSection->counter] = instruction;
						const size_t count = instruction->expandedSize();
						for (size_t i = 0; i < count; ++i)
							currentSection->append<Long>(0);
					} else
						node->debug();
				}
			}

		code.alignUp(8);
		data.alignUp(8);
	}

	void Assembler::evaluateExpressions() {
		for (auto &[label, expression]: symbolSizeExpressions)
			symbolSizes.emplace(label, expression->evaluate(*this));

		for (auto &[node, reloc]: relocationMap)
			if (const auto *directive = dynamic_cast<const ValueDirective *>(node))
				reloc.offset = directive->expression->evaluate(*this);
	}

	std::string Assembler::stringify(const std::vector<uint8_t> &bytes) {
		std::stringstream ss;
		size_t count = 0;
		for (uint8_t byte: bytes) {
			ss << std::hex << std::right << std::setw(2) << std::setfill('0') << int(byte);
			if (++count % 8 == 0)
				ss << '\n';
		}
		return ss.str();
	}

	Long Assembler::compileInstruction(const WASMInstructionNode &node) {
		if (const RType *rtype = dynamic_cast<const RType *>(&node)) {
			return compileR(node, *rtype);
		} else if (const IType *itype = dynamic_cast<const IType *>(&node)) {
			return compileI(node, *itype);
		} else if (const JType *jtype = dynamic_cast<const JType *>(&node)) {
			return compileJ(node, *jtype);
		} else if (node.nodeType() == WASMNodeType::Nop) {
			return 0;
		} else {
			node.debug();
			throw std::runtime_error("Node isn't an R-, I- or J-type");
		}
	}

	Long Assembler::compileR(const WASMInstructionNode &node, const RType &rtype) const {
		if (registerMap.count(rtype.rs) == 0)
			throw std::runtime_error("Invalid rs in R-type: " + (rtype.rs? *rtype.rs : "null"));
		if (registerMap.count(rtype.rt) == 0)
			throw std::runtime_error("Invalid rt in R-type: " + (rtype.rt? *rtype.rt : "null"));
		if (registerMap.count(rtype.rd) == 0)
			throw std::runtime_error("Invalid rd in R-type: " + (rtype.rd? *rtype.rd : "null"));

		const Funct funct = rtype.getFunct();
		if (FUNCT_MAX < funct)
			throw std::runtime_error("Invalid function value: " + std::to_string(funct));

		uint8_t condition = 0;
		if (const auto *has_condition = dynamic_cast<const HasCondition *>(&rtype))
			condition = static_cast<uint8_t>(has_condition->condition);

		return compileR(rtype.getOpcode(), registerMap.at(rtype.rs), registerMap.at(rtype.rt), registerMap.at(rtype.rd),
			funct, static_cast<uint8_t>(node.flags), condition);
	}

	Long Assembler::compileR(Opcode opcode, uint8_t rs, uint8_t rt, uint8_t rd, Funct function, uint8_t flags,
	                         uint8_t condition) {
		return static_cast<uint64_t>(function)
			| (static_cast<uint64_t>(flags) << 12)
			| (static_cast<uint64_t>(condition) << 14)
			| (static_cast<uint64_t>(rd) << 31)
			| (static_cast<uint64_t>(rs) << 38)
			| (static_cast<uint64_t>(rt) << 45)
			| (static_cast<uint64_t>(opcode) << 52);
	}

	Long Assembler::compileI(const WASMInstructionNode &node, const IType &itype) const {
		if (registerMap.count(itype.rs) == 0)
			throw std::runtime_error("Invalid rs in I-type: " + (itype.rs? *itype.rs : "null"));
		if (registerMap.count(itype.rd) == 0)
			throw std::runtime_error("Invalid rd in I-type: " + (itype.rd? *itype.rd : "null"));
		if (std::holds_alternative<const std::string *>(itype.imm))
			throw std::runtime_error("Can't compile an I-type with a label immediate");

		uint32_t imm = static_cast<uint32_t>(std::holds_alternative<int>(itype.imm)? std::get<int>(itype.imm)
			: std::get<char>(itype.imm));

		uint8_t condition = 0;
		if (const auto *has_condition = dynamic_cast<const HasCondition *>(&itype))
			condition = static_cast<uint8_t>(has_condition->condition);

		return compileI(itype.getOpcode(), registerMap.at(itype.rs), registerMap.at(itype.rd), imm,
			static_cast<uint8_t>(node.flags), static_cast<uint8_t>(condition));
	}

	Long Assembler::compileI(Opcode opcode, uint8_t rs, uint8_t rd, uint32_t immediate, uint8_t flags,
	                         uint8_t condition) {
		return static_cast<uint64_t>(immediate)
			| (static_cast<uint64_t>(rd) << 32)
			| (static_cast<uint64_t>(rs) << 39)
			| (static_cast<uint64_t>(flags) << 46)
			| (static_cast<uint64_t>(condition) << 48)
			| (static_cast<uint64_t>(opcode) << 52);
	}

	Long Assembler::compileJ(const WASMInstructionNode &node, const JType &jtype) const {
		if (registerMap.count(jtype.rs) == 0)
			throw std::runtime_error("Invalid rs in J-type: " + (jtype.rs? *jtype.rs : "null"));
		if (!std::holds_alternative<int>(jtype.imm))
			throw std::runtime_error("Can't compile a J-type with a label or character immediate");

		uint8_t condition = 0;
		if (const auto *has_condition = dynamic_cast<const HasCondition *>(&jtype))
			condition = static_cast<uint64_t>(has_condition->condition);

		return compileJ(jtype.getOpcode(), registerMap.at(jtype.rs), std::get<int>(jtype.imm), jtype.link,
			static_cast<uint8_t>(node.flags), condition);
	}

	Long Assembler::compileJ(Opcode opcode, uint8_t rs, uint32_t address, bool link, uint8_t flags, uint8_t condition) {
		return static_cast<uint64_t>(address)
			| (static_cast<uint64_t>(flags) << 32)
			| (static_cast<uint64_t>(condition) << 34)
			| (static_cast<uint64_t>(link? 1 : 0) << 44)
			| (static_cast<uint64_t>(rs) << 45)
			| (static_cast<uint64_t>(opcode) << 52);
	}

	void Assembler::expandLabels() {
		for (auto &[offset, statement]: instructionMap) {
			statement->labels.clear();
			if (auto *has_immediate = dynamic_cast<HasImmediate *>(statement)) {
				if (std::holds_alternative<const std::string *>(has_immediate->imm)) {
					const std::string *label = std::get<const std::string *>(has_immediate->imm);
					if (offsets.count(label) == 0) {
						const auto encoded = encodeSymbol(label);
						has_immediate->imm = static_cast<int>(encoded);
						statement->flags = LinkerFlags::UnknownSymbol;
						unknownSymbols.insert(label);
					} else {
						const Long offset = offsets.at(label);
						if (INT_MAX < offset)
							warn() << "Offset for label " << *label << " exceeds INT_MAX\n";
						has_immediate->imm = static_cast<int>(offset);
						statement->flags = LinkerFlags::KnownSymbol;
					}
				}
			}
		}
	}

	void Assembler::processRelocation() {
		for (const auto &[offset, statement]: instructionMap) {
			if (auto *has_immediate = dynamic_cast<HasImmediate *>(statement)) {
				if (std::holds_alternative<const std::string *>(has_immediate->imm)) {
					const std::string *label = std::get<const std::string *>(has_immediate->imm);
					const RelocationType type = statement->nodeType() == WASMNodeType::Lui?
						RelocationType::Upper4 : RelocationType::Lower4;
					RelocationData relocation_data(false, type, symbolTableIndices.at(label), 0, offset, &code, label);
					if (relocationMap.count(statement) != 0)
						relocationMap.erase(statement);
					relocationMap.emplace(statement, relocation_data);
				}
			}
		}

		for (const auto &[offset, statement]: extraInstructions) {
			if (auto *has_immediate = dynamic_cast<HasImmediate *>(statement.get())) {
				if (std::holds_alternative<const std::string *>(has_immediate->imm)) {
					const std::string *label = std::get<const std::string *>(has_immediate->imm);
					const RelocationType type = statement->nodeType() == WASMNodeType::Lui?
						RelocationType::Upper4 : RelocationType::Lower4;
					RelocationData relocation_data(false, type, symbolTableIndices.at(label), 0, offset, &code, label);
					if (relocationMap.count(statement.get()) != 0)
						relocationMap.erase(statement.get());
					relocationMap.emplace(statement.get(), relocation_data);
				}
			}
		}
	}

	void Assembler::encodeRelocation() {
		relocation.clear();
		for (const auto &[node, reloc]: relocationMap)
			relocation.appendAll(reloc.encode());
	}

	Section * Assembler::getSection(const std::string *label) {
		if (!label || symbolTypes.count(label) == 0)
			return nullptr;
		switch (symbolTypes.at(label)) {
			case SymbolType::Function:
			case SymbolType::Instruction:
				return &code;
			case SymbolType::Object:
				return &data;
			default:
				warn() << "Unhandled SymbolType: " << int(symbolTypes.at(label)) << '\n';
				return nullptr;
		}
	}

	void Assembler::relocateCode() {
		processRelocation();

		for (const auto &[node, reloc]: relocationMap) {
			if (!reloc.section)
				throw std::runtime_error("Relocation is missing a section");
			size_t index = reloc.symbolIndex;
			if (index == -1ul && reloc.label && symbolTableIndices.count(reloc.label) != 0)
				index = symbolTableIndices.at(reloc.label);
			long address = index == -1ul? 0 : long(symbolTableEntries.at(index).address);
			Section *definition_section = getSection(reloc.label);
			if (definition_section)
				address += getOffset(*definition_section);
			address += reloc.offset;
			switch (reloc.type) {
				case RelocationType::Full:
					reloc.section->insert(reloc.sectionOffset, Long(address));
					break;
				case RelocationType::Upper4:
					reloc.section->insert(reloc.sectionOffset, uint32_t(address >> 32));
					break;
				case RelocationType::Lower4:
					reloc.section->insert(reloc.sectionOffset, uint32_t(address & 0xffffffff));
					break;
				default:
					throw std::runtime_error("Invalid relocation type: " + std::to_string(int(reloc.type)));
			}
		}
	}

	size_t Assembler::getOffset(Section &section) const {
		if (&section == &meta)
			return 0;
		if (&section == &code)
			return metaOffsetCode();
		if (&section == &data)
			return metaOffsetData();
		if (&section == &symbols)
			return metaOffsetSymbols();
		if (&section == &debug)
			return metaOffsetDebug();
		if (&section == &relocation)
			return metaOffsetRelocation();
		throw std::runtime_error("Invalid section: " + Util::toHex(&section));
	}

	void Assembler::validateSectionCounts() {
		bool meta_found = false, include_found = false, debug_found = false, text_found = false;
		for (const ASTNode *node: *root)
			switch (node->symbol) {
				case WASMTOK_META_HEADER:
					if (meta_found)
						throw std::runtime_error("Multiple meta sections detected");
					meta_found = true;
					metaNode = node;
					break;
				case WASMTOK_INCLUDE_HEADER:
					if (include_found)
						throw std::runtime_error("Multiple include sections detected");
					include_found = true;
					includeNode = node;
					break;
				case WASMTOK_DEBUG_HEADER:
					if (debug_found)
						throw std::runtime_error("Multiple debug sections detected");
					debug_found = true;
					debugNode = node;
					break;
				case WASMTOK_TEXT_HEADER:
					if (text_found)
						throw std::runtime_error("Multiple text sections detected");
					text_found = true;
					textNode = node;
					break;
				default:
					throw std::runtime_error("Unexpected symbol found at root level: "
						+ std::string(wasmParser.getName(node->symbol)));
			}
	}

	void Assembler::findAllLabels() {
		allLabels.clear();

		if (textNode) {
			for (const ASTNode *node: *textNode)
				if (node->symbol == WASM_LABEL) {
					auto *label_node = dynamic_cast<const WASMLabelNode *>(node);
					if (!label_node)
						throw std::runtime_error("label_node is null in Assembler::findAllLabels");
					allLabels.insert(label_node->label);
				} else if (auto *imm_node = dynamic_cast<const HasImmediate *>(node)) {
					if (std::holds_alternative<const std::string *>(imm_node->imm))
						allLabels.insert(std::get<const std::string *>(imm_node->imm));
				}
		}
	}

	void Assembler::createSymbolTableSkeleton(StringPtrSet labels) {
		labels.insert(StringSet::intern(".end"));
		symbols.clear();

		for (const std::string *label: labels) {
			const size_t length = Util::updiv(label->size(), 8ul);
			if (0xffff < length)
				throw std::runtime_error("Symbol length too long: " + std::to_string(length));
			SymbolEnum type = SymbolEnum::Unknown;
			if (symbolTypes.count(label) != 0) {
				SymbolType specified_type = symbolTypes.at(label);
				switch (specified_type) {
					case SymbolType::Function:
					case SymbolType::Instruction:
						type = SymbolEnum::Code;
						break;
					case SymbolType::Object:
						type = SymbolEnum::Data;
						break;
					case SymbolType::Unknown:
						break;
					default:
						throw std::runtime_error("Invalid symbol type for " + *label + ": " +
							std::to_string(unsigned(specified_type)));
				}
			}
			SymbolTableEntry entry(encodeSymbol(label), 0, type);
			symbols.appendAll(entry.encode(*label));
			symbolTableIndices.emplace(label, symbolTableEntries.size());
			symbolTableEntries.push_back(entry);
		}
	}

	void Assembler::updateSymbolTable(StringPtrSet labels) {
		labels.insert(StringSet::intern(".end"));
		symbols.clear();
		symbolTableEntries.clear();

		for (const std::string *label: labels) {
			const size_t length = Util::updiv(label->size(), 8ul);
			if (0xffff < length)
				throw std::runtime_error("Symbol length too long: " + std::to_string(length));
			SymbolEnum type = SymbolEnum::Unknown;
			if (symbolTypes.count(label) != 0) {
				SymbolType specified_type = symbolTypes.at(label);
				switch (specified_type) {
					case SymbolType::Function:
					case SymbolType::Instruction:
						type = SymbolEnum::Code;
						break;
					case SymbolType::Object:
						type = SymbolEnum::Data;
						break;
					case SymbolType::Unknown:
						break;
					default:
						throw std::runtime_error("Invalid symbol type for " + *label + ": " +
							std::to_string(unsigned(specified_type)));
				}
			}

			Long address = 0;
			if (offsets.count(label) != 0)
				address = offsets.at(label);

			SymbolTableEntry entry(encodeSymbol(label), address, type);
			symbols.appendAll(entry.encode(*label));
			if (symbolTableIndices.count(label) != 0)
				symbolTableIndices.erase(label);
			symbolTableIndices.emplace(label, symbolTableEntries.size());
			symbolTableEntries.push_back(entry);
		}
	}

	uint32_t Assembler::encodeSymbol(const std::string *name) {
		std::vector<uint8_t> hash_vec(picosha2::k_digest_size);
		picosha2::hash256(name->begin(), name->end(), hash_vec.begin(), hash_vec.end());
		const uint32_t hash = hash_vec[4] | (hash_vec[5] << 8) | (hash_vec[6] << 16) | (hash_vec[7] << 24);
		if (hashes.count(hash) != 0 && hashes.at(hash) != name)
			throw std::runtime_error("\"" + *name + "\" and \"" + *hashes.at(hash) + "\" have the same hash!");
		hashes.emplace(hash, name);
		return hash;
	}

	uint32_t Assembler::encodeSymbol(const std::string &name) {
		std::vector<uint8_t> hash_vec(picosha2::k_digest_size);
		picosha2::hash256(name.begin(), name.end(), hash_vec.begin(), hash_vec.end());
		return hash_vec[4] | (hash_vec[5] << 8) | (hash_vec[6] << 16) | (hash_vec[7] << 24);
	}

	void Assembler::processMetadata() {
		std::string orcid = "0000000000000000", name = "?", version = "?", author = "?";

		if (metaNode)
			for (const ASTNode *node: *metaNode)
				switch (node->symbol) {
					case WASMTOK_NAME:
						name = node->front()->unquote();
						break;
					case WASMTOK_AUTHOR:
						author = node->front()->unquote();
						break;
					case WASMTOK_VERSION:
						version = node->front()->unquote();
						break;
					case WASMTOK_ORCID:
						orcid = node->front()->unquote();
						break;
					default:
						throw std::runtime_error("Unexpected symbol found in meta section at "
							+ std::string(node->location) + ": " + std::string(wasmParser.getName(node->symbol)));
				}

		if (orcid.find_first_not_of("0123456789") != std::string::npos) {
			std::string new_orcid;
			new_orcid.reserve(16);
			for (const char ch: orcid)
				if (isdigit(ch))
					new_orcid.push_back(ch);
			orcid.swap(new_orcid);
		}

		if (orcid.size() != 16)
			throw std::runtime_error("Invalid ORCID length");

		const auto orcid_longs = Util::getLongs(orcid);
		if (orcid_longs.size() != 2)
			throw std::runtime_error("ORCID longs count expected to be 2, not " + std::to_string(orcid_longs.size()));

		meta.clear();
		meta.appendAll(std::initializer_list<Long> {0, 0, 0, 0, 0, 0, orcid_longs[0], orcid_longs[1]});

		std::string nva = name;
		nva += '\0';
		nva.insert(nva.end(), version.begin(), version.end());
		nva += '\0';
		nva.insert(nva.end(), author.begin(), author.end());
		nva += '\0';

		meta.append(nva);

		metaOffsetCode() = meta.alignUp(8);
	}

	void Assembler::expandCode() {
		for (auto &[offset, instruction]: instructionMap) {
			switch (instruction->nodeType()) {
				case WASMNodeType::PseudoPrint:
					addPseudoPrint(offset, instruction);
					break;

				case WASMNodeType::IO:
					addIO(offset, instruction);
					break;

				case WASMNodeType::StringPrint:
					addStringPrint(offset, instruction);
					break;

				case WASMNodeType::Mv:
					addMove(offset, instruction);
					break;

				case WASMNodeType::Jeq:
					addJeq(offset, instruction);
					break;

				default:
					code.insert(offset, compileInstruction(*flipSigns(instruction)));
					break;
			}
		}
	}

	WASMInstructionNode * Assembler::flipSigns(WASMInstructionNode *node) const {
		if (RNode *rnode = dynamic_cast<RNode *>(node)) {
			if (*rnode->oper == ">=") {
				rnode->oper = StringSet::intern("<=");
				rnode->operToken = WASMTOK_LEQ;
				std::swap(rnode->rs, rnode->rt);
			} else if (*rnode->oper == ">") {
				rnode->oper = StringSet::intern("<");
				rnode->operToken = WASMTOK_LANGLE;
				std::swap(rnode->rs, rnode->rt);
			}
		}

		return node;
	}

	void Assembler::addJeq(size_t offset, const WASMInstructionNode *instruction) {
		const auto *jeq = dynamic_cast<const WASMJeqNode *>(instruction);
		const std::string *m7 = registerArray[Why::assemblerOffset + 7];
		const int bang = instruction->bang;
		if (std::holds_alternative<Register>(jeq->addr)) {
			// Address is a register
			if (std::holds_alternative<Register>(jeq->rt)) {
				// RHS is a register
				// rs == rt -> $m7
				auto seq = std::unique_ptr<RNode>(makeSeq(jeq->rs, std::get<Register>(jeq->rt), m7, bang));
				code.insert(offset, compileInstruction(*seq));
				extraInstructions.emplace(offset, std::move(seq));
				offset += 8;
			} else {
				addJeqImmediateRHS(offset, jeq, m7);
				// rs == $m7 -> $m7
				auto seq = std::unique_ptr<RNode>(makeSeq(jeq->rs, m7, m7, bang));
				code.insert(offset, compileInstruction(*seq));
				extraInstructions.emplace(offset, std::move(seq));
				offset += 8;
			}
			// : rd if $m7
			auto jrc = std::make_unique<WASMJrcNode>(jeq->link, m7, std::get<Register>(jeq->addr));
			code.insert(offset, compileInstruction(*jrc));
			extraInstructions.emplace(offset, std::move(jrc));
			offset += 8;
		} else if (std::holds_alternative<Register>(jeq->rt)) {
			// Address is an immediate, RHS is a register
			// rs == rt -> $m7
			auto seq = std::unique_ptr<RNode>(makeSeq(jeq->rs, std::get<Register>(jeq->rt), m7, bang));
			code.insert(offset, compileInstruction(*seq));
			extraInstructions.emplace(offset, std::move(seq));
			offset += 8;
			// : addr if $m7
			auto jc = std::make_unique<WASMJcNode>(std::get<Immediate>(jeq->addr), jeq->link, m7);
			jc->setBang(bang);
			code.insert(offset, compileInstruction(*jc));
			extraInstructions.emplace(offset, std::move(jc));
			offset += 8;
		} else {
			// Address is an immediate, RHS is an immediate
			addJeqImmediateRHS(offset, jeq, m7);
			// : addr if $m7
			auto jc = std::make_unique<WASMJcNode>(std::get<Immediate>(jeq->addr), jeq->link, m7);
			jc->setBang(bang);
			code.insert(offset, compileInstruction(*jc));
			extraInstructions.emplace(offset, std::move(jc));
			offset += 8;
		}
	}

	void Assembler::addJeqImmediateRHS(size_t &offset, const WASMJeqNode *jeq, const std::string *m7) {
		const Immediate &rhs = std::get<Immediate>(jeq->rt);
		std::unique_ptr<WASMInstructionNode> new_node;
		if (std::holds_alternative<const std::string *>(rhs)) {
			// RHS is a label
			// [label] -> $m7
			new_node = std::make_unique<WASMLiNode>(rhs, m7, false);
		} else if (std::holds_alternative<int>(rhs)) {
			// RHS is a number
			// imm -> $m7
			new_node = std::make_unique<WASMSetNode>(rhs, m7);
		} else {
			jeq->debug();
			throw std::runtime_error("Invalid right hand side in jeq instruction");
		}

		new_node->setBang(jeq->bang);
		code.insert(offset, compileInstruction(*new_node));
		extraInstructions.emplace(offset, std::move(new_node));
		offset += 8;
	}

	void Assembler::addMove(size_t offset, const WASMInstructionNode *instruction) {
		const auto *move = dynamic_cast<const WASMMvNode *>(instruction);

		auto rnode = std::make_unique<RNode>(move->rs, StringSet::intern("|"), registerArray[Why::zeroOffset], move->rd,
			WASMTOK_OR, false);
		code.insert(offset, compileInstruction(*rnode));
		extraInstructions.emplace(offset, std::move(rnode));
	}

	void Assembler::addPseudoPrint(size_t offset, const WASMInstructionNode *instruction) {
		const auto *print = dynamic_cast<const WASMPseudoPrintNode *>(instruction);
		if (std::holds_alternative<char>(print->imm)) {
			const std::string *m7 = registerArray[Why::assemblerOffset + 7];

			auto set = std::make_unique<WASMSetNode>(print->imm, m7);
			set->setBang(instruction->bang);
			code.insert(offset, compileInstruction(*set));
			extraInstructions.emplace(offset, std::move(set));
			offset += 8;

			auto new_print = std::make_unique<WASMPrintNode>(m7, PrintType::Char);
			new_print->setBang(instruction->bang);
			code.insert(offset, compileInstruction(*new_print));
			extraInstructions.emplace(offset, std::move(new_print));
		} else
			throw std::runtime_error("Invalid WASMPseudoPrintNode immediate type: expected char");
	}

	void Assembler::addIO(size_t offset, const WASMInstructionNode *instruction) {
		const auto *io = dynamic_cast<const WASMIONode *>(instruction);

		if (!io->ident) {
			code.insert(offset, compileInstruction(*io));
			return;
		}

		if (Why::ioIDs.count(*io->ident) == 0)
			throw std::runtime_error("Unknown IO ident: \"" + *io->ident + "\"");

		const std::string *a0 = registerArray[Why::argumentOffset];
		auto set = std::make_unique<WASMSetNode>(Why::ioIDs.at(*io->ident), a0);
		set->setBang(instruction->bang);
		code.insert(offset, compileInstruction(*set));
		extraInstructions.emplace(offset, std::move(set));
		offset += 8;

		auto new_io = std::make_unique<WASMIONode>(nullptr);
		new_io->setBang(instruction->bang);
		code.insert(offset, compileInstruction(*new_io));
		extraInstructions.emplace(offset, std::move(new_io));
	}

	void Assembler::addStringPrint(size_t offset, const WASMInstructionNode *instruction) {
		const auto *print = dynamic_cast<const WASMStringPrintNode *>(instruction);
		const std::string *m7 = registerArray[Why::assemblerOffset + 7];
		const std::string &str = *print->string;
		if (str.empty())
			return;
		char last_char = str.front() - 1;
		bool first = true;
		for (char ch: str) {
			if (ch != last_char) {
				auto set = std::make_unique<WASMSetNode>(ch, m7);
				set->setBang(print->bang);
				if (first)
					set->labels = print->labels;
				first = false;
				code.insert(offset, compileInstruction(*set));
				extraInstructions.emplace(offset, std::move(set));
				offset += 8;
				last_char = ch;
			}

			auto new_print = std::make_unique<WASMPrintNode>(m7, PrintType::Char);
			new_print->setBang(print->bang);
			code.insert(offset, compileInstruction(*new_print));
			extraInstructions.emplace(offset, std::move(new_print));
			offset += 8;
		}
	}

	void Assembler::createDebugData(const ASTNode *node) {
		debugEntries.clear();
		debug.clear();

		if (!node)
			return;

		debugEntries.reserve(node->size());

		for (const ASTNode *child: *node) {
			if (child->symbol != WASMTOK_NUMBER) {
				child->debug();
				throw std::runtime_error("Unexpected symbol found in debug section at " + std::string(node->location)
					+ ": " + std::string(wasmParser.getName(node->symbol)));
			}
			const int type = child->atoi();
			switch (type) {
				case 1:
				case 2: {
					const ASTNode *subchild = child->front();
					const std::string unquoted = subchild->unquote();
					const size_t length = unquoted.size();
					if (0xffffff < length)
						throw std::runtime_error("Name too long (" + std::to_string(length) + " characters, type = "
							+ std::to_string(type) + ")");
					std::vector<uint8_t> encoded {
						static_cast<uint8_t>(type), static_cast<uint8_t>(length & 0xff),
						static_cast<uint8_t>((length >> 8) & 0xff), static_cast<uint8_t>((length >> 16) & 0xff)
					};
					for (char ch: unquoted)
						encoded.push_back(static_cast<uint8_t>(ch));
					debug.appendAll(Util::getLongs(encoded));
					if (type == 1)
						debugEntries.emplace_back(new DebugFilename(unquoted));
					else
						debugEntries.emplace_back(new DebugFunction(unquoted));
					break;
				}

				case 3:
					debugEntries.emplace_back(new DebugLocation((*child)[0]->atoi(), (*child)[1]->atoi(),
						(*child)[2]->atoi(), (*child)[3]->atoi()));
					break;

				default:
					throw std::runtime_error("Unexpected debug node type: " + std::to_string(type));
			}
		}

		const size_t debug_size = debugEntries.size();
		std::unordered_set<Long> exclude;

		auto process = [this, debug_size, &exclude](size_t offset, const WASMInstructionNode *instruction) {
			if (exclude.count(offset) != 0)
				return;
			const int bang = instruction->bang;
			if (bang == -1)
				return;
			if (bang < 0 || debug_size <= size_t(bang)) {
				instruction->debug();
				throw std::runtime_error("Debug intbang out of bounds: " + std::to_string(bang));
			}
			if (debugEntries[bang]->getType() != DebugEntry::Type::Location) {
				instruction->debug();
				throw std::runtime_error("Debug intbang doesn't refer to a location");
			}
			DebugLocation *location = dynamic_cast<DebugLocation *>(debugEntries[bang].get());
			if (!location) {
				instruction->debug();
				throw std::runtime_error("DebugLocation cast failed");
			}
			const size_t address = metaOffsetCode() + offset;
			size_t count = 1;
			for (size_t j = address + 8; j < code.size(); j += 8) {
				int subbang;
				if (instructionMap.count(j) != 0) {
					subbang = instructionMap.at(j)->bang;
				} else if (extraInstructions.count(j) != 0) {
					subbang = extraInstructions.at(j)->bang;
				} else
					throw std::runtime_error("Couldn't find instruction at offset " + std::to_string(j));
				if (bang == subbang)
					++count;
				else
					break;
				exclude.insert(j);
			}

			if (0xff < count)
				throw std::runtime_error("Instruction count too high: " + std::to_string(count));

			if (0xffffff < location->fileIndex)
				throw std::runtime_error("File index too large: " + std::to_string(location->fileIndex));

			if (0xffffffff < location->line)
				throw std::runtime_error("Line number too large: " + std::to_string(location->line));

			if (0xffffff < location->column)
				throw std::runtime_error("Column number too large: " + std::to_string(location->column));

			if (0xffffff < location->functionIndex)
				throw std::runtime_error("Function index too large: " + std::to_string(location->functionIndex));

			std::vector<uint8_t> to_add {static_cast<uint8_t>(location->getType())};
			auto add = [&](size_t n, size_t byte_count) {
				for (size_t j = 0; j < byte_count; ++j)
					to_add.push_back((n >> (8 * j)) & 0xff);
			};

			add(location->fileIndex, 3);
			add(location->line, 4);
			add(location->column, 3);
			to_add.push_back(count & 0xff);
			add(location->functionIndex, 4);
			add(address, 8);

			while (to_add.size() % 8)
				to_add.push_back(0);

			debug.appendAll(to_add);
			exclude.insert(offset);
		};

		for (const auto &[offset, instruction]: instructionMap)
			process(offset, instruction);

		for (const auto &[offset, instruction]: extraInstructions)
			process(offset, instruction.get());

		debug.alignUp(8);
	}
}
