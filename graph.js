let _ = require("lodash");

/**
 * Contains various utilities.
 * 
 * @module util
 */

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
		 * @type {Array.<Node>}
		 */
		this.nodes = [];

		this.reset(n);
	};

	/**
	 * Deletes all nodes in the graph.
	 * @param {number} n - The number of new empty nodes to replace the old nodes.
	 */
	reset(n) {
		this.nodes = _.range(0, n).map((i) => new Node(i));
	};

	/**
	 * Adds a unidirectional connection from one node to another.
	 * @param {number} a - The ID of the source node.
	 * @param {number} b - The ID of the destination node.
	 */
	arc(a, b) {
		this.nodes[a].arc(b);
	};

	/**
	 * Adds a bidirectional connection between two nodes.
	 * @param {number} a - The ID of the first node.
	 * @param {number} b - The ID of the second node.
	 */
	edge(a, b) {
		this.nodes[a].arc(b);
		this.nodes[b].arc(a);
	};

	/**
	 * Removes all connections between two nodes.
	 * @param {number} a - The ID of the first node.
	 * @param {number} b - The ID of the second node.
	 */
	disconnect(a, b) {
		this.nodes[a].disconnect(b);
		this.nodes[b].disconnect(a);
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
			this.nodes[u].adjacent.forEach((v) => {
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
	 * @type {Array.<Array.<Node>>}
	 */
	get components() {
		const visited = _.fill(Array(this.nodes.length), false);
		const parents = _.fill(Array(this.nodes.length), null);
		const components = { }; 
		const t = this.transpose;
		const l = [];

		const visit = (u) => {
			if (!visited[u]) {
				visited[u] = true;
				this.nodes[u].adjacent.forEach(visit);
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

				t.nodes[u].adjacent.forEach((v) => assign(v, root));
			};
		};

		this.nodes.forEach((node, u) => visit(u));
		l.forEach((u) => assign(u, u));

		return Object.values(components).map((a) => a.map((u) => this.nodes[u]));
	};

	/**
	 * Calculates and returns the transpose of the graph.
	 * @type {Graph}
	 */
	get transpose() {
		let graph = new Graph(this.nodes.length);
		this.nodes.forEach(({ adjacent }, u) => {
			adjacent.forEach((v) => {
				graph.arc(v, u);
			});
		});

		return graph;
	};

	/**
	 * Returns a copy of this graph.
	 * @return {Graph} A copy of the graph.
	 */
	clone() {
		let newGraph = new Graph(this.nodes.length);
		newGraph.nodes = this.nodes.map((node) => node.clone());
		return newGraph;
	};

	/**
	 * Returns a string containing each node's adjacency list.
	 * @return {string} A string representation of the graph.
	 */
	toString() {
		return this.nodes.map(({ adjacent }, u) => `${u} => ${adjacent.join(", ")}`).join("\n");
	};
};

/**
 * Represents a node in a graph.
 * @class
 */
class Node {
	/**
	 * Creates a new node.
	 * @param {number} id - The node's ID.
	 */
	constructor(id) {
		/**
		 * The node's ID.
		 * @type {number}
		 */
		this.id = id;

		/**
		 * A list of the IDs of all nodes that this node connects to.
		 * @type {Array.<number>}
		 */
		this.adjacent = [];
	};

	/**
	 * Adds a node ID to this node's adjacency list.
	 * @param {number} i - The ID of the node to add.
	 */
	arc(i) {
		if (!this.adjacent.includes(i)) {
			this.adjacent.push(i);
		};
	};

	/**
	 * Removes a node ID from this node's adjacency list.
	 * @param {number} i - The ID of the node to remove.
	 */
	disconnect(i) {
		this.adjacent = this.adjacent.filter((j) => i != j);
	};

	/**
	 * Returns a copy of this node.
	 * @return {Node} A copy of the node.
	 */
	clone() {
		let newNode = new Node(this.id);
		newNode.adjacent = this.adjacent.slice(0);
		return newNode;
	};
};

module.exports = Graph;

/**
 * @typedef {Object} DFSResult
 * @property {Array.<number>} parents A list of each node's parent (null if nonexistent).
 * @property {Array.<number>} discovered A list of the times each node was discovered.
 * @property {Array.<number>} finished A list of the times each node was finished.
 */

if (require.main === module) {
	const chalk = require("chalk");
	const g = new Graph(8);
	g.arc(0, 1);
	g.arc(1, 2);
	g.arc(1, 4);
	g.arc(1, 5);
	g.arc(2, 3);
	g.arc(2, 6);
	g.arc(3, 2);
	g.arc(3, 7);
	g.arc(4, 0);
	g.arc(4, 5);
	g.arc(5, 6);
	g.arc(6, 5);
	g.arc(6, 7);
	g.arc(7, 7);
	console.log(`${chalk.bold("G:")}`);
	console.log(g.toString());
	console.log(`\n${chalk.bold("Transpose(G):")}`);
	console.log(g.transpose.toString());
	console.log(`\n${chalk.bold("Components(G):")}`);
	console.log(g.components);
	console.log(`\n${chalk.bold("DFS(G):")}`);
	console.log(g.dfs());
};