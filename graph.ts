#!/usr/bin/env ts-node
import * as fs from "fs";
import {Stream, Readable} from "stream";

const dominators = require("dominators");
const {lt} = dominators;

import Node, {NodeID, NodeOrID, getID} from "./node";
export {Node, NodeID, NodeOrID, getID};

import * as renderGraph from "./rendergraph";
import {RenderOptions, GraphRenderPromise} from "./rendergraph";

import _, {alpha, numerize, ForeachFunction, MapFunction, ReduceFunction} from "./util";
import {isCFG} from "./llvm/ll2w";

export type BothMap<T> = {[key: string]: T, [key: number]: T};
export type NodeIDMap = BothMap<NodeID>;
export type DFSResult = {parents: NodeID[], discovered: NodeID[], finished: NodeID[]};
export type NodeMapFunction<T> = (id: NodeID, node?: Node<T>) => string;

export type DJGraph = Graph<{jEdges: [NodeID, NodeID][]}>;
export type DTree = Graph<{}>;

/**
 * Represents a directed graph datatype.
 */
export default class Graph<D extends Object> {
	data: D;
	nodes: Node<any>[];
	title?: string;
	
	get  length():                     number { return this.nodes.length; }
	get    push():  (node: Node<any>) => void { return this.nodes.push.bind(this.nodes);    }
	get forEach(): ForeachFunction<Node<any>> { return this.nodes.forEach.bind(this.nodes); }
	get     map():     MapFunction<Node<any>> { return this.nodes.map.bind(this.nodes);     }
	get  reduce():  ReduceFunction<Node<any>> { return this.nodes.reduce.bind(this.nodes);  }

