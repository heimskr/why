#!/usr/bin/env node
let _ = require("lodash");

/**
 * Contains various utilities.
 * 
 * @module util
 */

const getID = (node) => node instanceof Node? node.id : node;

/**
 * Represents a directed graph datatype.
 */
class Graph {
	/**
	 * Creates a new graph.
	 * @param {number} n - The number of nodes in the graph.
	 */
	constructor(n) {
		/**
		 * An array of all the nodes in the graph.
		 * @type {Array<Node>}
		 * @name module:util~Graph#nodes
		 */

		this.reset(n);
	};

	/**
	 * Deletes all nodes in the graph.
	 * @param {number} n - The number of new empty nodes to replace the old nodes.
	 */
	reset(n) {
		this.nodes = _.range(0, n).map((i) => new Node(i, this));
	};

	/**
	 * Returns the nth node of the graph.
	 * @param {number} n - The ID of the node to return.
	 * @return {Node} The node corresponding to n if n is a number; n otherwise.
	 */
	getNode(n) {
		return typeof n == "number"? this.nodes[n] : n;
	};

	/**
	 * Adds a unidirectional connection from one node to another.
	 * @param {(Node|number)} source - The source node.
	 * @param {(Node|number)} destination - The destination node.
	 */
	arc(source, destination) {
		this.nodes[getID(source)].arc(destination);
	};

	/**
	 * Batch-adds arcs from an array of [source, ...destinations] sets.
	 * @param {...Array<Array<number, ...number>>} arcs - An array of arc sets to add.
	 */
	arcs(...sets) {
		sets.forEach(([source, ...destinations]) => destinations.forEach((destination) => this.arc(source, destination)));
	};

	/**
	 * Removes an edge from one node to another.
	 * @param {(Node|number)} source - The source node.
	 * @param {(Node|number)} destination - The destination node.
	 */
	removeArc(source, destination) {
		this.nodes[getID(source)].removeArc(destination);
	};

	/**
	 * Adds a bidirectional connection between two nodes.
	 * @param {(Node|number)} a - The first node.
	 * @param {(Node|number)} b - The second node.
	 */
	edge(a, b) {
		this.nodes[getID(a)].arc(b);
		this.nodes[getID(b)].arc(a);
	};

	/**
	 * Removes all connections between two nodes.
	 * @param {(Node|number)} a - The first node.
	 * @param {(Node|number)} b - The second node.
	 */
	disconnect(a, b) {
		this.nodes[getID(a)].removeArc(b);
		this.nodes[getID(b)].removeArc(a);
	};

	/**
	 * Runs a depth-first search on the graph.
	 * @return {module:util~DFSResult} The result of the search.
	 */
	dfs() {
		const parents    = _.range(0, this.nodes.length).map(() => null);
		const discovered = _.range(0, this.nodes.length).map(() => null);
		const finished   = _.range(0, this.nodes.length).map(() => null);
		let time = 0;

		const visit = (u) => {
			discovered[u] = ++time;
			this.nodes[u].out.forEach((v) => {
				if (discovered[v] == null) {
					parents[v] = u;
					visit(v);
				};
			});

			finished[u] = ++time;
		};

		_.range(0, this.nodes.length).forEach((u) => discovered[u] == null && visit(u));

		return { parents, discovered, finished };
	};

	/**
	 * Calculates and returns a list of this graph's connected components using Kosaraju's algorithm.
	 * @type {Array<Array<Node>>}
	 */
	get components() {
		const visited = _.fill(Array(this.nodes.length), false);
		const parents = _.fill(Array(this.nodes.length), null);
		const components = { }; 
		const l = [];

		const visit = (u) => {
			if (!visited[u]) {
				visited[u] = true;
				this.nodes[u].out.forEach(visit);
				l.unshift(u);
			};
		};

		const assign = (u, root) => {
			if (parents[u] == null) {
				parents[u] = root;
				if (!components[root]) {
					components[root] = [u];
				} else {
					components[root].push(u);
				};

				this.getNode(u).in.forEach((v) => assign(v, root));
			};
		};

		this.nodes.forEach((node, u) => visit(u));
		l.forEach((u) => assign(u, u));

		return Object.values(components).map((a) => a.map((u) => this.nodes[u]));
	};

	/**
	 * Calculates a topologically sorted list of nodes using Kahn's algorithm.
	 * @return {Array<Node>} A topologically sorted list of the graph's nodes.
	 * @throws Will throw an error if the graph is cyclic.
	 */
	sort() {
		const l = [], s = this.nodes.filter((node) => !node.in.length);
		if (!s.length) {
			throw new Error("Graph is cyclic.");
		};

		while (s.length) {
			let n = s.pop();
			l.unshift(n);
			
			this.nodes.filter((m) => m != n && m.connectsFrom(n)).forEach((m) => {
				m.removeArcFrom(n);
				
				if (!m.in.length) {
					s.unshift(m);
				};
			});
		};

		this.nodes.forEach((node) => {
			if (node.out.length) {
				throw new Error("Graph contains a cycle.");
			};
		});

		return l;
	};

	/**
	 * Removes all loop edges from the graph.
	 */
	removeLoops() {
		this.nodes.forEach((node) => this.disconnect(node, node));
	};

	/**
	 * Calculates and returns the transpose of the graph.
	 * @type {Graph}
	 */
	get transpose() {
		let graph = new Graph(this.nodes.length);
		this.nodes.forEach(({ out }, u) => out.forEach((v) => graph.arc(v, u)));

		return graph;
	};

	/**
	 * Returns a copy of this graph.
	 * @return {Graph} A copy of the graph.
	 */
	clone() {
		let newGraph = new Graph(this.nodes.length);
		newGraph.nodes = this.nodes.map((node) => node.clone(newGraph));
		return newGraph;
	};

