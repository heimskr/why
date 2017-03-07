# Table of Contents
<blockquote>
	<ol>
		<li><a href="#prog">Programs</a>
			<ol>
				<li><a href="#prog-meta">Metadata Section</a></li>
				<li><a href="#prog-ptrs">Handler Pointer Section</a></li>
				<li><a href="#prog-data">Data Section</a></li>
				<li><a href="#prog-code">Code Section</a></li>
			</ol>
		</li>
		<li><a href="#registers">Registers</a> </li>
		<li><a href="#operations">Operations</a>
			<ol>
				<li><a href="#ops-math-r">Math (R-Types)</a>
					<ol>
						<li><a href="#op-add">Addition</a> (<code>add</code>)</li>
						<li><a href="#op-sub">Subtraction</a> (<code>sub</code>)</li>
						<li><a href="#op-mult">Multiplication</a> (<code>mult</code>)</li>
					</ol>
				</li>
				<li><a href="#ops-logic-r">Logic (R-Types)</a>
					<ol>
						<li><a href="#op-and">Bitwise AND</a> (<code>and</code>)</li>
						<li><a href="#op-nand">Bitwise NAND</a> (<code>nand</code>)</li>
						<li><a href="#op-nor">Bitwise NOR</a> (<code>nor</code>)</li>
						<li><a href="#op-not">Bitwise NOT</a> (<code>not</code>)</li>
						<li><a href="#op-or">Bitwise OR</a> (<code>or</code>)</li>
						<li><a href="#op-xnor">Bitwise XNOR</a> (<code>xnor</code>)</li>
						<li><a href="#op-xor">Bitwise XOR</a> (<code>xor</code>)</li>
					</ol>
				</li>
				<li><a href="#ops-math-i">Math (I-Types)</a>
					<ol>
						<li><a href="#op-addi">Addition Immediate</a> (<code>addi</code>)</li>
						<li><a href="#op-subi">Subtraction Immediate</a> (<code>subi</code>)</li>
						<li><a href="#op-multi">Multiplication Immediate</a> (<code>multi</code>)</li>
					</ol>
				</li>
				<li><a href="#ops-logic-i">Logic (I-Types)</a>
					<ol>
						<li><a href="#op-andi">Bitwise AND Immediate</a> (<code>andi</code>)</li>
						<li><a href="#op-nandi">Bitwise NAND Immediate</a> (<code>nandi</code>)</li>
						<li><a href="#op-nori">Bitwise NOR Immediate</a> (<code>nori</code>)</li>
						<li><a href="#op-noti">Bitwise NOT Immediate</a> (<code>noti</code>)</li>
						<li><a href="#op-ori">Bitwise OR Immediate</a> (<code>ori</code>)</li>
						<li><a href="#op-xnori">Bitwise XNOR Immediate</a> (<code>xnori</code>)</li>
						<li><a href="#op-xori">Bitwise XOR Immediate</a> (<code>xori</code>)</li>
					</ol>
				</li>
				<li><a href="#ops-data-i">Data (I-Types)</a>
					<ol>
						<li><a href="#op-lui">Load Upper Immediate</a> (<code>lui</code>)</li>
						<li><a href="#op-mfhi">Move From HI Register</a> (<code>mfhi</code>)</li>
						<li><a href="#op-mflo">Move From LO Register</a> (<code>mflo</code>)</li>
					</ol>
				</li>
				<li><a href="#ops-comp-r">Comparisons (R-Types)</a>
					<ol>
						<li><a href="#op-sl">Set on Less Than</a> (<code>sl</code>)</li>
						<li><a href="#op-sle">Set on Less Than or Equal</a> (<code>sl</code>)</li>
						<li><a href="#op-seq">Set on Equal</a> (<code>sl</code>)</li>
						<li><a href="#op-sge">Set on Greater Than or Equal</a> (<code>sge</code>)</li>
						<li><a href="#op-sg">Set on Greater Than</a> (<code>sg</code>)</li>
					</ol>
				</li>
				<li><a href="#ops-jump">Jumps (J-Types)</a>
					<ol>
						<li><a href="#op-j">Jump</a> (<code>j</code>)</li>
					</ol>
				</li>
				<li><a href="#ops-pseudo">Pseudoinstructions</a>
					<ol>
						<li><a href="#op-la">Load Address</a> (<code>la</code>)</li>
						<li><a href="#op-li">Load Immediate</a> (<code>li</code>)</li>
						<li><a href="#op-mv">Move</a> (<code>mv</code>)</li>
					</ol>
				</li>
			</ol>
		</li>
		<li><a href="#format">Instruction Format</a>
			<ol>
				<li><a href="#format-r">R-Type Instructions</a></li>
				<li><a href="#format-i">I-Type Instructions</a></li>
				<li><a href="#format-j">J-Type Instructions</a></li>
			</ol>
		</li>
	</ol>
