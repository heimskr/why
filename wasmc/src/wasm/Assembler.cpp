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
#include "wasm/Nodes.h"
#include "wasm/Registers.h"
#include "wasm/Why.h"

namespace Wasmc {
	size_t Assembler::assemblerCount = 0;

	Assembler::Assembler(const ASTNode *root_): root(root_) {}

	std::string Assembler::assemble(bool can_warn) {
		wasmParser.errorCount = 0;
		// meta.resize(4, 0);
		validateSectionCounts();
		findAllLabels();
		processMetadata();

		processData(allLabels);

		symbolTable = createSymbolTable(allLabels, true);
		for (Long piece: symbolTable)
			append(piece);

		metaOffsetText() = alignUp(8);

		auto expanded = expandText();
		metaOffsetDebug() = alignUp(8);

		debugData = createDebugData(debugNode, expanded);
		offsets[StringSet::intern(".end")] = metaOffsetEnd() = metaOffsetDebug() + debugData.size() * 8;

		setDataOffsets();
		reprocessData();
		processCode(expandLabels(expanded));
		symbolTable = createSymbolTable(allLabels, false);

		assembled.clear();
		for (const auto &longs: {meta, symbolTable, code, data, debugData})
			assembled.insert(assembled.end(), longs.begin(), longs.end());

		if (can_warn && 0 < unknownSymbols.size()) {
			std::cerr << " \e[1;33m?\e[22;39m Unknown symbol" << (unknownSymbols.size() == 1? "" : "s") << ": ";
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
		return stringify(assembled);
	}

	std::string Assembler::stringify(const std::vector<Long> &longs) {
		std::stringstream ss;
		bool first = true;
		for (Long piece: longs) {
			if (first)
				first = false;
			else
				ss << "\n";
			for (int i = 0; i < 8; ++i)
				ss << std::hex << std::right << std::setw(2) << std::setfill('0') << ((piece >> (8 * i)) & 0xff);
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

	void Assembler::addCode(const WASMInstructionNode &node) {
		code.push_back(compileInstruction(node));
	}

	Statements & Assembler::expandLabels(Statements &statements) {
		// In the second pass, we replace label references with the corresponding
		// addresses now that we know the address of all the labels.
		for (auto &statement: statements) {
			statement->labels.clear();
			if (auto *has_immediate = dynamic_cast<HasImmediate *>(statement.get())) {
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

		return statements;
	}

	void Assembler::processCode(const Statements &statements) {
		for (const auto &statement: statements)
			addCode(*statement);
	}

	// void Assembler::reprocessData() {
	// 	for (const auto &[key, ref]: dataVariables)
	// 		data.at(dataOffsets.at(key) / 8) = offsets.count(ref) == 0? encodeSymbol(ref) : offsets.at(ref);
	// }

	// void Assembler::setDataOffsets() {
	// 	for (const auto &[name, offset]: dataOffsets)
	// 		offsets[name] = offset + metaOffsetData();
	// }

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

		if (textNode)
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

	std::vector<Long> Assembler::createSymbolTable(std::unordered_set<const std::string *> labels, bool skeleton) {
		labels.insert(StringSet::intern(".end"));
		std::vector<Long> out;

		for (const std::string *label: labels) {
			const size_t length = Util::updiv(label->size(), 8ul);
			if (0xffff < length)
				throw std::runtime_error("Symbol length too long: " + std::to_string(length));
			SymbolEnum type = SymbolEnum::Unknown;
			if (!skeleton && dataVariables.count(label)) {
				const std::string *ptr = dataVariables.at(label);
				type = offsets.count(ptr)? SymbolEnum::KnownPointer : SymbolEnum::UnknownPointer;
				if (!offsets.count(ptr) && !unknownSymbols.count(ptr)) {
					const size_t index = (offsets.at(label) - metaOffsetData()) / 8;
					if (data.size() < index)
						data.resize(index, 0);
					data[index] = encodeSymbol(ptr);
				}
			}

			out.push_back(length | (static_cast<Long>(type) << 16) | (static_cast<Long>(encodeSymbol(label)) << 32));
			out.push_back(skeleton? 0 : offsets.at(label));
			for (const Long piece: Util::getLongs(*label))
				out.push_back(piece);
		}

		return out;
	}

	uint32_t Assembler::encodeSymbol(const std::string *name) {
		std::vector<uint8_t> hash_vec(picosha2::k_digest_size);
		picosha2::hash256(name->begin(), name->end(), hash_vec.begin(), hash_vec.end());
		// TODO: verify
		const uint32_t hash = hash_vec[4] | (hash_vec[5] << 8) | (hash_vec[6] << 16) | (hash_vec[7] << 24);
		if (hashes.count(hash) != 0 && hashes.at(hash) != name)
			throw std::runtime_error("\"" + *name + "\" and \"" + *hashes.at(hash) + "\" have the same hash!");
		hashes.emplace(hash, name);
		return hash;
	}

	uint32_t Assembler::encodeSymbol(const std::string &name) {
		std::vector<uint8_t> hash_vec(picosha2::k_digest_size);
		picosha2::hash256(name.begin(), name.end(), hash_vec.begin(), hash_vec.end());
		// TODO: verify
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

		clear();
		append<Long>({0, 0, 0, 0, orcid_longs[0], orcid_longs[1]});

		std::string nva = name;
		nva += '\0';
		nva.insert(nva.end(), version.begin(), version.end());
		nva += '\0';
		nva.insert(nva.end(), author.begin(), author.end());
		nva += '\0';

		append(nva);

		metaOffsetText() = alignUp(8);
	}

	Statements Assembler::expandText() {
		if (!textNode)
			return {};

		Statements expanded;
		expanded.reserve(textNode->size());

		for (const ASTNode *node: *textNode) {
			const auto *instruction = dynamic_cast<const WASMInstructionNode *>(node);
			if (!instruction) {
				node->debug();
				throw std::runtime_error("Unexpected symbol found in code section at " + std::string(node->location)
					+ ": " + std::string(wasmParser.getName(node->symbol)));
			}

			for (const std::string *label: instruction->labels) {
				if (offsets.count(label) != 0)
					throw std::runtime_error("Label " + *label + " redefined at " + std::string(node->location));
				offsets[label] = metaOffsetCode() + expanded.size() * 8;
				if (verbose)
					std::cerr << "Assigning " << offsets[label] << " to " << *label << " based on an expanded length "
					             "equal to " << expanded.size() << "\n";
			}

			switch (instruction->nodeType()) {
				case WASMNodeType::Call:
					addCall(expanded, instruction);
					break;

				case WASMNodeType::PseudoPrint:
					addPseudoPrint(expanded, instruction);
					break;

				case WASMNodeType::IO:
					addIO(expanded, instruction);
					break;

				case WASMNodeType::StringPrint:
					addStringPrint(expanded, instruction);
					break;

				case WASMNodeType::Mv:
					addMove(expanded, instruction);
					break;

				case WASMNodeType::Jeq:
					addJeq(expanded, instruction);
					break;

				default:
					expanded.emplace_back(flipSigns(instruction->copy()));
					break;
			}
		}

		return expanded;
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

	void Assembler::addJeq(Statements &expanded, const WASMInstructionNode *instruction) {
		const auto *jeq = dynamic_cast<const WASMJeqNode *>(instruction);
		const std::string *m7 = registerArray[Why::assemblerOffset + 7];
		const int bang = instruction->bang;
		if (std::holds_alternative<Register>(jeq->addr)) {
			// Address is a register
			if (std::holds_alternative<Register>(jeq->rt)) {
				// RHS is a register
				// rs == rt -> $m7
				expanded.emplace_back(makeSeq(jeq->rs, std::get<Register>(jeq->rt), m7, bang));
			} else {
				addJeqImmediateRHS(expanded, jeq, m7);
				// rs == $m7 -> $m7
				expanded.emplace_back(makeSeq(jeq->rs, m7, m7, bang));
			}
			// : rd if $m7
			expanded.emplace_back(new WASMJrcNode(jeq->link, m7, std::get<Register>(jeq->addr)));
		} else if (std::holds_alternative<Register>(jeq->rt)) {
			// Address is an immediate, RHS is a register
			// rs == rt -> $m7
			expanded.emplace_back(makeSeq(jeq->rs, std::get<Register>(jeq->rt), m7, bang));
			// : addr if $m7
			expanded.emplace_back((new WASMJcNode(std::get<Immediate>(jeq->addr), jeq->link, m7))->setBang(bang));
		} else {
			// Address is an immediate, RHS is an immediate
			addJeqImmediateRHS(expanded, jeq, m7);
			// : addr if $m7
			expanded.emplace_back((new WASMJcNode(std::get<Immediate>(jeq->addr), jeq->link, m7))->setBang(bang));
		}
	}

	void Assembler::addJeqImmediateRHS(Statements &expanded, const WASMJeqNode *jeq, const std::string *m7) {
		const Immediate &rhs = std::get<Immediate>(jeq->rt);
		if (std::holds_alternative<const std::string *>(rhs)) {
			// RHS is a label
			// [label] -> $m7
			expanded.emplace_back(new WASMLiNode(rhs, m7, false));
		} else if (std::holds_alternative<int>(rhs)) {
			// RHS is a number
			// imm -> $m7
			expanded.emplace_back(new WASMSetNode(rhs, m7));
		} else {
			jeq->debug();
			throw std::runtime_error("Invalid right hand side in jeq instruction");
		}
		expanded.back()->setBang(jeq->bang);
	}

	void Assembler::addMove(Statements &expanded, const WASMInstructionNode *instruction) {
		const auto *move = dynamic_cast<const WASMMvNode *>(instruction);
		expanded.emplace_back((new RNode(move->rs, StringSet::intern("|"), registerArray[Why::zeroOffset], move->rd,
			WASMTOK_OR, false))->setBang(instruction->bang));
	}

	void Assembler::addPseudoPrint(Statements &expanded, const WASMInstructionNode *instruction) {
		const auto *print = dynamic_cast<const WASMPseudoPrintNode *>(instruction);
		if (std::holds_alternative<char>(print->imm)) {
			const std::string *m7 = registerArray[Why::assemblerOffset + 7];
			expanded.emplace_back((new WASMSetNode(print->imm, m7))->setBang(instruction->bang));
			expanded.emplace_back((new WASMPrintNode(m7, PrintType::Char))->setBang(instruction->bang));
		} else
			throw std::runtime_error("Invalid WASMPseudoPrintNode immediate type: expected char");
	}

	void Assembler::addIO(Statements &expanded, const WASMInstructionNode *instruction) {
		const auto *io = dynamic_cast<const WASMIONode *>(instruction);

		if (!io->ident) {
			expanded.emplace_back(io->copy());
			return;
		}

		if (Why::ioIDs.count(*io->ident) == 0)
			throw std::runtime_error("Unknown IO ident: \"" + *io->ident + "\"");

		const std::string *a0 = registerArray[Why::argumentOffset];
		expanded.emplace_back((new WASMSetNode(Why::ioIDs.at(*io->ident), a0))->setBang(instruction->bang));
		expanded.emplace_back((new WASMIONode(nullptr))->setBang(instruction->bang));
	}

	void Assembler::addStringPrint(Statements &expanded, const WASMInstructionNode *instruction) {
		const auto *print = dynamic_cast<const WASMStringPrintNode *>(instruction);
		const std::string *m7 = registerArray[Why::assemblerOffset + 7];
		const std::string &str = *print->string;
		if (str.empty())
			return;
		char last_char = str.front() - 1;
		bool first = true;
		for (char ch: str) {
			if (ch != last_char) {
				auto *set = (new WASMSetNode(ch, m7))->setBang(print->bang);
				if (first)
					set->labels = print->labels;
				first = false;
				expanded.emplace_back(set);
				last_char = ch;
			}

			expanded.emplace_back((new WASMPrintNode(m7, PrintType::Char))->setBang(print->bang));
		}
	}

	void Assembler::addCall(Statements &expanded, const WASMInstructionNode *instruction) {
		const auto *call = dynamic_cast<const WASMCallNode *>(instruction);
		const Args &args = call->args;

		if (Why::argumentCount < args.size())
			throw std::runtime_error("Too many arguments given in subroutine call (given "
				+ std::to_string(args.size()) + ", maximum is " + std::to_string(Why::argumentCount) + ")");

		std::vector<int> current_values;

		if (instruction->inSubroutine)
			current_values.push_back(Why::returnAddressOffset);

		for (size_t i = 0; i < args.size(); ++i)
			current_values.push_back(Why::argumentOffset + i);

		if (!current_values.empty())
			addStack(expanded, current_values, instruction->labels, true, call->bang);

		for (size_t i = 0; i < args.size(); ++i) {
			const std::string *reg = registerArray[Why::argumentOffset + i];
			Arg &arg = args[i];
			switch (arg.getType()) {
				case Arg::Type::Address:
					expanded.emplace_back(new WASMSetNode(dynamic_cast<AddressArg &>(arg).ident, reg));
					break;
				case Arg::Type::Value:
					expanded.emplace_back(new WASMLiNode(dynamic_cast<ValueArg &>(arg).ident, reg, false));
					break;
				case Arg::Type::Number:
					expanded.emplace_back(new WASMSetNode(dynamic_cast<NumberArg &>(arg).value, reg));
					break;
				case Arg::Type::Register:
					expanded.emplace_back(new WASMMvNode(
						registerArray[dynamic_cast<RegisterArg &>(arg).reg], reg));
					break;
				default:
					throw std::runtime_error("Invalid Arg type");
			}
			expanded.back()->setBang(call->bang);
		}

		expanded.emplace_back((new WASMJNode(call->function, true))->setBang(call->bang));

		std::reverse(current_values.begin(), current_values.end());
		addStack(expanded, current_values, {}, false, call->bang);
	}

	void Assembler::addStack(Statements &expanded, const std::vector<int> &regs, const Strings &labels, bool is_push,
	                         int bang) {
		bool first = true;
		for (const int reg: regs) {
			auto node = std::make_shared<WASMStackNode>(registerArray[reg], is_push);
			if (first)
				node->labels = labels;
			else
				first = false;
			node->setBang(bang);
			expanded.push_back(node);
		}
	}

	std::vector<Long> Assembler::createDebugData(const ASTNode *node, const Statements &expanded) {
		std::vector<Long> out;
		debugEntries.clear();

		if (!node)
			return out;

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
					for (const Long piece: Util::getLongs(encoded))
						out.push_back(piece);
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

		const size_t expanded_size = expanded.size();
		const size_t debug_size = debugEntries.size();
		for (size_t i = 0; i < expanded_size; ++i) {
			const auto &instruction = expanded[i];
			const int bang = instruction->bang;
			if (bang == -1)
				continue;
			if (bang < 0 || debug_size <= static_cast<size_t>(bang)) {
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
			const size_t address = metaOffsetCode() + 8 * i;
			size_t count = 1;
			for (size_t j = i + 1; j < expanded_size; ++j)
				if (bang == expanded[j]->bang)
					++count;
				else
					break;

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

			for (const Long piece: Util::getLongs(to_add))
				out.push_back(piece);
			i += count - 1;
		}

		return out;
	}
}
