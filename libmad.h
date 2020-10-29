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
 * Id: version.h,v 1.8 2000/03/05 07:31:55 rob Exp 
 */

# define MAD_VERSION		"0.9.6 (beta)"
# define MAD_PUBLISHYEAR	"2000"
# define MAD_AUTHOR		"Robert Leslie"
# define MAD_EMAIL		"rob@mars.org"

extern char const mad_version[];
extern char const mad_copyright[];
extern char const mad_author[];
extern char const mad_license[];

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
 * Id: fixed.h,v 1.7 2000/03/05 07:31:55 rob Exp 
 */

# ifndef FIXED_H
# define FIXED_H

typedef   signed long fixed_t;

typedef   signed long fixed64hi_t;
typedef unsigned long fixed64lo_t;

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
 * The CPU-specific versions of f_mul() perform rounding by truncation.
 */

# define f_add(x, y)		((x) + (y))
# define f_sub(x, y)		((x) - (y))

# define f_scale64(hi, lo)	((fixed_t) (((fixed64hi_t) (hi) << 4) |  \
                                            ((fixed64lo_t) (lo) >> 28)))

# if defined(FPM_APPROX)

/* This version is the most portable but loses 14 bits of accuracy. */

#  define FPM_MACRO
#  define f_mul(x, y)	((((x) + 0x00002000L) >> 14) *  \
                         (((y) + 0x00002000L) >> 14))

# elif defined(FPM_64BIT)

/* This version should be the most accurate if 64-bit (long long) types
   are supported by the compiler. */

#  define FPM_MACRO
#  define f_mul(x, y)  \
     ((fixed_t) (((((signed long long) (x) * (y)) + 0x08000000L) >> 28)))

# elif defined(FPM_INTEL)

/* This Intel version is accurate but always rounds down the least
   significant bit. */

#  define FPM_MACRO
#  define f_mul(x, y)  \
     ({ fixed64hi_t __hi;  \
        fixed64lo_t __lo;  \
        asm ("imull %3"  \
             : "=a" (__lo), "=d" (__hi)  \
	     : "%a" (x), "rm" (y));  \
        f_scale64(__hi, __lo);  \
     })

