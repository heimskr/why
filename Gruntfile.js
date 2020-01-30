let shellescape = require("shell-escape"),
	child_process = require("child_process"),
	path = require("path"),
	chalk = require("chalk"),
	sass = require("node-sass");

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
				// tasks: ["exorcise"]
			},
			
			wasm: {
				files: ["**/*.wasm"],
				tasks: ["wasmc"]
			},

			// ts: {
			// 	files: ["**/*.ts"],
			// 	tasks: ["ts"]
			// },
			
			nearley_wasm: { files: ["wasm/wasm.ne"], tasks: ["nearley_wasm"] },
			nearley_llvm: { files: ["llvm/llvm.ne"], tasks: ["nearley_llvm"] },
			
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
				implementation: sass,
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
				// src: ["dist/wvm/browser/**/*.js", "dist/wvm/browser/**/*.jsx", "wvm/browser/*.js"],
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
						["brfs"],
					],
					watch: true,
					keepAlive: false,
					browserifyOptions: {
						// debug: true,
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
		},

		ts: {
			options: {
				compile: true,
				// target: "es6",
				// lib: ["es5", "es6", "ESNext"],
				// module: "commonjs",
				sourceMap: true,
				// noImplicitAny: false,
				fast: "watch",
				rootDir: ".",
				// strict: true
			},
			default: {
				src: ["*.ts", "wasm/*.ts", "wvm/**/*.ts"],
				// outDir: "dist"
			}
		}
	});
	
	grunt.loadNpmTasks("grunt-contrib-watch");
	grunt.loadNpmTasks("grunt-sass");
	grunt.loadNpmTasks("grunt-browserify");
	grunt.loadNpmTasks("grunt-babel");
	grunt.loadNpmTasks("grunt-exorcise");
	grunt.loadNpmTasks("grunt-jsdoc");
	grunt.loadNpmTasks("grunt-ts");
	
	grunt.registerTask("wasmc", "Dummy task for wasmc.js.", () => { });

	function registerNearley(path, id) {
		grunt.registerTask(`nearley_${id}`, `Compiles nearley source for ${path}.ne.`, function() {
			child_process.exec(`node node_modules/nearley/bin/nearleyc.js ${path}.ne -o dist/${path}.js`, (error, stdout, stderr) => {
				if (error) {
					console.error(chalk.red(`Couldn't compile ${path}.ne:`));
					console.error(chalk.red.dim(error.message));
				} else if (stderr) {
					console.error(`Error while compilating ${path}.ne:`, stderr);
				} else {
					console.log(chalk.green(stdout));
				};
			});
		});
	}

	registerNearley("wasm/wasm", "wasm");
	registerNearley("llvm/llvm", "llvm");
	
	grunt.event.on("watch", (action, file, name) => {
		// There's almost certainly a better way to do this.
		if (name == "wasm") {
			child_process.exec(shellescape(["ts-node", "wasm/wld.ts", file, "-o", `wasm/compiled/${path.basename(file).replace(/\.wasm$/i, "")}.why`]), (error, stdout, stderr) => {
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
	
	// grunt.registerTask("default", ["browserify:dev", "jsdoc", "nearley", "sass", "watch"]);
	// grunt.registerTask("default", ["ts", "nearley_wasm", "nearley_llvm", "browserify:dev", "watch"]);
	grunt.registerTask("default", ["nearley_wasm", "nearley_llvm", "browserify:dev", "sass", "watch"]);
};
