const path   = require("path");
const gulp   = require("gulp");
const sass   = require("gulp-sass");
const gutil  = require("gulp-util");
const concat = require("gulp-concat");
const uglify = require("gulp-uglify");
const ts     = require("gulp-typescript");
const buffer = require("vinyl-buffer");
const source = require("vinyl-source-stream");
const sourcemaps = require("gulp-sourcemaps");
const browserify = require("browserify");
const babelify = require("babelify");
const tsify = require("tsify");
const tsc = require("typescript");

const debug = true;

const paths = {
	entry: "./wvm/browser/app.js",
	scriptsSrc: ["*.ts", "wvm/**/*.ts", "wasm/**/*.ts", "llvm/**/*.ts"],
	scriptsDst: "dist",
	tempDst: "dist/tmp"
};

const tsProject = ts.createProject("tsconfig.json");

gulp.task("sass", () =>
	gulp.src("wvm/style/app.scss")
		.pipe(sass().on("error", sass.logError))
		.pipe(gulp.dest("wvm/dist")));

gulp.task("sass:watch", () => gulp.watch("wvm/style/*.scss", ["sass"]));

// gulp.task("ts", () => tsProject
// 	.src()
// 	.pipe(sourcemaps.init())
// 	.pipe(ts(tsProject))
// 	.js
// 	.pipe(sourcemaps.write("", {debug, includeContent: true, sourceRoot: "."}))
// 	.pipe(gulp.dest(paths.tempDst)));


gulp.task("ts", () => 
	gulp.src(paths.scriptsSrc)
		.pipe(sourcemaps.init())
		.pipe(ts({
			noResolve: false,
			target: "es6",
			lib: ["es6", "esnext", "dom"],
			module: "commonjs",
			moduleResolution: "node",
			rootDirs: ["./", "./wvm", "./wasm", "./llvm"],
			// outDir: "./dist"
		}))
		.pipe(sourcemaps.write())
		.pipe(gulp.dest("dist")));

gulp.task("dev", gulp.series("ts", () =>
	browserify({entries: [paths.entry], debug: true})
		.on("error", gutil.log)
		.on("log", gutil.log)
		.plugin(tsify, {typescript: tsc})
		.transform(babelify.configure({
			presets: ["@babel/env"],
			plugins: [
				"syntax-class-properties",
				"transform-class-properties"
			]}))
		// .require("./dist/util.js", {expose: "../util"})
		// .require("./dist/parser.js", {expose: "../wasm/parser"})
		.require("./wasm/wasm.js", {expose: "./wasm.js"})
		.require("./dist/constants.js", {expose: "../dist/wasm/constants.js"})
		.bundle()
		.pipe(source("app.js"))
		.pipe(buffer())
		.pipe(gulp.dest("wvm/dist"))));

gulp.task("final", gulp.series("ts", () => {
    process.env.NODE_ENV = "production";
    return browserify({entries: [paths.entry], debug: false})
		.on("error", gutil.log)
		.on("log", gutil.log)
		.bundle()
        .pipe(source("app.js"))
        .pipe(buffer())
        .pipe(uglify())
        .pipe(gulp.dest("dist"));
}));

gulp.task("watch", function() {
    return gulp.watch([...paths.scriptsSrc, paths.entry], gulp.series("dev"));
});

// gulp.task("browserify", function() {
// 	const b = browserify({debug: true});
// 	b.on("update", this);
// 	b.on("log", log.info);
// });

gulp.task("default", gulp.parallel("sass", "dev", "watch"));

// gulp.task("default", gulp.parallel("sass", "ts"));
