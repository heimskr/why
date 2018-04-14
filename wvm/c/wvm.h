#ifndef __WVM_H__
#define __WVM_H__

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef int64_t word;
typedef uint64_t uword;
typedef uint8_t byte;

bool wvm_init(word length);
void wvm_free();
int wvm_load(char *filename);
void wvm_init_pc();
word wvm_get_word(word n);
char wvm_get_byte(word byte_offset);
void wvm_jump(word addr);
void wvm_link();
void wvm_print_memory();

word *memory;
size_t memsize;
word pc; // in bytes, not words.
word ir;
word offset_handlers;
word offset_data;
word offset_code;
word offset_end;
word registers[128];

#endif