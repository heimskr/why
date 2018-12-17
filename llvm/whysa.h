#ifndef _WHYSA_H_
#define _WHYSA_H_

#include <stdint.h>

// Special
void _int(int64_t type);
void _rit(int64_t *table[]);
void _time(int64_t micro);

// External
void _halt();
void _prc(int64_t s);
void _prd(int64_t n);
void _prx(int64_t n);

#endif
