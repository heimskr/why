#!/usr/bin/env node
let _ = require("lodash");

const chalk = require("chalk");
const {lt} = require("dominators");

const Node = require("./node.js");
const {alpha, numerize} = require("./util.js");

const ts = x => typeof x == "number"? x+1 : x;

/**
 * Contains various utilities.
 * 
 * @module util
 */

const getID = Node.getID;

/**
 * @typedef {number|string} NodeID
 */

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

		// This is where we exploit the black magic of proxies to enable some syntactic sugar.
		return new Proxy(this.reset(n), {
			// Symbols are always directly mapped to the graph.
			// All numeric properties are mapped to the node with a matching ID, or `undefined` if none exists.
			// All functions in Array.prototype that aren't already defined in `Graph` return the functions bound
			// to the graph's nodes. Anything else is mapped to the graph.
			get(target, prop) {
				if (typeof prop == "symbol") {
					return target[prop];
				}

				if (Number(prop) == prop) {
					return target.getNode(prop);
				}

				if (prop in Array.prototype && !(prop in target)) {
					if (typeof Array.prototype[prop] == "function") {
						return target.nodes[prop].bind(target.nodes);
					}
					
					return target.nodes[prop];
				}

				return target[prop];
			},

			// All sets for non-numberic properties are not interfered with.
			// Sets for numeric properties are passed to this.nodes instead,
			// unless the corresponding node doesn't exist.
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
	 * @param {number|Array} n The number of new empty nodes to replace the old nodes
	 *                         or an array of IDs to make new nodes with.
	 */
	reset(n) {
		if (n == undefined) {
			this.nodes = [];
		} else if (typeof n == "number") {
			this.nodes = _.range(0, n).map(i => new Node(i, this));
		} else {
			this.nodes = _.range(0, n.length).map(i => new Node(n[i], this));
		}

		return this;
	}

	/**
	 * Generates an ID not already assigned to any nodes in the graph.
	 * @return {number} An available numeric ID.
	 */
	newID() {
		let id = this.length;
		if (!this.getNode(id)) {
			// There isn't already a node whose ID is equal to the number of nodes, which makes things easy.
			return id;
		}
		
		// Otherwise, we start from 0 and keep incrementing until we find an ID that isn't in use.
		id = 0;
		for (id = 0; this.getNode(id); ++id);
		return id;
	}

	/**
	 * Adds a new node with a given piece of data.
	 * @param  {*} data The node's data.
	 * @return {Node} The new node.
	 */
	add(data) {
		const newNode = new Node(this.newID(), this, data);
		this.push(newNode);
		return newNode;
	}

	/**
	 * Returns the node with a given ID.
	 * @param  {Node|NodeID} n The ID of the node to return.
	 * @return {Node} The node corresponding to n if n is a number; n otherwise.
	 */
	getNode(n) {
		if (n instanceof Node) {
			return n;
		}

		if (n == undefined) {
			throw new Error("Graph.getID() called with undefined");
		}

		n = numerize(n);
		return _.find(this.nodes, node => numerize(node.id) == n);
	}

	/**
	 * Adds a unidirectional connection from one node to another.
	 * @param  {Node|NodeID} source      The source node.
	 * @param  {Node|NodeID} destination The destination node.
	 * @return {boolean} Whether the connection already existed.
	 */
	arc(source, destination) {
		return this.getNode(source).arc(destination);
	}

	/**
	 * Convenience method to batch-link nodes.
	 * @param  {string} str A space-separated string of letter pairs, e.g. "AB DE AF".
	 *                      "A" corresponds to 0, "B" to 1, "C" to 2 and so on.
	 *                      Can also be a space-separated string of pairs of numbers joined by hyphens,
	 *                      e.g. "0-1 1-2 1-3 2-3". If a pair is simply a string of two digits, there's
	 *                      an implicit hyphen between the two digits.
	 * @return {Graph} The same graph the method was called on.
	 */
	arcString(str) {
		let pairs = str.split(/\s+/);
		if (str.match(/\d/)) {
			pairs.forEach(pair =>  {
				if (pair.match(/^\d{2}$/)) {
					this.arc(parseInt(pair[0]), parseInt(pair[1]));
				} else {
					this.arc(...pair.split("-").map(s => parseInt(s)));
				}
			});
		} else {
			pairs.forEach(s => this.arc(...s.split("").map(c => alpha.indexOf(c.toLowerCase()))));
		}
		return this;
	}

	/**
	 * Batch-adds arcs from an array of [source, ...destinations] sets.
	 * @param  {...Array<Array<number, ...number>>} arcs An array of arc sets to add.
	 * @return {Graph} The same graph the method was called on.
	 */
	arcs(...sets) {
		sets.forEach(([src, ...dests]) => dests.forEach(dest => this.arc(src, dest)));
		return this;
	}

	/**
	 * Removes an edge from one node to another.
	 * @param  {Node|NodeID} source      The source node.
	 * @param  {Node|NodeID} destination The destination node.
	 * @return {Graph} The same graph the method was called on.
	 */
	removeArc(source, destination) {
		this.nodes[getID(source)].removeArc(destination);
		return this;
	}

	/**
	 * Adds a bidirectional connection between two nodes.
	 * @param  {Node|NodeID} a The first node.
	 * @param  {Node|NodeID} b The second node.
	 * @return {Graph} The same graph the method was called on.
	 */
	edge(a, b) {
		this.getNode(a).arc(b);
		this.getNode(b).arc(a);
		return this;
	}

	/**
	 * Removes all connections between two nodes.
	 * @param  {Node|NodeID} a The first node.
	 * @param  {Node|NodeID} b The second node.
	 * @return {Graph} The same graph the method was called on.
	 */
	disconnect(a, b) {
		this.nodes[getID(a)].removeArc(b);
		this.nodes[getID(b)].removeArc(a);
		return this;
	}

	/**
	 * Attempts to find a single node matching a predicate.
	 * @param  {Function} predicate A function mapping a node to a boolean.
	 * @return {Node} The result of the search.
	 * @throws Throws an exception if the number of nodes matching the predicate isn't exactly 1.
	 */
	findSingle(predicate) {
		const found = this.nodes.filter(predicate);
		if (found.length != 1) {
			throw `Predicate matched ${found.length} results`;
		}

		return found[0];
	}

	/**
	 * Returns an array whose length is equal to the number of nodes in this graph
	 * and fills it with a predefined value.
	 * @param  {*} [value=null] A value to fill the array with.
	 * @return {Array} The filled array.
	 */
	fill(value=null) {
		return _.fill(Array(this.length), value);
	}

	/**
	 * Returns an object whose keys are the IDs of each node in the graph
	 * and whose values are all equal to the given value.
	 * @param  {*} value A value to use as the value of each entry in the object.
	 * @return {Object} The filled object.
	 */
	fillObj(value=null) {
		const out = {};
		this.forEach(v => out[v.id] = _.cloneDeep(value));
		return out;
	}

	/**
	 * Returns an object whose keys are the IDs of each node in the graph and whose values
	 * are the return value of the given function when called with a Node object.
	 * @param  {Function} fn A function that takes a Node and returns any value.
	 * @return {Object} The mapped object.
	 */
	mapValues(fn) {
		const out = {};
		this.forEach((v, i) => out[v.id] = fn(v, i));
		return out;
	}

	/**
	 * Renames all nodes in the graph such that they have numeric IDs in the range [offset, n + offset).
	 * @param  {number} [offset=0] The starting point of the range.
	 * @return {Object<NodeID, number>} A map of old IDs to new IDs.
	 */
	normalize(offset=0) {
		const renameMap = this.mapValues((v, i) => i + offset);
		const oldNodes = Object.values(this.nodes);

		this.nodes = [];
		oldNodes.forEach(v => {
			["in", "out"].forEach(p => v[p] = v[p].map(i => renameMap[i]))
			v.id = renameMap[v.id];
			this.nodes.push(v);
		});

		return renameMap;
	}

	/**
	 * Calculates the dominator tree of the graph for a given start node.
	 * @param  {Node|NodeID} [startID=0] The ID of the start node.
	 * @param  {boolean} [bidirectional=false] Whether the D-edges should be bidirectional.
	 * @return {Graph} A tree in which each node other than the start node is linked to by its immediate dominator.
	 */
	dTree(startID=0, bidirectional=false) {
		const lentar = this.lengauerTarjan(startID);
		const out = new Graph(Object.keys(lentar).map(numerize));
		const fn = (bidirectional? out.edge : out.arc).bind(out);
		Object.entries(lentar).forEach(([k, v]) => fn(v == undefined? k : v, k));
		return out;
	}

	/**
	 * Extracts the strict dominators of a D-tree.
	 * @param  {Graph} dt A D-tree.
	 * @return {Object<NodeID, NodeID[]>}
	 *         An object mapping a node ID to an array of the IDs of its strict dominators.
	 */
	static strictDominators(dt) {
		const out = {};
		for (const node of dt.nodes) {
			let parent = dt[node.in[0]];
			out[node.id] = [];
			while (parent) {
				out[node.id].push(parent.id);
				if (parent.in[0] == parent.id) {
					break;
				}

				parent = dt[parent.in[0]];
			}
		}

		return out;
	}

	/**
	 * Returns an array of all edge pairs in the graph.
	 * @return {Array<Array<number, number>>} An array of edge pairs.
	 */
	allEdges() {
		return this.reduce((a, {id: src, out}) =>
			[...a, ...out.map(dst => [src, dst])],
		[]);
	}

	/**
	 * Computes the DJ-graph of the graph for a given start node.
	 * @param  {Node|NodeID|Graph} [start=0] The ID of the start node, or a precomputed D-tree.
	 * @param  {boolean} [bidirectional=false] Whether D-edges should be bidirectional.
	 * @return {Graph} A DJ-graph.
	 */
	djGraph(start=0, bidirectional=false) {
		const dt = start instanceof Graph? start.clone() : this.dTree(start, bidirectional);
		const sdom = Graph.strictDominators(dt);
		dt.jEdges = [];
		this.allEdges()
			.filter(([src, dst]) => !sdom[dst].includes(src))
			.forEach(p => (dt.arc(...p), dt.jEdges.push(p)));
		return dt;
	}

	/**
	 * Finds the dominators of each node given a start node using the Lengauer-Tarjan algorithm.
	 * This is a wrapper for the `lt` function from the `dominators` package by Julian Jensen.
	 * @param  {Node|NodeID} [startID=0] The ID of the start node.
	 * @return {Object<NodeID, NodeID>} A map of node IDs to the IDs of their dominators.
	 */
	lengauerTarjan(startID=0) {
		const normalized = this.clone();
		const renames = _.mapValues(_.invert(normalized.normalize()), v => numerize(v));
		const formatted = normalized.map(v => v.out);
		return lt(formatted, this.getNode(startID).id).reduce((a, b, i) => ({...a, [renames[i]]: renames[b]}), {});
	}

	static mergeSets(djTree, startID=0, exitID=1) {
		// "A Practical and Fast Iterative Algorithm for φ-Function Computation Using DJ Graphs"
		// Das and Ramakrishna (2005)
		// Top Down Merge Set Computation (TDMSC-I)

		let t; let T = s => console.time(t = s); let E = s => { console.timeEnd(t); if (s) T(s) };
		const bfs = djTree.bfs(startID);
		const {jEdges} = djTree;
		const visited = djTree.fillObj({}); // out node ID => in node ID
		const merge = djTree.fillObj([]); // node ID => IDs in merge set

		// console.log("\nD-Tree:");
		// console.log(dTree.toString(ts));
		// console.log("\nDJ-Tree:");
		// console.log(djTree.toString(ts));
		console.log("\nJ-edges:");
		console.log(jEdges);
		console.log("\nBFS:", bfs.map(n => n.id).join(" ") + "\n");


		const parent = node => djTree.getNode(node.in[0]);
		const isJEdge = (es, ed) => _.some(jEdges, ([js, jd]) => js == es && jd == ed);
		const allIn = node => {
			// console.log("searching for id", node.id);
			const js = jEdges.filter(j => {
				// console.log("J:", j);
				return j[1] == node.id;
			}).map(j => {
				// console.log(chalk.red("!!!"), j, "→", j[0]);
				return j[0];
			});
			return _.uniq([...node.in, ...js]);
		};

		const level = node => {
			let n;
			for (n = 0; node.id != startID; n++)
				node = djTree.getNode(parent(node));
			return n;
		};

		let pass = 0;
		let reqPass = false;
		do {
			console.log(`\n\x1b[1;38;5;202m█████ PASS ${++pass} █████\x1b[0m`);

			for (const node of bfs) {
				// console.log(ts(node.id), "node of bfs");
				const id = node.id;
				console.log("\nNode", chalk.green(id) + ":", allIn(node).map(x => chalk.dim(x)).join(" "));
				for (const e of allIn(node)) {
					console.log("   ←", chalk.yellow(e));
					// if (e == id) {
					// 	continue;
					// }

					// console.log(ts(e), ts(id), "e of node.in");
					// if (e is a J-edge ∧ e not visited)
					if (isJEdge(e, id) && id != exitID) {
						console.log("Encountered J-edge:", chalk.bold.cyan(e + " " + id), "...", visited[e][id]? chalk.red("visited") : chalk.green("not visited"));
						console.log(visited);
						if (!visited[e][id]) {
							// console.log(ts(e), ts(id), "yep, it's a j edge.");
							visited[e][id] = true;
							const sNode = djTree.getNode(e);
							const tNode = node; // djTree.getNode(id) would be redundant.
							let tmp = sNode;
							let lNode = null;
							// console.log(ts(e), "wow");
							// console.log(`level(${ts(tmp.id)}), level(${ts(tNode.id)}) -> ${level(tmp)} >= ${level(tNode)}`);
							while (1) {
								const good = level(tmp) >= level(tNode);
								console.log(chalk.cyan("::"), chalk[good? "green" : "red"](`level(tmp ${chalk.dim(("["+tmp.id+"]").padStart(4, " "))}) >= level(tNode ${chalk.dim(("["+tNode.id+"]").padStart(4, " "))})  ${chalk.dim("::")}  ${chalk.bold(level(tmp))} >= ${chalk.bold(level(tNode))}`));
								if (!good) break;
								// console.log(ts(tNode.id), ts(tmp.id), "level(tNode) <= level(tmp)");
								// console.log(`level(${ts(tNode.id)}), ${level(tNode)} >= ${level(tmp)}, level(${ts(tmp.id)})`);
								// Merge(tmp) = Merge(tmp) ∪ Merge(tnode) ∪ {tnode}

								console.log("     " + chalk.magenta(`Merge(tmp ${chalk.dim("["+tmp.id+"]")}) = Merge(tmp ${chalk.dim("["+tmp.id+"]")}) ∪ Merge(tnode ${chalk.dim("["+tNode.id+"]")}) ∪ {tnode ${chalk.dim("["+tNode.id+"]")}}`));
								console.log("     " + chalk.yellow(tmp.id.toString().padStart(2," "))+":", merge[tmp.id], "<-", merge[tNode.id], "+", tNode.id);

								merge[tmp.id].push(merge[tNode.id]);
								merge[tmp.id].push(tNode.id);
								// merge[tmp.id] = [merge[tmp.id], merge[tNode.id], [tNode.id]];
								// console.log(`lNode: ${lNode? ts(lNode.id) : lNode} -> ${ts(tmp.id)}`);
								lNode = tmp;
								tmp = parent(tmp);
							}

							// console.log("we're here.");

							const lID = lNode.id;
							// for (all incoming edges to lnode)
							for (const e_ of lNode.in) {
								// console.log(ts(e_), "e_ of lNode.in");
								if (isJEdge(e_, lID) && visited[e][lID]) {
									//// const sNode_ = djTree.getNode(e_);
									// if (Merge(snode') ⊉ Merge(lnode))
									//// if (_.notSuperOrEq(merge[sNode_.id], merge[lID]))

									console.log(merge[e_], merge[lID], _.notSuperOrEq(merge[e_], merge[lID]));

									if (_.notSuperOrEq(merge[e_], merge[lID]))
										reqPass = true;
								}
							}
						}
					}
				}
			}
		} while (reqPass);

		// The merge sets are defined in terms of each other (if you just push in order, earlier computed merge sets
		// won't include the items added in later computed merge sets), so we need to combine them all together.
		// _.flattenDeep doesn't handle circular references, so we need to flatten the merge sets ourselves.
		const flatten = (x, out=[]) => {
			if (typeof x == "number") {
				out.push(x);
			} else if (x instanceof Array) {
				x.forEach(y => x != y && flatten(y, out));
			}

			return out;
		};

		return _.fromPairs(_.keys(merge).map(key => [key, _.uniq(flatten(merge[key])).sort()]));
	}

	/**
	 * @typedef {Object} DFSResult
	 * @property {number[]} parents    A list of each node's parent (null if nonexistent).
	 * @property {number[]} discovered A list of the times each node was discovered.
	 * @property {number[]} finished   A list of the times each node was finished.
	 */

	/**
	 * Runs a depth-first search on the graph.
	 * @param  {Node|NodeID} [startID=0] The ID of the start node.
	 * @return {module:util~DFSResult} The result of the search.
	 */
	dfs(startID=0) {
		const parents    = this.fill(null);
		const discovered = this.fill(null);
		const finished   = this.fill(null);
		let time = 0;

		const visit = u => {
			discovered[u] = ++time;
			this.getNode(u).out.sort().forEach(v => {
				if (discovered[v] == null) {
					parents[v] = u;
					visit(v);
				}
			});

			finished[u] = ++time;
		};

		visit(startID);
		return {parents, discovered, finished};
	}

	/**
	 * Performs a breadth-first search of the graph from a given start node.
	 * @param {Node|NodeID} startID The ID of the start node.
	 * @return {Node[]} An array of nodes in BFS order.
	 */
	bfs(startID=0) {
		let node = this.getNode(startID);
		const visited = this.fillObj(false);
		const queue = [node];
		const order = [node];
		visited[startID] = true;

		while (queue.length) {
			queue.shift().out.forEach(id => {
				if (!visited[id]) {
					visited[id] = true;
					node = this.getNode(id);
					order.push(node);
					queue.push(node);
				}
			});
		}

		return order;
	}

	/**
	 * Renames the node with a given ID (if one exists) to a new ID.
	 * @param  {Node|NodeID} oldID The old ID of the node to rename.
	 * @param  {NodeID} newID The new ID to assign to the node.
	 * @return {Graph} The same graph the method was called on.
	 */
	renameNode(oldID, newID) {
		const node = this.getNode(oldID);
		if (node) {
			node.rename(newID);
		}

		return this;
	}

	/**
	 * Returns an array of this graph's connected components using Kosaraju's algorithm.
	 * @return {Array<Array<Node>>} An array of connected components.
	 */
	components() {
		const visited = this.fill(false);
		const parents = this.fill(null);
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
	 * Runs a DFS on each node that hasn't been visited, appending each discovered node to the output array,
	 * until no unvisited nodes remain. The initial list of unvisited nodes is ordered the same as the `nodes`
	 * array of this Graph object.
	 * @return {Node[]} An array of ordered nodes.
	 */
	sortedDFS() {
		const list = [];
		const visited = this.fill(false);
		const unvisited = _.range(0, this.length);

		const visit = u => {
			visited[u] = true;
			_.pull(unvisited, u);

			for (const v of this.nodes[u].out.sort()) {
				if (!visited[v]) {
					visit(v);
				}
			}
			
			list.push(u);
		};
		
		while (unvisited.length) {
			visit(unvisited[0]);
		}

		return list.map(n => this.nodes[n]);
	}

	/**
	 * Calculates a topologically sorted list of nodes using Kahn's algorithm.
	 * @return {Node[]} A topologically sorted list of the graph's nodes.
	 * @throws Will throw an error if the graph is cyclic.
	 */
	topoSort() {
		// We need to clone the graph to prevent any changes to it.
		const copy = this.clone();

		// The sorted list.
		const l = [];

		// An array containing every node that has no in-edges.
		const s = copy.nodes.filter(node => !node.in.length);

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
		return this;
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
	 * Returns a string listing each node's adjacency array.
	 * @param  {Function} [idFn] A function mapping a node ID and its corresponding Node to a string representation.
	 *                           By default, it returns the node ID.
	 * @param  {Function} [outFn] Another mapping function. If none is given, it will be equal to idFn.
	 * @return {string} A string representation of the graph.
	 */
	toString(idFn = x=>x, outFn) {
		if (outFn === undefined) {
			outFn = idFn;
		}
		
		return _.sortBy(this.nodes, "id").map(node =>
			`${idFn(node.id, node)} => ${node.out.map(out => outFn(out, node)).join(", ")}`
		).join("\n");
	}

	/**
	 * Checks whether all `in` arrays and `out` arrays match up (i.e., ∀N,M∈G M∈In(N) ⟷ N∈Out(M)).
	 * @return {boolean} Whether the graph's `in` and `out` arrays match up.
	 */
	validateDirections() {
		for (const node of this.nodes) {
			for (const o of node.out) {
				if (!this[o].in.includes(node.id)) {
					return false;
				}
			}

			for (const i of node.in) {
				if (!this[i].out.includes(node.id)) {
					return false;
				}
			}
		}

		return true;
	}
}

module.exports = Graph;
module.exports.Node = Node;
