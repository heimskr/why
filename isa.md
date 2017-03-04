# Instruction Set Architecture

The VM emulates a custom instruction set. This instruction set has 64-bit word length.

## Math

> `add rd, rs, rt`  
> `[$rs + $rt -> $rd]`

Adds the values in `rs` and `rt` and stores the result in `rd`.

> `sub rd, rs, rt`  
> `[$rs - $rt -> $rd]`

Subtracts the value in `rt` from the value in `rs` and stores the result in `rd`.

> `mult rs, rt`  
> `[$rs * $rt]`

Multiplies the value in `rs` by the value in `rt` and stories the upper half in `HI` and the lower half in `LO`.

## Logic

### Bitwise AND
> `and rd, rs, rt`  
> `[$rs & $rt -> $rd]`
   
Computes the bitwise AND of `rs` and `rt` and stores the result in `rd`.

### Bitwise AND Immediate
> `andi rd, rs, imm`  
> `[$rs & imm -> $rd]`

Computes the bitwise AND of `rs` and a constant and stores the result in `rd`.

### Bitwise NAND
> `nand rd, rs, rt`  
> `[$rs ~& $rt -> $rd]`
   
Computes the bitwise NAND of `rs` and `rt` and stores the result in `rd`.

### Bitwise NAND Immediate
> `nandi rd, rs, imm`  
> `[$rs ~& imm -> $rd]`
   
Computes the bitwise NAND of `rs` and a constant and stores the result in `rd`.

### Bitwise NOR
> `nor rd, rs, rt`  
> `[$rs ~| $rt -> $rd]`
   
Computes the bitwise NOR of `rs` and `rt` and stores the result in `rd`.

### Bitwise NOR Immediate
> `nori rd, rs, imm`  
> `[$rs ~| imm -> $rd]`
   
Computes the bitwise NOR of `rs` and a constant and stores the result in `rd`.

### Bitwise NOT
> `not rd, rs`  
> `[~$rs -> $rd]`
   
Computes the bitwise NOT of `rs` and stores the result in `rd`.

### Bitwise OR
> `or rd, rs, rt`  
> `[$rs | $rt -> $rd]`
   
Computes the bitwise OR of `rs` and `rt` and stores the result in `rd`.

### Bitwise OR Immediate
> `ori rd, rs, imm`  
> `[$rs | imm -> $rd]`
   
Computes the bitwise OR of `rs` and a constant and stores the result in `rd`.

### Bitwise XNOR
> `xnor rd, rs, rt`  
> `[$rs ~x $rt -> $rd]`
   
Computes the bitwise XNOR of `rs` and `rt` and stores the result in `rd`.

### Bitwise XNOR Immediate
> `xnori rd, rs, imm`  
> `[$rs ~x imm -> $rd]`
   
Computes the bitwise XNOR of `rs` and a constant and stores the result in `rd`.

### Bitwise XOR
> `xor rd, rs, rt`  
> `[$rs x $rt -> $rd]`

Computes the bitwise XOR of `rs` and `rt` and stores the result in `rd`.

### Bitwise XOR Immediate
> `xori rd, rs, imm`  
> `[$rs x imm -> $rd]`

Computes the bitwise XOR of `rs` and a constant and stores the result in `rd`.

## Data

### Load Upper Immediate
> `lui rd, imm`  
> `[lui: imm -> $rd]`

Loads an immediate value into the upper half the word at `rd`. The lower half is replaced with zeroes.

### Move From HI Register
> `mfhi rd`  
> `[%hi -> $rd]`

Copies the value of the `HI` register into `rd`.

### Move From LO Register
> `mflo rd`  
> `[%lo -> $rd]`

Copies the value of the `LO` register into `rd`.

## Pseudoinstructions

### Load Address
> `la rd, var`  
> `[&var -> $rd]`

Loads the address of a variable into `rd`. <!-- TODO: translation -->

### Load Immediate
> `li rd, imm`  
> `[imm -> $rd]`
 
Loads a constant into `rd`.  
Translates to `lui rd, u`  `ori rd, rd, l`, where `u` is the upper half of `imm` and `l` is the lower half.

### Move
> `mv rd, rs`  
> `[$rs -> $rd]`

Copies the value of `rs` into `rd`.  
Translates to `add rd, rs, $0`.

# Registers
There are 64 registers. Their purposes and layout are pretty much stolen from MIPS:

| Number | Name         | Purpose                                     |
|--------|--------------|---------------------------------------------|
| 0      | `$0`         | Always contains zero.                       |
| 1–4    | `$r0`–`$r3`  | Contains return values.                     |
| 5–12   | `$a0`–`$a7`  | Contains arguments for subroutines.         |
| 13-28  | `$t0`–`$tf`  | Temporary registers.                        |
| 29-44  | `$s0`–`$sf`  | Saved registers.                            |
| 45–52  | `$k0`–`$k7`  | Kernel registers.                           |
| 53-55  | `$m0`–`$m2`  | Reserved for use by the assembler.          |
| 56     | `$gp`        | Global area pointer (start of data segment) |
| 57     | `$sp`        | Stack pointer.                              |
| 58     | `$fp`        | Frame pointer.                              |
| 59     | `$ra`        | Return address.                             |
| 60–63  | `$f0`–`$f3`  | Floating point return values.               |

In addition, there are two extra registers (`HI` and `LO`), but they aren't directly accessible from code; the contents are accessed using the  `mfhi` and `mflo` instructions.

# Programs

Programs are divided into two main sections (data and code) and one secondary section (exception handler pointers). The data section consists of data not meant to be executed, while the code section consists of executable code. The exception handler pointer section, as its name suggests, contains pointers to functions in the code section to handle exceptions.