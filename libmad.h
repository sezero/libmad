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
 * Id: version.h,v 1.17 2000/06/03 23:07:41 rob Exp 
 */

# define MAD_VERSION		"0.11.0 (beta)"
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
 * Id: fixed.h,v 1.9 2000/04/22 04:36:50 rob Exp 
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
 * Id: bit.h,v 1.7 2000/03/22 23:52:59 rob Exp 
 */

# ifndef BIT_H
# define BIT_H

struct mad_bitptr {
  unsigned char const *byte;
  unsigned short cache;
  unsigned short left;
};

void mad_bit_init(struct mad_bitptr *, unsigned char const *);

# define mad_bit_finish(bitptr)  /* nothing */

unsigned int mad_bit_length(struct mad_bitptr const *,
			    struct mad_bitptr const *);

# define mad_bit_bitsleft(bitptr)  ((bitptr)->left)
unsigned char const *mad_bit_nextbyte(struct mad_bitptr const *);

void mad_bit_skip(struct mad_bitptr *, unsigned int);
unsigned long mad_bit_read(struct mad_bitptr *, unsigned int);
void mad_bit_write(struct mad_bitptr *, unsigned int, unsigned long);

unsigned short mad_bit_crc(struct mad_bitptr, unsigned int, unsigned short);

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
 * Id: timer.h,v 1.6 2000/03/19 06:43:39 rob Exp 
 */

# ifndef TIMER_H
# define TIMER_H

struct mad_timer {
  unsigned long seconds;		/* whole seconds */
  unsigned int parts36750;		/* 1/36750 seconds */
};

void mad_timer_init(struct mad_timer *);

# define mad_timer_finish(timer)  /* nothing */

void mad_timer_add(struct mad_timer *, struct mad_timer const *);
void mad_timer_str(struct mad_timer const *, char *, char const *, int);

# define mad_timer_seconds(timer)	((timer)->seconds)

# define MAD_TIMER_HOURS	0x0002
# define MAD_TIMER_MINUTES	0x0001
# define MAD_TIMER_SECONDS	0x0000

# define MAD_TIMER_DECISECONDS	0x0003
# define MAD_TIMER_CENTISECONDS	0x0004
# define MAD_TIMER_MILLISECONDS	0x0005

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
 * Id: stream.h,v 1.7 2000/06/03 23:07:41 rob Exp 
 */

# ifndef STREAM_H
# define STREAM_H


struct mad_stream {
  unsigned char const *buffer;		/* input bitstream buffer */
  unsigned char const *bufend;		/* end of buffer */
  unsigned long skiplen;		/* bytes to skip before next frame */

  int sync;				/* stream sync found */
  unsigned int freerate;		/* free bitrate (fixed) */

  unsigned char const *this_frame;	/* start of current frame */
  unsigned char const *next_frame;	/* start of next frame */
  struct mad_bitptr ptr;		/* current processing bit pointer */

  struct mad_bitptr anc_ptr;		/* ancillary bits pointer */
  unsigned int anc_bitlen;		/* number of ancillary bits */

  unsigned char (*main_data)[1935];	/* Layer III main_data */
  unsigned int md_len;			/* bytes in main_data */

  int error;				/* error code (see below) */
};

# define MAD_ERR_BUFLEN		0x0001	/* input buffer too small (or EOF) */
# define MAD_ERR_BUFPTR		0x0002	/* invalid (null) buffer pointer */

# define MAD_ERR_NOMEM		0x0031	/* not enough memory */

# define MAD_ERR_LOSTSYNC	0x0101	/* lost synchronization */
# define MAD_ERR_BADLAYER	0x0102	/* reserved header layer value */
# define MAD_ERR_BADBITRATE	0x0103	/* forbidden bitrate value */
# define MAD_ERR_BADSAMPLEFREQ	0x0104	/* reserved sample frequency value */
# define MAD_ERR_BADEMPHASIS	0x0105	/* reserved emphasis value */

# define MAD_ERR_BADCRC		0x0201	/* CRC check failed */
# define MAD_ERR_BADBITALLOC	0x0211	/* forbidden bit allocation value */
# define MAD_ERR_BADSCALEFACTOR	0x0221	/* bad scalefactor index */
# define MAD_ERR_BADFRAMELEN	0x0231	/* bad frame length */
# define MAD_ERR_BADBIGVALUES	0x0232	/* bad big_values count */
# define MAD_ERR_BADBLOCKTYPE	0x0233	/* reserved block_type */
# define MAD_ERR_BADDATAPTR	0x0234	/* bad main_data_begin pointer */
# define MAD_ERR_BADDATALEN	0x0235	/* bad main data length */
# define MAD_ERR_BADPART3LEN	0x0236	/* bad audio data length */
# define MAD_ERR_BADHUFFTABLE	0x0237	/* bad Huffman table select */
# define MAD_ERR_BADHUFFDATA	0x0238	/* Huffman data value out of range */
# define MAD_ERR_BADSTEREO	0x0239	/* incompatible block_type for M/S */

# define MAD_RECOVERABLE(error)	((error) & 0xff00)

void mad_stream_init(struct mad_stream *);
void mad_stream_finish(struct mad_stream *);

void mad_stream_buffer(struct mad_stream *,
		       unsigned char const *, unsigned long);
void mad_stream_skip(struct mad_stream *, unsigned long);

unsigned char const *mad_stream_sync(unsigned char const *,
				     unsigned char const *);

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
 * Id: frame.h,v 1.6 2000/06/03 23:07:41 rob Exp 
 */

# ifndef FRAME_H
# define FRAME_H


