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
 * $Id: fixed.h,v 1.12 2000/10/25 21:52:31 rob Exp $
 */

# ifndef MAD_FIXED_H
# define MAD_FIXED_H

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
 * 0x00000001 == 0.0000000037252902984619140625 (i.e. about 3.725e-9)
 *
 * 28 bits of fractional accuracy represent about
 * 8.6 digits of decimal accuracy.
 *
 * Fixed-point numbers can be added or subtracted as normal
 * integers, but multiplication requires shifting the 64-bit result
 * from 56 fractional bits back to 28 (and rounding.)
 */

# define MAD_F_FRACBITS		28

# define MAD_F_MIN		(-0x80000000L)
# define MAD_F_MAX		  0x7fffffffL

# define MAD_F_ONE		  0x10000000L

# define mad_f_tofixed(x)	((mad_fixed_t) ((x) * 268435456.0 + 0.5))
# define mad_f_todouble(x)	((double)      ((x) / 268435456.0))

# define mad_f_intpart(x)	((x) >> MAD_F_FRACBITS)
# define mad_f_fracpart(x)	((x) & ((1L << MAD_F_FRACBITS) - 1))
				/* (x should be positive) */

# define mad_f_fromint(x)	((x) << MAD_F_FRACBITS)

# define mad_f_add(x, y)	((x) + (y))
# define mad_f_sub(x, y)	((x) - (y))

# if defined(OPT_ACCURACY)
#  define mad_f_scale64(hi, lo)  \
    ((((mad_fixed_t)  \
       (((mad_fixed64hi_t) (hi) << (32 - (MAD_F_SCALEBITS - 1))) |  \
	((mad_fixed64lo_t) (lo) >> (MAD_F_SCALEBITS - 1)))) + 1) >> 1)
# else
#  define mad_f_scale64(hi, lo)  \
    ((mad_fixed_t)  \
     (((mad_fixed64hi_t) (hi) << (32 - MAD_F_SCALEBITS)) |  \
      ((mad_fixed64lo_t) (lo) >> MAD_F_SCALEBITS)))
# endif

# if defined(FPM_APPROX)

/*
 * This version is the most portable but it loses significant accuracy.
 * Furthermore, accuracy is biased against the second argument, so care
 * should be taken when ordering operands.
 *
 * The scale factors are constant as this is not used with SSO.
 *
 * Pre-rounding is required to stay within the limits of compliance.
 */
#  define mad_f_mul(x, y)	((((x) + (1L << 11)) >> 12) *  \
				 (((y) + (1L << 15)) >> 16))

# elif defined(FPM_64BIT)

/*
 * This version should be the most accurate if 64-bit (long long) types are
 * supported by the compiler, although it may not be the most efficient.
 */
#  if defined(OPT_ACCURACY)
#   define mad_f_mul(x, y)  \
    ((mad_fixed_t)  \
     ((((signed long long) (x) * (y)) +  \
       (1L << (MAD_F_SCALEBITS - 1))) >> MAD_F_SCALEBITS))
#  else
#   define mad_f_mul(x, y)  \
    ((mad_fixed_t) (((signed long long) (x) * (y)) >> MAD_F_SCALEBITS))
#  endif

#  define MAD_F_SCALEBITS  MAD_F_FRACBITS

# elif defined(FPM_INTEL)

/*
 * This Intel version is fast and accurate; the disposition of the least
 * significant bit depends on OPT_ACCURACY via mad_f_scale64().
 */
#  define mad_f_mul(x, y)  \
    ({ mad_fixed64hi_t __hi;  \
       mad_fixed64lo_t __lo;  \
       asm ("imull %3"  \
	    : "=a" (__lo), "=d" (__hi)  \
	    : "%a" (x), "rm" (y)  \
	    : "cc");  \
       mad_f_scale64(__hi, __lo);  \
    })

#  if defined(OPT_ACCURACY)
/*
 * This gives best accuracy but is not very fast.
 */
#   define MAD_F_HAVEMLA
#   define mad_f_mla(hi, lo, x, y)  \
    ({ mad_fixed64hi_t __hi;  \
       mad_fixed64lo_t __lo;  \
       asm ("imull %3"  \
	    : "=a" (__lo), "=d" (__hi)  \
	    : "%a" (x), "rm" (y)  \
	    : "cc");  \
       asm ("addl %3, %0\n\t"  \
	    "adcl %5, %1\n\t"  \
	    : "=r,rm" (*(lo)), "=r,rm" (*(hi))  \
	    : "%0,0" (*(lo)), "rm,r" (__lo), "%1,1" (*(hi)), "rm,r" (__hi)  \
	    : "cc");  \
    })
#  endif  /* OPT_ACCURACY */

#  if defined(OPT_ACCURACY)
/*
 * Surprisingly, this is faster than SHRD followed by ADC.
 */
