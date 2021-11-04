#pragma once

#include <string>
#include <set>

namespace Wasmc {
	class ASTNode;
	struct Section;

	class Expression: public ASTNode {
		private:
			static void findLabels(const ASTNode *, std::set<const std::string *> &);

		public:
			Section *section = nullptr;
			size_t counter = -1;

			Expression() = delete;
			Expression(ASTNode *);

			std::set<const std::string *> findLabels();
	};
}
