#ifndef __WVM_H__
#define __WVM_H__

#include <stdint.h>
typedef long long int lomg;

int wvm_init(lomg length);
void wvm_free();
int wvm_load(char *filename);
void wvm_init_pc();

enum ins_type {J, I, R};

#endif