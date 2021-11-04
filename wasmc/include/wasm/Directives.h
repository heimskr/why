#pragma once

#include <memory>
#include <string>

#include "parser/ASTNode.h"
#include "wasm/Types.h"

namespace Wasmc {
	enum class DirectiveType {Type, Size, String, Value, Align, Fill, Data, Code};

	struct Directive: ASTNode {
		Directive(int symbol_);
		virtual DirectiveType getType() const = 0;
	};

	struct TypeDirective: Directive {
		const std::string *symbol;
		SymbolType type;
		TypeDirective() = delete;
		TypeDirective(const std::string *, SymbolType);
		TypeDirective(const std::string &, SymbolType);
		TypeDirective(const ASTNode *symbol_, const ASTNode *type_);
		DirectiveType getType() const override { return DirectiveType::Type; }
	};

	struct SizeDirective: Directive {
		const std::string *symbol;
		std::shared_ptr<ASTNode> expression;

		SizeDirective() = delete;
		SizeDirective(const std::string *, ASTNode *expression_);
		SizeDirective(const ASTNode *symbol_, ASTNode *expression_);
		DirectiveType getType() const override { return DirectiveType::Size; }
	};

	struct StringDirective: Directive {
		const std::string *string;
		bool nullTerminate;

		StringDirective() = delete;
		StringDirective(const std::string *, bool);
		StringDirective(const ASTNode *, bool);
		DirectiveType getType() const override { return DirectiveType::String; }
	};

	class ValueDirective: public Directive {
		private:
			static uint8_t getSize(const ASTNode *);

		public:
			uint8_t size;
			std::shared_ptr<ASTNode> expression;

			ValueDirective() = delete;
			ValueDirective(const ASTNode *size_, ASTNode *expression_);
			DirectiveType getType() const override { return DirectiveType::Value; }
	};

	struct AlignDirective: Directive {
		long alignment;

		AlignDirective() = delete;
		AlignDirective(long);
		DirectiveType getType() const override { return DirectiveType::Align; }
	};

	struct FillDirective: Directive {
		long count, value;

		FillDirective() = delete;
		FillDirective(long count_, long value_);
		DirectiveType getType() const override { return DirectiveType::Fill; }
	};

	struct DataDirective: Directive {
		DataDirective() = default;
		DirectiveType getType() const override { return DirectiveType::Data; }
	};

	struct CodeDirective: Directive {
		CodeDirective() = default;
		DirectiveType getType() const override { return DirectiveType::Code; }
	};
}
