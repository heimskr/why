#pragma once

#include <string>
#include <set>

namespace Wasmc {
	class ASTNode;

	class Expression {
		private:
			static void findLabels(const ASTNode *, std::set<const std::string *> &);

		public:
			Expression() = delete;
			static std::set<const std::string *> findLabels(const ASTNode *);
	};
}
