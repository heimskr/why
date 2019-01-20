#!/usr/bin/env node
let _ = require("lodash");

/**
 * Contains various utilities.
 * 
 * @module util
 */

const getID = node => node instanceof Node? node.id : node;

/**
 * Represents a directed graph datatype.
 */
class Graph {
	/**
	 * Creates a new graph.
	 * @param {number} n The number of nodes in the graph.
	 */
	constructor(n) {
		/**
		 * An array of all the nodes in the graph.
		 * @type {Array<Node>}
		 * @name module:util~Graph#nodes
		 */

		this.reset(n);

		return new Proxy(this, {
			get(target, prop) {
				if (Number(prop) == prop) {
					return target.getNode(prop);
				}

				return target[prop];
			},

			set(target, prop, val) {
				if (Number(prop) != prop) {
					return Reflect.set(...arguments);
				}
				
				const i = _.findIndex(target.nodes, node => node.id == prop);
				if (i == -1) {
					return Reflect.set(...arguments);
				}

				target.nodes[i] = val;
				return true;
			}
		});
	}

	/**
	 * Deletes all nodes in the graph.
	 * @param {number} n The number of new empty nodes to replace the old nodes.
	 */
	reset(n) {
		this.nodes = _.range(0, n).map(i => new Node(i, this));
	}

	/**
	 * Returns the nth node of the graph.
	 * @param  {number} n The ID of the node to return.
	 * @return {Node}     The node corresponding to n if n is a number; n otherwise.
	 */
	getNode(n) {
		n = Number(n) == n? n : n.id;
		return _.find(this.nodes, node => node.id == n);
	}

	/**
	 * Adds a unidirectional connection from one node to another.
	 * @param {(Node|number)} source      The source node.
	 * @param {(Node|number)} destination The destination node.
	 */
	arc(source, destination) {
		this.nodes[getID(source)].arc(destination);
	}

	/**
	 * Batch-adds arcs from an array of [source, ...destinations] sets.
	 * @param {...Array<Array<number, ...number>>} arcs - An array of arc sets to add.
	 */
	arcs(...sets) {
		sets.forEach(([src, ...dests]) => dests.forEach(dest => this.arc(src, dest)));
	}

	/**
	 * Removes an edge from one node to another.
	 * @param {(Node|number)} source      The source node.
	 * @param {(Node|number)} destination The destination node.
	 */
	removeArc(source, destination) {
		this.nodes[getID(source)].removeArc(destination);
	}

	/**
	 * Adds a bidirectional connection between two nodes.
	 * @param {(Node|number)} a The first node.
	 * @param {(Node|number)} b The second node.
	 */
	edge(a, b) {
		this.nodes[getID(a)].arc(b);
		this.nodes[getID(b)].arc(a);
	}

	/**
	 * Removes all connections between two nodes.
	 * @param {(Node|number)} a The first node.
	 * @param {(Node|number)} b The second node.
	 */
	disconnect(a, b) {
		this.nodes[getID(a)].removeArc(b);
		this.nodes[getID(b)].removeArc(a);
	}

	/**
	 * Runs a depth-first search on the graph.
	 * @return {module:util~DFSResult} The result of the search.
	 */
	dfs() {
		const parents    = _.fill(Array(this.nodes.length), null);
		const discovered = _.fill(Array(this.nodes.length), null);
		const finished   = _.fill(Array(this.nodes.length), null);
		let time = 0;

		const visit = u => {
			discovered[u] = ++time;
			this.nodes[u].out.forEach(v => {
				if (discovered[v] == null) {
					parents[v] = u;
					visit(v);
				}
			});

			finished[u] = ++time;
		};

		_.range(0, this.nodes.length).forEach(u => discovered[u] == null && visit(u));

		return {parents, discovered, finished};
	}

