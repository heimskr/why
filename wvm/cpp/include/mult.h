#ifndef MULT_H_
#define MULT_H_

#include <stdint.h>

// https://github.com/catid/fp61/blob/master/fp61.h
// Copyright (c) 2018, Chris Taylor
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

// Compiler-specific force inline keyword
#ifdef _MSC_VER
# define FP61_FORCE_INLINE inline __forceinline
#else
# define FP61_FORCE_INLINE inline __attribute__((always_inline))
#endif


//------------------------------------------------------------------------------
// Portable 64x64->128 Multiply
// CAT_MUL128: r{hi,lo} = x * y

// Returns low part of product, and high part is set in r_hi
FP61_FORCE_INLINE uint64_t Emulate64x64to128(uint64_t *r_hi, const uint64_t x, const uint64_t y) {
    const uint64_t x0 = (uint32_t) x, x1 = x >> 32;
    const uint64_t y0 = (uint32_t) y, y1 = y >> 32;
    const uint64_t p11 = x1 * y1, p01 = x0 * y1;
    const uint64_t p10 = x1 * y0, p00 = x0 * y0;
    /*
        This is implementing schoolbook multiplication:

                x1 x0
        X       y1 y0
        -------------
                   00  LOW PART
        -------------
                00
             10 10     MIDDLE PART
        +       01
        -------------
             01 
        + 11 11        HIGH PART
        -------------
    */

    // 64-bit product + two 32-bit values
    const uint64_t middle = p10 + (p00 >> 32) + (uint32_t) p01;

    /*
        Proof that 64-bit products can accumulate two more 32-bit values
        without overflowing:

        Max 32-bit value is 2^32 - 1.
        PSum = (2^32-1) * (2^32-1) + (2^32-1) + (2^32-1)
             = 2^64 - 2^32 - 2^32 + 1 + 2^32 - 1 + 2^32 - 1
             = 2^64 - 1
        Therefore it cannot overflow regardless of input.
    */

    // 64-bit product + two 32-bit values
    *r_hi = p11 + (middle >> 32) + (p01 >> 32);

    // Add LOW PART and lower half of MIDDLE PART
    return (middle << 32) | (uint32_t) p00;
}

#if defined(_MSC_VER) && defined(_WIN64)
// Visual Studio 64-bit

# include <intrin.h>
# pragma intrinsic(_umul128)
# define CAT_MUL128(r_hi, r_lo, x, y) \
    r_lo = _umul128(x, y, &(r_hi));

#elif defined(__SIZEOF_INT128__)
// Compiler supporting 128-bit values (GCC/Clang)

# define CAT_MUL128(r_hi, r_lo, x, y) {                 \
        unsigned __int128 w = (unsigned __int128)x * y; \
        r_lo = (uint64_t)w;                             \
        r_hi = (uint64_t)(w >> 64);                     \
    }

#else
// Emulate 64x64->128-bit multiply with 64x64->64 operations

# define CAT_MUL128(r_hi, r_lo, x, y) \
    r_lo = Emulate64x64to128(&(r_hi), x, y);

#endif // End CAT_MUL128
#endif