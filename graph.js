#!/usr/bin/env node
let _ = require("lodash");

const chalk = require("chalk");

/**
 * Contains various utilities.
 * 
 * @module util
 */

const alpha = "abcdefghijklmnopqrstuvwxyz";
const isLetter = x => typeof x == "string" && x.length == 1 && -1 < alpha.indexOf(x.toLowerCase());
const getID = node => {
	if (node instanceof Node) {
		return node.id;
	} else if (isLetter(node)) {
		return alpha.indexOf(node.toLowerCase());
	} else {
		return node;
	}
};


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

		return new Proxy(this.reset(n), {
			get(target, prop) {
				if (typeof prop == "symbol") {
					return target[prop];
				}

				if (Number(prop) == prop || isLetter(prop)) {
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
		return this;
	}

	add(data) {
		const newNode = new Node(this.length, this, data);
		this.push(newNode);
		return newNode;
	}

	/**
	 * Returns the nth node of the graph.
	 * @param  {number} n The ID of the node to return.
	 * @return {Node}     The node corresponding to n if n is a number; n otherwise.
	 */
	getNode(n) {
		if (isLetter(n)) {
			n = alpha.indexOf(n.toLowerCase());
		} else if (n.id !== undefined) {
			n = n.id;
		}

		return _.find(this.nodes, node => node.id == n);
	}

	/**
	 * Adds a unidirectional connection from one node to another.
	 * @param {(Node|number)} source      The source node.
	 * @param {(Node|number)} destination The destination node.
	 */
	arc(source, destination) {
		if (typeof source === "string" && isLetter(source[0]) && isLetter(source[1]) && destination === undefined) {
			this.nodes[getID(source[0])].arc(getID(source[1]));
		} else {
			this.getNode(source).arc(destination);
		}
		
		return this;
	}

	arcString(str) {
		str.split(/\s+/).forEach(s => this.arc(s));
		return this;
	}

	/**
	 * Batch-adds arcs from an array of [source, ...destinations] sets.
	 * @param {...Array<Array<number, ...number>>} arcs - An array of arc sets to add.
	 */
	arcs(...sets) {
		sets.forEach(([src, ...dests]) => dests.forEach(dest => this.arc(src, dest)));
		return this;
	}

	/**
	 * Removes an edge from one node to another.
	 * @param {(Node|number)} source      The source node.
	 * @param {(Node|number)} destination The destination node.
	 */
	removeArc(source, destination) {
		this.nodes[getID(source)].removeArc(destination);
		return this;
	}

	/**
	 * Adds a bidirectional connection between two nodes.
	 * @param {(Node|number)} a The first node.
	 * @param {(Node|number)} b The second node.
	 */
	edge(a, b) {
		this.nodes[getID(a)].arc(b);
		this.nodes[getID(b)].arc(a);
		return this;
	}

	/**
	 * Removes all connections between two nodes.
	 * @param {(Node|number)} a The first node.
	 * @param {(Node|number)} b The second node.
	 */
	disconnect(a, b) {
		this.nodes[getID(a)].removeArc(b);
		this.nodes[getID(b)].removeArc(a);
		return this;
	}

	findSingle(predicate) {
		const found = this.nodes.filter(predicate);
		if (found.length != 1) {
			throw `Predicate matched ${found.length} results`;
		}

		return found[0];
	}

	fill(value=null) {
		return _.fill(Array(this.length), value);
	}

	fillObj(value=null) {
		const out = {};
		this.forEach(v => out[v.id] = value);
		return out;
	}

	mapObj(fn) {
		const out = {};
		this.forEach((v, i) => out[v.id] = fn(v, i));
		return out;
	}

	flatten(offset=0) {
		const rename = this.mapObj((v, i) => i + offset);
		const oldNodes = Object.values(this.nodes);

		this.nodes = [];
		oldNodes.forEach(v => {
			["in", "out"].forEach(p => v[p] = v[p].map(i => rename[i]))
			v.id = rename[v.id];
			this.nodes.push(v);
		});

		return rename;
	}

	dTree(startID = 0) {
		const out = new Graph(this.length);

		Object.entries(this.lengauerTarjan(startID)).forEach(([k, v]) => {
			if (v === null) {
				out.arc(k, k);
			} else {
				out.arc(v, k);
			}
		});

		return out;
	}

	dTree_(startID = 0) {
		const out = new Graph(this.length);

		Object.entries(this.lengauerTarjan_(startID)).forEach(([k, v]) => {
			if (v == null) {
				out.arc(k, k);
			} else if (k != null) {
				out.arc(v, k);
			} else {
				console.log("???", v, k);
			}
		});

		return out;
	}

	lengauerTarjan(startID=0) {
		// Thomas Lengauer and Robert Endre Tarjan. 1979. A fast algorithm for finding dominators in a flowgraph.
		// ACM Trans. Program. Lang. Syst. 1, 1 (January 1979), 121-141.
		// DOI: https://doi-org.oca.ucsc.edu/10.1145/357062.357071
		const {parents: parent} = this.dfs(startID);
		const succ = this.mapObj(v => v.out);
		const pred = this.fillObj([]);
		const semi = this.fillObj(null);
		const bucket = this.fillObj([]);
		const dom = this.fillObj(null);
		const vertex = {};
		const size = this.fillObj(1);
		const child = this.fillObj(null);
		const ancestor = this.fillObj(0);
		const label = this.mapObj(v => v.id);
		let n = 0;

		const _Z = 0;

		const ltDFS = v => {
			vertex[n] = v;
			semi[v] = n;
			n++;
			for (const w of succ[v]) {
				if (semi[w] == _Z) {
					parent[w] = v;
					ltDFS(w);
					pred[w].push(v);
				}
			}
		};

		const compress = v => {
			if (ancestor[ancestor[v]] != _Z) {
				compress(ancestor[v]);
				if (semi[label[ancestor[v]]] < semi[label[v]]) {
					label[v] = label[ancestor[v]];
				}

				ancestor[v] = ancestor[ancestor[v]];
			}
		};

		// const doEval = v => {
		// 	if (ancestor[v] == _Z) {
		// 		return v;
		// 	}

		// 	compress(v);
		// 	return label[v];
		// };

		// const link = (v, w) => ancestor[w] = v;
		
		const doEval = v => {
			if (ancestor[v] == _Z) {
				return label[v];
			}
			
			compress(v);
			if (semi[label[ancestor[v]]] >= semi[label[v]]) {
				return label[v];
			} else {
				return label[ancestor[v]];
			}
		};

		const link = (v, w) => {
			let s = w;
			while (semi[label[w]] < semi[label[child[s]]]) {
				if (size[s] + size[child[child[s]]] >= 2 * size[child[s]]) {
					parent[child[s]] = s;
					child[s] = child[child[s]];
				} else {
					size[child[s]] = size[s];
					s = parent[s] = child[s];
				}
			}

			label[s] = label[w];
			size[v] += size[w];
			if (size[v] < 2 * size[w]) {
				[s, child[v]] = [child[v], s];
			}

			while (s != null) {
				parent[s] = v;
				s = child[s];
			}
		};

		// Step 1
		this.forEach(v => {
			pred[v.id] = [];
			semi[v.id] = _Z;
			ltDFS(v.id);
		});

		// Steps 2 and 3

		let u;
		for (let i = n - 1; 1 <= i; i--) {
			const w = vertex[i];
			for (const v of pred[w]) {
				u = doEval(v);
				if (semi[u] < semi[w]) {
					semi[w] = semi[u];
				}
			}

			bucket[vertex[semi[w]]].push(w);
			link(parent[w], w);
			for (const v of bucket[parent[w]]) {
				_.pull(bucket[parent[w]], v);
				u = doEval(v);
				dom[v] = semi[u] < semi[v]? u : parent[w];
			}
		}

		// Step 4
		for (let i = 1; i < n; i++) {
			const w = vertex[i];
			if (dom[w] != vertex[semi[w]]) {
				dom[w] = dom[dom[w]];
			}
		}

		dom[startID] = null;
		return dom;
	}

	lengauerTarjan_(startID=0) {
		// Source: "The Lengauer Tarjan Algorithm for Computing the Immediate Dominator Tree of a Flowgraph"
		//          by Martin Richards
		// https://www.cl.cam.ac.uk/~mr10/lengtarj.pdf

		const n = this.length;
		const {parents} = this.dfs(startID);
		const succs = this.mapObj(v => v.out);
		const preds = this.mapObj(v => v.in);
		const semis = this.mapObj(v => v.id);
		const idoms = this.fillObj();
		const ancestors = this.fillObj();
		const best = this.mapObj(v => v.id);
		const bucket = this.fillObj([]);

		const doEval = v => {
			if (ancestors[v] === null) {
				return v;
			}

			compress(v);
			return best[v];
		};

		const compress = v => {
			const a = ancestors[v];
			// console.log(`${v}.ancestor = ${a}, ${a}.ancestor = ${ancestors[a]}`);
			if (ancestors[a] === null) {
				return;
			}

			// console.log(`${v} -> ${a}`);
			compress(a);
			if (semis[best[v]] > semis[best[a]]) {
				best[v] = best[a];
			}

			ancestors[v] = ancestors[a];
		};

		const link = (v, w) => ancestors[w] = v;

		for (let w = n - 1 ; 1 <= w; w--) {
			const p = parents[w];
			for (const v of this[w].pred) {
				const u = doEval(v);
				if (semis[w] > semis[u])
					semis[w] = semis[u];
			}

			bucket[semis[w]].push(w);
			link(p, w);

			for (const v of bucket[p]) {
				const u = doEval(v);
				idoms[v] = semis[u] < semis[v]? u : p;
			}

			bucket[p] = [];
		}

		for (let w = 1; w < n; w++) {
			if (idoms[w] != semis[w])
				idoms[w] = idoms[idoms[w]];
		}

		idoms[startID] = null;
		return idoms;
	}

	dominance(startNode) {
		// https://www.cs.rice.edu/~keith/Embed/dom.pdf
		const doms = this.fill();
		
		if (startNode === undefined) {
			startNode = this[0];
		} else {
			startNode = this.getNode(startNode);
		}

		const intersect = (b1, b2) => {
			let finger1 = getID(b1), finger2 = getID(b2);

			while (finger1 != finger2) {
				while (finger1 < finger2) {
					finger1 = doms[finger1];
				}
				
				while (finger2 < finger1) {
					finger2 = doms[finger2];
				}
			}

			return finger1;
		};

		const postage = this.labelPostOrder(startNode.id);
		let changed = true;
		let iter = 1;

		doms[startNode.id] = startNode.id;

		while (changed) {
			changed = false;

			for (let node = postage.tail; node; node = node.prev) {
				if (node.id == startNode.id) {
					continue;
				}

				const b = this.getNode(node.id);
				const preds = _.sortBy(b.in, x => postage.objs[x].order);
				let newIDom = preds.filter(c => doms[c] !== null)[0];

				if (newIDom === undefined) {
					newIDom = null;
				}

				const others = _.without(preds, newIDom);
				for (const p of others) {
					if (doms[p] !== null ) {
						newIDom = intersect(p, newIDom);
					}
				}

				if (doms[node.id] !== newIDom) {
					doms[node.id] = newIDom;
					changed = true;
				}
			}
		}

		return doms;
	}

	reversePost(start) {
		return _.sortBy(this.dfs(start).finished.map((n, i) => [i, n]), 1).map(_.head);
	}

	labelPostOrder(id, state) {
		if (state === undefined) {
			state = {
				n: 0,
				start: id,
				list: [],
				objs: this.nodes.map(x => ({id: x.id, order: null, done: false}))
			};
		}

		const obj = state.objs[id];
		if (obj.done === false) {
			obj.done = true;
			for (const c of this.getNode(id).out.sort()) {
				this.labelPostOrder(c, state);
			}

			obj.order = state.n++;
			state.list.push(id);
		}

		if (state.list.length == this.length) {
			for (let i = 0; i < this.length; i++) {
				const here = _.find(state.objs, o => o.order == i);
				const next = _.find(state.objs, o => o.order == i + 1);

				delete here.done;

				if (i == 0) {
					state.head = here;
				}

				if (next) {
					here.next = next;
					next.prev = here;
				} else {
					state.tail = here;
				}
			}
		}

		return state;
	}

	/**
	 * Runs a depth-first search on the graph.
	 * @return {module:util~DFSResult} The result of the search.
	 */
	dfs(start=0) {
		const n = this.nodes.length;
		const parents    = this.fill();
		const discovered = this.fill();
		const finished   = this.fill();
		let time = 0;

		const visit = u => {
			discovered[u] = ++time;
			this.nodes[u].out.sort().forEach(v => {
			// this.nodes[u].out.forEach(v => {
				if (discovered[v] == null) {
					parents[v] = u;
					visit(v);
				}
			});

			finished[u] = ++time;
		};

		console.log("\n");
		visit(start);
		console.log("\n");

		return {parents, discovered, finished};
	}

	/**
	 * Calculates and returns a list of this graph's connected components using Kosaraju's algorithm.
	 * @type {Array<Array<Node>>}
	 */
	get components() {
		const visited = this.fill(false);
		const parents = this.fill();
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

	sortedDFS() {
		const list = [];
		const visited = this.fill(false);
		const unvisited = _.range(0, this.length);

		const visit = u => {
			visited[u] = true;
			_.pull(unvisited, u);

			for (const v of this.getNode(u).out.sort()) {
				if (!visited[v]) {
					visit(v);
				}
			}
			
			list.push(u);
		};
		
		while (unvisited.length) {
			visit(unvisited[0]);
		}

		return list;
	}

	/**
	 * Calculates a topologically sorted list of nodes using Kahn's algorithm.
	 * @return {Array<Node>} A topologically sorted list of the graph's nodes.
	 * @throws Will throw an error if the graph is cyclic.
	 */
	topoSort() {
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
	 * Returns a string containing each node's adjacency list.
	 * @return {string} A string representation of the graph.
	 */
	toString(idFn = x=>x, outFn = x=>x) {
		return _.sortBy(this.nodes, "id").map((node) => `${idFn(node.id, node)} => ${node.out.map(out => outFn(out, node)).join(", ")}`).join("\n");
	}

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
	 * @param {(Node|number)} n The node to add.
	 */
	arc(n) {
		n = getID(n);
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

		return this;
	}

	/**
	 * Removes an outward connection from this node and the other node's corresponding inward connection.
	 * @param {(Node|number)} n The node whose arc will be removed.
	 */
	removeArc(n) {
		n = getID(n);
		this.out = this.out.filter(edge => edge != n);
		this.graph.nodes[n].in = this.graph.nodes[n].in.filter(edge => edge != this.id);
		return this;
	}

	/**
	 * Removes an inward connection to this node and the other node's corresponding outward connection.
	 * @param {(Node|number)} n The node whose arc will be removed.
	 */
	removeArcFrom(n) {
		n = getID(n);
		this.in = this.in.filter(edge => edge != n);
		this.graph.nodes[n].out = this.graph.nodes[n].out.filter(edge => edge != this.id);
		return this;
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
		console.log(g.sortedDFS());
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
