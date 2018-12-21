; ModuleID = 'registers.c'
source_filename = "registers.c"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.14.0"

@.str = private unnamed_addr constant [2 x i8] c"0\00", align 1
@.str.1 = private unnamed_addr constant [2 x i8] c"g\00", align 1
@.str.2 = private unnamed_addr constant [3 x i8] c"sp\00", align 1
@.str.3 = private unnamed_addr constant [3 x i8] c"fp\00", align 1
@.str.4 = private unnamed_addr constant [3 x i8] c"rt\00", align 1
@.str.5 = private unnamed_addr constant [3 x i8] c"lo\00", align 1
@.str.6 = private unnamed_addr constant [3 x i8] c"hi\00", align 1
@.str.7 = private unnamed_addr constant [3 x i8] c"r0\00", align 1
@.str.8 = private unnamed_addr constant [3 x i8] c"r1\00", align 1
@.str.9 = private unnamed_addr constant [3 x i8] c"r2\00", align 1
@.str.10 = private unnamed_addr constant [3 x i8] c"r3\00", align 1
@.str.11 = private unnamed_addr constant [3 x i8] c"r4\00", align 1
@.str.12 = private unnamed_addr constant [3 x i8] c"r5\00", align 1
@.str.13 = private unnamed_addr constant [3 x i8] c"r6\00", align 1
@.str.14 = private unnamed_addr constant [3 x i8] c"r7\00", align 1
@.str.15 = private unnamed_addr constant [3 x i8] c"r8\00", align 1
@.str.16 = private unnamed_addr constant [3 x i8] c"r9\00", align 1
@.str.17 = private unnamed_addr constant [3 x i8] c"ra\00", align 1
@.str.18 = private unnamed_addr constant [3 x i8] c"rb\00", align 1
@.str.19 = private unnamed_addr constant [3 x i8] c"rc\00", align 1
@.str.20 = private unnamed_addr constant [3 x i8] c"rd\00", align 1
@.str.21 = private unnamed_addr constant [3 x i8] c"re\00", align 1
@.str.22 = private unnamed_addr constant [3 x i8] c"rf\00", align 1
@.str.23 = private unnamed_addr constant [3 x i8] c"a0\00", align 1
@.str.24 = private unnamed_addr constant [3 x i8] c"a1\00", align 1
@.str.25 = private unnamed_addr constant [3 x i8] c"a2\00", align 1
@.str.26 = private unnamed_addr constant [3 x i8] c"a3\00", align 1
@.str.27 = private unnamed_addr constant [3 x i8] c"a4\00", align 1
@.str.28 = private unnamed_addr constant [3 x i8] c"a5\00", align 1
@.str.29 = private unnamed_addr constant [3 x i8] c"a6\00", align 1
@.str.30 = private unnamed_addr constant [3 x i8] c"a7\00", align 1
@.str.31 = private unnamed_addr constant [3 x i8] c"a8\00", align 1
@.str.32 = private unnamed_addr constant [3 x i8] c"a9\00", align 1
@.str.33 = private unnamed_addr constant [3 x i8] c"aa\00", align 1
@.str.34 = private unnamed_addr constant [3 x i8] c"ab\00", align 1
@.str.35 = private unnamed_addr constant [3 x i8] c"ac\00", align 1
@.str.36 = private unnamed_addr constant [3 x i8] c"ad\00", align 1
@.str.37 = private unnamed_addr constant [3 x i8] c"ae\00", align 1
@.str.38 = private unnamed_addr constant [3 x i8] c"af\00", align 1
@.str.39 = private unnamed_addr constant [3 x i8] c"t0\00", align 1
@.str.40 = private unnamed_addr constant [3 x i8] c"t1\00", align 1
@.str.41 = private unnamed_addr constant [3 x i8] c"t2\00", align 1
@.str.42 = private unnamed_addr constant [3 x i8] c"t3\00", align 1
@.str.43 = private unnamed_addr constant [3 x i8] c"t4\00", align 1
@.str.44 = private unnamed_addr constant [3 x i8] c"t5\00", align 1
@.str.45 = private unnamed_addr constant [3 x i8] c"t6\00", align 1
@.str.46 = private unnamed_addr constant [3 x i8] c"t7\00", align 1
@.str.47 = private unnamed_addr constant [3 x i8] c"t8\00", align 1
@.str.48 = private unnamed_addr constant [3 x i8] c"t9\00", align 1
@.str.49 = private unnamed_addr constant [3 x i8] c"ta\00", align 1
@.str.50 = private unnamed_addr constant [3 x i8] c"tb\00", align 1
@.str.51 = private unnamed_addr constant [3 x i8] c"tc\00", align 1
@.str.52 = private unnamed_addr constant [3 x i8] c"td\00", align 1
@.str.53 = private unnamed_addr constant [3 x i8] c"te\00", align 1
@.str.54 = private unnamed_addr constant [3 x i8] c"tf\00", align 1
@.str.55 = private unnamed_addr constant [4 x i8] c"t10\00", align 1
@.str.56 = private unnamed_addr constant [4 x i8] c"t11\00", align 1
@.str.57 = private unnamed_addr constant [4 x i8] c"t12\00", align 1
@.str.58 = private unnamed_addr constant [4 x i8] c"t13\00", align 1
@.str.59 = private unnamed_addr constant [4 x i8] c"t14\00", align 1
@.str.60 = private unnamed_addr constant [4 x i8] c"t15\00", align 1
@.str.61 = private unnamed_addr constant [4 x i8] c"t16\00", align 1
@.str.62 = private unnamed_addr constant [3 x i8] c"s0\00", align 1
@.str.63 = private unnamed_addr constant [3 x i8] c"s1\00", align 1
@.str.64 = private unnamed_addr constant [3 x i8] c"s2\00", align 1
@.str.65 = private unnamed_addr constant [3 x i8] c"s3\00", align 1
@.str.66 = private unnamed_addr constant [3 x i8] c"s4\00", align 1
@.str.67 = private unnamed_addr constant [3 x i8] c"s5\00", align 1
@.str.68 = private unnamed_addr constant [3 x i8] c"s6\00", align 1
@.str.69 = private unnamed_addr constant [3 x i8] c"s7\00", align 1
@.str.70 = private unnamed_addr constant [3 x i8] c"s8\00", align 1
@.str.71 = private unnamed_addr constant [3 x i8] c"s9\00", align 1
@.str.72 = private unnamed_addr constant [3 x i8] c"sa\00", align 1
@.str.73 = private unnamed_addr constant [3 x i8] c"sb\00", align 1
@.str.74 = private unnamed_addr constant [3 x i8] c"sc\00", align 1
@.str.75 = private unnamed_addr constant [3 x i8] c"sd\00", align 1
@.str.76 = private unnamed_addr constant [3 x i8] c"se\00", align 1
@.str.77 = private unnamed_addr constant [3 x i8] c"sf\00", align 1
@.str.78 = private unnamed_addr constant [4 x i8] c"s10\00", align 1
@.str.79 = private unnamed_addr constant [4 x i8] c"s11\00", align 1
@.str.80 = private unnamed_addr constant [4 x i8] c"s12\00", align 1
@.str.81 = private unnamed_addr constant [4 x i8] c"s13\00", align 1
@.str.82 = private unnamed_addr constant [4 x i8] c"s14\00", align 1
@.str.83 = private unnamed_addr constant [4 x i8] c"s15\00", align 1
@.str.84 = private unnamed_addr constant [4 x i8] c"s16\00", align 1
@.str.85 = private unnamed_addr constant [3 x i8] c"k0\00", align 1
@.str.86 = private unnamed_addr constant [3 x i8] c"k1\00", align 1
@.str.87 = private unnamed_addr constant [3 x i8] c"k2\00", align 1
@.str.88 = private unnamed_addr constant [3 x i8] c"k3\00", align 1
@.str.89 = private unnamed_addr constant [3 x i8] c"k4\00", align 1
@.str.90 = private unnamed_addr constant [3 x i8] c"k5\00", align 1
@.str.91 = private unnamed_addr constant [3 x i8] c"k6\00", align 1
@.str.92 = private unnamed_addr constant [3 x i8] c"k7\00", align 1
@.str.93 = private unnamed_addr constant [3 x i8] c"k8\00", align 1
@.str.94 = private unnamed_addr constant [3 x i8] c"k9\00", align 1
@.str.95 = private unnamed_addr constant [3 x i8] c"ka\00", align 1
@.str.96 = private unnamed_addr constant [3 x i8] c"kb\00", align 1
@.str.97 = private unnamed_addr constant [3 x i8] c"kc\00", align 1
@.str.98 = private unnamed_addr constant [3 x i8] c"kd\00", align 1
@.str.99 = private unnamed_addr constant [3 x i8] c"ke\00", align 1
@.str.100 = private unnamed_addr constant [3 x i8] c"kf\00", align 1
@.str.101 = private unnamed_addr constant [4 x i8] c"k10\00", align 1
@.str.102 = private unnamed_addr constant [3 x i8] c"m0\00", align 1
@.str.103 = private unnamed_addr constant [3 x i8] c"m1\00", align 1
@.str.104 = private unnamed_addr constant [3 x i8] c"m2\00", align 1
@.str.105 = private unnamed_addr constant [3 x i8] c"m3\00", align 1
@.str.106 = private unnamed_addr constant [3 x i8] c"m4\00", align 1
@.str.107 = private unnamed_addr constant [3 x i8] c"m5\00", align 1
@.str.108 = private unnamed_addr constant [3 x i8] c"m6\00", align 1
@.str.109 = private unnamed_addr constant [3 x i8] c"m7\00", align 1
@.str.110 = private unnamed_addr constant [3 x i8] c"m8\00", align 1
@.str.111 = private unnamed_addr constant [3 x i8] c"m9\00", align 1
@.str.112 = private unnamed_addr constant [3 x i8] c"ma\00", align 1
@.str.113 = private unnamed_addr constant [3 x i8] c"mb\00", align 1
@.str.114 = private unnamed_addr constant [3 x i8] c"mc\00", align 1
@.str.115 = private unnamed_addr constant [3 x i8] c"md\00", align 1
@.str.116 = private unnamed_addr constant [3 x i8] c"me\00", align 1
@.str.117 = private unnamed_addr constant [3 x i8] c"mf\00", align 1
@.str.118 = private unnamed_addr constant [3 x i8] c"f0\00", align 1
@.str.119 = private unnamed_addr constant [3 x i8] c"f1\00", align 1
@.str.120 = private unnamed_addr constant [3 x i8] c"f2\00", align 1
@.str.121 = private unnamed_addr constant [3 x i8] c"f3\00", align 1
@.str.122 = private unnamed_addr constant [3 x i8] c"e0\00", align 1
@.str.123 = private unnamed_addr constant [3 x i8] c"e1\00", align 1
@.str.124 = private unnamed_addr constant [3 x i8] c"e2\00", align 1
@.str.125 = private unnamed_addr constant [3 x i8] c"e3\00", align 1
@.str.126 = private unnamed_addr constant [3 x i8] c"e4\00", align 1
@.str.127 = private unnamed_addr constant [3 x i8] c"e5\00", align 1
@.str.128 = private unnamed_addr constant [2 x i8] c"?\00", align 1
@switch.table = private unnamed_addr constant
	[128 x i8*]
	[i8* getelementptr inbounds (
		[2 x i8],
		[2 x i8]* @.str,
		i64 0,
		i64 0),
	i8* getelementptr inbounds (
		[2 x i8],
		[2 x i8]* @.str.1,
		i64 0,
		i64 0),
	i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.2, i64 0, i64 0),
	i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.3, i64 0, i64 0),
	i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.4, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.5, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.6, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.7, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.8, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.9, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.10, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.11, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.12, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.13, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.14, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.15, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.16, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.17, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.18, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.19, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.20, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.21, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.22, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.23, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.24, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.25, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.26, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.27, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.28, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.29, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.30, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.31, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.32, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.33, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.34, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.35, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.36, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.37, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.38, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.39, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.40, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.41, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.42, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.43, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.44, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.45, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.46, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.47, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.48, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.49, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.50, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.51, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.52, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.53, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.54, i64 0, i64 0), i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.55, i64 0, i64 0), i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.56, i64 0, i64 0), i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.57, i64 0, i64 0), i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.58, i64 0, i64 0), i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.59, i64 0, i64 0), i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.60, i64 0, i64 0), i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.61, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.62, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.63, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.64, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.65, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.66, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.67, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.68, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.69, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.70, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.71, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.72, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.73, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.74, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.75, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.76, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.77, i64 0, i64 0), i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.78, i64 0, i64 0), i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.79, i64 0, i64 0), i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.80, i64 0, i64 0), i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.81, i64 0, i64 0), i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.82, i64 0, i64 0), i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.83, i64 0, i64 0), i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.84, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.85, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.86, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.87, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.88, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.89, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.90, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.91, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.92, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.93, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.94, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.95, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.96, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.97, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.98, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.99, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.100, i64 0, i64 0), i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str.101, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.102, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.103, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.104, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.105, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.106, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.107, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.108, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.109, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.110, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.111, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.112, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.113, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.114, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.115, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.116, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.117, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.118, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.119, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.120, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.121, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.122, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.123, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.124, i64 0, i64 0), i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.125, i64 0, i64 0),
	i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.126, i64 0, i64 0),
	i8* getelementptr inbounds ([3 x i8], [3 x i8]* @.str.127, i64 0, i64 0)
]

; Function Attrs: norecurse nounwind readnone ssp uwtable
define i8* @wvm_decode_reg(i8 zeroext) local_unnamed_addr #0 {
  %2 = icmp sgt i8 %0, -1
  br i1 %2, label %3, label %7

; <label>:3:                                      ; preds = %1
  %4 = sext i8 %0 to i64
  %5 = getelementptr inbounds [128 x i8*], [128 x i8*]* @switch.table, i64 0, i64 %4
  %6 = load i8*, i8** %5, align 8
  ret i8* %6

; <label>:7:                                      ; preds = %1
  ret i8* getelementptr inbounds ([2 x i8], [2 x i8]* @.str.128, i64 0, i64 0)
}

attributes #0 = { norecurse nounwind readnone ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"PIC Level", i32 2}
!1 = !{!"clang version 4.0.0 (tags/RELEASE_400/final)"}
