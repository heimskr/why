// Generated automatically by nearley
// http://github.com/Hardmath123/nearley
(function () {
function id(x) {return x[0]; }

// Thanks to nanalan (https://github.com/nanalan/g) for providing a nearley sample I could use to teach myself how to use nearley
"use strict";

const special = {
	chars: "()<>. +-*/^\\!#='\",!{}:[]~$|&;",
	words: "+ - / * ^ -> < > <= >= = == [ ]".split(" ")
};

const colors = "1 3 2 6 4 5".split(" ");
const color = (x, n) => `${1 < n? "\n    " : ""}\u001b[3${colors[n % colors.length]}m${x instanceof Array || typeof x == "object"? JSON.stringify(x) : x}\u001b[0m`;
// const intercept = (out, ...dbg) => { console.log(...dbg.concat(out instanceof Array && out.length == 0? "[]" : (typeof out == "string"? `"${out}"` : out)).map(color), "\n"); return out };
const intercept = (out, ...dbg) => { console.log(...dbg.concat(JSON.stringify(out)).map(color), "\n"); return out };
const _ = intercept;
const __ = (out, ...dbg) => { console.log(...[out].concat(dbg).map(color), "\n"); return out };
const flatten = (arr) => arr.filter((e) => e != null).reduce((flat, toFlatten) => flat.filter((e) => e != null).concat(Array.isArray(toFlatten) ? flatten(toFlatten) : toFlatten), []);
const filter = (arr, index=null, ...remove) => arr.filter((item) => !(remove.length? remove : [null]).includes(typeof index == "number" && 0 <= index? item[index] : item));


function nth(n) {
    return function(d) {
        return d[n];
    };
}


function $(o) {
    return function(d) {
        var ret = {};
        Object.keys(o).forEach(function(k) {
            ret[k] = d[o[k]];
        });
        return ret;
    };
}
var grammar = {
    ParserRules: [
    {"name": "dqstring$ebnf$1", "symbols": []},
    {"name": "dqstring$ebnf$1", "symbols": ["dqstring$ebnf$1", "dstrchar"], "postprocess": function arrpush(d) {return d[0].concat([d[1]]);}},
    {"name": "dqstring", "symbols": [{"literal":"\""}, "dqstring$ebnf$1", {"literal":"\""}], "postprocess": function(d) {return d[1].join(""); }},
    {"name": "sqstring$ebnf$1", "symbols": []},
    {"name": "sqstring$ebnf$1", "symbols": ["sqstring$ebnf$1", "sstrchar"], "postprocess": function arrpush(d) {return d[0].concat([d[1]]);}},
    {"name": "sqstring", "symbols": [{"literal":"'"}, "sqstring$ebnf$1", {"literal":"'"}], "postprocess": function(d) {return d[1].join(""); }},
    {"name": "btstring$ebnf$1", "symbols": []},
    {"name": "btstring$ebnf$1", "symbols": ["btstring$ebnf$1", /[^`]/], "postprocess": function arrpush(d) {return d[0].concat([d[1]]);}},
    {"name": "btstring", "symbols": [{"literal":"`"}, "btstring$ebnf$1", {"literal":"`"}], "postprocess": function(d) {return d[1].join(""); }},
    {"name": "dstrchar", "symbols": [/[^\\"\n]/], "postprocess": id},
    {"name": "dstrchar", "symbols": [{"literal":"\\"}, "strescape"], "postprocess": 
        function(d) {
            return JSON.parse("\""+d.join("")+"\"");
        }
        },
    {"name": "sstrchar", "symbols": [/[^\\\n]/], "postprocess": id},
    {"name": "sstrchar", "symbols": [{"literal":"\\"}, "strescape"], "postprocess": function(d) { return JSON.parse("\""+d.join("")+"\""); }},
    {"name": "sstrchar$string$1", "symbols": [{"literal":"\\"}, {"literal":"'"}], "postprocess": function joiner(d) {return d.join('');}},
    {"name": "sstrchar", "symbols": ["sstrchar$string$1"], "postprocess": function(d) {return "'"; }},
    {"name": "strescape", "symbols": [/["\\\/bfnrt]/], "postprocess": id},
    {"name": "strescape", "symbols": [{"literal":"u"}, /[a-fA-F0-9]/, /[a-fA-F0-9]/, /[a-fA-F0-9]/, /[a-fA-F0-9]/], "postprocess": 
        function(d) {
            return d.join("");
        }
        },
    {"name": "main", "symbols": ["program"], "postprocess": d => _(d[0], "main", "d[0]", d)},
    {"name": "program$ebnf$1", "symbols": []},
    {"name": "program$ebnf$1", "symbols": ["program$ebnf$1", "newline"], "postprocess": function arrpush(d) {return d[0].concat([d[1]]);}},
    {"name": "program$ebnf$2", "symbols": []},
    {"name": "program$ebnf$2", "symbols": ["program$ebnf$2", "section"], "postprocess": function arrpush(d) {return d[0].concat([d[1]]);}},
    {"name": "program", "symbols": ["program$ebnf$1", "program$ebnf$2"], "postprocess": d => _(filter(d[1], 0, null, "\n"), "program", `filter(d[1], 0, null, "\\n")`, d)},
    {"name": "commands$ebnf$1", "symbols": []},
    {"name": "commands$ebnf$1", "symbols": ["commands$ebnf$1", "command"], "postprocess": function arrpush(d) {return d[0].concat([d[1]]);}},
    {"name": "commands", "symbols": ["commands$ebnf$1"], "postprocess": d => _(filter(d[0]), "commands", "filter(d[0])", d)},
    {"name": "command", "symbols": ["_", "newline"], "postprocess": d => null},
    {"name": "separator$subexpression$1", "symbols": [{"literal":";"}]},
    {"name": "separator$subexpression$1", "symbols": ["newline"]},
    {"name": "separator", "symbols": ["_", "separator$subexpression$1"], "postprocess": d => null},
    {"name": "float$ebnf$1", "symbols": [/[0-9]/]},
    {"name": "float$ebnf$1", "symbols": ["float$ebnf$1", /[0-9]/], "postprocess": function arrpush(d) {return d[0].concat([d[1]]);}},
    {"name": "float$ebnf$2", "symbols": [/[0-9]/]},
    {"name": "float$ebnf$2", "symbols": ["float$ebnf$2", /[0-9]/], "postprocess": function arrpush(d) {return d[0].concat([d[1]]);}},
    {"name": "float", "symbols": ["float$ebnf$1", {"literal":"."}, "float$ebnf$2"], "postprocess": d => parseFloat(d[0].join("") + d[1] + d[2].join(""))},
    {"name": "int$ebnf$1", "symbols": [/[0-9]/]},
    {"name": "int$ebnf$1", "symbols": ["int$ebnf$1", /[0-9]/], "postprocess": function arrpush(d) {return d[0].concat([d[1]]);}},
    {"name": "int", "symbols": ["int$ebnf$1"], "postprocess": d => parseInt(d[0].join(""))},
    {"name": "string", "symbols": ["dqstring"], "postprocess": d => _(d[0], "dqstring", "d[0]", d)},
    {"name": "value$subexpression$1", "symbols": ["float"]},
    {"name": "value$subexpression$1", "symbols": ["int"]},
    {"name": "value$subexpression$1", "symbols": ["string"]},
    {"name": "value", "symbols": ["value$subexpression$1"], "postprocess": d => _(d[0][0], "value", "d[0][0]", d)},
    {"name": "section$subexpression$1", "symbols": ["data_section"]},
    {"name": "section$subexpression$1", "symbols": ["code_section"]},
    {"name": "section", "symbols": ["section$subexpression$1"], "postprocess": d => _(d[0][0], "section", "d[0][0]", d)},
    {"name": "data_section$string$1", "symbols": [{"literal":"<"}, {"literal":"d"}, {"literal":"a"}, {"literal":"t"}, {"literal":"a"}, {"literal":">"}], "postprocess": function joiner(d) {return d.join('');}},
    {"name": "data_section$ebnf$1", "symbols": []},
    {"name": "data_section$ebnf$1", "symbols": ["data_section$ebnf$1", "datadef"], "postprocess": function arrpush(d) {return d[0].concat([d[1]]);}},
    {"name": "data_section", "symbols": ["_", "data_section$string$1", "data_section$ebnf$1"], "postprocess": d => _(["data", filter(d[2])], "data_section", `["data", filter(d[2])]`, d)},
    {"name": "datadef", "symbols": ["_", "newline"], "postprocess": d => null},
    {"name": "datadef", "symbols": ["_", "var", "_", {"literal":":"}, "_", "float", "separator"], "postprocess": d => _(["float",  d[1], d[5]], "datadef", `["float",  d[1], d[5]]`, d)},
    {"name": "datadef", "symbols": ["_", "var", "_", {"literal":":"}, "_", "int", "separator"], "postprocess": d => _(["int",    d[1], d[5]], "datadef", `["int",    d[1], d[5]]`, d)},
    {"name": "datadef", "symbols": ["_", "var", "_", {"literal":":"}, "_", "string", "separator"], "postprocess": d => _(["string", d[1], d[5]], "datadef", `["string", d[1], d[5]]`, d)},
    {"name": "var$ebnf$1", "symbols": ["varchar"]},
    {"name": "var$ebnf$1", "symbols": ["var$ebnf$1", "varchar"], "postprocess": function arrpush(d) {return d[0].concat([d[1]]);}},
    {"name": "var", "symbols": ["var$ebnf$1"], "postprocess": 
        (d, _, no) => {
        	const identifier = d[0].join("");
        	return /[0-9]/.test(identifier.charAt(0)) || special.words.indexOf(identifier) !== -1? no : identifier;
        }
        },
    {"name": "varchar", "symbols": [/./], "postprocess": (d, _, no) => d[0] && special.chars.indexOf(d[0]) === -1 ? d[0] : no},
    {"name": "newline", "symbols": [{"literal":"\r"}, {"literal":"\n"}]},
    {"name": "newline", "symbols": [{"literal":"\r"}]},
    {"name": "newline", "symbols": [{"literal":"\n"}]},
    {"name": "_$ebnf$1", "symbols": []},
    {"name": "_$ebnf$1", "symbols": ["_$ebnf$1", /[\t ]/], "postprocess": function arrpush(d) {return d[0].concat([d[1]]);}},
    {"name": "_", "symbols": ["_$ebnf$1"], "postprocess": d => null},
    {"name": "__$ebnf$1", "symbols": [/[\t ]/]},
    {"name": "__$ebnf$1", "symbols": ["__$ebnf$1", /[\t ]/], "postprocess": function arrpush(d) {return d[0].concat([d[1]]);}},
    {"name": "__", "symbols": ["__$ebnf$1"], "postprocess": d => null}
]
  , ParserStart: "main"
}
if (typeof module !== 'undefined'&& typeof module.exports !== 'undefined') {
   module.exports = grammar;
} else {
   window.grammar = grammar;
}
})();