#   undef  mad_f_scale64
#   define mad_f_scale64(hi, lo)  \
    ({ mad_fixed64hi_t __hi_;  \
       mad_fixed64lo_t __lo_;  \
       mad_fixed_t __result;  \
       asm ("addl %4, %2\n\t"  \
	    "adcl %5, %3"  \
	    : "=rm" (__lo_), "=rm" (__hi_)  \
	    : "0" (lo), "1" (hi),  \
	      "ir" (1L << (MAD_F_SCALEBITS - 1)), "ir" (0)  \
	    : "cc");  \
       asm ("shrdl %3, %2, %1"  \
	    : "=rm" (__result)  \
	    : "0" (__lo_), "r" (__hi_), "I" (MAD_F_SCALEBITS)  \
	    : "cc");  \
       __result;  \
    })
#  else
#   undef mad_f_scale64
#   define mad_f_scale64(hi, lo)  \
    ({ mad_fixed_t __result;  \
       asm ("shrdl %3, %2, %1"  \
	    : "=rm" (__result)  \
	    : "0" (lo), "r" (hi), "I" (MAD_F_SCALEBITS)  \
	    : "cc");  \
       __result;  \
    })
#  endif  /* OPT_ACCURACY */

#  define MAD_F_SCALEBITS  MAD_F_FRACBITS

# elif defined(FPM_ARM)

/* 
 * This ARM V4 version is as accurate as FPM_64BIT but much faster. The
 * least significant bit is properly rounded at no CPU cycle cost!
 */
#  define mad_f_mul(x, y)  \
    ({ mad_fixed64hi_t __hi;  \
       mad_fixed64lo_t __lo;  \
       mad_fixed_t __result;  \
       asm ("smull	%0, %1, %3, %4\n\t"  \
	    "movs	%0, %0, lsr %5\n\t"  \
	    "adc	%2, %0, %1, lsl %6"  \
	    : "=&r" (__lo), "=&r" (__hi), "=r" (__result)  \
	    : "%r" (x), "r" (y),  \
	      "M" (MAD_F_SCALEBITS), "M" (32 - MAD_F_SCALEBITS)  \
	    : "cc");  \
       __result;  \
    })

#  define MAD_F_HAVEMLA
#  define mad_f_mla(hi, lo, x, y)  \
    asm ("smlal	%0, %1, %2, %3"  \
	 : "+r" (*(lo)), "+r" (*(hi))  \
	 : "%r" (x), "r" (y))

#  undef  mad_f_scale64
#  define mad_f_scale64(hi, lo)  \
    ({ mad_fixed_t __result;  \
       asm ("movs	%0, %1, lsr %3\n\t"  \
	    "adc	%0, %0, %2, lsl %4"  \
	    : "=r" (__result)  \
	    : "r" (lo), "r" (hi),  \
	      "M" (MAD_F_SCALEBITS), "M" (32 - MAD_F_SCALEBITS)  \
	    : "cc");  \
       __result;  \
    })

#  define MAD_F_SCALEBITS  MAD_F_FRACBITS

# elif defined(FPM_MIPS)

/*
 * This MIPS version is fast and accurate; the disposition of the least
 * significant bit depends on OPT_ACCURACY via mad_f_scale64().
 */
#  define mad_f_mul(x, y)  \
    ({ mad_fixed64hi_t __hi;  \
       mad_fixed64lo_t __lo;  \
       asm ("mult %2, %3"  \
	    : "=l" (__lo), "=h" (__hi)  \
	    : "%r" (x), "r" (y));  \
       mad_f_scale64(__hi, __lo);  \
    })

# if 0  /* assembler doesn't recognize this instruction? */
#  define MAD_F_HAVEMLA
#  define mad_f_mla(hi, lo, x, y)  \
    asm ("macc r0, %2, %3"  \
	 : "+l" (*(lo)), "+h" (*(hi))  \
	 : "%r" (x), "r" (y));
# endif

#  define MAD_F_SCALEBITS  MAD_F_FRACBITS

# elif defined(FPM_SPARC)

/*
 * This SPARC V8 version is fast and accurate; the disposition of the least
 * significant bit depends on OPT_ACCURACY via mad_f_scale64().
 */
#  define mad_f_mul(x, y)  \
    ({ mad_fixed64hi_t __hi;  \
       mad_fixed64lo_t __lo;  \
       asm ("smul %2, %3, %0\n\t"  \
	    "rd %%y, %1"  \
	    : "=r" (__lo), "=r" (__hi)  \
	    : "%r" (x), "rI" (y));  \
       mad_f_scale64(__hi, __lo);  \
    })

#  define MAD_F_SCALEBITS  MAD_F_FRACBITS

# else

mad_fixed_t mad_f_mul(mad_fixed_t, mad_fixed_t);

# endif

mad_fixed_t mad_f_abs(mad_fixed_t);

# endif
