/*
 * mad - MPEG audio decoder
 * Copyright (C) 2000 Robert Leslie
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Id: fixed.h,v 1.9 2000/04/22 04:36:50 rob Exp $
 */

# ifndef FIXED_H
# define FIXED_H

# if SIZEOF_INT >= 4
typedef   signed int mad_fixed_t;

typedef   signed int mad_fixed64hi_t;
typedef unsigned int mad_fixed64lo_t;
# else
typedef   signed long mad_fixed_t;

typedef   signed long mad_fixed64hi_t;
typedef unsigned long mad_fixed64lo_t;
# endif

/*
 * Fixed-point format: 0xABBBBBBB
 * A == whole part      (sign + 3 bits)
 * B == fractional part (28 bits)
 *
 * Values are signed two's complement, so the effective range is:
 * 0x80000000 to 0x7fffffff
 *       -8.0 to +7.9999999962747097015380859375
 *
 * The smallest representable value is:
 * 0x00000001 == 0.0000000037252902984619140625 (i.e. about 3.73e-9)
 *
 * 28 bits of fractional accuracy represent about
 * 8.6 digits of decimal accuracy.
 *
 * Fixed-point numbers can be added or subtracted as normal
 * integers, but multiplication requires shifting the 64-bit result
 * from 56 fractional bits back to 28 (and rounding.)
 *
 * The CPU-specific versions of mad_f_mul() perform rounding by truncation.
 */

# define MAD_F_MIN		0x80000000L
# define MAD_F_MAX		0x7fffffffL

# define mad_f_add(x, y)	((x) + (y))
# define mad_f_sub(x, y)	((x) - (y))

# define mad_f_scale64(hi, lo)	((mad_fixed_t)  \
                                 (((mad_fixed64hi_t) (hi) << 4) |  \
                                  ((mad_fixed64lo_t) (lo) >> 28)))

# if defined(FPM_APPROX)

/* This version is the most portable but loses 14 bits of accuracy. */

#  define FPM_MACRO
#  define mad_f_mul(x, y)	((((x) + 0x00002000L) >> 14) *  \
                                 (((y) + 0x00002000L) >> 14))

# elif defined(FPM_64BIT)

/* This version should be the most accurate if 64-bit (long long) types
   are supported by the compiler. */

#  define FPM_MACRO
#  define mad_f_mul(x, y)  \
     ((mad_fixed_t) (((((signed long long) (x) * (y)) + 0x08000000L) >> 28)))

# elif defined(FPM_INTEL)

/* This Intel version is accurate but always rounds down the least
   significant bit. */

#  define FPM_MACRO
#  define mad_f_mul(x, y)  \
     ({ mad_fixed64hi_t __hi;  \
        mad_fixed64lo_t __lo;  \
        asm ("imull %3"  \
             : "=a" (__lo), "=d" (__hi)  \
	     : "%a" (x), "rm" (y));  \
        mad_f_scale64(__hi, __lo);  \
     })

# if 0
/* this is slower than the C multiply/add version */
#  define FPM_MACC
#  define mad_f_macc(hi, lo, x, y)  \
     ({ mad_fixed64hi_t __hi;  \
        mad_fixed64lo_t __lo;  \
        asm ("imull %3"  \
	     : "=a" (__lo), "=d" (__hi)  \
	     : "%a" (x), "rm" (y));  \
        asm ("add %2,%0"  \
             : "=r,rm" (lo)  \
             : "%0,0" (lo), "rm,r" (__lo));  \
        asm ("adc %2,%0"  \
             : "=r,rm" (hi)  \
             : "%0,0" (hi), "rm,r" (__hi));  \
     })
# endif

# elif defined(FPM_ARM)

/* This ARM V4 version is accurate but always rounds down the least
   significant bit. */

#  define FPM_MACRO
#  define mad_f_mul(x, y)  \
     ({ mad_fixed64hi_t __hi;  \
        mad_fixed64lo_t __lo;  \
        asm ("smull %0,%1,%2,%3"  \
	     : "=&r" (__lo), "=&r" (__hi)  \
	     : "%r" (x), "r" (y));  \
        mad_f_scale64(__hi, __lo);  \
     })

# if 0
/* this is slower than explicit multiply/add */
#  define FPM_MACC
# if 0
/* this was for debugging only */
#  define mad_f_macc(hi, lo, x, y)  \
     ({ mad_fixed64hi_t __hi;  \
        mad_fixed64lo_t __lo;  \
        asm ("smull %0,%1,%2,%3"  \
             : "=&r" (__lo), "=&r" (__hi)  \
             : "%r" (x), "r" (y));  \
        asm ("adds %0,%2,%3; adc %1,%4,%5"  \
             : "=r" (lo), "=r" (hi)  \
             : "%r" (lo), "r" (__lo), "%r" (hi), "r" (__hi)  \
	     : "cc");  \
     })
# else
#  define mad_f_macc(hi, lo, x, y)  \
     asm ("smlal %0,%1,%2,%3"  \
          : "+r" (lo), "+r" (hi)  \
          : "%r" (x), "r" (y))
# endif
# endif

# elif defined(FPM_MIPS)

/* This MIPS version is accurate but always rounds down the least
   significant bit. */

#  define FPM_MACRO
#  define mad_f_mul(x, y)  \
     ({ mad_fixed64hi_t __hi;  \
        mad_fixed64lo_t __lo;  \
        asm ("mult %2,%3"  \
             : "=l" (__lo), "=h" (__hi)  \
             : "%r" (x), "r" (y));  \
        mad_f_scale64(__hi, __lo);  \
     })

# if 0
/* assembler doesn't recognize this instruction? */
#  define FPM_MACC
#  define mad_f_macc(hi, lo, x, y)  \
     asm ("macc r0,%2,%3"  \
          : "+l" (lo), "+h" (hi)  \
          : "%r" (x), "r" (y));
# endif

# elif defined(FPM_SPARC)

/* This SPARC V8 version is accurate but always rounds down the least
   significant bit. */

#  define FPM_MACRO
#  define mad_f_mul(x, y)  \
     ({ mad_fixed64hi_t __hi;  \
        mad_fixed64lo_t __lo;  \
        asm ("smul %2,%3,%0; rd %%y,%1"  \
             : "=r" (__lo), "=r" (__hi)  \
             : "%r" (x), "rI" (y));  \
        mad_f_scale64(__hi, __lo);  \
     })

# else
mad_fixed_t mad_f_mul(mad_fixed_t, mad_fixed_t);
# endif

# ifdef DEBUG
mad_fixed_t mad_f_abs(mad_fixed_t);

mad_fixed_t mad_f_tofixed(double);
double mad_f_todouble(mad_fixed_t);
# endif

# endif

