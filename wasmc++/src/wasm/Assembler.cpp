#include <cctype>
#include <iostream>

#include "lib/picosha2.h"
#include "parser/Lexer.h"
#include "parser/StringSet.h"
#include "util/Util.h"
#include "wasm/Assembler.h"
#include "wasm/Nodes.h"
#include "wasm/Registers.h"
#include "wasm/Why.h"

namespace Wasmc {
	Assembler::Assembler(Parser &parser_): parser(parser_) {}

	void Assembler::assemble() {
		meta.resize(5, 0);
		validateSectionCounts();
		findAllLabels();
		symbolTable = createSymbolTable(allLabels, true);
		processMetadata();
		processData();

		const auto expanded = expandCode();
	}

	void Assembler::validateSectionCounts() {
		bool meta_found = false, include_found = false, data_found = false, debug_found = false, code_found = false;
		for (ASTNode *node: *parser.root)
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
				case WASMTOK_DATA_HEADER:
					if (data_found)
						throw std::runtime_error("Multiple data sections detected");
					data_found = true;
					dataNode = node;
					break;
				case WASMTOK_DEBUG_HEADER:
					if (debug_found)
						throw std::runtime_error("Multiple debug sections detected");
					debug_found = true;
					debugNode = node;
					break;
				case WASMTOK_CODE_HEADER:
					if (code_found)
						throw std::runtime_error("Multiple code sections detected");
					code_found = true;
					codeNode = node;
					break;
				default:
					throw std::runtime_error("Unexpected symbol found at root level: "
						+ std::string(parser.getName(node->symbol)));
			}
	}

	void Assembler::findAllLabels() {
		allLabels.clear();

		if (dataNode) {
			for (const ASTNode *node: *dataNode) {
				if (node->symbol != WASMTOK_IDENT)
					throw std::runtime_error("Unexpected symbol found in data section at "
						+ std::string(node->location) + ": " + std::string(parser.getName(node->symbol)));
				allLabels.insert(node->lexerInfo);
			}
		}

		if (codeNode) {
			for (const ASTNode *node: *codeNode) {
				const auto *instruction = dynamic_cast<const WASMInstructionNode *>(node);
				if (!instruction)
					throw std::runtime_error("Unexpected symbol found in code section at "
						+ std::string(node->location) + ": " + std::string(parser.getName(node->symbol)));
				for (const std::string *label: instruction->labels)
					allLabels.insert(label);
			}
		}
	}

	std::vector<Long> Assembler::createSymbolTable(std::unordered_set<const std::string *> labels, bool skeleton) {
		labels.insert(StringSet::intern(".end"));
		std::vector<Long> out;

		for (const std::string *label: labels) {
			const size_t length = Util::updiv(label->size(), 8ul);
			if (0xffff < length)
				throw std::runtime_error("Symbol length too long: " + std::to_string(length));
			SymbolType type = SymbolType::Unknown;
			if (!skeleton && dataVariables.count(label)) {
				const std::string *ptr = dataVariables.at(label);
				type = offsets.count(ptr)? SymbolType::KnownPointer : SymbolType::UnknownPointer;
				if (!offsets.count(ptr) && !unknownSymbols.count(ptr)) {
					const size_t index = (offsets.at(label) - metaOffsetData()) / 8;
					data.resize(index, 0);
					data[index] = encodeSymbol(ptr);
				}
			}

			out.push_back(length | (static_cast<int>(type) << 16) | (static_cast<uint64_t>(encodeSymbol(label)) << 32));
			out.push_back(skeleton? 0 : offsets.at(label));
			for (const Long piece: str2longs(*label))
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

	std::vector<Long> Assembler::str2longs(const std::string &str) const {
		if (str.empty())
			return {0};

		std::vector<Long> out;
		out.reserve(Util::updiv(str.size(), 8ul));

		uint8_t count = 0;
		Long next = 0;
		for (char ch: str) {
			next = (next << 8) | ch;
			if (++count == 8) {
				out.push_back(next);
				next = 0;
				count = 0;
			}
		}

		if (count != 0)
			out.push_back(next << (8 * (8 - count)));

		return out;
	}

	void Assembler::processMetadata() {
		std::string orcid = "0000000000000000", name = "?", version = "?", author = "?";

		if (metaNode)
			for (const ASTNode *node: *metaNode)
				switch (node->symbol) {
					case WASMTOK_NAME:
						name = *node->front()->lexerInfo;
						break;
					case WASMTOK_AUTHOR:
						author = *node->front()->lexerInfo;
						break;
					case WASMTOK_VERSION:
						version = *node->front()->lexerInfo;
						break;
					case WASMTOK_ORCID:
						orcid = *node->front()->lexerInfo;
						break;
					default:
						throw std::runtime_error("Unexpected symbol found in meta section at "
							+ std::string(node->location) + ": " + std::string(parser.getName(node->symbol)));
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

		const auto longs = str2longs(orcid);
		if (longs.size() != 2)
			throw std::runtime_error("ORCID longs count expected to be 2, not " + std::to_string(longs.size()));
		meta = {0, 0, 0, 0, 0, longs[0], longs[1]};

		std::string nva = name;
		nva.push_back('\0');
		nva.insert(nva.end(), version.begin(), version.end());
		nva.push_back('\0');
		nva.insert(nva.end(), author.begin(), author.end());
		nva.push_back('\0');

		for (const Long piece: str2longs(nva))
			meta.push_back(piece);

		metaOffsetSymbols() = meta.size() * 8;
		metaOffsetCode() = metaOffsetSymbols() + symbolTable.size() * 8;
	}

	void Assembler::processData() {
		if (!dataNode)
			return;

		size_t length = 0;
		for (ASTNode *node: *dataNode) {
			const std::string *ident = node->lexerInfo;
			if (ident->front() != '%') {
				if (verbose)
					std::cerr << "Assigning " << length << " to " << *ident << "\n";
				dataOffsets.emplace(ident, length);
			}

			const auto pieces = convertDataPieces(node);
			for (const Long piece: pieces)
				data.push_back(piece);
			length += pieces.size() * 8;
		}
	}

	std::vector<Long> Assembler::convertDataPieces(const ASTNode *node) {
		const ASTNode *child = node->front();
		static_assert(sizeof(Long) == sizeof(double));
		switch (child->symbol) {
			case WASMTOK_FLOAT: {
				double parsed = Util::parseDouble(child->lexerInfo);
				return {*reinterpret_cast<Long *>(&parsed)};
			}
			case WASMTOK_NUMBER:
				return {static_cast<Long>(child->atoi())};
			case WASMTOK_STRING: {
				std::string str = *child->lexerInfo;
				str.push_back('\0');
				return str2longs(str);
			}
			case WASMTOK_LPAR:
				return str2longs(std::string(child->front()->atoi(), '\0'));
			case WASMTOK_AND:
				dataVariables[node->lexerInfo] = child->front()->lexerInfo;
				return {0};
			default:
				throw std::runtime_error("Unexpected symbol found in data section at " + std::string(child->location)
					+ ": " + std::string(parser.getName(child->symbol)));
		}
	}

	Statements Assembler::expandCode() {
		// Known bug: locations for statement nodes are inaccurate.

		if (!codeNode)
			return {};

		Statements expanded;
		expanded.reserve(codeNode->size());

		// auto add = [&](std::shared_ptr<WASMStatementNode> &&statement) {
		// 	expanded.push_back(statement);
		// };

		for (const ASTNode *node: *codeNode) {
			const auto *instruction = dynamic_cast<const WASMInstructionNode *>(node);
			if (!instruction) {
				node->debug();
				throw std::runtime_error("Unexpected symbol found in code section at " + std::string(node->location)
					+ ": " + std::string(parser.getName(node->symbol)));
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
				case WASMNodeType::Call: {
					const auto *call = dynamic_cast<const WASMCallNode *>(instruction);
					const std::string *function = call->function;
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
						addStack(expanded, current_values, instruction->labels, true);

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
					break;
				}

				default:
					break;
			}
		}

		return expanded;
	}

	void Assembler::addStack(Statements &expanded, const std::vector<int> &regs, const Strings &labels, bool is_push) {
		bool first = true;
		for (const int reg: regs) {
			auto node = std::make_shared<WASMStackNode>(registerArray[reg], is_push);
			if (first)
				node->labels = labels;
			else
				first = false;
			expanded.push_back(node);
		}
	}
}
