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

const debug = true;

const paths = {
	entry: "./wvm/browser/app.js",
	scriptsSrc: ["*.ts", "wvm/**/*.ts", "wasm/**/*.ts", "llvm/**/*.ts"],
	scriptsDst: "dist",
	outDev: "bundle.dev.js",
	outFinal: "bundle.js",
	tempDst: "dist/tmp"
};

const tsProject = ts.createProject("tsconfig.json", {noResolve: false});

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
		.pipe(tsProject())
		.pipe(sourcemaps.write(".", {includeContent: false, sourceRoot: ".."}))
		.pipe(gulp.dest("dist")));

gulp.task("dev", gulp.series("ts"), () => {
	return browserify({entries: [paths.entry], debug: true})
		.bundle()
		.on("error", gutil.log)
		.on("log", gutil.log)
		.pipe(source("app.js"))
		.pipe(buffer())
		.pipe(gulp.dest("wvm/dist"))
});

gulp.task("final", gulp.series("ts"), () => {
    process.env.NODE_ENV = "production";
    return browserify({entries: [paths.entry], debug: false})
		.bundle()
		.on("error", gutil.log)
		.on("log", gutil.log)
        .pipe(source(path.join(paths.scriptsDst, paths.outFinal)))
        .pipe(buffer())
        .pipe(uglify())
        .pipe(gulp.dest("dist"));
});

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
