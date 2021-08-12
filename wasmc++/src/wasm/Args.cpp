#include "parser/ASTNode.h"
#include "wasm/Args.h"

namespace Wasmc {
	Args::Args(ASTNode *node) {
		if (node) {
			delete node;
		}
	}
}
