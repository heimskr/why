# Table of Contents
<blockquote>
	<ol>
		<li><a href="#intro">Introduction</a>
		<li><a href="#registers">Registers</a>
			<ol>
				<li><a href="#reg-st">Status Register</a></li>
			</ol>
		</li>
		<li><a href="#prog">Programs</a>
			<ol>
				<li><a href="#prog-meta">Metadata Section</a></li>
				<li><a href="#prog-code">Code Section</a></li>
				<li><a href="#prog-data">Data Section</a></li>
				<li><a href="#prog-symtab">Symbol Table</a></li>
				<li><a href="#prog-reloc">Relocation Data Section</a></li>
				<li><a href="#prog-debug">Debug Data Section</a></li>
			</ol>
		</li>
		<li><a href="#directives">Directives</a>
			<ol>
				<li><a href="#dir-code"><code>%code</code></a></li>
				<li><a href="#dir-data"><code>%data</code></a></li>
				<li><a href="#dir-type"><code>%type</code></a></li>
				<li><a href="#dir-size"><code>%size</code></a></li>
				<li><a href="#dir-string"><code>%string</code></a></li>
				<li><a href="#dir-stringz"><code>%stringz</code></a></li>
				<li><a href="#dir-value"><code>%8b, %4b, %2b, %1b</code></a></li>
				<li><a href="#dir-align"><code>%align</code></a></li>
				<li><a href="#dir-fill"><code>%fill</code></a></li>
			</ol>
		</li>
		<li><a href="#rings">Rings</a></li>
		<li><a href="#interrupts">Interrupts</a>
			<ol>
				<li><a href="#int-system"><code>SYSTEM</code></a>
				<li><a href="#int-timer"><code>TIMER</code></a>
				<li><a href="#int-protec"><code>PROTEC</code></a>
				<li><a href="#int-pfault"><code>PFAULT</code></a>
				<li><a href="#int-inexec"><code>INEXEC</code></a>
				<li><a href="#int-bwrite"><code>BWRITE</code></a>
				<li><a href="#int-keybrd"><code>KEYBRD</code></a>
			</ol>
		</li>
		<li><a href="#paging">Paging</a></li>
		<li><a href="#format">Instruction Format</a>
			<ol>
				<li><a href="#format-r">R-Type Instructions</a></li>
				<li><a href="#format-i">I-Type Instructions</a></li>
				<li><a href="#format-j">J-Type Instructions</a></li>
				<li><a href="#linkerflags">Linker Flags</a></li>
				<li><a href="#condbits">Condition Bits</a></li>
			</ol>
		</li>
		<li><a href="#operations">Operations</a>
			<ol>
				<li><a href="#ops-math-r">Math (R-Types)</a>
					<ol>
						<li><a href="#op-add">Add</a> (<code>add</code>)</li>
						<li><a href="#op-sub">Subtract</a> (<code>sub</code>)</li>
						<li><a href="#op-mult">Multiply</a> (<code>mult</code>)</li>
						<li><a href="#op-multu">Multiply Unsigned</a> (<code>multu</code>)</li>
						<li><a href="#op-sll">Shift Left Logical</a> (<code>sll</code>)</li>
						<li><a href="#op-srl">Shift Right Logical</a> (<code>srl</code>)</li>
						<li><a href="#op-sra">Shift Right Arithmetic</a> (<code>sra</code>)</li>
						<li><a href="#op-mod">Modulo</a> (<code>mod</code>)</li>
						<li><a href="#op-div">Divide</a> (<code>div</code>)</li>
						<li><a href="#op-divu">Divide Unsigned</a> (<code>divu</code>)</li>
						<li><a href="#op-modu">Modulo Unsigned</a> (<code>modu</code>)</li>
						<li><a href="#op-sext32">Sign Extend 32 to 64</a> (<code>sext32</code>)</li>
					</ol>
				</li>
				<li><a href="#ops-logic-r">Logic (R-Types)</a>
					<ol>
						<li><a href="#op-and">Bitwise AND</a>    (<code>and</code>)</li>
						<li><a href="#op-nand">Bitwise NAND</a>  (<code>nand</code>)</li>
						<li><a href="#op-nor">Bitwise NOR</a>    (<code>nor</code>)</li>
						<li><a href="#op-not">Bitwise NOT</a>    (<code>not</code>)</li>
						<li><a href="#op-or">Bitwise OR</a>      (<code>or</code>)</li>
						<li><a href="#op-xnor">Bitwise XNOR</a>  (<code>xnor</code>)</li>
						<li><a href="#op-xor">Bitwise XOR</a>    (<code>xor</code>)</li>
						<li><a href="#op-land">Logical AND</a>   (<code>land</code>)</li>
						<li><a href="#op-lnand">Logical NAND</a> (<code>lnand</code>)</li>
						<li><a href="#op-lnor">Logical NOR</a>   (<code>lnor</code>)</li>
						<li><a href="#op-lnot">Logical NOT</a>   (<code>lnot</code>)</li>
						<li><a href="#op-lor">Logical OR</a>     (<code>lor</code>)</li>
						<li><a href="#op-lxnor">Logical XNOR</a> (<code>lxnor</code>)</li>
						<li><a href="#op-lxor">Logical XOR</a>   (<code>lxor</code>)</li>
					</ol>
				</li>
				<li><a href="#ops-math-i">Math (I-Types)</a>
					<ol>
						<li><a href="#op-addi">Add Immediate</a> (<code>addi</code>)</li>
						<li><a href="#op-subi">Subtract Immediate</a> (<code>subi</code>)</li>
						<li><a href="#op-multi">Multiply Immediate</a> (<code>multi</code>)</li>
						<li><a href="#op-multui">Multiply Unsigned Immediate</a> (<code>multui</code>)</li>
						<li><a href="#op-slli">Shift Left Logical Immediate</a> (<code>slli</code>)</li>
						<li><a href="#op-srli">Shift Right Logical Immediate</a> (<code>srli</code>)</li>
						<li><a href="#op-srai">Shift Right Arithmetic Immediate</a> (<code>srai</code>)</li>
						<li><a href="#op-sllii">Shift Left Logical Inverse Immediate</a> (<code>sllii</code>)</li>
						<li><a href="#op-srlii">Shift Right Logical Inverse Immediate</a> (<code>srlii</code>)</li>
						<li><a href="#op-sraii">Shift Right Arithmetic Inverse Immediate</a> (<code>sraii</code>)</li>
						<li><a href="#op-modi">Modulo Immediate</a> (<code>modi</code>)</li>
						<li><a href="#op-divi">Divide Immediate</a> (<code>divi</code>)</li>
						<li><a href="#op-divui">Divide Unsigned Immediate</a> (<code>divui</code>)</li>
						<li><a href="#op-divii">Divide Inverse Immediate</a> (<code>divii</code>)</li>
						<li><a href="#op-divuii">Divide Unsigned Inverse Immediate</a> (<code>divuii</code>)</li>
						<li><a href="#op-modui">Modulo Unsigned Immediate</a> (<code>modui</code>)</li>
					</ol>
				</li>
				<li><a href="#ops-logic-i">Logic (I-Types)</a>
					<ol>
						<li><a href="#op-andi">Bitwise AND Immediate</a>   (<code>andi</code>)</li>
						<li><a href="#op-nandi">Bitwise NAND Immediate</a> (<code>nandi</code>)</li>
						<li><a href="#op-nori">Bitwise NOR Immediate</a>   (<code>nori</code>)</li>
						<li><a href="#op-ori">Bitwise OR Immediate</a>     (<code>ori</code>)</li>
						<li><a href="#op-xnori">Bitwise XNOR Immediate</a> (<code>xnori</code>)</li>
						<li><a href="#op-xori">Bitwise XOR Immediate</a>   (<code>xori</code>)</li>
					</ol>
				</li>
				<li><a href="#ops-comp-r">Comparisons (R-Types)</a>
					<ol>
						<li><a href="#op-cmp">Compare</a> (<code>cmp</code>)</li>
						<li><a href="#op-sl">Set on Less Than</a> (<code>sl</code>)</li>
						<li><a href="#op-sle">Set on Less Than or Equal</a> (<code>sle</code>)</li>
						<li><a href="#op-seq">Set on Equal</a> (<code>seq</code>)</li>
						<li><a href="#op-sge">Set on Greater Than or Equal</a> (<code>sge</code>)</li>
						<li><a href="#op-sg">Set on Greater Than</a> (<code>sg</code>)</li>
						<li><a href="#op-slu">Set on Less Than Unsigned</a> (<code>slu</code>)</li>
						<li><a href="#op-sleu">Set on Less Than or Equal Unsigned</a> (<code>sleu</code>)</li>
						<li><a href="#op-sgeu">Set on Greater Than or Equal Unsigned</a> (<code>sgeu</code>)</li>
						<li><a href="#op-sgu">Set on Greater Than Unsigned</a> (<code>sgu</code>)</li>
					</ol>
				</li>
				<li><a href="#ops-comp-i">Comparisons (I-Types)</a>
					<ol>
						<li><a href="#op-cmpi">Compare Immediate</a> (<code>cmpi</code>)</li>
						<li><a href="#op-sli">Set on Less Than Immediate</a> (<code>sli</code>)</li>
						<li><a href="#op-slei">Set on Less Than or Equal Immediate</a> (<code>slei</code>)</li>
						<li><a href="#op-seqi">Set on Equal Immediate</a> (<code>seqi</code>)</li>
						<li><a href="#op-sgei">Set on Greater Than or Equal Immediate</a> (<code>sgei</code>)</li>
						<li><a href="#op-sgi">Set on Greater Than Immediate</a> (<code>sgi</code>)</li>
						<li><a href="#op-slui">Set on Less Than Unsigned Immediate</a> (<code>slui</code>)</li>
						<li><a href="#op-sleui">Set on Less Than or Equal Unsigned Immediate</a> (<code>sleui</code>)</li>
						<li><a href="#op-sgeui">Set on Greater Than or Equal Unsigned Immediate</a> (<code>sgeui</code>)</li>
						<li><a href="#op-sgui">Set on Greater Than Unsigned Immediate</a> (<code>sgui</code>)</li>
					</ol>
				</li>
				<li><a href="#ops-jump-j">Jumps (J-Types)</a>
					<ol>
						<li><a href="#op-j">Jump</a> (<code>j</code>)</li>
						<li><a href="#op-jc">Jump Conditional</a> (<code>jc</code>)</li>
					</ol>
				</li>
				<li><a href="#ops-jump-r">Jumps (R-Types)</a>
					<ol>
						<li><a href="#op-jr">Jump to Register</a> (<code>jr</code>)</li>
						<li><a href="#op-jrc">Jump to Register Conditional</a> (<code>jrc</code>)</li>
						<li><a href="#op-jrl">Jump to Register and Link</a> (<code>jrl</code>)</li>
						<li><a href="#op-jrlc">Jump to Register and Link Conditional</a> (<code>jrlc</code>)</li>
					</ol>
				</li>
				<li><a href="#ops-mem-r">Memory (R-Types)</a>
					<ol>
						<li><a href="#op-c">Copy</a>            (<code>c</code>)</li>
						<li><a href="#op-l">Load</a>            (<code>l</code>)</li>
						<li><a href="#op-s">Store</a>           (<code>s</code>)</li>
						<li><a href="#op-cb">Copy Byte</a>      (<code>cb</code>)</li>
						<li><a href="#op-lb">Load Byte</a>      (<code>lb</code>)</li>
						<li><a href="#op-sb">Store Byte</a>     (<code>sb</code>)</li>
						<li><a href="#op-ch">Copy Halfword</a>  (<code>ch</code>)</li>
						<li><a href="#op-lh">Load Halfword</a>  (<code>lh</code>)</li>
						<li><a href="#op-sh">Store Halfword</a> (<code>sh</code>)</li>
						<li><a href="#op-cs">Copy Short</a>     (<code>cs</code>)</li>
						<li><a href="#op-ls">Load Short</a>     (<code>ls</code>)</li>
						<li><a href="#op-ss">Store Short</a>    (<code>ss</code>)</li>
						<li><a href="#op-spush">Stack Push</a>  (<code>spush</code>)</li>
						<li><a href="#op-spop">Stack Pop</a>    (<code>spop</code>)</li>
						<li><a href="#op-ms">Memset</a>         (<code>ms</code>)</li>
					</ol>
				</li>
				<li><a href="#ops-mem-i">Memory (I-Types)</a>
					<ol>
						<li><a href="#op-li">Load Immediate</a> (<code>li</code>)</li>
						<li><a href="#op-si">Store Immediate</a> (<code>si</code>)</li>
						<li><a href="#op-lni">Load Indirect Immediate</a> (<code>lni</code>)</li>
						<li><a href="#op-lbi">Load Byte Immediate</a> (<code>lbi</code>)</li>
						<li><a href="#op-sbi">Store Byte Immediate</a> (<code>sbi</code>)</li>
						<li><a href="#op-lbni">Load Byte Indirect Immediate</a> (<code>lbni</code>)</li>
						<li><a href="#op-set">Set</a> (<code>set</code>)</li>
						<li><a href="#op-lui">Load Upper Immediate</a> (<code>lui</code>)</li>
						<li><a href="#op-sspush">Sized Stack Push</a> (<code>sspush</code>)</li>
						<li><a href="#op-sspop">Sized Stack Pop</a> (<code>sspop</code>)</li>
					</ol>
				</li>
				<li><a href="#ops-special">Special Instructions</a>
					<ol>
						<li><a href="#op-ext">External</a> (<code>ext</code>)</li>
						<li><a href="#op-sel">Select</a> (<code>sel</code>)</li>
						<li><a href="#op-int">Interrupt</a> (<code>int</code>)</li>
						<li><a href="#op-rit">Register Interrupt Table</a> (<code>rit</code>)</li>
						<li><a href="#op-time">Set Timer</a> (<code>time</code>)</li>
						<li><a href="#op-timei">Set Timer Immediate</a> (<code>timei</code>)</li>
						<li><a href="#op-ring">Change Ring</a> (<code>ring</code>)</li>
						<li><a href="#op-ringi">Change Ring Immediate</a> (<code>ringi</code>)</li>
						<li><a href="#op-pgoff">Disable Paging</a> (<code>pgoff</code>)</li>
						<li><a href="#op-pgon">Enable Paging</a> (<code>pgon</code>)</li>
						<li><a href="#op-setpt">Set Page Table</a> (<code>setpt</code>)</li>
						<li><a href="#op-svpg">Save Paging</a> (<code>svpg</code>)</li>
						<li><a href="#op-qm">Query Memory</a> (<code>qm</code>)</li>
						<li><a href="#op-di">Disable Interrupts</a> (<code>di</code>)</li>
						<li><a href="#op-ei">Enable Interrupts</a> (<code>ei</code>)</li>
					</ol>
				</li>
				<li><a href="#ops-pseudo">Pseudoinstructions</a>
					<ol>
						<li><a href="#op-mv">Move</a> (<code>mv</code>)</li>
						<li><a href="#op-ret">Return</a> (<code>ret</code>)</li>
						<li><a href="#op-push">Push</a> (<code>push</code>)</li>
						<li><a href="#op-pop">Pop</a> (<code>pop</code>)</li>
						<li><a href="#op-jeq">Jump if Equal</a> (<code>jeq</code>)</li>
					</ol>
				</li>
			</ol>
		</li>
		<li><a href="#ext">Externals</a>
			<ol>
				<li><a href="#ext-printr">Print Register</a> (<code>printr</code>)</li>
				<li><a href="#ext-halt">Halt</a> (<code>halt</code>)</li>
				<li><a href="#ext-eval">Evaluate String</a> (<code>eval</code>)</li>
				<li><a href="#ext-prc">Print Character</a> (<code>prc</code>)</li>
				<li><a href="#ext-prd">Print Decimal</a> (<code>prd</code>)</li>
				<li><a href="#ext-prx">Print Hexadecimal</a> (<code>prx</code>)</li>
				<li><a href="#ext-sleep">Sleep</a> (<code>sleep</code>)</li>
				<li><a href="#ext-prb">Print Binary</a> (<code>prb</code>)</li>
				<li><a href="#ext-rest">Rest</a> (<code>rest</code>)</li>
				<li><a href="#ext-io">I/O</a> (<code>io</code>)</li>
			</ol>
		</li>
	</ol>
