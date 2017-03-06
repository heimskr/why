The VM emulates a custom instruction set that may or may not actually be theoretically implementable as real hardware. (As of this writing, the only relevant class I've taken is CMPE 12). This instruction set has 64-bit word length.

# Programs

Programs are divided into four sections: metadata, handler pointers, data and code. The code section consists of executable code. The exception handler pointer section, as its name suggests, contains pointers to functions in the code section to handle exceptions (such as overflows and division by zero).

## Metadata Section
The metadata section is a fixed-size (exact size TBD) block of data at the beginning of the program that contains the beginning addresses of the other sections.

## Handler Pointer Section
As its name suggests, the handler pointer section contains pointers to functions stored in the code section that handle various situations, such as exceptions (e.g., overflows and division by zero).

## Data Section
The data section contains non-code program data. Execution is not expected to occur in the data section, but there is no error checking to prevent it.

## Code Section
The code section consists of executable code. This is the only section of the code that the program counter is expected to point to.

# Registers
There are 64 registers. Their purposes and layout are pretty much stolen from MIPS:

| Number | Name         | Purpose                                     |
|--------|--------------|---------------------------------------------|
| 0      | `$0`         | Always contains zero.                       |
| 1–4    | `$r0`–`$r3`  | Contains return values.                     |
| 5–12   | `$a0`–`$a7`  | Contains arguments for subroutines.         |
| 13–28  | `$t0`–`$tf`  | Temporary registers.                        |
| 29–44  | `$s0`–`$sf`  | Saved registers.                            |
| 45–52  | `$k0`–`$k7`  | Kernel registers.                           |
| 53–55  | `$m0`–`$m2`  | Reserved for use by the assembler.          |
| 56     | `$gp`        | Global area pointer (start of data segment) |
| 57     | `$sp`        | Stack pointer.                              |
| 58     | `$fp`        | Frame pointer.                              |
| 59     | `$ra`        | Return address.                             |
| 60–63  | `$f0`–`$f3`  | Floating point return values.               |

<a name="hi-lo"></a>In addition, there are two extra registers (`HI` and `LO`), but they aren't directly accessible from code; the contents are accessed using the  [`mfhi`](#mfhi) and [`mflo`](#mflo) instructions.



# Operations

## Math

### <a name="add"></a>Addition
> `add rd, rs, rt`  
> `$rs + $rt -> $rd`

Adds the values in `rs` and `rt` and stores the result in `rd`.

### <a name="sub"></a>Subtraction
> `sub rd, rs, rt`  
> `$rs - $rt -> $rd`

Subtracts the value in `rt` from the value in `rs` and stores the result in `rd`.

### <a name="mult"></a>Multiplication
> `mult rs, rt`  
> `$rs * $rt`

Multiplies the value in `rs` by the value in `rt` and stories the upper half in [`HI`](#hi-lo) and the lower half in [`LO`](#hi-lo).

## Logic

### <a name="and"></a>Bitwise AND
> `and rd, rs, rt`  
> `$rs & $rt -> $rd`
   
Computes the bitwise AND of `rs` and `rt` and stores the result in `rd`.

### <a name="andi"></a>Bitwise AND Immediate
> `andi rd, rs, imm`  
> `$rs & imm -> $rd`

Computes the bitwise AND of `rs` and a constant and stores the result in `rd`.

### <a name="nand"></a>Bitwise NAND
> `nand rd, rs, rt`  
> `$rs ~& $rt -> $rd`
   
Computes the bitwise NAND of `rs` and `rt` and stores the result in `rd`.

### <a name="nandi"></a>Bitwise NAND Immediate
> `nandi rd, rs, imm`  
> `$rs ~& imm -> $rd`
   
Computes the bitwise NAND of `rs` and a constant and stores the result in `rd`.

### <a name="nor"></a>Bitwise NOR
> `nor rd, rs, rt`  
> `$rs ~| $rt -> $rd`
   
Computes the bitwise NOR of `rs` and `rt` and stores the result in `rd`.

### <a name="nori"></a>Bitwise NOR Immediate
> `nori rd, rs, imm`  
> `$rs ~| imm -> $rd`
   
Computes the bitwise NOR of `rs` and a constant and stores the result in `rd`.

### <a name="not"></a>Bitwise NOT
> `not rd, rs`  
> `~$rs -> $rd`
   
Computes the bitwise NOT of `rs` and stores the result in `rd`.

### <a name="or"></a>Bitwise OR
> `or rd, rs, rt`  
> `$rs | $rt -> $rd`
   
Computes the bitwise OR of `rs` and `rt` and stores the result in `rd`.

### <a name="ori"></a>Bitwise OR Immediate
> `ori rd, rs, imm`  
> `$rs | imm -> $rd`
   
Computes the bitwise OR of `rs` and a constant and stores the result in `rd`.

### <a name="nxor"></a>Bitwise XNOR
> `xnor rd, rs, rt`  
> `$rs ~x $rt -> $rd`
   
Computes the bitwise XNOR of `rs` and `rt` and stores the result in `rd`.

### <a name="xnori"></a>Bitwise XNOR Immediate
> `xnori rd, rs, imm`  
> `$rs ~x imm -> $rd`
   
Computes the bitwise XNOR of `rs` and a constant and stores the result in `rd`.

### <a name="xor"></a>Bitwise XOR
> `xor rd, rs, rt`  
> `$rs x $rt -> $rd`

Computes the bitwise XOR of `rs` and `rt` and stores the result in `rd`.

### <a name="xori"></a>Bitwise XOR Immediate
> `xori rd, rs, imm`  
> `$rs x imm -> $rd`

Computes the bitwise XOR of `rs` and a constant and stores the result in `rd`.

## Data

### <a name="lui"></a>Load Upper Immediate
> `lui rd, imm`  
> `lui: imm -> $rd`

Loads an immediate value into the upper half the word at `rd`. The lower half is replaced with zeroes.

### <a name="mfhi"></a>Move From HI Register
> `mfhi rd`  
> `%hi -> $rd`

Copies the value of the [`HI`](#hi-lo) register into `rd`.

### <a name="mflo"></a>Move From LO Register
> `mflo rd`  
> `%lo -> $rd`

Copies the value of the [`LO`](#hi-lo) register into `rd`.

## Pseudoinstructions

### <a name="la"></a>Load Address
> <a name="la"></a>`la rd, var`  
> `&var -> $rd`

Loads the address of a variable into `rd`. <!-- TODO: translation -->

### <a name="li"></a>Load Immediate
> `li rd, imm`  
> `imm -> $rd`
 
Loads a constant into `rd`.  
Translates to <code>[lui](#lui) rd, u</code>  <code>[ori](#ori) rd, rd, l</code>, where `u` is the upper half of `imm` and `l` is the lower half.

### Move
> `mv rd, rs`  
> `$rs -> $rd`

Copies the value of `rs` into `rd`.  
Translates to <code>[add](#add) rd, rs, $0</code>.