struct mad_frame {
  int layer;				/* audio layer (1, 2, or 3) */
  int mode;				/* channel mode (see below) */
  int mode_ext;				/* additional mode info */
  int emphasis;				/* de-emphasis to use (see below) */

  unsigned long bitrate;		/* stream bitrate (bps) */
  unsigned int sfreq;			/* sampling frequency (Hz) */

  int flags;				/* flags (below) */
  int private;				/* private bits (below) */

  struct mad_timer duration;		/* audio playing time of frame */

  mad_fixed_t sbsample[2][36][32];	/* synthesis subband filter samples */
  mad_fixed_t (*overlap)[2][32][18];	/* Layer III block overlap data */
};

# define MAD_NUMCHANNELS(frame)		((frame)->mode ? 2 : 1)
# define MAD_NUMSBSAMPLES(frame)  \
  ((frame)->layer == 1 ? 12 :  \
   (((frame)->layer == 3 && ((frame)->flags & MAD_FLAG_LSF_EXT)) ? 18 : 36))

# define MAD_MODE_SINGLE_CHANNEL	0
# define MAD_MODE_DUAL_CHANNEL		1
# define MAD_MODE_JOINT_STEREO		2
# define MAD_MODE_STEREO		3

# define MAD_EMPH_NONE		0	/* no emphasis */
# define MAD_EMPH_50_15_MS	1	/* 50/15 microseconds */
# define MAD_EMPH_CCITT_J_17	3	/* CCITT J.17 */

# define MAD_FLAG_III_NPRIVATE	0x0007	/* number of Layer III private bits */

# define MAD_FLAG_PROTECTION	0x0010	/* frame has CRC protection */
# define MAD_FLAG_COPYRIGHT	0x0020	/* frame is copyright */
# define MAD_FLAG_ORIGINAL	0x0040	/* frame is original (else copy) */
# define MAD_FLAG_PADDING	0x0080	/* frame has additional slot */

# define MAD_FLAG_I_STEREO	0x0100	/* uses intensity joint stereo */
# define MAD_FLAG_MS_STEREO	0x0200	/* uses middle/side joint stereo */

# define MAD_FLAG_LSF_EXT	0x1000	/* lower sampling freq. extension */
# define MAD_FLAG_MC_EXT	0x2000	/* multichannel audio extension */

# define MAD_PRIV_HEADER	0x0100	/* header private bit */
# define MAD_PRIV_III		0x001f	/* Layer III private bits (up to 5) */

void mad_frame_init(struct mad_frame *);
void mad_frame_finish(struct mad_frame *);

void mad_frame_mute(struct mad_frame *);

int mad_frame_header(struct mad_frame *, struct mad_stream *,
		     unsigned short *);
int mad_frame_decode(struct mad_frame *, struct mad_stream *);

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
 * Id: synth.h,v 1.6 2000/04/22 04:36:51 rob Exp 
 */

# ifndef SYNTH_H
# define SYNTH_H


struct mad_synth {
  mad_fixed_t filterout[2][2][256];	/* polyphase filterbank outputs */
  unsigned short slot;			/* current processing slot */

  unsigned short pcmlen;		/* number of PCM samples */
  mad_fixed_t pcmout[2][1152];		/* PCM sample outputs */
};

void mad_synth_init(struct mad_synth *);

# define mad_synth_finish(synth)  /* nothing */

void mad_synth_frame(struct mad_synth *, struct mad_frame const *);

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
 * Id: decoder.h,v 1.3 2000/05/09 17:36:27 rob Exp 
 */

# ifndef DECODER_H
# define DECODER_H


struct mad_decoder {
  int mode;

  struct {
    int pid;
    int in;
    int out;
  } async;

  struct {
    struct mad_stream stream;
    struct mad_frame frame;
    struct mad_synth synth;
  } *sync;

  int (*input_func)(void *, struct mad_stream *);
  int (*filter_func)(void *, struct mad_frame *);
  int (*output_func)(void *, struct mad_frame const *,
		     struct mad_synth const *);
  int (*error_func)(void *, struct mad_stream *, struct mad_frame *);

  void *input_data;
  void *filter_data;
  void *output_data;
  void *error_data;
};

enum {
  mad_cmd_play,
  mad_cmd_pause,
  mad_cmd_rewind,
  mad_cmd_skip,
  mad_cmd_stop
};

union mad_control {
  short command;

  struct mad_cmd_play {
    short command;
  } play;

  struct mad_cmd_pause {
    short command;
  } pause;

  struct mad_cmd_rewind {
    short command;
  } rewind;

  struct mad_cmd_skip {
    short command;
  } skip;

  struct mad_cmd_stop {
    short command;
  } stop;
};

void mad_decoder_init(struct mad_decoder *);
int mad_decoder_finish(struct mad_decoder *);

void mad_decoder_input(struct mad_decoder *,
		       int (*)(void *, struct mad_stream *), void *);
void mad_decoder_filter(struct mad_decoder *,
			int (*)(void *, struct mad_frame *), void *);
void mad_decoder_output(struct mad_decoder *,
			int (*)(void *, struct mad_frame const *,
				struct mad_synth const *), void *);
void mad_decoder_error(struct mad_decoder *,
		       int (*)(void *, struct mad_stream *,
			       struct mad_frame *), void *);

int mad_decoder_run(struct mad_decoder *, int);
int mad_decoder_command(struct mad_decoder *, union mad_control *);

# define MAD_DECODER_SYNC	0x0000
# define MAD_DECODER_ASYNC	0x0001

# define MAD_DECODER_CONTINUE	0x0000
# define MAD_DECODER_STOP	0x0010
# define MAD_DECODER_BREAK	0x0011
# define MAD_DECODER_IGNORE	0x0020

# endif

