const {alpha, numerize} = require("./util.js");

const getID = node => {
	if (node instanceof Node) {
		return node.id;
	} else {
		return numerize(node);
	}
};

/**
 * Represents a node in a graph.
 */
class Node {
	/**
	 * Creates a new node.
	 * @param {number} id    The node's ID.
	 * @param {Graph}  graph The graph containing this node.
	 * @param {*}      data  The data attached to the node.
	 */
	constructor(id, graph, data=null) {
		/**
		 * The node's ID.
		 * @type {number}
		 * @name module:util~Node#id
		 */
		this.id = id;

		/**
		 * The node's parent graph.
		 * @type {Graph}
		 * @name module:util~Node#graph
		 */
		this.graph = graph;

		/**
		 * An array of the IDs of all nodes that this node connects to.
		 * @type {Array<number>}
		 * @name module:util~Node#out
		 */
		this.out = [];

		/**
		 * An array of the IDs of all nodes that connect to this node.
		 * @type {Array<number>}
		 * @name module:util~Node#in
		 */
		this.in = [];

		/**
		 * The data attached to the node.
		 * @type {*}
		 * @name module:util~Node#data
		 */
		this.data = data;
	}

	get succ() { return this.out }
	get pred() { return this.in  }
	set succ(to) { return this.out = to }
	set pred(to) { return this.in = to }

	/**
	 * Adds a node to this node's outward edge list and adds this node to the node's inward edge list.
	 * @param {(Node|number|string)} n The node to add.
	 */
	arc(n) {
		n = numerize(getID(n));

		if (!this.out.includes(n)) {
			this.out.push(n);
		}

		const node = this.graph.getNode(n);
		if (!node.in.includes(this.id)) {
			node.in.push(this.id);
		}

		return this;
	}

	/**
	 * Adds a node to this node's inward edge list and adds this node to the node's outward edge list.
	 * @param {(Node|number|string)} n The node to add.
	 */
	arcFrom(n) {
		n = getID(n);
		if (!this.in.includes(n)) {
			this.in.push(n);
		}

		const node = this.graph.nodes[n];
		if (!node.out.includes(this.id)) {
			node.out.push(this.id);
		}

		return this;
	}

	/**
	 * Removes an outward connection from this node and the other node's corresponding inward connection.
	 * @param {(Node|number|string)} n The node whose arc will be removed.
	 */
	removeArc(n) {
		n = getID(n);
		this.out = this.out.filter(edge => edge != n);
		this.graph.nodes[n].in = this.graph.nodes[n].in.filter(edge => edge != this.id);
		return this;
	}

	/**
	 * Removes an inward connection to this node and the other node's corresponding outward connection.
	 * @param {(Node|number|string)} n The node whose arc will be removed.
	 */
	removeArcFrom(n) {
		n = getID(n);
		this.in = this.in.filter(edge => edge != n);
		this.graph.nodes[n].out = this.graph.nodes[n].out.filter(edge => edge != this.id);
		return this;
	}

	/**
	 * Checks for the existence of a connection from this node to another.
	 * @param  {(Node|number|string)} n The node to check.
	 * @return {boolean}         Whether there exists an connection from this node to the other.
	 */
	connectsTo(n) {
		return this.out.includes(getID(n));
	}

	/**
	 * Checks for the existence of a connection to this node from another.
	 * @param  {(Node|number|string)} n The node to check.
	 * @return {boolean}         Whether there exists an connection to this node from the other.
	 */
	connectsFrom(n) {
		return this.in.includes(getID(n));
	}

	/**
	 * Checks for the existence of a bidirectional connection between this node and another.
	 * @param  {(Node|*)} n The node to check.
	 * @return {boolean}         Whether there exists a bidirectional connection between this node and the other.
	 */
	connects(n) {
		return this.connectsTo(n) && this.connectsFrom(n);
	}

	/**
	 * Returns a copy of this node.
	 * @param  {?Graph} newGraph If non-null, this will be the cloned node's parent graph.
	 * @return {Node}            A copy of the node.
	 */
	clone(newGraph=null) {
		let newNode = new Node(this.id, newGraph === null? this.graph : newGraph);
		newNode.out = this.out.slice(0);
		newNode.in = this.in.slice(0);
		return newNode;
	}

	/**
	 * Assigns the node a new ID and updates the in/out arrays of all other nodes in the graph accordingly.
	 * @param {number|string} newID The new ID for this node.
	 */
	rename(newID) {
		const oldID = this.id;
		const replace = (o, ...ps) => ps.forEach(p => o[p] = o[p].map(x => x == oldID? newID : x));
		this.graph.nodes.forEach(n => replace(n, "out", "in"));
		this.id = newID;
	}

	get isReflexive() {
		// If one of these conditions is true, the other should also be true...
		return this.out.includes(this.id) || this.in.includes(this.id);
	}
}

module.exports = Node;
module.exports.getID = getID;
