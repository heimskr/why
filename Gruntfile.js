let shellescape = require("shell-escape"),
	child_process = require("child_process"),
	path = require("path"),
	chalk = require("chalk");

module.exports = function(grunt) {
	const jsdoc_files = ["*.js", "wasm/*.js", "wvm/*.js", "wvm/browser/*.js", "llvm/*.js"];
	grunt.initConfig({
		pkg: grunt.file.readJSON("package.json"),

		watch: {
			sass: {
				files: "wvm/style/*.scss",
				tasks: ["sass"],
				options: {
					spawn: false
				}
			},

			browserify: {
				files: ["wvm/dist/app.js"],
				options: { spawn: false },
				tasks: ["exorcise"]
			},

			wasm: {
				files: ["**/*.wasm"],
				tasks: ["wasmc"]
			},

			nearley: {
				files: ["wasm/wasm.ne"],
				tasks: ["nearley"]
			},

//			jsdoc: {
//				files: ["jsdoc.json", ...jsdoc_files],
//				tasks: ["jsdoc"]
//			}
		},

		exorcise: {
			bundle: {
				options: { },
				files: {
					"wvm/dist/app.js.map": ["wvm/dist/app.js"]
				}
			}
		},

		sass: {
			options: {
				sourceMap: true
			},

			dist: {
				files: {
					"wvm/dist/app.css": "wvm/style/app.scss"
				}
			}
		},

		browserify: {
			dev: {
				src: ["wvm/browser/**/*.js", "wvm/browser/**/*.jsx"],
				dest: "wvm/dist/app.js",
				options: {
					transform: [
						["babelify", {
							presets: [["env", {targets: {chrome: 64}}]],
							plugins: [
								"syntax-class-properties",
								"transform-class-properties"
							]
						}],
						["brfs"]
					],
					watch: true,
					keepAlive: false,
					browserifyOptions: {
						debug: true,
					}
				}
			}
		},

		jsdoc: {
			dist: {
				src: jsdoc_files,
				options: {
					destination: "wvm/dist/doc",
					config: "jsdoc.json",
					template: "./node_modules/docdash"
				}
			}
		}
	});

	grunt.loadNpmTasks("grunt-contrib-watch");
	grunt.loadNpmTasks("grunt-sass");
	grunt.loadNpmTasks("grunt-browserify");
	grunt.loadNpmTasks("grunt-babel");
	grunt.loadNpmTasks("grunt-exorcise");
	grunt.loadNpmTasks("grunt-jsdoc");

	grunt.registerTask("wasmc", "Dummy task for wasmc.js.", () => { });
	grunt.registerTask("nearley", "Compiles WASM's nearley source.", () => {
		child_process.exec("node node_modules/nearley/bin/nearleyc.js wasm/wasm.ne -o wasm/wasm.js", (error, stdout, stderr) => {
			if (error) {
				console.error(chalk.red(`Couldn't compile wasm.ne:`));
				console.error(chalk.red.dim(error.message));
			} else if (stderr) {
				console.error(`Error during compilation:`, stderr);
			} else {
				console.log(chalk.green(stdout));
			};
		});
	});

	grunt.event.on("watch", (action, file, name) => {
		// There's almost certainly a better way to do this.
		if (name == "wasm") {
			child_process.exec(shellescape(["node", "wasm/wld.js", file, "-o", `wasm/compiled/${path.basename(file).replace(/\.wasm$/i, "")}.why`]), (error, stdout, stderr) => {
				if (error) {
					console.error(chalk.red(`Couldn't compile ${file}:`));
					console.error(chalk.red(error.message));
				} else if (stderr) {
					console.error(`Error during compilation:`, stderr);
				} else if (stdout.match(/Successfully linked/)) {
					console.log(chalk.green(stdout));
				} else {
					console.log("???", stdout);
				};
			});
		};
	});

	grunt.registerTask("default", ["browserify:dev", "jsdoc", "nearley", "sass", "watch"]);
};