</blockquote>

# <a name="intro"></a>Introduction

The VM emulates Why, a custom RISC instruction set that may or may not actually be theoretically implementable as real hardware. This instruction set has 64-bit word length, and memory addressability is also 64 bits.

# <a name="registers"></a>Registers
There are 128 registers. Their purposes are pretty much stolen from MIPS:

| Number   | Name         | Purpose                                           |
|----------|--------------|---------------------------------------------------|
| 0        | `$0`         | Always contains zero.                             |
| 1        | `$g`         | Global area pointer (right after end of program). |
| 2        | `$sp`        | Stack pointer.                                    |
| 3        | `$fp`        | Frame pointer.                                    |
| 4        | `$rt`        | Return address.                                   |
| 5        | `$lo`        | Stores the lower half of a mult result.           |
| 6        | `$hi`        | Stores the upper half of a mult result.           |
| 7–22     | `$r0`–`$rf`  | Contains return values.                           |
| 23–38    | `$a0`–`$af`  | Contains arguments for subroutines.               |
| 39–61    | `$t0`–`$t16` | Temporary registers.                              |
| 62–84    | `$s0`–`$s16` | Saved registers.                                  |
| 85–100   | `$k0`–`$kf`  | Kernel registers.                                 |
| 101      | `$st`        | Status register.                                  |
| 102–117  | `$m0`–`$mf`  | Reserved for use by the assembler.                |
| 118–121  | `$f0`–`$f3`  | Floating point return values.                     |
| 122–127  | `$e0`–`$e5`  | Contains data about exceptions.                   |

## <a name="reg-st"></a>Status Register
The `$st` register stores flag bits. Currently, this includes the results of arithmetic instructions. In ascending order of significance, these are:
<ol>
	<li><code>Z</code> (zero):
		Whether the last arithmetic result was zero (for <code>cmp</code>/<code>cmpi</code>, whether the compared values are equal)
	</li>
	<li><code>N</code> (negative):
		Whether the result of the last arithmetic result was negative (for <code>cmp</code>/<code>cmpi</code>, whether the left value was less than the right value)
	</li>
	<li><code>C</code> (carry):
		Whether the result of an addition was truncated. (Currently not implemented for subtraction.)
	</li>
	<li><code>O</code> (overflow):
		Whether the addition of two positive signed numbers had a negative result due to overflow.
	</li>