# if 0
/* this is slower than the C multiply/add version */
#  define FPM_MACC
#  define f_macc(hi, lo, x, y)  \
     ({ fixed64hi_t __hi;  \
        fixed64lo_t __lo;  \
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
#  define f_mul(x, y)  \
     ({ fixed64hi_t __hi;  \
        fixed64lo_t __lo;  \
        asm ("smull %0,%1,%2,%3"  \
	     : "=&r" (__lo), "=&r" (__hi)  \
	     : "%r" (x), "r" (y));  \
        f_scale64(__hi, __lo);  \
     })

# if 0
/* this is slower than explicit multiply/add */
#  define FPM_MACC
# if 0
/* this was for debugging only */
#  define f_macc(hi, lo, x, y)  \
     ({ fixed64hi_t __hi;  \
        fixed64lo_t __lo;  \
        asm ("smull %0,%1,%2,%3"  \
             : "=&r" (__lo), "=&r" (__hi)  \
             : "%r" (x), "r" (y));  \
        asm ("adds %0,%2,%3; adc %1,%4,%5"  \
             : "=r" (lo), "=r" (hi)  \
             : "%r" (lo), "r" (__lo), "%r" (hi), "r" (__hi)  \
	     : "cc");  \
     })
# else
#  define f_macc(hi, lo, x, y)  \
     asm ("smlal %0,%1,%2,%3"  \
          : "+r" (lo), "+r" (hi)  \
          : "%r" (x), "r" (y))
# endif
# endif

# elif defined(FPM_MIPS)

/* This MIPS version is accurate but always rounds down the least
   significant bit. */

#  define FPM_MACRO
#  define f_mul(x, y)  \
     ({ fixed64hi_t __hi;  \
        fixed64lo_t __lo;  \
        asm ("mult %2,%3"  \
             : "=l" (__lo), "=h" (__hi)  \
             : "%r" (x), "r" (y));  \
        f_scale64(__hi, __lo);  \
     })

# if 0
/* assembler doesn't recognize this instruction? */
#  define FPM_MACC
#  define f_macc(hi, lo, x, y)  \
     asm ("macc r0,%2,%3"  \
          : "+l" (lo), "+h" (hi)  \
          : "%r" (x), "r" (y));
# endif

# else
fixed_t f_mul(fixed_t, fixed_t);
# endif

fixed_t f_abs(fixed_t);

# ifdef DEBUG
fixed_t f_tofixed(double);
double f_todouble(fixed_t);
# endif

# endif

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
 * Id: bit.h,v 1.4 2000/03/05 07:31:54 rob Exp 
 */

# ifndef BIT_H
# define BIT_H

struct mad_bitptr {
  unsigned char const *byte;
  unsigned short cache;
  unsigned short left;
};

void mad_bit_init(struct mad_bitptr *, unsigned char const *);
unsigned int mad_bit_length(struct mad_bitptr const *,
			    struct mad_bitptr const *);
unsigned char const *mad_bit_byte(struct mad_bitptr const *);
void mad_bit_seek(struct mad_bitptr *, unsigned int);
unsigned long mad_bit_read(struct mad_bitptr *, unsigned int);

# define mad_bit_left(bits)  ((bits)->left)

# endif

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
 * Id: timer.h,v 1.4 2000/03/05 07:31:55 rob Exp 
 */

# ifndef TIMER_H
# define TIMER_H

enum {
  timer_hours,
  timer_minutes,
  timer_seconds
};

struct mad_timer {
  unsigned long seconds;		/* whole seconds */
  unsigned int parts36750;		/* 1/36750 seconds */
};

void mad_timer_init(struct mad_timer *);
void mad_timer_add(struct mad_timer *, struct mad_timer const *);
void mad_timer_str(struct mad_timer const *, char *, char const *, int);

# endif

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
 * Id: stream.h,v 1.2 2000/03/05 07:31:55 rob Exp 
 */

# ifndef STREAM_H
# define STREAM_H


struct mad_stream {
  unsigned char const *buffer;		/* input bitstream buffer */
  unsigned char const *bufend;		/* end of buffer */

  int sync;				/* stream sync found */
  unsigned int freerate;		/* free bitrate (fixed) */

  unsigned char const *this_frame;	/* start of current frame */
  unsigned char const *next_frame;	/* start of next frame */
  struct mad_bitptr ptr;		/* current processing bit pointer */

  struct mad_bitptr anc_ptr;		/* ancillary bits pointer */
  unsigned int anc_bitlen;		/* number of ancillary bits */

  unsigned char main_data[1935];	/* layer III main_data */
  unsigned int md_len;			/* bytes in main_data */

  int error;				/* error code (see below) */
};

# define MAD_ERR_BUFLEN		0x0001	/* input buffer too small (or EOF) */
# define MAD_ERR_BUFPTR		0x0002	/* invalid (null) buffer pointer */

# define MAD_ERR_LOSTSYNC	0x0101	/* lost synchronization */
# define MAD_ERR_BADID		0x0102	/* bad header ID field */
# define MAD_ERR_BADLAYER	0x0103	/* reserved header layer value */
# define MAD_ERR_BADBITRATE	0x0104	/* forbidden bitrate value */
# define MAD_ERR_BADSAMPLEFREQ	0x0105	/* reserved sample frequency value */
# define MAD_ERR_BADEMPHASIS	0x0106	/* reserved emphasis value */

# define MAD_ERR_BADCRC		0x0201	/* CRC check failed */
# define MAD_ERR_BADBITALLOC	0x0211	/* forbidden bit allocation value */
# define MAD_ERR_BADSCALEFACTOR	0x0221	/* bad scalefactor index */
# define MAD_ERR_BADBIGVALUES	0x0231	/* bad big_values count */
# define MAD_ERR_BADBLOCKTYPE	0x0232	/* reserved block_type */
# define MAD_ERR_BADDATAPTR	0x0233	/* bad main_data_begin pointer */
# define MAD_ERR_BADDATALENGTH	0x0234	/* bad main data length */
# define MAD_ERR_BADPART3LEN	0x0235	/* bad audio data length */
# define MAD_ERR_BADHUFFTABLE	0x0236	/* bad Huffman table select */
# define MAD_ERR_BADHUFFDATA	0x0237	/* Huffman data value out of range */
# define MAD_ERR_BADSTEREO	0x0238	/* incompatible block_type for M/S */

# define MAD_RECOVERABLE(error)	((error) & 0xff00)

void mad_stream_init(struct mad_stream *);
void mad_stream_buffer(struct mad_stream *,
		       unsigned char const *, unsigned long);

# endif

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
 * Id: frame.h,v 1.2 2000/03/05 07:31:55 rob Exp 
 */

# ifndef FRAME_H
# define FRAME_H


struct mad_frame {
  int layer;				/* audio layer (1, 2, or 3) */
  int mode;				/* channel mode (see below) */
  int mode_ext;				/* additional mode info */
  int emphasis;				/* de-emphasis to use (see below) */

  unsigned int bitrate;			/* stream bitrate (bps) */
  unsigned int samplefreq;		/* sampling frequency (Hz) */

  struct mad_timer duration;		/* audio playing time of frame */

  int flags;				/* flags and private bits (below) */

  fixed_t sbsample[2][36][32];		/* synthesis subband filter samples */
  fixed_t overlap[2][32][18];		/* layer III block overlap data */
};

# define MAD_NUMCHANNELS(frame)		((frame)->mode ? 2 : 1)
# define MAD_NUMSAMPLES(frame)		((frame)->layer == 1 ? 12 : 36)

# define MAD_MODE_SINGLE_CHANNEL	0
# define MAD_MODE_DUAL_CHANNEL		1
# define MAD_MODE_JOINT_STEREO		2
# define MAD_MODE_STEREO		3

# define MAD_EMPH_NONE		0	/* no emphasis */
# define MAD_EMPH_50_15_MS	1	/* 50/15 microseconds */
# define MAD_EMPH_CCITT_J_17	3	/* CCITT J.17 */

# define MAD_FLAG_PROTECTION	0x0100	/* frame has CRC protection */
# define MAD_FLAG_CRCFAILED	0x0200	/* frame CRC failed */
# define MAD_FLAG_COPYRIGHT	0x0400	/* frame is copyright */
# define MAD_FLAG_ORIGINAL	0x0800	/* frame is original (else copy) */
# define MAD_FLAG_PADDING	0x1000	/* frame has additional slot */

# define MAD_FLAG_PRIVATE	0x0040	/* header private bit */
# define MAD_FLAG_III_PRIVATE	0x001f	/* layer III private bits */
# define MAD_FLAG_III_5BITPRIV	0x0020	/* 5 bits in III private (else 3) */

void mad_frame_init(struct mad_frame *);
void mad_frame_mute(struct mad_frame *);

# endif

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
 * Id: synth.h,v 1.2 2000/03/05 07:31:55 rob Exp 
 */

# ifndef SYNTH_H
# define SYNTH_H


struct mad_synth {
  fixed_t filterout[2][2][256];		/* polyphase filterbank outputs */
  unsigned short slot;			/* current processing slot */

  unsigned short pcmlen;		/* number of PCM samples */
  fixed_t pcmout[2][1152];		/* PCM sample outputs */
};

void mad_synth_init(struct mad_synth *);

void mad_synthesis(struct mad_frame *, struct mad_synth *);

# endif

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
 * Id: mad.h,v 1.4 2000/03/05 07:31:55 rob Exp 
 */

# ifndef MAD_H
# define MAD_H


unsigned short mad_crc(unsigned short, struct mad_bitptr, unsigned int);
int mad_decode(struct mad_stream *, struct mad_frame *);

# endif