	/**
	 * Calculates and returns a list of this graph's connected components using Kosaraju's algorithm.
	 * @type {Array<Array<Node>>}
	 */
	get components() {
		const visited = _.fill(Array(this.nodes.length), false);
		const parents = _.fill(Array(this.nodes.length), null);
		const components = {}; 
		const l = [];

		const visit = u => {
			if (!visited[u]) {
				visited[u] = true;
				this.nodes[u].out.forEach(visit);
				l.unshift(u);
			}
		};

		const assign = (u, root) => {
			if (parents[u] == null) {
				parents[u] = root;
				if (!components[root]) {
					components[root] = [u];
				} else {
					components[root].push(u);
				}

				this.getNode(u).in.forEach(v => assign(v, root));
			}
		};

		this.nodes.forEach((node, u) => visit(u));
		l.forEach(u => assign(u, u));

		return Object.values(components).map(a => a.map(u => this.nodes[u]));
	}

	/**
	 * Calculates a topologically sorted list of nodes using Kahn's algorithm.
	 * @return {Array<Node>} A topologically sorted list of the graph's nodes.
	 * @throws Will throw an error if the graph is cyclic.
	 */
	sorted() {
		let copy = this.clone();
		const l = [], s = copy.nodes.filter(node => !node.in.length);
		if (1 < this.nodes.length && !s.length) {
			// If there are multiple nodes in the graph and none of them lack in-edges, the graph has to be cyclic.
			// The converse isn't necessarily true, so this is just an preliminary check.
			throw new Error("Graph is cyclic.");
		}

		while (s.length) {
			let n = s.pop();
			l.unshift(n);
			
			copy.nodes.filter(m => m != n && m.connectsFrom(n)).forEach(m => {
				m.removeArcFrom(n);
				
				if (!m.in.length) {
					s.unshift(m);
				}
			});
		}

		copy.nodes.forEach(node => {
			if (node.out.length) {
				throw new Error("Graph contains a cycle.");
			}
		});

		return l.map(node => this.nodes[node.id]);
	}

	/**
	 * Removes all loop edges from the graph.
	 */
	removeLoops() {
		this.nodes.forEach(node => this.disconnect(node, node));
	}

	/**
	 * Condenses a list of nodes into a single node, removes the old nodes from the graph and inserts the new node.
	 * The new node's in/out arrays are the unions of the given nodes' in/out arrays.
	 * The new node is reflexive if any of the given nodes is reflexive.
	 * @param  {Array<Node | number>} nodes A list of nodes.
	 * @return {Node} The coalesced node.
	 */
	coalesce(nodes) {
		if (!nodes.length) {
			return undefined;
		}

		// The input may contain either Nodes or numeric IDs. Convert all the IDs to nodes.
		nodes = nodes.map(n => this.nodes[getID(n)]);

		// Calculate the union of all in/out edges, but don't include edges between any of the given nodes.
		const combinedIn  = _.without(_.union(...nodes.map(node => node.in)),  ...nodes);
		const combinedOut = _.without(_.union(...nodes.map(node => node.out)), ...nodes);

		const reflexive = _.some(nodes, n => n.isReflexive);

		const allIDs = nodes.map(node => node.id);
		const newID = nodes[0].id;
		const oldIDs = _.without(allIDs, newID);
		let newNode = this.getNode(newID);
		
		// Remove all the old nodes from the graph.
		_.remove(this.nodes, v => allIDs.includes(getID(v)));

		// Go through every remaining node's in/out arrays, remove the old node IDs
		// and insert the new ID where applicable.
		for (const node of this.nodes) {
			if (_.intersection(node.in,   allIDs).length)
				node.in  = _.sortBy(_.without(node.in,  ...allIDs).concat(newID), getID);
			if (_.intersection(node.out,  allIDs).length)
				node.out = _.sortBy(_.without(node.out, ...allIDs).concat(newID), getID);
		}

		if (!newNode) {
			newNode = new Node(newID, this);
		}

		newNode.in  = _.without(combinedIn,  ...allIDs);
		newNode.out = _.without(combinedOut, ...allIDs);
		
		if (reflexive) {
			newNode.in.push(newID);
			newNode.out.push(newID);
		}

		newNode.in  = _.sortBy(newNode.in,  getID);
		newNode.out = _.sortBy(newNode.out, getID);
		
		this.nodes.push(newNode);
		return newNode;
	}

	/**
	 * Calculates and returns the transpose of the graph.
	 * @type {Graph}
	 */
	get transpose() {
		let graph = new Graph(this.nodes.length);
		this.nodes.forEach(({out}, u) => out.forEach(v => graph.arc(v, u)));

		return graph;
	}

