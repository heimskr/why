class WhyJSReg<bits<16> Enc, string n> : Register<n> {
	let HWEncoding = Enc;
	let Namespace = "WhyJS";
}

def ZERO : R<0, "$0">;
def G    : R<1, "$g">;
def SP   : R<2, "$sp">;
def FP   : R<3, "$fp">;
def RT   : R<4, "$rt">;
def LO   : R<5, "$lo">;
def HI   : R<6, "$hi">;

foreach i = 0-9 in { def R#i : R<i+7, "$r"#i >; }
def RA : R<17, "$ra">;
def RB : R<18, "$rb">;
def RC : R<19, "$rc">;
def RD : R<20, "$rd">;
def RE : R<21, "$re">;
def RF : R<22, "$rf">;

foreach i = 0-9 in { def A#i : R<i+23, "$a"#i >; }
def AA : R<33, "$aa">; 
def AB : R<34, "$ab">;
def AC : R<35, "$ac">;
def AD : R<36, "$ad">;
def AE : R<37, "$ae">;
def AF : R<38, "$af">;

foreach i =  0-9  in { def T#i : R<i+39, "$t"#i >; }
foreach i = 10-16 in { def T#i : R<i+45, "$t"#i >; }
def TA : R<49, "$ta">; 
def TB : R<50, "$tb">;
def TC : R<51, "$tc">;
def TD : R<52, "$td">;
def TE : R<53, "$te">;
def TF : R<54, "$tf">;

foreach i =  0-9  in { def S#i : R<i+62, "$s"#i >; }
foreach i = 10-16 in { def S#i : R<i+68, "$s"#i >; }
def SA : R<72, "$sa">; 
def SB : R<73, "$sb">;
def SC : R<74, "$sc">;
def SD : R<75, "$sd">;
def SE : R<76, "$se">;
def SF : R<77, "$sf">;

foreach i =  0-9 in  { def K#i : R<i+85, "$k"#i >; }
def KA  : R< 95, "$ka">; 
def KB  : R< 96, "$kb">;
def KC  : R< 97, "$kc">;
def KD  : R< 98, "$kd">;
def KE  : R< 99, "$ke">;
def KF  : R<100, "$kf">;
def K10 : R<101, "$k10">;

foreach i = 0-9 in { def M#i : R<i+102, "$m"#i >; }
def MA : R<112, "$ma">; 
def MB : R<113, "$mb">;
def MC : R<114, "$mc">;
def MD : R<115, "$md">;
def ME : R<116, "$me">;
def MF : R<117, "$mf">;

foreach i = 0-3 in { def F#i : R<i+118, "$f"#i >; }
foreach i = 0-5 in { def E#i : R<i+122, "$e"#i >; }

def GRRegs : RegisterClass<"WhyJS", [i64], 64, (add
	SP,
	R0, R1, R2, R3, R4, R5, R6, R7, R8, R9, RA, RB, RC, RD, RE, RF,
	A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, AA, AB, AC, AD, AE, AF,
	T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, TA, TB, TC, TD, TE, TF, T10, T11, T12, T13, T14, T15, T16,
	S0, S1, S2, S3, S4, S5, S6, S7, S8, S9, SA, SB, SC, SD, SE, SF, S10, S11, S12, S13, S14, S15, S16
	// K0, K1, K2, K3, K4, K5, K6, K7, K8, K9, KA, KB, KC, KD, KE, KF, K10,
	// M0, M1, M2, M3, M4, M5, M6, M7, M8, M9, MA, MB, MC, MD, ME, MF,
	// F0, F1, F2, F3,
	// E0, E1, E2, E3, E4, E5,
)>;