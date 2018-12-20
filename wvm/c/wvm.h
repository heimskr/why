#ifndef __WVM_H__
#define __WVM_H__

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define DIE(s) { fprintf(stderr, s); exit(1); }
#define DIEF(f, a...) { fprintf(stderr, f, a); exit(1); }
#define FLAG_Z (registers[R_ST] & 0b0001)
#define FLAG_N (registers[R_ST] & 0b0010)
#define FLAG_C (registers[R_ST] & 0b0100)
#define FLAG_O (registers[R_ST] & 0b1000)
#define FLAG_Z_SET(n) registers[R_ST] = (registers[R_ST] & ~0b0001) | n
#define FLAG_N_SET(n) registers[R_ST] = (registers[R_ST] & ~0b0010) | (n << 1)
#define FLAG_C_SET(n) registers[R_ST] = (registers[R_ST] & ~0b0100) | (n << 2)
#define FLAG_O_SET(n) registers[R_ST] = (registers[R_ST] & ~0b1000) | (n << 3)

#define RING_MIN 0
#define RING_MAX 3

typedef int64_t word;
typedef uint64_t uword;
typedef uint8_t byte;
typedef enum {RING_INVALID = -2, RING_ANY = -1, RING_KERNEL = 0, RING_1 = 1, RING_2 = 2, RING_USER = 3} ring_t;

bool wvm_init(word length);
void wvm_free();
int wvm_load(char *filename);
void wvm_init_vm();
word wvm_get_word(word addr);
void wvm_set_word(word addr, word value);
byte wvm_get_byte(word addr);
void wvm_set_byte(word addr, byte value);
char * wvm_get_string(word addr);
void wvm_jump(word addr);
void wvm_link();
void wvm_increment_pc();
int wvm_change_ring(ring_t new_ring);
bool wvm_tick();
void wvm_alu_flags_clear();
void wvm_alu_flags_update(word n);
int wvm_check_condition(cond_t n);
void wvm_print_memory();

byte *memory;
size_t memsize;
word pc; // in bytes, not words.
word offset_symtab;
word offset_data;
word offset_code;
word offset_end;
word registers[128];
word membytes;
int cycles;
bool alive;
ring_t cur_ring;

#endif
