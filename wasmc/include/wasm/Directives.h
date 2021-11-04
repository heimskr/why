#pragma once

#include <string>

#include "parser/ASTNode.h"

namespace Wasmc {
	struct Directive: ASTNode {
		Directive(int symbol_);
	};

	struct TypeDirective: Directive {
		enum class Type {Function, Object};
		const std::string *symbol;
		Type type;
		TypeDirective() = delete;
		TypeDirective(const std::string *, Type);
		TypeDirective(const std::string &, Type);
		TypeDirective(const ASTNode *symbol_, const ASTNode *type_);
	};

	struct SizeDirective: Directive {
		const std::string *symbol;
		const ASTNode *expression; // will be deleted when SizeDirective is destroyed

		SizeDirective() = delete;
		SizeDirective(const std::string *, const ASTNode *expression_);
		SizeDirective(const ASTNode *symbol_, const ASTNode *expression_);
		~SizeDirective();
	};

	struct StringDirective: Directive {
		const std::string *string;
		bool nullTerminate;

		StringDirective() = delete;
		StringDirective(const std::string *, bool);
		StringDirective(const ASTNode *, bool);
	};

	class ValueDirective: public Directive {
		private:
			static uint8_t getSize(const ASTNode *);

		public:
			uint8_t size;
			const ASTNode *expression; // will be deleted when ValueDirective is destroyed

			ValueDirective() = delete;
			ValueDirective(const ASTNode *size_, const ASTNode *expression_);
	};
}
