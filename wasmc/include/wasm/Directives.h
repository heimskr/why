#pragma once

#include <memory>
#include <string>

#include "parser/ASTNode.h"
#include "wasm/Types.h"

namespace Wasmc {
	enum class DirectiveType {Type, Size, String, Value, Align, Fill, Data, Code};
	class Expression;

	struct Directive: ASTNode {
		Directive(int symbol_);
		virtual DirectiveType getType() const = 0;
	};

	struct TypeDirective: Directive {
		const std::string *symbolName;
		SymbolType type;
		TypeDirective() = delete;
		TypeDirective(const std::string *, SymbolType);
		TypeDirective(const std::string &, SymbolType);
		TypeDirective(const ASTNode *symbol_name, const ASTNode *type_);
		DirectiveType getType() const override { return DirectiveType::Type; }
	};

	struct SizeDirective: Directive {
		const std::string *symbolName;
		std::shared_ptr<Expression> expression;

		SizeDirective() = delete;
		SizeDirective(const std::string *, Expression *expression_);
		SizeDirective(const ASTNode *symbol_name, Expression *expression_);
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
			uint8_t valueSize;
			std::shared_ptr<Expression> expression;

			ValueDirective() = delete;
			ValueDirective(const ASTNode *value_size, Expression *expression_);
			DirectiveType getType() const override { return DirectiveType::Value; }
			std::string debugExtra() const override;
	};

	struct AlignDirective: Directive {
		size_t alignment;

		AlignDirective() = delete;
		AlignDirective(size_t);
		DirectiveType getType() const override { return DirectiveType::Align; }
	};

	struct FillDirective: Directive {
		long count, value;

		FillDirective() = delete;
		FillDirective(long count_, long value_);
		DirectiveType getType() const override { return DirectiveType::Fill; }
	};

	struct DataDirective: Directive {
		DataDirective();
		DirectiveType getType() const override { return DirectiveType::Data; }
	};

	struct CodeDirective: Directive {
		CodeDirective();
		DirectiveType getType() const override { return DirectiveType::Code; }
	};
}
