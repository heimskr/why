#!/usr/bin/env node

const cytosnap = require("cytosnap");

function renderBase64(graph, opts={}) {
	const defaults = {
		background: "#171717",
		node: "#fff",
		edge: "#ccc",
		layout: "cose-bilkent",
		rankDir: "LR"
	};

	Object.keys(defaults).forEach(key => {
		if (opts[key] === undefined) {
			opts[key] = defaults[key];
		}
	});

	cytosnap.use(["cytoscape-" + opts.layout]);

	const snap = cytosnap();
	const elements = [
		...graph.map((node) => ({data: {id: node.id}})),
		...graph.allEdges().map(([source, target]) => ({data: {source, target}}))
	];
	
	return snap.start().then(() => snap.shot({
		elements,
		layout: {
			name: opts.layout,
			rankDir: opts.rankDir,
		},

		style: [{
			selector: "node",
			style: {"background-color": opts.node}
		}, {
			selector: "edge",
			style: {
				"line-color": opts.edge

			}
		}],

		resolvesTo: "base64",
		format: "png",
		width: 2560,
		height: 1000,
		background: opts.background

		// style: [
		// 	{
		// 		selector: 'node',
		// 		style: {
		// 			'background-color': '#dd4de2'
		// 		}
		// 	},
		// 	{
		// 		selector: 'edge',
		// 		style: {
		// 			'curve-style': 'bezier',
		// 			'target-arrow-shape': 'triangle',
		// 			'line-color': '#dd4de2',
		// 			'target-arrow-color': '#dd4de2',
		// 			'opacity': 0.5
		// 		}
		// 	}
		// ],
	}));
};

module.exports = {base64: renderBase64};

if (require.main === module) {
	const Graph = require("../graph.js");
	const g = new Graph(24);
	g.arcString("01 02 23 34 35 3-23 38 45 56 57 23-5 23-8 67 75 78 89 8-10 8-14 9-10 14-15 14-16 15-16 10-11 11-12 12-13 13-1 16-22 22-10 16-17 17-21 21-22 17-18 18-19 18-20 19-20 20-18 20-21");
	renderBase64(g).then(b64 => {
		console.log(`\x1b]1337;File=inline=1:${b64}\u0007\n`);
		process.exit(0);
	});
}
