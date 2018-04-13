let Graph = require("../graph.js"), { Node } = Graph;
/**
 * @module llvm/instruction_node
 */

/**
 * Represents an instruction in the DAG.
 */
class InstructionNode extends Node {
	constructor(id, graph, opcode, results=[], operands=[]) {
		super(id, graph, null);

		/**
		 * Contains the instruction's opcode.
		 * @name module:llvm/instruction_node~InstructionNode#opcode
		 */
		this.opcode = opcode;

		/**
		 * Contains information about where the data returned by the instruction should go.
		 * @name module:llvm/instruction_node~InstructionNode#results
		 */
		this.results = results;

		/**
		 * Contains information about the instruction's operands.
		 * @name module:llvm/instruction_node~InstructionNode#operands
		 */
		this.operands = operands;
	}
}

if (require.main === module) {
	
}