	/**
	 * Returns a copy of this graph.
	 * @return {Graph} A copy of the graph.
	 */
	clone() {
		let newGraph = new Graph(this.nodes.length);
		newGraph.nodes = this.nodes.map(node => node.clone(newGraph));
		return newGraph;
	}

	/**
	 * Returns a string containing each node's adjacency list.
	 * @return {string} A string representation of the graph.
	 */
	toString() {
		return _.sortBy(this.nodes, "id").map(({out, id}) => `${id} => ${out.join(", ")}`).join("\n");
	}
}

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

	/**
	 * Adds a node to this node's outward edge list and adds this node to the node's inward edge list.
	 * @param {(Node|number)} n The node to add.
	 */
	arc(n) {
		n = getID(n);
		if (!this.out.includes(n)) {
			this.out.push(n);
		}

		const node = this.graph.nodes[n];
		if (!node.in.includes(this.id)) {
			node.in.push(this.id);
		}
	}

	/**
	 * Adds a node to this node's inward edge list and adds this node to the node's outward edge list.
	 * @param {(Node|number)} n The node to add.
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
	}

	/**
	 * Removes an outward connection from this node and the other node's corresponding inward connection.
	 * @param {(Node|number)} n The node whose arc will be removed.
	 */
	removeArc(n) {
		n = getID(n);
		this.out = this.out.filter(edge => edge != n);
		this.graph.nodes[n].in = this.graph.nodes[n].in.filter(edge => edge != this.id);
	}

	/**
	 * Removes an inward connection to this node and the other node's corresponding outward connection.
	 * @param {(Node|number)} n The node whose arc will be removed.
	 */
	removeArcFrom(n) {
		n = getID(n);
		this.in = this.in.filter(edge => edge != n);
		this.graph.nodes[n].out = this.graph.nodes[n].out.filter(edge => edge != this.id);
	}

	/**
	 * Checks for the existence of a connection from this node to another.
	 * @param  {(Node|number)} n The node to check.
	 * @return {boolean}         Whether there exists an connection from this node to the other.
	 */
	connectsTo(n) {
		return this.out.includes(getID(n));
	}

	/**
	 * Checks for the existence of a connection to this node from another.
	 * @param  {(Node|number)} n The node to check.
	 * @return {boolean}         Whether there exists an connection to this node from the other.
	 */
	connectsFrom(n) {
		return this.in.includes(getID(n));
	}

	/**
	 * Checks for the existence of a bidirectional connection between this node and another.
	 * @param  {(Node|number)} n The node to check.
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

	get isReflexive() {
		// If one of these conditions is true, the other should also be true...
		return this.out.includes(this.id) || this.in.includes(this.id);
	}
}

module.exports = Graph;
module.exports.Node = Node;

/**
 * @typedef {Object} DFSResult
 * @property {Array<number>} parents    A list of each node's parent (null if nonexistent).
 * @property {Array<number>} discovered A list of the times each node was discovered.
 * @property {Array<number>} finished   A list of the times each node was finished.
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
	}

	console.log(`${chalk.bold("G:")}`);
	console.log(g.toString());
	console.log(`\n${chalk.bold("Transpose(G):")}`);
	console.log(g.transpose.toString());
	console.log(`\n${chalk.bold("Components(G):")}`);
	console.log(g.components);
	console.log(`\n${chalk.bold("DFS(G):")}`);
	console.log(g.dfs());
	console.log(`\n${chalk.bold("Sorted(G):")}`);
	try {
		console.log(g.sorted());
	} catch(e) {
		if (e.message.match(/cycl/)) {
			console.log("(graph is cyclic)");
		} else {
			throw e;
		}
	}

	console.log(`\n${chalk.bold("Coalese(G, G[0:2]):")}`);
	console.log("Old node IDs:", g.nodes.map(getID));
	console.log("Coalesced node ID:", chalk.bold((g.coalesce(g.nodes.slice(0, 3)) || {id: "?"}).id));
	console.log("New node IDs:", g.nodes.map(getID));
	console.log(`\n${chalk.bold("G:")}`);
	console.log(g.toString());
}