</blockquote>

# <a name="intro"></a>Introduction

The VM emulates a custom instruction set that may or may not actually be theoretically implementable as real hardware. (As of this writing, the only relevant class I've taken is CMPE 12). This instruction set has 64-bit word length, but the memory addressability is 32 bits. 

# <a name="prog"></a>Programs

Programs are divided into four sections: metadata, handler pointers, data and code. The code section consists of executable code. The exception handler pointer section, as its name suggests, contains pointers to functions in the code section to handle exceptions (such as overflows and division by zero).

## <a name="prog-meta"></a>Metadata Section
The metadata section is a fixed-size (exact size TBD) block of data at the beginning of the program that contains the beginning addresses of the other sections.

## <a name="prog-ptrs"></a>Handler Pointer Section
As its name suggests, the handler pointer section contains pointers to functions stored in the code section that handle various situations, such as exceptions (e.g., overflows and division by zero).

## <a name="prog-data"></a>Data Section
The data section contains non-code program data. Execution is not expected to occur in the data section, but there is no error checking to prevent it.

## <a name="prog-code"></a>Code Section
The code section consists of executable code. This is the only section of the code that the program counter is expected to point to.

# <a name="registers"></a>Registers
There are 128 registers. Their purposes are pretty much stolen from MIPS:

| Number   | Name         | Purpose                                     |
|----------|--------------|---------------------------------------------|
| 0        | `$0`         | Always contains zero.                       |
| 1        | `$gp`        | Global area pointer (start of data segment) |
| 2        | `$sp`        | Stack pointer.                              |
| 3        | `$fp`        | Frame pointer.                              |
| 4        | `$ra`        | Return address.                             |
| 5–20     | `$r0`–`$rf`  | Contains return values.                     |
| 21–36    | `$a0`–`$af`  | Contains arguments for subroutines.         |
| 37–63    | `$t0`–`$t1a` | Temporary registers.                        |
| 64–90    | `$s0`–`$s1a` | Saved registers.                            |
| 91–107   | `$k0`–`$k10` | Kernel registers.                           |
| 108–123  | `$m0`–`$mf`  | Reserved for use by the assembler.          |
| 124–127  | `$f0`–`$f3`  | Floating point return values.               |

<a name="hi-lo"></a>In addition, there are two extra registers (`HI` and `LO`), but they aren't directly accessible from code; the contents are accessed using the  [`mfhi`](#mfhi) and [`mflo`](#mflo) instructions.

# <a name="operations"></a>Operations

## <a name="ops-math-r"></a>Math (R-Types)

### <a name="op-add"></a>Addition
> `add rd, rs, rt`  
> `$rs + $rt -> $rd`  
> `000000000001` `ttttttt` `sssssss` `ddddddd` `000` `0000000000000000` `000000000000`

Adds the values in `rs` and `rt` and stores the result in `rd`.

### <a name="op-sub"></a>Subtraction
> `sub rd, rs, rt`  
> `$rs - $rt -> $rd`  
> `000000000001` `ttttttt` `sssssss` `ddddddd` `000` `0000000000000000` `000000000001`

Subtracts the value in `rt` from the value in `rs` and stores the result in `rd`.

### <a name="op-mult"></a>Multiplication
> `mult rs, rt`  
> `$rs * $rt`  
> `000000000001` `ttttttt` `sssssss` `ddddddd` `000` `0000000000000000` `000000000002`

Multiplies the value in `rs` by the value in `rt` and stories the upper half in [`HI`](#hi-lo) and the lower half in [`LO`](#hi-lo).

## <a name="ops-logic-r"></a>Logic (R-Types)

### <a name="op-and"></a>Bitwise AND
> `and rd, rs, rt`  
> `$rs & $rt -> $rd`  
> `000000000010` `ttttttt` `sssssss` `ddddddd` `000` `0000000000000000` `000000000000`
   
Computes the bitwise AND of `rs` and `rt` and stores the result in `rd`.

### <a name="op-nand"></a>Bitwise NAND
> `nand rd, rs, rt`  
> `$rs ~& $rt -> $rd`  
> `000000000010` `ttttttt` `sssssss` `ddddddd` `000` `0000000000000000` `000000000001`
   
Computes the bitwise NAND of `rs` and `rt` and stores the result in `rd`.

### <a name="op-nor"></a>Bitwise NOR
> `nor rd, rs, rt`  
> `$rs ~| $rt -> $rd`  
> `000000000010` `ttttttt` `sssssss` `ddddddd` `000` `0000000000000000` `000000000002`
   
Computes the bitwise NOR of `rs` and `rt` and stores the result in `rd`.

### <a name="op-not"></a>Bitwise NOT
> `not rd, rs`  
> `~$rs -> $rd`  
> `000000000010` `ttttttt` `sssssss` `ddddddd` `000` `0000000000000000` `000000000003`
   
Computes the bitwise NOT of `rs` and stores the result in `rd`.

### <a name="op-or"></a>Bitwise OR
> `or rd, rs, rt`  
> `$rs | $rt -> $rd`  
> `000000000010` `ttttttt` `sssssss` `ddddddd` `000` `0000000000000000` `000000000004`
   
Computes the bitwise OR of `rs` and `rt` and stores the result in `rd`.

### <a name="op-nxor"></a>Bitwise XNOR
> `xnor rd, rs, rt`  
> `$rs ~x $rt -> $rd`  
> `000000000010` `ttttttt` `sssssss` `ddddddd` `000` `0000000000000000` `000000000005`
   
Computes the bitwise XNOR of `rs` and `rt` and stores the result in `rd`.

### <a name="op-xor"></a>Bitwise XOR
> `xor rd, rs, rt`  
> `$rs x $rt -> $rd`  
> `000000000010` `ttttttt` `sssssss` `ddddddd` `000` `0000000000000000` `000000000006`

Computes the bitwise XOR of `rs` and `rt` and stores the result in `rd`.

## <a name="ops-math-i"></a>Math (I-Types)

### <a name="op-addi"></a>Addition Immediate
> `addi rd, rs, imm`  
> `$rs + imm -> $rd`  
> `000000000011` `000000` `sssssss` `ddddddd` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`

Adds the value in `rs` and a constant and stores the result in `rd`.

### <a name="op-subi"></a>Subtraction Immediate
> `subi rd, rs, imm`  
> `$rs - imm -> $rd`  
> `000000000100` `000000` `sssssss` `ddddddd` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`

Subtracts a constant from the value in `rs` and stores the result in `rd`.

### <a name="op-multi"></a>Multiplication Immediate
> `multi rd, rs, imm`  
> `$rs * imm -> $rd`  
> `000000000101` `000000` `sssssss` `ddddddd` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`

Multiplies the value in `rs` by a constant and stories the upper half in [`HI`](#hi-lo) and the lower half in [`LO`](#hi-lo).

## <a name="ops-logic-i"></a>Logic (I-Types)

### <a name="op-andi"></a>Bitwise AND Immediate
> `andi rd, rs, imm`  
> `$rs & imm -> $rd`  
> `000000000110` `000000` `sssssss` `ddddddd` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`

Computes the bitwise AND of `rs` and a constant and stores the result in `rd`.

### <a name="op-nandi"></a>Bitwise NAND Immediate
> `nandi rd, rs, imm`  
> `$rs ~& imm -> $rd`  
> `000000000111` `000000` `sssssss` `ddddddd` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`

Computes the bitwise NAND of `rs` and a constant and stores the result in `rd`.

### <a name="op-nori"></a>Bitwise NOR Immediate
> `nori rd, rs, imm`  
> `$rs ~| imm -> $rd`  
> `000000001000` `000000` `sssssss` `ddddddd` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`
   
Computes the bitwise NOR of `rs` and a constant and stores the result in `rd`.

### <a name="op-ori"></a>Bitwise OR Immediate
> `ori rd, rs, imm`  
> `$rs | imm -> $rd`  
> `000000001001` `000000` `sssssss` `ddddddd` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`
   
Computes the bitwise OR of `rs` and a constant and stores the result in `rd`.

### <a name="op-xnori"></a>Bitwise XNOR Immediate
> `xnori rd, rs, imm`  
> `$rs ~x imm -> $rd`  
> `000000001010` `000000` `sssssss` `ddddddd` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`
   
Computes the bitwise XNOR of `rs` and a constant and stores the result in `rd`.

### <a name="op-xori"></a>Bitwise XOR Immediate
> `xori rd, rs, imm`  
> `$rs x imm -> $rd`  
> `000000001011` `000000` `sssssss` `ddddddd` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`

Computes the bitwise XOR of `rs` and a constant and stores the result in `rd`.

## <a name="ops-data-i"></a>Data (I-Types)

### <a name="op-lui"></a>Load Upper Immediate
> `lui rd, imm`  
> `lui: imm -> $rd`  
> `000000001100` `000000` `0000000` `ddddddd` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`

Loads an immediate value into the upper half the word at `rd`. The lower half is replaced with zeroes.

### <a name="op-mfhi"></a>Move From HI Register
> `mfhi rd`  
> `%hi -> $rd`  
> `000000001101` `000000` `0000000` `ddddddd` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`

Copies the value of the [`HI`](#hi-lo) register into `rd`.

### <a name="op-mflo"></a>Move From LO Register
> `mflo rd`  
> `%lo -> $rd`  
> `000000001110` `000000` `0000000` `ddddddd` `iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii`

Copies the value of the [`LO`](#hi-lo) register into `rd`.

## <a name="ops-comp-r"></a>Comparisons (R-Type)

### <a name="op-sl"></a>Set on Less Than
> `sl rd, rs, rt`  
> `$rs < $rt -> $rd`  
> `000000001111` `ttttttt` `sssssss` `ddddddd` `000` `0000000000000000` `000000000000`

If the value in `rs` is less than the value in `rt`, `rd` is set to 1; otherwise, `rd` is set to 0.

### <a name="op-sle"></a>Set on Less Than or Equal
> `sle rd, rs, rt`  
> `$rs <= $rt -> $rd`  
> `000000001111` `ttttttt` `sssssss` `ddddddd` `000` `0000000000000000` `000000000001`

If the value in `rs` is less than or equal to the value in `rt`, `rd` is set to 1; otherwise, `rd` is set to 0.

### <a name="op-seq"></a>Set on Equal
> `seq rd, rs, rt`  
> `$rs = $rt -> $rd`  
> `000000001111` `ttttttt` `sssssss` `ddddddd` `000` `0000000000000000` `000000000002`

If the value in `rs` is equal to the value in `rt`, `rd` is set to 1; otherwise, `rd` is set to 0.

### <a name="op-sge"></a>*Set on Greater Than or Equal*
> `sge rd, rs, rt`  
> `$rs >= $rt -> $rd`

If the value in `rs` is greather than or equal to the value in `rt`, `rd` is set to 1; otherwise, `rd` is set to 0.  
This is a pseudoinstruction; its translation is `sle rd, rt, rs`.

### <a name="op-sg"></a>*Set on Greater Than*
> `sg rd, rs, rt`  
> `$rs > $rt -> $rd`

If the value in `rs` is greather than the value in `rt`, `rd` is set to 1; otherwise, `rd` is set to 0.  
This is a pseudoinstruction; its translation is `sl rd, rt, rs`.

## <a name="ops-jump"></a>Jumps (J-Types)

### <a name="op-j"></a>Jump
> `j target`  
> `: &var` or `: imm`  
> `TODO`

Jumps to the address of a given label or directly to a given address.

## <a name="ops-pseudo"></a>Pseudoinstructions

### <a name="op-la"></a>Load Address
> `la rd, var`  
> `&var -> $rd`

Loads the address of a variable into `rd`. <!-- TODO: translation -->

### <a name="op-li"></a>Load Immediate
> `li rd, imm`  
> `imm -> $rd`
 
Loads a constant into `rd`.  
Translates to <code>[lui](#lui) rd, u</code>  <code>[ori](#ori) rd, rd, l</code>, where `u` is the upper half of `imm` and `l` is the lower half.

### <a name="op-mv"></a>Move
> `mv rd, rs`  
> `$rs -> $rd`

Copies the value of `rs` into `rd`.  
Translates to <code>[add](#add) rd, rs, $0</code>.

# <a name="format"></a>Instruction Format
Like much of this instruction set, the formatting for instructions is copied from MIPS with a few modifications (for example, instructions are 64 bits long in this instruction set, as opposed to 32 for MIPS64).

## <a name="format-r"></a>R-Type Instructions
R-type instructions perform computations with multiple registers.

| Range       | 63–52 (12)  | 51–45 (7) | 44–38 (7) | 37–31 (7) | 30–28 (3) | 27–12 (16) | 11–0 (12)  |
|------------:|:-----------:|:---------:|:---------:|:---------:|:---------:|:----------:|:----------:|
| **Purpose** | Opcode      | rt        | rs        | rd        | Unused    | Shift      | Function   |

## <a name="format-i"></a>I-Type Instructions
I-type instructions perform computations with registers and an immediate value.

| Range       | 63–52 (12) | 51–46 (6) | 45–39 (7) | 38–32 (7) | 31–0 (32)       |
|------------:|:----------:|:---------:|:---------:|:---------:|:---------------:|
| **Purpose** | Opcode     | Unused    | rs        | rd        | Immediate Value |

## <a name="format-j"></a>J-Type Instructions
J-type instructions point the program counter to a given address under certain circumstances.

|   Range | 63–52 (12)  | 51–32 (20) | 31–0 (32)   |
|--------:|:-----------:|:----------:|:-----------:|
| Purpose | Opcode      | Unused     | Address     |