#!/usr/bin/env node

let cytosnap = null;

function render(graph, opts={}) {
	if (cytosnap === null) {
		cytosnap = require("cytosnap");
	}
	
	const defaults = {
		background: "#000",
		node: "#fff",
		edge: "#ccc",
		label: "#fff",
		arrow: "#888",
		labelOutline: {
			color: opts.background === undefined? opts.background : "#000",
			opacity: 1,
			width: 1.5
		},
		layout: "dagre",
		layoutExtras: {rankDir: "LR"},
		enter: 0,
		exit: 1,
		unreachable: [],
		enterColor: "#0f0",
		exitColor: "#f00",
		unreachableColor: "#ff0",
		curveStyle: "bezier",
		arrowShape: "triangle",
		centeredMax: 2,
		width: 2560,
		height: 1000,
		format: "png",
		type: "base64",
	};

	const assign = (target, source) => {
		Object.keys(source).forEach(key => {
			const tval = target[key], sval = source[key];
			if (tval === undefined) {
				target[key] = sval;
			} else if (typeof tval == "object" && typeof sval == "object") {
				assign(tval, sval);
			}
		});
	};

	assign(opts, defaults);

	if (opts.layout) {
		cytosnap.use(["cytoscape-" + opts.layout]);
	}

	const snap = cytosnap();
	const elements = [
		...graph.map(({id, data}) => ({
			data: {id, label: data && data.label? data.label : id},
			classes: [
				id == opts.enter? "node-enter"
					: id == opts.exit? "node-exit"
					: opts.unreachable.includes(id)? "node-unreachable"
					: null,
				id.toString().length <= opts.centeredMax? "centered" : null
			].filter(x => x !== null).join(" "),
		})),
		...graph.allEdges().map(([source, target]) => ({data: {source, target}}))
	];
	
	return snap.start().then(() => snap.shot({
		elements,
		layout: {
			name: opts.layout,
			...opts.layoutExtras,
		},

		style: [{
			selector: "node",
			style: {"background-color": opts.node, label: "data(label)"}
		}, {
			selector: "edge",
			style: {
				"line-color": opts.edge,
				"curve-style": opts.curveStyle,
				"target-arrow-shape": opts.arrowShape,
				"target-arrow-color": opts.arrow,
			}
		}, {
			selector: "label",
			style: {
				"color": opts.label,
				"text-outline-color": opts.labelOutline.color,
				"text-outline-opacity": opts.labelOutline.opacity,
				"text-outline-width": opts.labelOutline.width,
			},
		}, {
			selector: ".node-enter",
			style: {"background-color": opts.enterColor}
		}, {
			selector: ".node-exit",
			style: {"background-color": opts.exitColor}
		}, {
			selector: ".node-unreachable",
			style: {"background-color": opts.unreachableColor}
		}, {
			selector: ".centered",
			style: {"text-halign": "center", "text-valign": "center"}
		}, {
			selector: "label.centered", 
			style: {
				color: opts.background,
				"text-outline-color": "transparent",
				"text-outline-width": 0,
				"text-outline-opacity": 0
			}
		}],

		resolvesTo: opts.type,
		format: opts.format,
		width: opts.width,
		height: opts.height,
		background: opts.background
	})).then(image => (snap.stop(), image));
}

function iterm(graph, opts={}) {
	return render(graph, opts).then(b64 => process.stdout.write(`\x1b]1337;File=inline=1:${b64}\u0007`));
}

module.exports = {render, iterm};

if (require.main === module) {
	const Graph = require("../graph.js");
	const g = new Graph(24);
	g.arcString("01 02 23 34 35 3-23 38 45 56 57 23-5 23-8 67 75 78 89 8-10 8-14 9-10 14-15 14-16 15-16 10-11 11-12 12-13 13-1 16-22 22-10 16-17 17-21 21-22 17-18 18-19 18-20 19-20 20-18 20-21");
	iterm(g, {}).then(() => process.exit(0));
}