	/**
	 * Returns a string containing each node's adjacency list.
	 * @return {string} A string representation of the graph.
	 */
	toString() {
		return this.nodes.map(({ out }, u) => `${u} => ${out.join(", ")}`).join("\n");
	};
};

/**
 * Represents a node in a graph.
 */
class Node {
	/**
	 * Creates a new node.
	 * @param {number} id - The node's ID.
	 * @param {Graph} graph - The graph containing this node.
	 */
	constructor(id, graph) {
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
	};

	/**
	 * Adds a node to this node's outward edge list and adds this node to the node's inward edge list.
	 * @param {(Node|number)} n - The node to add.
	 */
	arc(n) {
		n = getID(n);
		if (!this.out.includes(n)) {
			this.out.push(n);
		};

		n = this.graph.nodes[n];
		if (!n.in.includes(this.id)) {
			n.in.push(this.id);
		};
	};

	/**
	 * Adds a node to this node's inward edge list and adds this node to the node's outward edge list.
	 * @param {(Node|number)} n - The node to add.
	 */
	arcFrom(n) {
		n = getID(n);
		if (!this.in.includes(n)) {
			this.in.push(n);
		};

		n = this.graph.nodes[n];
		if (!n.out.includes(this.id)) {
			n.out.push(this.id);
		};
	};

	/**
	 * Removes an outward connection from this node and the other node's corresponding inward connection.
	 * @param {(Node|number)} n - The node whose arc will be removed.
	 */
	removeArc(n) {
		n = getID(n);
		this.out = this.out.filter((edge) => edge != n);
		this.graph.nodes[n].in = this.graph.nodes[n].in.filter((edge) => edge != this.id);
	};

	/**
	 * Removes an inward connection to this node and the other node's corresponding outward connection.
	 * @param {(Node|number)} n - The node whose arc will be removed.
	 */
	removeArcFrom(n) {
		n = getID(n);
		this.in = this.in.filter((edge) => edge != n);
		this.graph.nodes[n].out = this.graph.nodes[n].out.filter((edge) => edge != this.id);
	};

	/**
	 * Checks for the existence of a connection from this node to another.
	 * @param {(Node|number)} n - The node to check.
	 * @return {boolean} Whether there exists an connection from this node to the other.
	 */
	connectsTo(n) {
		return this.out.includes(getID(n));
	};

	/**
	 * Checks for the existence of a connection to this node from another.
	 * @param {(Node|number)} n - The node to check.
	 * @return {boolean} Whether there exists an connection to this node from the other.
	 */
	connectsFrom(n) {
		return this.in.includes(getID(n));
	};

	/**
	 * Checks for the existence of a bidirectional connection between this node and another.
	 * @param {(Node|number)} n - The node to check.
	 * @return {boolean} Whether there exists a bidirectional connection between this node and the other.
	 */
	connects(n) {
		return this.connectsTo(n) && this.connectsFrom(n);
	};

	/**
	 * Returns a copy of this node.
	 * @param {?Graph} newGraph - If non-null, this will be the cloned node's parent graph.
	 * @return {Node} A copy of the node.
	 */
	clone(newGraph=null) {
		let newNode = new Node(this.id, newGraph === null? this.graph : newGraph);
		newNode.out = this.out.slice(0);
		newNode.in = this.in.slice(0);
		return newNode;
	};
};

module.exports = Graph;

/**
 * @typedef {Object} DFSResult
 * @property {Array<number>} parents A list of each node's parent (null if nonexistent).
 * @property {Array<number>} discovered A list of the times each node was discovered.
 * @property {Array<number>} finished A list of the times each node was finished.
 */

if (require.main === module) {
	let chalk = require("chalk"),
		minimist = require("minimist");

	let g, choice = minimist(process.argv.slice(2), { })._[0] || "in0";

	if (choice == "in0") {
		g = new Graph(8);
		g.arcs([0, 1], [1, 2, 5], [2, 3, 6], [3, 2, 7], [4, 0, 5], [5, 6], [6, 5, 7], [7, 7]);
	} else if (choice == "test.ll") {
		g = new Graph(5);
		g.arcs([2, 2, 3], [4, 3, 4]);
		g.removeLoops();
	} else if (choice == "table.ll") {
		g = new Graph(32);
		g.arcs([2, 3], [3, 3, 4], [4, 4], [5, 6], [6, 6, 7], [8, 9], [9, 9, 10], [10, 10], [11, 12], [12, 12, 13],
			   [13, 13], [14, 15], [15, 15, 16], [16, 16], [17, 18], [18, 18, 19], [19, 19], [20, 21], [21, 21, 22],
			   [23, 24], [24, 24, 25], [25, 25], [26, 27], [27, 27, 28], [28, 28], [29, 30], [30, 30, 31], [31, 31]);
		g.removeLoops();
	} else {
		console.error(`${chalk.bold(`"${choice}"`)} isn't a recognized sample graph.`);
		process.exit(1);
	};

	console.log(`${chalk.bold("G:")}`);
	console.log(g.toString());
	console.log(`\n${chalk.bold("Transpose(G):")}`);
	console.log(g.transpose.toString());
	console.log(`\n${chalk.bold("Components(G):")}`);
	console.log(g.components);
	console.log(`\n${chalk.bold("DFS(G):")}`);
	console.log(g.dfs());
	console.log(`\n${chalk.bold("Sort(G):")}`);
	try {
		console.log(g.sort());
	} catch(e) {
		if (e.message.match(/cycl/)) {
			console.log("(graph is cyclic)");
		} else {
			throw e;
		};
	};
};
