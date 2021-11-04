#pragma once

#include <string>
#include <set>

namespace Wasmc {
	class ASTNode;
	struct Section;

	class Expression: public ASTNode {
		private:
			static void findLabels(const ASTNode *, std::set<const std::string *> &);
			static size_t labelCount(const ASTNode *);
			static bool validate(const ASTNode *);
			static bool fullyNumeric(const ASTNode *);
			static bool isSymbol(const ASTNode *);

		public:
			Section *section = nullptr;
			size_t counter = -1;

			Expression() = delete;
			Expression(ASTNode *);

			std::set<const std::string *> findLabels() const;
			/** Counts the number of label references in the Expression and all its children. */
			size_t labelCount() const;
			/** Throws an exception if the number of label references is greater than the argument. */
			void checkLabelCount(size_t) const;
			/** Ensures that the expression fits the standard expression constraints. */
			bool validate() const;
	};
}