	/**
	 * Creates a new graph.
	 * @param {number} n     The number of nodes in the graph.
	 * @param {*}     [data] Any extra data to associate with the graph.
	 */
	constructor(n: number, data: D) {
		this.data = data;
		this.nodes = [];

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
			set(target: Graph<D>, prop: string | number | symbol, val: any, receiver?: any) {
				if (Number(prop) != prop) {
					return Reflect.set(target, prop, val, receiver);
				}
				
				const i = _.findIndex(target.nodes, node => node.id == prop);
				if (i == -1) {
					return Reflect.set(target, prop, val, receiver);
				}

				target.nodes[i] = val;
				return true;
			}
		});
	}

	/**
	 * Deletes all nodes in the graph.
	 * @param {number|NodeID[]} n The number of new empty nodes to replace the old nodes
	 *                         or an array of IDs to make new nodes with.
	 */
	reset(n: number | NodeID[]): this {
		if (n == undefined) {
			this.nodes = [];
		} else if (typeof n == "number") {
			this.nodes = _.range(0, n).map(i => new Node(i, this));
		} else {
			this.nodes = _.range(0, n.length).map(i => new Node(n[i], this));
		}

		this.title = undefined;
		return this;
	}

	/**
	 * Generates an ID not already assigned to any nodes in the graph.
	 * @return {number} An available numeric ID.
	 */
	newID(): number {
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
	add<N>(data: N): Node<N> {
		const newNode = new Node<N>(this.newID(), this, data);
		this.nodes.push(newNode);
		return newNode;
	}

	/**
	 * Returns the node with a given ID.
	 * @param  {Node|NodeID} n The ID of the node to return.
	 * @return {?Node} The node corresponding to n if n is a number; n otherwise.
	 */
	getNode<N>(n: NodeOrID): Node<N> | undefined {
		if (n instanceof Node) {
			return n.graph == this? n : this.getNode(n.id);
		}

		if (n == undefined) {
			throw new Error("Graph.getID() called with undefined");
		}

		const numerized = numerize(n);
		return _.find(this.nodes, (node: Node<any>) => numerize(node.id) == numerized);
	}

	/**
	 * Attempts to return the node with a given ID, throwing an error if one couldn't be found.
	 * @param  {Node|NodeID} n The ID of the node to return.
	 * @return {Node} The node corresponding to n if n is a number; n otherwise.
	 */
	getNodeSafe<N>(n: NodeOrID): Node<N> {
		const node = this.getNode<N>(n);
		if (node === undefined) {
			throw new Error("Couldn't find node.");
		}

		return node;
	}

	/**
	 * Adds a unidirectional connection from one node to another.
	 * @param  {Node|NodeID} source      The source node.
	 * @param  {Node|NodeID} destination The destination node.
	 * @return {boolean} Whether the connection already existed.
	 */
	arc(source: NodeOrID, destination: NodeOrID): boolean {
		return this.getNodeSafe(source).arc(destination);
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
	arcString(str: string): this {
		let pairs = str.split(/\s+/);
		if (str.match(/\d/)) {
			pairs.forEach(pair =>  {
				if (pair.match(/^\d{2}$/)) {
					this.arc(parseInt(pair[0]), parseInt(pair[1]));
				} else {
					const [src, dest] = pair.split("-").map(s => parseInt(s));
					this.arc(src, dest);
				}
			});
		} else {
			pairs.forEach(s => {
				const [src, dest] = s.split("").map(c => alpha.indexOf(c.toLowerCase()));
				this.arc(src, dest);
			});
		}

		return this;
	}

	/**
	 * Batch-adds arcs from an array of [source, ...destinations] sets.
	 * @param  {...Array<Array<number, ...number>>} arcs An array of arc sets to add.
	 * @return {Graph} The same graph the method was called on.
	 */
	arcs(...sets: [NodeID, ...NodeID[]][]): this {
		sets.forEach(([src, ...dests]) => dests.forEach(dest => this.arc(src, dest)));
		return this;
	}

	/**
	 * Removes an edge from one node to another.
	 * @param  {Node|NodeID} source      The source node.
	 * @param  {Node|NodeID} destination The destination node.
	 * @return {Graph} The same graph the method was called on.
	 */
	removeArc(source: NodeOrID, destination: NodeOrID): this {
		this.nodes[getID(source)].removeArc(destination);
		return this;
	}

	/**
	 * Adds a bidirectional connection between two nodes.
	 * @param  {Node|NodeID} a The first node.
	 * @param  {Node|NodeID} b The second node.
	 * @return {Graph} The same graph the method was called on.
	 */
	edge(a: NodeOrID, b: NodeOrID): this {
		this.getNodeSafe(a).arc(b);
		this.getNodeSafe(b).arc(a);
		return this;
	}

	/**
	 * Removes all connections between two nodes.
	 * @param  {Node|NodeID} a The first node.
	 * @param  {Node|NodeID} b The second node.
	 * @return {Graph} The same graph the method was called on.
	 */
	disconnect(a: NodeOrID, b: NodeOrID): this {
		this.getNodeSafe(a).removeArc(b);
		this.getNodeSafe(b).removeArc(a);
		return this;
	}

	/**
	 * Attempts to find a single node matching a predicate.
	 * @param  {Function|string} predicate A function mapping a node to a boolean or a string corresponding to a label.
	 * @return {?Node} The result of the search.
	 * @throws Throws an exception if the number of nodes matching the predicate isn't exactly 1.
	 */
	findSingle<N>(predicate: string | ((n: Node<N>) => boolean)): Node<N> | undefined {
		let found: Node<N>[];
		if (typeof predicate == "function") {
			found = this.nodes.filter(predicate);
		} else {
			found = this.nodes.filter(node => node.data && node.data.label == predicate);
		}

		if (found.length != 1) {
			throw new Error(`Predicate matched ${found.length} results.`);
		}

		return found[0];
	}

	/**
	 * Returns an array whose length is equal to the number of nodes in this graph
	 * and fills it with a predefined value.
	 * @param  {*} [value=null] A value to fill the array with.
	 * @return {Array} The filled array.
	 */
	fill<T>(value: T): T[] {
		return _.fill(Array(this.length), value);
	}

	/**
	 * Returns an object whose keys are the IDs of each node in the graph
	 * and whose values are all equal to the given value.
	 * @param  {*} value A value to use as the value of each entry in the object.
	 * @return {Object} The filled object.
	 */
	fillObj<T>(value: T): BothMap<T> {
		const out = {};
		this.forEach((v: Node<any>) => out[v.id] = _.cloneDeep(value));
		return out;
	}

	/**
	 * Returns an object whose keys are the IDs of each node in the graph and whose values
	 * are the return value of the given function when called with a Node object.
	 * @param  {Function} fn A function that takes a Node and returns any value.
	 * @return {Object} The mapped object.
	 */
	mapValues<T>(fn: (node: Node<any>, index?: number) => T): BothMap<T> {
		const out = {};
		this.forEach((v, i) => out[v.id] = fn(v, i));
		return out;
	}

	/**
	 * Renames all nodes in the graph such that they have numeric IDs in the range [offset, n + offset).
	 * @param  {number} [offset=0] The starting point of the range.
	 * @return {Object<NodeID, number>} A map of old IDs to new IDs.
	 */
	normalize(offset: number = 0): BothMap<number> {
		const renameMap: BothMap<number> = this.mapValues((v, i) => i + offset);
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
	dTree(startID: NodeOrID = 0, bidirectional: boolean = false): DTree {
		const [lentar] = this.lengauerTarjan(getID(startID));
		const out = new Graph(Object.keys(lentar).length, {});
		const fn = (bidirectional? out.edge : out.arc).bind(out);
		Object.entries(lentar).forEach(([k, v]) => {
			out.nodes[k].data = this.nodes[k].data;
			fn(v == undefined? k : v, k)
		});
		
		out.title = "Dominator Tree";
		return out;
	}

	/**
	 * Extracts the strict dominators of a D-tree.
	 * @param  {Graph} dt A D-tree.
	 * @return {Object<NodeID, NodeID[]>}
	 *         An object mapping a node ID to an array of the IDs of its strict dominators.
	 */
	static strictDominators(dt: DTree): BothMap<NodeID[]> {
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
	allEdges(): [NodeID, NodeID][] {
		return this.reduce((a, {id: src, out}) =>
			[...a, ...out.map(dst => [src, dst])],
		[]);
	}

	/**
	 * Computes the DJ-graph of the graph for a given start node.
	 * @param  {Node|NodeID|Graph} [start=0] The ID of the start node, or a precomputed D-tree.
	 * @param  {boolean} [bidirectional=false] Whether D-edges should be bidirectional.
	 * @return {DJGraph} A DJ-graph.
	 */
	djGraph(start: NodeOrID | Graph<any> = 0, bidirectional: boolean = false): DJGraph {
		const dj: DJGraph = start instanceof Graph? start.clone(false) : this.dTree(start, bidirectional);
		const sdom = Graph.strictDominators(dj);
		console.log({sdom});
		dj.data.jEdges = [];
		this.allEdges()
			.filter(([src, dst]) => !sdom[dst].includes(src))
			.forEach(([src, dst]) => (dj.arc(src, dst), dj.data.jEdges.push([src, dst])));
		dj.title = "DJ Graph";
		return dj;
	}

	/**
	 * Finds the dominators of each node given a start node using the Lengauer-Tarjan algorithm.
	 * This is a wrapper for the `lt` function from the `dominators` package by Julian Jensen.
	 * @param  {Node|NodeID} [startID=0] The ID of the start node.
	 * @return {Array<Object<NodeID, NodeID>, Object<NodeID, NodeID>>}
	 *         A tuple that has a map of node IDs to the IDs of their dominators and a map of old IDs to normalized IDs.
	 */
	lengauerTarjan(startID: NodeOrID = 0): [NodeIDMap, NodeIDMap, NodeIDMap] {
		const normalized = this.clone();
		const renameMap = normalized.normalize();
		const renames = _.mapValues(_.invert(renameMap), v => numerize(v));
		const formatted = normalized.nodes.map(v => v.out);
		return [
			lt(formatted, this.getNodeSafe(startID).id).reduce((a: {[key: number]: number}, b: number, i: number) => ({
				...a,
				[renames[i]]: renames[b]
			}), {}),
			renameMap,
			renames
		];
	}

	static mergeSets(djGraph: DJGraph, startID: number = 0, exitID: number = 1): BothMap<NodeID[]> {
		// "A Practical and Fast Iterative Algorithm for φ-Function Computation Using DJ Graphs"
		// Das and Ramakrishna (2005)
		// Top Down Merge Set Computation (TDMSC-I)

		const bfs = djGraph.bfs(startID);
		const {jEdges} = djGraph.data;
		
		console.log({jEdges});
		
		// out node ID => in node ID
		const visited = djGraph.fillObj({});

		// node ID => IDs in merge set
		const merge: {[key: string]: any[], [key: number]: any[]} = djGraph.fillObj([]);

		const parent = (node: Node<any>) => djGraph.getNode(node.in[0]);
		const isJEdge = (es: NodeID, ed: NodeID) => _.some(jEdges, ([js, jd]) => js == es && jd == ed);
		const allIn = (node: Node<any>) => _.uniq([
			...node.in,
			...jEdges.filter(([s, d]) => d == node.id).map(([s, d]) => s)
		]);
		const level = (node: Node<any>) => {
			let n: number;
			for (n = 0; node.id != startID; n++)
				node = djGraph.getNode(parent(node));
			return n;
		};

		let reqPass: boolean;
		do {
			reqPass = false;
			for (const node of bfs) {
				const id = node.id;
				const unvisitedJEdges = allIn(node).filter(e => isJEdge(e, id) && id != exitID && !visited[e][id]);
				console.log("id:", id, {allIn: allIn(node), unvisitedJEdges});
				for (const e of unvisitedJEdges) {
					console.log("e:", e);
					visited[e][id] = true;
					let lNode: Node<any> | null = null;
					let tmp = djGraph.getNode(e);
					console.log(level(tmp), level(node));
					while (level(tmp) >= level(node)) {
						merge[tmp.id].push(merge[id]);
						merge[tmp.id].push(id);
						tmp = parent(lNode = tmp);
					}

					if (lNode === null) {
						throw new Error("lNode is null");
					}

					const originalLNodeID = lNode.id;
					for (const e_ of lNode.in) {
						if (isJEdge(e_, originalLNodeID) && visited[e ][originalLNodeID] &&
						    _.notSuperOrEq(merge[e_], merge[originalLNodeID])) {
							reqPass = true;
						}
					}
				}
			}
		} while (reqPass);

		// The merge sets are defined in terms of each other (if you just push in order, earlier computed merge sets
		// won't include the items added in later computed merge sets), so we need to combine them all together.
		// _.flattenDeep doesn't handle circular references, so we need to flatten the merge sets ourselves.
		const flatten = (x: any, out: NodeID[] = [], processed: any[] = []) => {
			if (x instanceof Array) {
				x.forEach(y => {
					if (x != y && !processed.includes(y)) {
						processed.push(y);
						flatten(y, out, processed)
					}
				});
			} else {
				out.push(x);
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
	dfs(startID: NodeOrID = 0): DFSResult {
		const parents:    NodeID[] = [];
		const discovered: NodeID[] = [];
		const finished:   NodeID[] = [];
		let time = 0;

		const visit = (u: NodeID) => {
			discovered[u] = ++time;
			this.getNodeSafe(u).out.sort().forEach(v => {
				if (discovered[v] == null) {
					parents[v] = u;
					visit(v);
				}
			});

			finished[u] = ++time;
		};

		visit(getID(startID));
		return {parents: parents as NodeID[], discovered: discovered as NodeID[], finished: finished as NodeID[]};
	}

	/**
	 * Performs a breadth-first search of the graph from a given start node.
	 * @param {Node|NodeID} startID The ID of the start node.
	 * @return {Node[]} An array of nodes in BFS order.
	 */
	bfs(startID: NodeOrID = 0): Node<any>[] {
		let node = this.getNodeSafe(startID);
		const visited = this.fillObj(false);
		const queue = [node];
		const order = [node];
		visited[getID(startID)] = true;

		for (let next: Node<any> | undefined = queue[0]; next !== undefined; next = queue.shift()) {
			next.out.forEach(id => {
				if (!visited[id]) {
					visited[id] = true;
					node = this.getNodeSafe(id);
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
	renameNode(oldID: NodeOrID, newID: NodeID): this {
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
	components<N>(): Node<N>[][] {
		const visited: boolean[] = [];
		const parents:  NodeID[] = [];
		const components: {[key: string]: NodeID[], [key: number]: NodeID[]} = {}; 
		const l: NodeID[] = [];

		const visit = (u: NodeID) => {
			if (!visited[u]) {
				visited[u] = true;
				this.nodes[u].out.forEach(visit);
				l.unshift(u);
			}
		};

		const assign = (u: NodeID, root: NodeID) => {
			if (parents[u] == null) {
				parents[u] = root;
				if (!components[root]) {
					components[root] = [u];
				} else {
					components[root].push(u);
				}

				this.getNodeSafe(u).in.forEach((v: NodeID) => assign(v, root));
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
	sortedDFS<N>(): Node<N>[] {
		const list:      NodeID[] = [];
		const visited:  boolean[] = this.fill(false);
		const unvisited: NodeID[] = _.range(0, this.length);

		const visit = (u: NodeID) => {
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

		return list.map(n => this.getNodeSafe(n));
	}

	/**
	 * Calculates a topologically sorted list of nodes using Kahn's algorithm.
	 * @return {Node[]} A topologically sorted list of the graph's nodes.
	 * @throws Will throw an error if the graph is cyclic.
	 */
	topoSort<N>(): Node<N>[] {
		// We need to clone the graph to prevent any changes to it.
		const copy: Graph<D> = this.clone();

		// The sorted list.
		const out: Node<N>[] = [];

		// An array containing every node that has no in-edges.
		const s = copy.nodes.filter(node => !node.in.length);

		if (1 < this.nodes.length && !s.length) {
			// If there are multiple nodes in the graph and none of them lack in-edges, the graph has to be cyclic.
			// The converse isn't necessarily true, so this is just an preliminary check.
			throw new Error("Graph is cyclic.");
		}

		while (s.length) {
			let n = s.pop();
			if (!n) break;
			out.unshift(n);
			
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

		return out.map(n => this.getNodeSafe(n));
	}

	/**
	 * Removes all loop edges from the graph.
	 */
	removeLoops(): this {
		this.nodes.forEach(node => this.disconnect(node, node));
		return this;
	}

	/**
	 * Condenses a list of nodes into a single node, removes the old nodes from the graph and inserts the new node.
	 * The new node's in/out arrays are the unions of the given nodes' in/out arrays. The new node's data is the same
	 * as the data of the first node in the arguments. The new node is reflexive if any of the given nodes is reflexive.
	 * @param  {Node[]} nodes A list of nodes or node IDs.
	 * @return {?Node} The coalesced node.
	 */
	coalesce<N>(nodes: Node<N>[]): Node<N> | undefined {
		if (!nodes.length) {
			return undefined;
		}

		// The input may contain either Nodes or node IDs. Convert all the IDs to nodes.
		nodes = nodes.map(n => this.nodes[getID(n)]);

		// Calculate the union of all in/out edges, but don't include edges between any of the given nodes.
		const combinedIn  = _.without(_.union(...nodes.map(node => node.in)),  ...nodes.map(node => node.id));
		const combinedOut = _.without(_.union(...nodes.map(node => node.out)), ...nodes.map(node => node.id));

		const reflexive = _.some(nodes, n => n.isReflexive);

		const allIDs = nodes.map(node => node.id);
		const newID = nodes[0].id;
		const oldIDs = _.without(allIDs, newID);
		let newNode: Node<N> = this.getNode(newID);
		
		// Remove all the old nodes from the graph.
		_.remove(this.nodes, v => allIDs.includes(getID(v)));

		// Go through every remaining node's in/out arrays, remove the old node IDs
		// and insert the new ID where applicable.
		for (const node of this.nodes) {
			if (_.intersection(node.in,   allIDs).length)
				node.in  = _.without(node.in,  ...allIDs).concat(newID).sort();
			if (_.intersection(node.out,  allIDs).length)
				node.out = _.without(node.out, ...allIDs).concat(newID).sort();
		}

		if (!newNode) {
			newNode = new Node(newID, this, nodes[0].data);
		}

		newNode.in  = _.without(combinedIn,  ...allIDs);
		newNode.out = _.without(combinedOut, ...allIDs);
		
		if (reflexive) {
			newNode.in.push(newID);
			newNode.out.push(newID);
		}

		newNode.in  = _.sortBy(newNode.in,  numerize);
		newNode.out = _.sortBy(newNode.out, numerize);
		
		this.nodes.push(newNode);
		return newNode;
	}

	/**
	 * Calculates and returns the transpose of the graph.
	 * @type {Graph}
	 */
	get transpose(): Graph<D> {
		const graph: Graph<D> = new Graph(this.nodes.length, this.data);
		this.nodes.forEach(({out}, u) => out.forEach(v => graph.arc(v, u)));

		return graph;
	}

	/**
	 * Returns a copy of this graph.
	 * @param {boolean} [cloneNodeData=true]  Whether to clone the node data instead of copying the references.
	 * @param {boolean} [cloneGraphData=true] Whether to clone the graph data instead of copying the reference.
	 * @return {Graph} A copy of the graph.
	 */
	clone(cloneNodeData: boolean = true, cloneGraphData: boolean = true): Graph<D> {
		const newGraph: Graph<D> = new Graph(this.nodes.length, cloneGraphData? _.cloneDeep(this.data) : this.data);
		newGraph.nodes = this.nodes.map(node => node.clone(newGraph, cloneNodeData));
		return newGraph;
	}

	/**
	 * Returns a string listing each node's adjacency array.
	 * @param  {Function} [idFn] A function mapping a node ID and its corresponding Node to a string representation.
	 *                           By default, it returns the node ID.
	 * @param  {Function} [outFn] Another mapping function. If none is given, it will be equal to idFn.
	 * @return {string} A string representation of the graph.
	 */
	toString<N>(idFn: NodeMapFunction<N> = x => x.toString(), outFn: NodeMapFunction<N> = idFn): string {
		return _.sortBy(this.nodes, "id").map((node: Node<N>) =>
			`${idFn(node.id, node)} => ${node.out.map(out => outFn(out, node)).join(", ")}`
		).join("\n");
	}

	render(opts: RenderOptions = {}, display: boolean = false): GraphRenderPromise {
		if (isCFG(this)) {
			opts.enter = this.data.enter;
			opts.exit  = this.data.exit;
			opts.unreachable = [...this.data.unreachable];
		}
		
		const newOpts: RenderOptions = Object.assign({layout: "dagre"}, opts);
		if (display) {
			return renderGraph.iterm(this, newOpts);
		}

		return renderGraph.render(this, newOpts);
	}

	display(opts: RenderOptions = {}): GraphRenderPromise {
		return this.render(opts, true);
	}

	printTitle(): this {
		if (this.title) console.log(this.title + ":");
		return this;
	}

	static displayMultiple<G>(opts: RenderOptions = {}, ...graphs: Graph<G>[]): GraphRenderPromise {
		if (!(graphs instanceof Array)) throw new Error("Expected an array.");
		if (graphs.length == 0) return;
		const p = graphs[0].printTitle().display(opts).then(() => console.log());
		for (const graph of graphs.slice(1)) {
			p.then(() => graph.printTitle()).then(() => graph.display(opts)).then(() => console.log());
		}

		return p;
	}

	async writePNG(path: string, opts: RenderOptions = {}): Promise<void> {
		if (!path) throw new Error("Expected path");

		// return this.render(opts, true);

		return this.render({...opts, type: "stream"}, false).catch((e) => console.log("Error:", e)).then((stream) => {
			// console.log({stream: stream.read()});
			if (stream instanceof Readable) {
				fs.writeFileSync(path, stream.read());
			}
		});
	}

	/**
	 * Checks whether all `in` arrays and `out` arrays match up (i.e., ∀N,M∈G M∈In(N) ⟷ N∈Out(M)).
	 * @return {boolean} Whether the graph's `in` and `out` arrays match up.
	 */
	validateDirections(): boolean {
		for (const node of this.nodes) {
			for (const o of node.out) {
				if (!this[o].in.includes(node.id))
					return false;
			}

			for (const i of node.in) {
				if (!this[i].out.includes(node.id))
					return false;
			}
		}

		return true;
	}
}