</ol>

These status numbers are used in conditional branches, but they can also be accessed by programs. Writing to the `$st` register is possible, but strange behavior may occur as a result.

# <a name="prog"></a>Programs

Programs are divided into five sections: metadata, code, data, symbol table and debug data. The <a href="#prog-meta">metadata section</a> contains information about the program. The <a href="#prog-code">code section</a> consists of executable code. The <a href="#prog-data">data section</a> consists of read/write data. The <a href="#prog-symtab">symbol table</a> contains the names, locations and types of all visible symbols. The <a href="#prog-debug">debug data section</a> contains data that correlates assembly instructions to locations in source files for higher-level languages like C.

Note that the code section and data section are combined into the `#text` section in assembly. Code and data sections can be switched between using the [`%code`](#dir-code) and [`%data`](#dir-data) directives.

## <a name="prog-meta"></a>Metadata Section
The metadata section is a block of data at the beginning of the program that contains the beginning addresses of the other sections. The first value in this section represents the beginning address of the code section and is therefore equivalent to the size of the metadata section.

* `0x00`: Address of the beginning of the [code section](#prog-code).
* `0x08`: Address of the beginning of the [data section](#prog-data).
* `0x10`: Address of the beginning of the [symbol table](#prog-symtab).
* `0x18`: Address of the beginning of the [debug data section](#prog-debug).
* `0x20`: Address of the beginning of the [relocation data section](#prog-reloc).
* `0x28`: Total size of the program.
* `0x30`–`0x38`: ORCID of the author (represented in ASCII without hyphens).
* `0x40`–`...`: Program name, version string and author name of the program (represented with null-terminated ASCII).
	* Example: given a program name `"Example"`, version string `"4"` and author name `"Kai Tamkun"`, this will be `0x4578616d706c6500` `0x34004b6169205461` `0x6d6b756e00000000`.

### Assembly syntax
<pre>
#meta
author: "Kai Tamkun"
orcid: "0000-0001-7405-6654"
name: "Example"
version: "4"
</pre>

## <a name="prog-code"></a>Code Section
The code section consists of executable code. This is the only section of the code that the program counter is expected to point to. Code is represented using the syntax described by entries in the <a href="#operations">Operations</a> section.

## <a name="prog-data"></a>Data Section
The data section contains read/write data. Data is added using directives.

## <a name="prog-symtab"></a>Symbol Table Section
The symbol table contains a list of debug symbols. Each debug symbol is assigned a numeric ID equal to part of the SHA256 hash of its name. (See [/wasmc/src/wasm/Assembler.cpp](https://github.com/heimskr/why/blob/master/wasmc/src/wasm/Assembler.cpp) for the precise transformation.) Each symbol is encoded in the table as a variable number of words. The upper half of the first is the numeric ID. The next 16 bits comprise the symbol type, while the lowest 16 bits comprise the length (in words) of the symbol's name. The second word represents the symbol's offset (its position relative to the start of the section containing it). The third word is the length of what the symbol points to (for functions, this will generally be eight times the number of instructions in the function). The remaining words encode the symbol's name. The length of the name in words is equal to the ceiling of 1/8 of the symbol name's length in characters. Any extra bytes in the last word are null.

## <a name="prog-debug"></a>Debug Data Section
The debug data section contains data mapping instructions to their positions in source files. It's stored as a list of entries whose order is important and must be maintained. The first byte of each entry determines the entry's type. All multibyte items in an entry are encoded as little-endian.

An entry with type `0` is invalid.

An entry with type `1` declares the filename of a source file and can be referenced by other entries. After the first byte in a type `1` entry, the next three bytes indicate the length of the filename. The filename then follows, plus padding until the total length of the entry is divisible by eight bytes.

An entry with type `2` declares a function name. It uses the same format as a type `1` entry, but instead of a filename, a function name is stored instead.

An entry with type `3` references a line on a source file defined by a type `1` entry. After the first byte in a type `3` entry, the next three bytes indicate the index of the referenced type `1` entry. The four bytes after that indicate the line number in the referenced file and the next three bytes indicate the column number. The next byte indicates how many contiguous instructions the entry applies to. The next four bytes indicate the index of the referenced type `2` entry. The final eight bytes indicate the address of an instruction.

The assembly syntax for type `3` entries defines a template. Multiple type `3` entries will be generated per template depending on how many instructions reference the type `3` entry. In the example below, only one entry is generated per template because each template occurs in a single continuous span. If a `!2` instruction were added after the last `!3` instruction, two type `3` entries would be generated for the template with index 2.

## <a name="prog-reloc"></a>Relocation Data Section
Relocation data allows the linker to combine multiple binaries. Some instructions have immediate values that contain not an absolute value but instead an address or an address plus a constant offset. Jumps to labels are the most common example.

The upper 61 bits of the first word of a relocation data entry represent the index of the symbol in the symbol table, while the next two bits are `0` if the value to relocate is 8 bytes wide, `2` if it's the lower 4 bytes of the symbol's address or `3` if it's the upper 4 bytes of the symbol's address. A value of `1` is invalid. The lowest bit of the first word is `1` if the value to be relocated is in the data section or `0` if it's in the code section. The second word is the signed offset to be applied to the symbol's location. The third and final word is the address of the value relative to the start of the code section.

### Assembly syntax
<pre>
#debug
1 "src/printf.cpp" // 0
2 "_vsnprintf"     // 1
3 0 541 10 1       // 2 (File 0, line 541, column 10, function 1)
3 0 551 12 1       // 3 (File 0, line 551, column 12, function 1)

#code
// ...
	$t6 -> [$ta]     !2
	[$t3] -> $t4 /b  !3
	1 -> $m0         !3
	$m0 << 7  -> $m0 !3
	$t4 x $m0 -> $t5 !3
	$t5 - $m0 -> $t5 !3
	$t5 & -1  -> $t5 !3
// ...
</pre>

# <a name="directives"></a>Directives

Directives are instructions to the assembler. They control the assembly process.

## <a name="dir-code"></a><code>%code</code>
Switches the current section to the code section. After using this, anything emitted using `%8b`, `%string` and other such directives will be placed in the code section. (Note that inserting 8-byte values directly in the code section is discouraged and that inserting strings is absolutely pointless.)

## <a name="dir-data"></a><code>%data</code>
Switches the current section to the data section. After using this, anything emitted using `%8b`, `%string` and other such directives will be placed in the data section. (Note that putting instructions in the data section causes unspecified behavior.)

## <a name="dir-type"></a><code>%type</code>
<!-- TODO: explain whether it's necessary to specify the type, and if so, why -->
Tells the assembler the type of a symbol. The type can be either `object` (for data) or `function` (for code).

### Example
<pre>
%type data_value object
%type strprint function

@data_value
%8b 42

@strprint
	// ...
	: $rt
</pre>

## <a name="dir-size"></a><code>%size</code>
Tells the assembler the size (in bytes) of a symbol. The value supports expressions. Expressions are mathematical expressions whose operands are numeric constants, symbol names or `.`, which represents the value of the location counter. An expression is valid if any of the following is true:

* None of its children contain any label references (that is, all leaves are numeric or `.`)
* It's the difference of two labels
* It's the difference of `.` and a label
* It's the difference of a label and a number
* It's the sum of a label and a number

These restrictions are in place because relocation data supports constant offsets only. All other information has to be known at compile time.

### Example
<pre>
@some_8b
%8b 42

%size some_8b 8

@function_one
	// ...
	: $rt

@function_two
	// ...
	: $ rt

%size function_one function_two-function_one
%size function_two .-function_two
</pre>

## <a name="dir-string"></a><code>%string</code>
Emits a string (*not* terminated with a null byte).

### Example
<pre>
@some_string
%string "Hello, world!\n"
</pre>

## <a name="dir-stringz"></a><code>%stringz</code>
Emits a string (terminated with a null byte).

### Example
<pre>
@some_string
%stringz "Hello, world!\n"
</pre>

## <a name="dir-value"></a><code>%8b</code>, <code>%4b</code>, <code>%2b</code>, <code>%1b</code>
Emits a value of the specified width (1 byte for `%1b`, 2 bytes for `%2b` and so on). The values can be expressions (see [`%size`](#dir-size)).

### Example
<pre>
%8b some_function+32 // 4 instructions past the start of some_function
%8b some_label-10
%8b .
%4b data_value // Valid if data_value is within the first 4 GiB
%2b 65535
%1b 255
</pre>

## <a name="dir-align"></a><code>%align</code>
Inserts zero until the location counter reaches a given alignment (in bytes).

### Example
<pre>
%align 65536
@p0
// ...
</pre>

## <a name="dir-fill"></a><code>%fill</code>
Adds a given number of bytes with a given value. The number of bytes is the first operand and the value is the second operand.

### Example
<pre>
@ten_ones
%fill 10 0x01
</pre>

# <a name="rings"></a>Rings
Why has support for four protection rings, just like x86. Ring 0 is called kernel mode and ring 3 is called user mode; rings 1 and 2 are currently unused. 

# <a name="interrupts"></a>Interrupts
Interrupts can be triggered by software or by the VM itself. Whenever an interrupt is triggered, `$e0` is set to the program counter right before the interrupt was raised and `$e1` is set to the current <a href="#rings">ring</a> at the time the interrupt occurred. Interrupt handlers are expected to deal with properly resuming normal program execution after finishing up. The VM stores an address to a table containing pointers to interrupt handlers. The table is 32 words long.

## <a name="int-system"></a>1: `SYSTEM`
The `SYSTEM` interrupt is a software-triggered interrupt handled by the operating system. It can be called from any ring and causes a switch to kernel mode.

## <a name="int-timer"></a>2: `TIMER`
The `TIMER` interrupt is a hardware-triggered interrupt caused when the hardware timer expires. Usable by ring zero only. This is to prevent unprivileged code from interfering with schedulers; operating systems can implement their own mechanisms to expose timer functionality to lower-privileged code. This interrupt causes a switch to kernel mode and sets `$e2` to the number of milliseconds the timer waited for.

## <a name="int-protec"></a>3: `PROTEC`
The `PROTEC` interrupt is a hardware-triggered interrupt caused when a called instruction attempts to do something not possible within the current <a href="#rings">ring</a>. This causes a switch to kernel mode.

## <a name="int-pfault"></a>4: `PFAULT`
The `PFAULT` interrupt is raised if paging is enabled and an access to a non-present page is attempted. This causes a switch to kernel mode.

## <a name="int-inexec"></a>5: `INEXEC`
The `INEXEC` interrupt is raised if program control flows to an address whose page is not marked as executable. This causes a switch to kernel mode.

## <a name="int-bwrite"></a>6: `BWRITE`
The `BWRITE` (bad write) interrupt is raised if paging is enabled and an instruction attempts to write to a nonwritable page.

## <a name="int-keybrd"></a>7: `KEYBRD`
The `KEYBRD` interrupt is raised when a key is pressed. The value of the key will be stored in `$e2` according to the format below. If this interrupt isn't set in the interrupt table, key presses will be ignored. This interrupt causes a switch to kernel mode.

| Range       | 63–35 (29) | 34   | 33  | 32    | 31–0 (32)         |
|------------:|:----------:|:----:|:---:|:-----:|:-----------------:|
| **Purpose** | Unused     | Ctrl | Alt | Shift | Key value (UTF-8) |

# <a name="format"></a>Instruction Format
Like much of this instruction set, the formatting for instructions is copied from MIPS with a few modifications (for example, instructions are 64 bits long in this instruction set, as opposed to 32 for MIPS64).

## <a name="format-r"></a>R-Type Instructions
R-type instructions perform computations with multiple registers.

| Range       | 63–52 (12)  | 51–45 (7) | 44–38 (7) | 37–31 (7) | 30–18 (13) | 17–14 (4)  | 13-12 (2)    | 11–0 (12) |
|------------:|:-----------:|:---------:|:---------:|:---------:|:----------:|:----------:|:------------:|:---------:|
| **Purpose** | Opcode      | rt        | rs        | rd        | Unused     | Conditions | Linker flags | Function  |

## <a name="format-i"></a>I-Type Instructions
I-type instructions perform computations with registers and an immediate value.

| Range       | 63–52 (12) | 51–48 (4)  | 47–46 (2)    | 45–39 (7) | 38–32 (7) | 31–0 (32)       |
|------------:|:----------:|:----------:|:------------:|:---------:|:---------:|:---------------:|
| **Purpose** | Opcode     | Conditions | Linker flags | rs        | rd        | Immediate Value |

## <a name="format-j"></a>J-Type Instructions
J-type instructions point the program counter to a given address under certain circumstances.

|   Range | 63–52 (12) | 51–45 (7) | 44   | 43–38 (6) | 37–34 (4)  | 33–32 (2)    | 31–0 (32) |
|--------:|:----------:|:---------:|:----:|:---------:|:----------:|:------------:|:---------:|
| Purpose | Opcode     | rs        | Link | Unused    | Conditions | Linker flags | Address   |

If the link bit is set, the current value of the program counter will be stored in `$rt`, the return address register.

## <a name="linkerflags"></a>Linker Flags
Before the relocation overhaul, linker flags were used to give the linker clues about relocation. There were four possible values. Back then, as now, linker flag values other than `0` are valid for I-type and J-type instructions only.

Now, the linker flags have three possible values. `0` indicates that the immediate value of the instruction isn't affected by relocation, whereas `1` indicates that the symbol needs to be relocated at link time. `2` is the same as `1`, but indicates that the current immediate value is invalid because the symbol is currently unknown. Executing an instruction whose linker flag is `2` will cause an error. Other values are invalid.

## <a name="condbits"></a>Condition Bits
For operations that support conditions, the condition bits indicate what combination of [ALU flags](#reg-st)
are required for the operation to occur.

<ul>
	<li><code>0xxx</code>: Conditions disabled</li>
	<li><code>1000</code>: Positive (<code>!N & !Z</code>)</li>
	<li><code>1001</code>: Negative</li>
	<li><code>1010</code>: Zero</li>
	<li><code>1011</code>: Nonzero</li>
</ul>

# <a name="paging"></a>Paging

Why.js supports paging. It's disabled by default and must be enabled with the [`pgon` instruction](#op-pgon). It uses a six-level system with a similar philosophy to x86_64's four-level system, but page sizes are 65,536 bytes large. The tables are named `P0` through and `P5` and each table contains 256 entries. To translate a virtual address to a physical one, `P0` is inspected at the address set with [`setpt`](#op-setpt). The most significant eight bits in the virtual address are used as an offset into `P0` to find the address of `P1`. If the `P0` entry isn't present, a [`PFAULT` interrupt](#int-pfault) is raised. Otherwise, the `P1` entry is inspected to find the `P2` address, and so on, until the address for the `P5` table is found and the entry specified by bits 23–16 in the virtual address is read. If the present bit isn't set, `PFAULT` is raised as usual. Otherwise, the low 16 bits of the virtual address are added to the page table entry with the low 16 bits set to zero and this result is used as the physical address.

## Address Format

| 63–56 (8)   | 55–48 (8)   | 47–40 (8)   | 39–32 (8)   | 31–24 (8)   | 23–16 (8)   | 15–0 (16)      |
|:-----------:|:-----------:|:-----------:|:-----------:|:-----------:|:-----------:|:--------------:|
| `P0` offset | `P1` offset | `P2` offset | `P3` offset | `P4` offset | `P5` offset | Offset in page |

## Page Table Entry Format

### `P0` through `P4`

| 63–11 (53)            | 10–1 (10) | 0       |
|:---------------------:|:---------:|:-------:|
| Address of next table | Unused    | Present |

### `P5`

| 63–16 (48)            | 15–6 (10) | 5        | 4        | 3         | 2          | 1        | 0       |
|:---------------------:|:---------:|:--------:|:--------:|:---------:|:----------:|:--------:|:-------:|
| Address of page start | Unused    | Modified | Accessed | User Page | Executable | Writable | Present |

User Page: Whether the page can be accessed in Ring 3. If zero, only rings 2 and lower can access it.

# <a name="operations"></a>Operations

## <a name="ops-math-r"></a>Math (R-Types)

### <a name="op-add"></a>Add (`add`)
> `$rs + $rt -> $rd` or `$rd += $rt`  
> `000000000001` `ttttttt` `sssssss` `ddddddd` `0000000000000` `......` `000000000000`

Adds the values in `rs` and `rt` and stores the result in `rd`.

### <a name="op-sub"></a>Subtract (`sub`)
> `$rs - $rt -> $rd` or `$rd -= $rt`  
> `000000000001` `ttttttt` `sssssss` `ddddddd` `0000000000000` `......` `000000000001`

Subtracts the value in `rt` from the value in `rs` and stores the result in `rd`.

### <a name="op-mult"></a>Multiply (`mult`)
> `$rs * $rt`  
> `000000000001` `ttttttt` `sssssss` `0000000` `0000000000000` `......` `000000000010`

Multiplies the value in `rs` by the value in `rt` and stores the upper half in `$hi` and the lower half in `$lo`.

### <a name="op-multu"></a>Multiply Unsigned (`multu`)
> `$rs * $rt /u`  
> `000000000001` `ttttttt` `sssssss` `0000000` `0000000000000` `......` `000000000101`

Multiplies the value in `rs` by the value in `rt` (treating both as unsigned values) and stores the upper half in `$hi` and the lower half in `$lo`.

### <a name="op-sll"></a>Shift Left Logical (`sll`)
> `$rs << $rt -> $rd` or `$rd <<= $rt`  
> `000000000001` `ttttttt` `sssssss` `ddddddd` `0000000000000` `......` `000000000110`

Logically shifts the value in `rs` to the left by a number of bits equal to the value in `rt` and stores the result in `rd`.

### <a name="op-srl"></a>Shift Right Logical (`srl`)
> `$rs >>> $rt -> $rd` or `$rd >>>= $rt`  
> `000000000001` `ttttttt` `sssssss` `ddddddd` `0000000000000` `......` `000000000111`

Logically shifts the value in `rs` to the right by a number of bits equal to the value in `rt` and stores the result in `rd`.

### <a name="op-sra"></a>Shift Right Arithmetic (`sra`)
> `$rs >> $rt -> $rd` or `$rd >>= $rt`  
> `000000000001` `ttttttt` `sssssss` `ddddddd` `0000000000000` `......` `000000001000`

Arithmetically shifts the value in `rs` to the left by a number of bits equal to the value in `rt` and stores the result in `rd`.

### <a name="op-mod"></a>Modulo (`mod`)
> `$rs % $rt -> $rd` or `$rs %= $rt`  
> `000000000001` `ttttttt` `sssssss` `ddddddd` `0000000000000` `......` `000000001001`

Computes the signed `rt`-modulo of `rs` and stores the result in `rd`.

### <a name="op-div"></a>Divide (`div`)
> `$rs / $rt -> $rd`  
> `000000000001` `ttttttt` `sssssss` `ddddddd` `0000000000000` `......` `000000001010`

Divides the value in `rs` by the value in `rt` and stores the result in `rd`, discarding the remainder.

### <a name="op-divu"></a>Divide Unsigned (`divu`)
> `$rs / $rt -> $rd /u`  
> `000000000001` `ttttttt` `sssssss` `ddddddd` `0000000000000` `......` `000000001011`

Divides the value in `rs` by the value in `rt` (treating both as unsigned values) and stores the result in `rd`, discarding the remainder.

### <a name="op-modu"></a>Modulo Unsigned (`modu`)
> `$rs % $rt -> $rd /u` or `$rs %= $rt /u`  
> `000000000001` `ttttttt` `sssssss` `ddddddd` `0000000000000` `......` `000000001100`

Computes the unsigned `rt`-modulo of `rs` and stores the result in `rd`.

### <a name="op-sext32"></a>Sign Extend 32 to 64 (`sext32`)
> `sext32 $rs -> $rd`  
> `000000000001` `0000000` `sssssss` `ddddddd` `0000000000000` `......` `000000001101`

If bit 31 (zero-indexed) of `rs` is 1, this instruction copies `rs` into `rd` and sets all upper 32 bits of `rd` to 1.
Otherwise, it just copies `rs` into `rd`.

## <a name="ops-logic-r"></a>Logic (R-Types)

### <a name="op-and"></a>Bitwise AND (`and`)
> `$rs & $rt -> $rd` or `$rd &= $rt`  
> `000000000010` `ttttttt` `sssssss` `ddddddd` `0000000000000` `......` `000000000000`
   
Computes the bitwise AND of `rs` and `rt` and stores the result in `rd`.

### <a name="op-nand"></a>Bitwise NAND (`nand`)
> `$rs ~& $rt -> $rd` or `$rd ~&= $rt`  
> `000000000010` `ttttttt` `sssssss` `ddddddd` `0000000000000` `......` `000000000001`
   
Computes the bitwise NAND of `rs` and `rt` and stores the result in `rd`.

### <a name="op-nor"></a>Bitwise NOR (`nor`)
> `$rs ~| $rt -> $rd` or `$rd ~|= $rt`  
> `000000000010` `ttttttt` `sssssss` `ddddddd` `0000000000000` `......` `000000000010`
   
Computes the bitwise NOR of `rs` and `rt` and stores the result in `rd`.

### <a name="op-not"></a>Bitwise NOT (`not`)
> `~$rs -> $rd` or `~$rs.`  
> `000000000010` `0000000` `sssssss` `ddddddd` `0000000000000` `......` `000000000011`
   
Computes the bitwise NOT of `rs` and stores the result in `rd`.

### <a name="op-or"></a>Bitwise OR (`or`)
> `$rs | $rt -> $rd` or `$rd |= $rt`  
> `000000000010` `ttttttt` `sssssss` `ddddddd` `0000000000000` `......` `000000000100`
   
Computes the bitwise OR of `rs` and `rt` and stores the result in `rd`.

### <a name="op-xnor"></a>Bitwise XNOR (`xnor`)
> `$rs ~x $rt -> $rd` or `$rd ~x= $rt`  
> `000000000010` `ttttttt` `sssssss` `ddddddd` `0000000000000` `......` `000000000101`
   
Computes the bitwise XNOR of `rs` and `rt` and stores the result in `rd`.

### <a name="op-xor"></a>Bitwise XOR (`xor`)
> `$rs x $rt -> $rd` or `$rd x= $rt`  
> `000000000010` `ttttttt` `sssssss` `ddddddd` `0000000000000` `......` `000000000110`

Computes the bitwise XOR of `rs` and `rt` and stores the result in `rd`.

### <a name="op-land"></a>Logical AND (`land`)
> `$rs && $rt -> $rd` or `$rd &&= $rt`  
> `000000000010` `ttttttt` `sssssss` `ddddddd` `0000000000000` `......` `000000001000`
   
Computes the logical AND of `rs` and `rt` and stores the result in `rd`.

### <a name="op-lnand"></a>Logical NAND (`lnand`)
> `$rs !&& $rt -> $rd` or `$rd !&&= $rt`  
> `000000000010` `ttttttt` `sssssss` `ddddddd` `0000000000000` `......` `000000001001`
   
Computes the logical NAND of `rs` and `rt` and stores the result in `rd`.

### <a name="op-lnor"></a>Logical NOR (`lnor`)
> `$rs !|| $rt -> $rd` or `$rd !||= $rt`  
> `000000000010` `ttttttt` `sssssss` `ddddddd` `0000000000000` `......` `000000001010`
   
Computes the logical NOR of `rs` and `rt` and stores the result in `rd`.

### <a name="op-lnot"></a>Logical NOT (`lnot`)
> `!$rs -> $rd` or `!$rs.`  
> `000000000010` `0000000` `sssssss` `ddddddd` `0000000000000` `......` `000000001011`
   
Computes the logical NOT of `rs` and stores the result in `rd`.

### <a name="op-lor"></a>Logical OR (`lor`)
> `$rs || $rt -> $rd` or `$rd ||= $rt`  
> `000000000010` `ttttttt` `sssssss` `ddddddd` `0000000000000` `......` `000000001100`
   
Computes the logical OR of `rs` and `rt` and stores the result in `rd`.

### <a name="op-lxnor"></a>Logical XNOR (`lxnor`)
> `$rs !xx $rt -> $rd` or `$rd !xx= $rt`  
> `000000000010` `ttttttt` `sssssss` `ddddddd` `0000000000000` `......` `000000001101`
   
Computes the logical XNOR of `rs` and `rt` and stores the result in `rd`.

### <a name="op-lxor"></a>Logical XOR (`lxor`)
> `$rs xx $rt -> $rd` or `$rd xx= $rt`  
> `000000000010` `ttttttt` `sssssss` `ddddddd` `0000000000000` `......` `000000001110`

Computes the logical XOR of `rs` and `rt` and stores the result in `rd`.

## <a name="ops-math-i"></a>Math (I-Types)

### <a name="op-addi"></a>Add Immediate (`addi`)
> `$rs + imm -> $rd` or `$rd += imm`  
> `000000000011` `......` `sssssss` `ddddddd` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`

Adds the value in `rs` and a constant and stores the result in `rd`.

### <a name="op-subi"></a>Subtract Immediate (`subi`)
> `$rs - imm -> $rd` or `$rd -= imm`  
> `000000000100` `......` `sssssss` `ddddddd` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`

Subtracts a constant from the value in `rs` and stores the result in `rd`.

### <a name="op-multi"></a>Multiply Immediate (`multi`)
> `$rs * imm`  
> `000000000101` `......` `sssssss` `0000000` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`

Multiplies the value in `rs` by a constant and stores the upper half in `$hi` and the lower half in `$lo`.

### <a name="op-multui"></a>Multiply Unsigned Immediate (`multui`)
> `$rs * imm /u`  
> `000000011000` `......` `sssssss` `0000000` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`

Multiplies the value in `rs` by a constant (treating both as unsigned values) and stores the upper half in `$hi` and the lower half in `$lo`.

### <a name="op-slli"></a>Shift Left Logical Immediate (`slli`)
> `$rs << imm -> $rd` or `$rd <<= imm`  
> `000000100010` `......` `sssssss` `ddddddd` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`

Logically shifts the value in `rs` to the left by a number of bits equal to `imm` and stores the result in `rd`.

### <a name="op-srli"></a>Shift Right Logical Immediate (`srli`)
> `$rs >>> imm -> $rd` or `$rd >>>= imm`  
> `000000100011` `......` `sssssss` `ddddddd` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`

Logically shifts the value in `rs` to the right by a number of bits equal to `imm` and stores the result in `rd`.

### <a name="op-srai"></a>Shift Right Arithmetic Immediate (`srai`)
> `$rs >> imm -> $rd` or `$rd >>= imm`  
> `000000100100` `......` `sssssss` `ddddddd` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`

Arithmetically shifts the value in `rs` to the right by a number of bits equal to `imm` and stores the result in `rd`.

### <a name="op-sllii"></a>Shift Left Logical Inverse Immediate (`sllii`)
> `imm << $rs -> $rd`  
> `000000111110` `......` `sssssss` `ddddddd` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`

Logically shifts `imm` to the left by a number of bits equal to the value in `rs` and stores the result in `rd`.

### <a name="op-srlii"></a>Shift Right Logical Inverse Immediate (`srlii`)
> `imm >>> $rs -> $rd`  
> `000000111111` `......` `sssssss` `ddddddd` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`

Logically shifts `imm` to the right by a number of bits equal to the value in `rs` and stores the result in `rd`.

### <a name="op-sraii"></a>Shift Right Arithmetic Inverse Immediate (`sraii`)
> `imm >> $rs -> $rd`  
> `000001000000` `......` `sssssss` `ddddddd` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`

Arithmetically shifts `imm` to the right by a number of bits equal to the value in `rs` and stores the result in `rd`.

### <a name="op-modi"></a>Modulo Immediate (`modi`)
> `$rs % imm -> $rd` or `$rd %= imm`  
> `000000011110` `......` `sssssss` `ddddddd` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`

Computes the signed `imm`-modulo of `rs` and stores the result in `rd`.

### <a name="op-divi"></a>Divide Immediate (`divi`)
> `$rs / imm -> $rd`  
> `000000110100` `......` `sssssss` `ddddddd` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`

Divides the value in `rs` by a constant and stores the result in `rd`, discarding the remainder.

### <a name="op-divui"></a>Divide Unsigned Immediate (`divui`)
> `$rs / imm -> $rd /u`  
> `000000110101` `......` `sssssss` `ddddddd` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`

### <a name="op-divii"></a>Divide Inverse Immediate (`divii`)
> `imm / $rs -> $rd`  
> `000000110110` `......` `sssssss` `ddddddd` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`

Divides a constant by the value in `rs` and stores the result in `rd`, discarding the remainder.

### <a name="op-modui"></a>Modulo Unsigned Immediate (`modui`)
> `$rs % imm -> $rd /u` or `$rd %= imm /u`  
> `000001000011` `......` `sssssss` `ddddddd` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`

Computes the unsigned `imm`-modulo of `rs` and stores the result in `rd`.

### <a name="op-divuii"></a>Divide Unsigned Inverse Immediate (`divuii`)
> `imm / $rs -> $rd /u`  
> `000000110111` `......` `sssssss` `ddddddd` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`

Divides a constant by the value in `rs` (treating both as unsigned values) and stores the result in `rd`, discarding the remainder.

## <a name="ops-logic-i"></a>Logic (I-Types)

### <a name="op-andi"></a>Bitwise AND Immediate (`andi`)
> `$rs & imm -> $rd` or `$rd &= imm`  
> `000000000110` `......` `sssssss` `ddddddd` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`

Computes the bitwise AND of `rs` and a constant and stores the result in `rd`.

### <a name="op-nandi"></a>Bitwise NAND Immediate (`nandi`)
> `$rs ~& imm -> $rd` or `$rd ~&= imm`  
> `000000000111` `......` `sssssss` `ddddddd` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`

Computes the bitwise NAND of `rs` and a constant and stores the result in `rd`.

### <a name="op-nori"></a>Bitwise NOR Immediate (`nori`)
> `$rs ~| imm -> $rd` or `$rd ~|= imm`  
> `000000001000` `......` `sssssss` `ddddddd` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`
   
Computes the bitwise NOR of `rs` and a constant and stores the result in `rd`.

### <a name="op-ori"></a>Bitwise OR Immediate (`ori`)
> `$rs | imm -> $rd` or `$rd |= imm`  
> `000000001001` `......` `sssssss` `ddddddd` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`
   
Computes the bitwise OR of `rs` and a constant and stores the result in `rd`.

### <a name="op-xnori"></a>Bitwise XNOR Immediate (`xnori`)
> `$rs ~x imm -> $rd` or `$rd ~x= imm`  
> `000000001010` `......` `sssssss` `ddddddd` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`
   
Computes the bitwise XNOR of `rs` and a constant and stores the result in `rd`.

### <a name="op-xori"></a>Bitwise XOR Immediate (`xori`)
> `$rs x imm -> $rd` or `$rd x= imm`  
> `000000001011` `......` `sssssss` `ddddddd` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`

Computes the bitwise XOR of `rs` and a constant and stores the result in `rd`.

## <a name="ops-comp-r"></a>Comparisons (R-Types)

### <a name="op-cmp"></a>Compare (`cmp`)
> `$rs ~ $rt`  
> `000000001110` `ttttttt` `sssssss` `.......` `0000000000000` `......` `000000000101`

Compares the value in `rs` to the value in `rt` (treating both as signed values) and updates the [status register](#reg-st).

### <a name="op-sl"></a>Set on Less Than (`sl`)
> `$rs < $rt -> $rd`  
> `000000001110` `ttttttt` `sssssss` `ddddddd` `0000000000000` `......` `000000000000`

If the value in `rs` is less than the value in `rt`, `rd` is set to 1; otherwise, `rd` is set to 0.

### <a name="op-sle"></a>Set on Less Than or Equal (`sle`)
> `$rs <= $rt -> $rd`  
> `000000001110` `ttttttt` `sssssss` `ddddddd` `0000000000000` `......` `000000000001`

If the value in `rs` is less than or equal to the value in `rt`, `rd` is set to 1; otherwise, `rd` is set to 0.

### <a name="op-seq"></a>Set on Equal (`seq`)
> `$rs == $rt -> $rd`  
> `000000001110` `ttttttt` `sssssss` `ddddddd` `0000000000000` `......` `000000000010`

If the value in `rs` is equal to the value in `rt`, `rd` is set to 1; otherwise, `rd` is set to 0.

### <a href="#op-sge">Set on Greater Than or Equal</a> (`sge`)

### <a href="#op-sg">Set on Greater Than</a> (`sg`)

### <a name="op-slu"></a>Set on Less Than Unsigned (`slu`)
> `$rs < $rt -> $rd /u`  
> `000000001110` `ttttttt` `sssssss` `ddddddd` `0000000000000` `......` `000000000011`

If the value in `rs` is less than the value in `rt` (treating both as unsigned values), `rd` is set to 1; otherwise, `rd` is set to 0.

### <a name="op-sleu"></a>Set on Less Than or Equal Unsigned (`sleu`)
> `$rs <= $rt -> $rd /u`  
> `000000001110` `ttttttt` `sssssss` `ddddddd` `0000000000000` `......` `000000000100`

If the value in `rs` is less than or equal to the value in `rt` (treating both as unsigned values), `rd` is set to 1; otherwise, `rd` is set to 0.

## <a name="ops-comp-i"></a>Comparisons (I-Types)

### <a name="op-cmpi"></a>Compare Immediate (`cmpi`)
> `$rs ~ imm`  
> `000000101011` `......` `sssssss` `ddddddd` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`

Compares the value in `rs` to `imm` and updates the [status register](#reg-st).

### <a name="op-sli"></a>Set on Less Than Immediate (`sli`)
> `$rs < imm -> $rd`  
> `000000011001` `......` `sssssss` `ddddddd` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`

If the value in `rs` is less than `imm`, `rd` is set to 1; otherwise, `rd` is set to 0.

### <a name="op-slei"></a>Set on Less Than or Equal Immediate (`slei`)
> `$rs <= imm -> $rd`  
> `000000011010` `......` `sssssss` `ddddddd` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`

If the value in `rs` is less than or equal to `imm`, `rd` is set to 1; otherwise, `rd` is set to 0.

### <a name="op-seqi"></a>Set on Equal Immediate (`seqi`)
> `$rs == imm -> $rd`  
> `000000011011` `......` `sssssss` `ddddddd` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`

If the value in `rs` is equal to `imm`, `rd` is set to 1; otherwise, `rd` is set to 0.

### <a href="#op-sgeu">Set on Greater Than or Equal Unsigned</a> (`sgeu`)

### <a href="#op-sgu">Set on Greater Than Unsigned</a> (`sgu`)

### <a name="op-sgi"></a>Set on Greater Than Immediate (`sgi`)
> `$rs > imm -> $rd`  
> `000000101001` `......` `sssssss` `ddddddd` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`

If the value in `rs` is greater than `imm`, `rd` is set to 1; otherwise, `rd` is set to 0.

### <a name="op-sgei"></a>Set on Greater Than or Equal Immediate (`sgei`)
> `$rs >= imm -> $rd`  
> `000000101010` `......` `sssssss` `ddddddd` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`

If the value in `rs` is greater than or equal to `imm`, `rd` is set to 1; otherwise, `rd` is set to 0.

### <a name="op-slui"></a>Set on Less Than Unsigned Immediate (`slui`)
> `$rs < imm -> $rd /u`  
> `000000011100` `......` `sssssss` `ddddddd` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`

If the value in `rs` is less than `imm` (treating both as unsigned values), `rd` is set to 1; otherwise, `rd` is set to 0.

### <a name="op-sleui"></a>Set on Less Than or Equal Unsigned Immediate (`sleui`)
> `$rs <= imm -> $rd /u`  
> `000000011101` `......` `sssssss` `ddddddd` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`

If the value in `rs` is less than or equal to `imm` (treating both as unsigned values), `rd` is set to 1; otherwise, `rd` is set to 0.

### <a name="op-sgeui"></a>Set on Greater Than or Equal Unsigned Immediate (`sgeui`)
> `$rs >= imm -> $rd /u`  
> `000000111011` `......` `sssssss` `ddddddd` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`

If the value in `rs` is greater than or equal to `imm` (treating both as unsigned values), `rd` is set to 1; otherwise, `rd` is set to 0.

### <a name="op-sgui"></a>Set on Greater Than Unsigned Immediate (`sgui`)
> `$rs > imm -> $rd /u`  
> `000000111100` `......` `sssssss` `ddddddd` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`

If the value in `rs` is greater than `imm` (treating both as unsigned values), `rd` is set to 1; otherwise, `rd` is set to 0.

## <a name="ops-jump-j"></a>Jumps (J-Types)

### <a name="op-j"></a>Jump (`j`)
> `: label` or `: imm`  
> `:: label` or `:: imm` (linking variant)  
> `000000001111` `0000000` `0000000` `cccc` `..` `aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa`

Supports conditions:
> `+:`/`+::`: jump if positive  
> `-:`/`-::`: jump if negative  
> `0:`/`0::`: jump if zero  
> `*:`/`*::`: jump if nonzero  

Jumps to the address of a given label or directly to a given address.

### <a name="op-jc"></a>Jump Conditional (`jc`)
> `: label if $rs` or `: imm if $rs`  
> `:: label if $rs` or `:: imm if $rs` (linking variant)  
> `000000010000` `sssssss` `0000000` `0000` `..` `aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa`

Jumps to the address of a given label or directly to a given address, provided the value in `rs` is nonzero.

## <a name="ops-jump-r"></a>Jumps (R-Types)

### <a name="op-jr"></a>Jump to Register (`jr`)
> `: $rd`  
> `000000010001` `0000000` `0000000` `ddddddd` `0000000000000` `cccc` `..` `000000000000`  
> Supports conditions

Jumps to the address stored in `rd`.

### <a name="op-jrc"></a>Jump to Register Conditional (`jrc`)
> `: $rd if $rs`  
> `000000010001` `0000000` `sssssss` `ddddddd` `0000000000000` `......` `000000000001`  

Jumps to the address stored in `rd`, provided the value in `rs` is nonzero.

### <a name="op-jrl"></a>Jump to Register and Link (`jrl`)
> `:: $rd`  
> `000000010001` `0000000` `0000000` `ddddddd` `0000000000000` `cccc` `..` `000000000010`  
> Supports conditions

Stores the address of the next instruction in `$rt` and jumps to the address stored in `rd`.

### <a name="op-jrlc"></a>Jump to Register and Link Conditional (`jrlc`)
> `:: $rd if $rs`  
> `000000010001` `0000000` `sssssss` `ddddddd` `0000000000000` `......` `000000000011`

Stores the address of the next instruction in `$rt` and jumps to the address stored in `rd`, provided the value in `rs` is nonzero.

## <a name="ops-mem-r"></a>Memory (R-Types)

### <a name="op-c"></a>Copy (`c`)
> `[$rs] -> [$rd]`  
> `000000010010` `0000000` `sssssss` `ddddddd` `0000000000000` `......` `000000000000`

Copies the word beginning at the memory address pointed to by `rs` into memory beginning at the address pointed to by `rd`.

### <a name="op-l"></a>Load (`l`)
> `[$rs] -> $rd`  
> `000000010010` `0000000` `sssssss` `ddddddd` `0000000000000` `......` `000000000001`

Loads the word beginning at the memory address pointed to by `rs` into `rd`.

### <a name="op-s"></a>Store (`s`)
> `$rs -> [$rd]`  
> `000000010010` `0000000` `sssssss` `ddddddd` `0000000000000` `......` `000000000010`

Stores the value of `rs` into memory beginning at the address pointed to by `rd`.

### <a name="op-cb"></a>Copy Byte (`cb`)
> `[$rs] -> [$rd] /b`  
> `000000010010` `0000000` `sssssss` `ddddddd` `0000000000000` `......` `000000000011`

Copies the byte stored at the memory address pointed to by `rs` into the memory address pointed to by `rd`.

### <a name="op-lb"></a>Load Byte (`lb`)
> `[$rs] -> $rd /b`  
> `000000010010` `0000000` `sssssss` `ddddddd` `0000000000000` `......` `000000000100`

Loads the byte stored at the memory address pointed to by `rs` into `rd`.

### <a name="op-sb"></a>Store Byte (`sb`)
> `$rs -> [$rd] /b`  
> `000000010010` `0000000` `sssssss` `ddddddd` `0000000000000` `......` `000000000101`

Stores the lowest 8 bits of `rs` into the memory address pointed to by `rd`.

### <a name="op-ch"></a>Copy Halfword (`ch`)
> `[$rs] -> [$rd] /h`  
> `000000010010` `0000000` `sssssss` `ddddddd` `0000000000000` `......` `000000001000`

Copies the halfword stored at the memory address pointed to by `rs` into the memory address pointed to by `rd`.

### <a name="op-lh"></a>Load Halfword (`lh`)
> `[$rs] -> $rd /h`  
> `000000010010` `0000000` `sssssss` `ddddddd` `0000000000000` `......` `000000001001`

Loads the halfword stored at the memory address pointed to by `rs` into `rd`.

### <a name="op-sh"></a>Store Halfword (`sh`)
> `$rs -> [$rd] /h`  
> `000000010010` `0000000` `sssssss` `ddddddd` `0000000000000` `......` `000000001010`

Stores the lowest 32 bits of `rs` into the memory address pointed to by `rd`.

### <a name="op-cs"></a>Copy Short (`cs`)
> `[$rs] -> [$rd] /s`  
> `000000010010` `0000000` `sssssss` `ddddddd` `0000000000000` `......` `000000001100`

Copies the short stored at the memory address pointed to by `rs` into the memory address pointed to by `rd`.

### <a name="op-ls"></a>Load Short (`ls`)
> `[$rs] -> $rd /s`  
> `000000010010` `0000000` `sssssss` `ddddddd` `0000000000000` `......` `000000001101`

Loads the short stored at the memory address pointed to by `rs` into `rd`.

### <a name="op-ss"></a>Store Short (`ss`)
> `$rs -> [$rd] /s`  
> `000000010010` `0000000` `sssssss` `ddddddd` `0000000000000` `......` `000000001110`

Stores the lowest 16 bits of `rs` into the memory address pointed to by `rd`.

### <a name="op-spush"></a>Stack Push (`spush`)
> `[ $rs`  
> `000000010010` `0000000` `sssssss` `0000000` `0000000000000` `......` `000000000110`

Copies the word at `rs` into the stack and adjusts the stack pointer.  
See also: <a href="#op-push">push pseudoinstruction</a>

### <a name="op-spop"></a>Stack Pop (`spop`)
> `] $rd`  
> `000000010010` `0000000` `ddddddd` `0000000` `0000000000000` `......` `000000000111`

Adjusts the stack pointer and copies the word at the stack pointer into `rd`.  
See also: <a href="#op-pop">pop pseudoinstruction</a>

### <a name="op-ms"></a>Memset (`ms`)
> `memset $rs x $rt -> $rd`  
> `000000010010` `ttttttt` `ddddddd` `sssssss` `0000000000000` `......` `000000001011`

Sets `rs` bytes to `rt` starting at address `rd`. The value in `rt` will be truncated to 8 bits.

## <a name="ops-mem-i"></a>Memory (I-Types)

### <a name="op-li"></a>Load Immediate (`li`)
> `[imm] -> $rd`  
> `000000010011` `......` `0000000` `ddddddd` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`

Loads the word beginning at address `imm` into `rd`.

### <a name="op-si"></a>Store Immediate (`si`)
> `$rs -> [imm]`  
> `000000010100` `......` `sssssss` `0000000` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`

Stores the value of `rs` into memory beginning at address `imm`.

### <a name="op-lbi"></a>Load Byte Immediate (`lbi`)
> `[imm] -> $rd /b`  
> `000000100101` `......` `0000000` `ddddddd` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`

Loads the byte at address `imm` into `rd`.

### <a name="op-sbi"></a>Store Byte Immediate (`sbi`)
> `$rs -> [imm] /b`  
> `000000100110` `......` `sssssss` `0000000` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`

Stores the lowest 8 bits of `rs` into memory at address `imm`.

### <a name="op-lni"></a>Load Indirect Immediate (`lni`)
> `[imm] -> [$rd]`  
> `000000100111` `......` `sssssss` `0000000` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`

Copies the word stored in memory at address `imm` into the memory beginning at the address pointed to by `rd`.

### <a name="op-lbni"></a>Load Byte Indirect Immediate (`lbni`)
> `[imm] -> [$rd] /b`  
> `000000101000` `......` `sssssss` `0000000` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`

Copies the byte stored in memory at address `imm` into the memory address pointed to by `rd`.

### <a name="op-set"></a>Set (`set`)
> `imm -> $rd`  
> `000000010101` `......` `0000000` `ddddddd` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`

Sets a register to the given immediate value.

### <a name="op-lui"></a>Load Upper Immediate (`lui`)
> `lui: imm -> $rd`  
> `000000001101` `......` `0000000` `ddddddd` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`

Loads an immediate value into the upper half of the word at `rd`. The lower half is not affected.

### <a name="op-sspush"></a>Sized Stack Push (`sspush`)
> `[:imm $rs`  
> `000000111001` `......` `sssssss` `0000000` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`

Writes `rs` to the stack and decrements the stack pointer by `imm` bytes.

### <a name="op-sspop"></a>Sized Stack Pop (`sspop`)
> `]:imm $rd`  
> `000000111010` `......` `0000000` `ddddddd` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`

Increments the stack pointer by `imm` bytes and reads into `rd` from the stack.

## <a name="ops-special"></a>Special Instructions

### <a name="op-ext"></a>External (`ext`)
> (varies; see <a href="#ext">Externals</a>)  
> `000000011111` `ttttttt` `sssssss` `ddddddd` `0000000000000` `......` `xxxxxxxxxxxx`

Performs a special instruction, typically for interaction with the world outside the VM.

### <a name="op-sel"></a>Select (`sel`)
> `[$rs = $rt] -> $rd`  
> `[$rs < $rt] -> $rd`  
> `[$rs > $rt] -> $rd`  
> `[$rs != $rt] -> $rd`  
> `000000111000` `ttttttt` `sssssss` `ddddddd` `0000000000000` `cond` `..` `xxxxxxxxxxxx`

Checks the [status register](#reg-st) and the [condition bits](#condbits). If the condition matches the status register, `rd` is set to `rs`; otherwise, it's set to `rt`.

### <a name="op-int"></a>Interrupt (`int`)
> `%int imm`  
> `000000100000` `......` `.......` `.......` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`

Performs an interrupt. If no interrupt table has been registered, nothing interesting happens.

### <a name="op-rit"></a>Register Interrupt Table (`rit`)
> `%rit imm`  
> `000000100001` `......` `.......` `.......` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`

Registers the interrupt table. Takes a pointer to a table in the data section. Valid only in kernel mode; will cause the machine to halt if called in user mode.

### <a name="op-time"></a>Set Timer (`time`)
> `%time $rs`  
> `000000110000` `.......` `sssssss` `.......` `0000000000000` `......` `000000000000`

Sets the hardware timer to the number stored in `rs` (in microseconds), canceling any previous timer. Requires kernel mode. Sub-millisecond precision may be unsupported or inaccurate. Once the timer expires, a <a href="#int-timer">timer interrupt</a> occurs.

### <a name="op-timei"></a>Set Timer Immediate (`timei`)
> `%time imm`  
> `000000110001` `......` `.......` `.......` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`

Sets the hardware timer to the number stored in `imm` (in microseconds), canceling any previous timer. Requires kernel mode. Sub-millisecond precision may be unsupported or inaccurate. Once the timer expires, a <a href="#int-timer">timer interrupt</a> occurs.

### <a name="op-ring"></a>Change Ring (`ring`)
> `%ring $rs`  
> `000000110010` `.......` `sssssss` `.......` `0000000000000` `......` `000000000000`

Sets the <a href="#rings">protection ring</a> to the value stored in `rs`. A <a href="#int-protec">protection interrupt</a> will occur if the indicated ring is lower than the current ring to prevent privilege escalation.

### <a name="op-ringi"></a>Change Ring Immediate (`ringi`)
> `%ring imm`  
> `000000110011` `......` `.......` `.......` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`

Sets the <a href="#rings">protection ring</a> to `imm`. A <a href="#int-protec">protection interrupt</a> will occur if the indicated ring is lower than the current ring to prevent privilege escalation.

### <a name="op-pgoff"></a>Disable Paging (`pgoff`)
> `%page off`  
> `000000111101` `.......` `.......` `.......` `0000000000000` `......` `000000000000`

Disables virtual memory. Raises [`PROTEC`](#int-protec) if used in a ring other than ring zero.

### <a name="op-pgon"></a>Enable Paging (`pgon`)
> `%page on`  
> `000000111101` `.......` `.......` `.......` `0000000000000` `......` `000000000001`

Enables virtual memory. Raises [`PROTEC`](#int-protec) if used in a ring other than ring zero.

### <a name="op-setpt"></a>Set Page Table (`setpt`)
> `%setpt $rs`  
> `000000111101` `.......` `.......` `.......` `0000000000000` `......` `000000000010`

Sets the address of [`P0`](#paging). Raises [`PROTEC`](#int-protec) if used in a ring other than ring zero.

### <a name="op-svpg"></a>Save Paging (`svpg`)
> `%page -> $rd`  
> `000000111101` `.......` `.......` `ddddddd` `0000000000000` `......` `000000000011`

Sets `rd` to 1 if paging is enabled or 0 if paging is disabled.

### <a name="op-qm"></a>Query Memory (`qm`)
> `? mem -> $rd`  
> `000001000001` `.......` `.......` `ddddddd` `0000000000000` `......` `000000000000`

Sets `rd` to the size of the main memory in bytes.

### <a name="op-di"></a>Disable Interrupts (`di`)
> `%di`  
> `000001000010` `.......` `.......` `.......` `0000000000000` `......` `000000000000`

Disables hardware interrupts. This currently includes `TIMER` and `KEYBRD`.

### <a name="op-ei"></a>Enable Interrupts (`ei`)
> `%ei`  
> `000001000010` `.......` `.......` `.......` `0000000000000` `......` `000000000001`

Enables hardware interrupts. This currently includes `TIMER` and `KEYBRD`.

## <a name="ops-pseudo"></a>Pseudoinstructions

### <a name="op-mv"></a>Move (`mv`)
> `$rs -> $rd`

Copies the value of `rs` into `rd`.

Translation:  
<code>$rs [|](#op-or) $0 -> $rd</code>

### <s><a name="op-ret"></a>Return (`ret`)</s> (deprecated in wasmc++)
> `ret`

Jumps to the return address.

Translation:  
<code>[:](#op-jr) $r</code>

### <s><a name="op-push"></a>Push (`push`)</s> (deprecated in wasmc++)
> `[ $x $y $z ...`

Pushes the value of the specified register(s) to the stack. Acts as a shorthand for calling
<a href="#op-spush">spush</a> on multiple registers.

Translation for each register in order:  
<code><a href="#op-spush">[</a> $rs</code>

### <s><a name="op-pop"></a>Pop (`pop`)</s> (deprecated in wasmc++)
> `] $x $y $z`

Pops the value(s) at the top of the stack and stores the value(s) in the specified register(s).
Acts as a shorthand for calling <a href="#op-spop">spop</a> on multiple registers.

Translation for each register in order:
<code><a href="#op-spop">]</a> $rs</code>

### <a name="op-jeq"></a>Jump if Equal (`jeq`)
> `: $rd if $rs == $rt`

If the value in `rs` is equal to the value in `rt`, jumps to the address stored in `rd`. (Modifies `m7`.)

Translation:  
<code>$rs [==](#op-seq) $rt -> $m7</code>  
<code>[:](#op-jrc) $rd if $m7</code>

> `: label if $rs == $rt`

If the value in `rs` is equal to the value in `rt`, jumps to `label`. (Modifies `m7`.)

### <a name="op-sge"></a>Set on Greater Than or Equal (`sge`)
> `$rs >= $rt -> $rd`

If the value in `rs` is greater than or equal to the value in `rt`, `rd` is set to 1; otherwise, `rd` is set to 0.

Translation:  
<code>$rt [<=](#op-sle) $rs -> $rd</code>

### <a name="op-sg"></a>Set on Greater Than (`sg`)
> `$rs > $rt -> $rd`

If the value in `rs` is greater than the value in `rt`, `rd` is set to 1; otherwise, `rd` is set to 0.

Translation:  
<code>$rt [<](#op-sl) $rs -> $rd</code>

### <a name="op-sgeu"></a>Set on Greater Than or Equal Unsigned (`sgeu`)
> `$rs >= $rt -> $rd /u`

If the value in `rs` is greater than or equal to the value in `rt` (treating both as unsigned values), `rd` is set to 1; otherwise, `rd` is set to 0.

Translation:  
<code>$rt [<=](#op-sleu) $rs -> $rd</code>

### <a name="op-sgu"></a>Set on Greater Than Unsigned (`sgu`)
> `$rs > $rt -> $rd /u`

If the value in `rs` is greater than the value in `rt` (treating both as unsigned values), `rd` is set to 1; otherwise, `rd` is set to 0.

Translation:  
<code>$rt [<](#op-slu) $rs -> $rd /u</code>

# <a name="ext"></a>Externals

### <a name="ext-printr"></a>Print Register
Syntax: `<print $rs>`  
Function value: `000000000001`

Prints the value stored in `rs` to the console.

### <a name="ext-halt"></a>Halt
Syntax: `<halt>`  
Function value: `000000000010`

Halts the VM.

### <a name="ext-eval"></a>Evaluate String
Syntax: `<eval $rs>`  
Function value: `000000000011`

Evaluates the string beginning at the address stored in `rs`. Unimplemented.

### <a name="ext-prc"></a>Print Character
Syntax: `<prc $rs>`  
Function value: `000000000100`

Prints the character stored in `rs` to the console.

### <a name="ext-prd"></a>Print Decimal
Syntax: `<prd $rs>`  
Function value: `000000000101`

Prints the number stored in `rs` to the console as a decimal number.

### <a name="ext-prx"></a>Print Hexadecimal
Syntax: `<prx $rs>`  
Function value: `000000000110`

Prints the number stored in `rs` to the console as a hexadecimal number.

### <a name="ext-sleep"></a>Sleep
Syntax: `<sleep $rs>`  
Function value: `000000000111`

Sleeps for the number of microseconds stored in `rs`.

### <a name="ext-prb"></a>Print Binary
Syntax: `<prb $rs>`  
Function value: `000000001000`

Prints the number stored in `rs` to the console as a binary number.

### <a name="ext-rest"></a>Rest
Syntax: `<rest>`  
Function value: `000000001001`

Pauses execution until an interrupt occurs.

### <a name="ext-io"></a>I/O
Syntax: `<io operation_name>`  
Function value: `000000001010`

Interacts with storage devices. See [the I/O document](IO.md) for details.
