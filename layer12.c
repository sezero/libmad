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
 * $Id: layer12.c,v 1.9 2000/03/05 07:31:55 rob Exp $
 */

# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif

# include "fixed.h"
# include "bit.h"
# include "mad.h"
# include "layer12.h"

static
fixed_t const sf_table[63] = {
# include "sf_table.dat"
};

/* --- Layer I ------------------------------------------------------------- */

static
fixed_t const linear_table[14] = {
  0x15555555L,  /*  2^2 / (2^2  - 1) == 1.33333333333333 */
  0x12492492L,  /*  2^3 / (2^3  - 1) == 1.14285714285714 */
  0x11111111L,  /*  2^4 / (2^4  - 1) == 1.06666666666667 */
  0x10842108L,  /*  2^5 / (2^5  - 1) == 1.03225806451613 */
  0x10410410L,  /*  2^6 / (2^6  - 1) == 1.01587301587302 */
  0x10204081L,  /*  2^7 / (2^7  - 1) == 1.00787401574803 */
  0x10101010L,  /*  2^8 / (2^8  - 1) == 1.00392156862745 */
  0x10080402L,  /*  2^9 / (2^9  - 1) == 1.00195694716243 */

  0x10040100L,  /* 2^10 / (2^10 - 1) == 1.00097751710655 */
  0x10020040L,  /* 2^11 / (2^11 - 1) == 1.00048851978505 */
  0x10010010L,  /* 2^12 / (2^12 - 1) == 1.00024420024420 */
  0x10008004L,  /* 2^13 / (2^13 - 1) == 1.00012208521548 */
  0x10004001L,  /* 2^14 / (2^14 - 1) == 1.00006103888177 */
  0x10002000L   /* 2^15 / (2^15 - 1) == 1.00003051850948 */
};

static
fixed_t I_sample(struct mad_bitptr *ptr, unsigned int nb)
{
  unsigned int sample;
  fixed_t requantized;

  sample = mad_bit_read(ptr, nb);

  /* invert most significant bit, then scale sample to fixed format */

  requantized = ((fixed_t) (-0x80000000L ^ (sample << (32 - nb)))) >> 3;

  /* requantize the sample */

  /* s'' = (2^nb / (2^nb - 1)) * (s''' + 2^(-nb + 1)) */

  requantized += 0x10000000L >> (nb - 1);
  requantized  = f_mul(requantized, linear_table[nb - 2]);

  /* s' = factor * s'' */
  /* (to be performed by caller) */

  return requantized;
}

int mad_layer_I(struct mad_stream *stream, struct mad_frame *frame,
		unsigned short const crc[2])
{
  unsigned int nch, bound, ch, s, sb, nb;
  unsigned char allocation[2][32], scalefactor[2][32];

  nch = MAD_NUMCHANNELS(frame);

  if (frame->mode == MAD_MODE_JOINT_STEREO)
    bound = 4 + frame->mode_ext * 4;
  else
    bound = 32;

  /* check CRC word */

  if ((frame->flags & MAD_FLAG_PROTECTION) &&
      mad_crc(crc[0], stream->ptr,
	      4 * (bound * nch + (32 - bound))) != crc[1]) {
    stream->error = MAD_ERR_BADCRC;
    return -1;
  }

  /* decode bit allocations */

  for (sb = 0; sb < bound; ++sb) {
    for (ch = 0; ch < nch; ++ch) {
      nb = mad_bit_read(&stream->ptr, 4);

      if (nb == 15) {
	stream->error = MAD_ERR_BADBITALLOC;
	return -1;
      }

      allocation[ch][sb] = nb ? nb + 1 : 0;
    }
  }

  for (sb = bound; sb < 32; ++sb) {
    nb = mad_bit_read(&stream->ptr, 4);

    if (nb == 15) {
      stream->error = MAD_ERR_BADBITALLOC;
      return -1;
    }

    allocation[0][sb] = allocation[1][sb] = nb ? nb + 1 : 0;
  }

  /* decode scalefactors */

  for (sb = 0; sb < 32; ++sb) {
    for (ch = 0; ch < nch; ++ch) {
      if (allocation[ch][sb]) {
	scalefactor[ch][sb] = mad_bit_read(&stream->ptr, 6);

	if (scalefactor[ch][sb] == 63) {
	  stream->error = MAD_ERR_BADSCALEFACTOR;
	  return -1;
	}
      }
    }
  }

  /* decode samples */

  for (s = 0; s < 12; ++s) {
    for (sb = 0; sb < bound; ++sb) {
      for (ch = 0; ch < nch; ++ch) {
	if ((nb = allocation[ch][sb])) {
	  frame->sbsample[ch][s][sb] =
	    f_mul(sf_table[scalefactor[ch][sb]], I_sample(&stream->ptr, nb));
	}
	else
	  frame->sbsample[ch][s][sb] = 0;
      }
    }

    for (sb = bound; sb < 32; ++sb) {
      if ((nb = allocation[0][sb])) {
	fixed_t sample;

	sample = I_sample(&stream->ptr, nb);

	for (ch = 0; ch < nch; ++ch) {
	  frame->sbsample[ch][s][sb] =
	    f_mul(sample, sf_table[scalefactor[ch][sb]]);
	}
      }
      else {
	for (ch = 0; ch < nch; ++ch)
	  frame->sbsample[ch][s][sb] = 0;
      }
    }
  }

  return 0;
}

/* --- Layer II ------------------------------------------------------------ */

static
unsigned char const bitalloc_table[4][32][16] = {
  {
    /* table 0 */
    { 4, 0, 2,  4, 5, 6, 7,  8, 9, 10, 11, 12, 13, 14, 15, 16 },  /* 0 */
    { 4, 0, 2,  4, 5, 6, 7,  8, 9, 10, 11, 12, 13, 14, 15, 16 },  /* 1 */
    { 4, 0, 2,  4, 5, 6, 7,  8, 9, 10, 11, 12, 13, 14, 15, 16 },  /* 2 */
    { 4, 0, 1,  2, 3, 4, 5,  6, 7,  8,  9, 10, 11, 12, 13, 16 },  /* 3 */
    { 4, 0, 1,  2, 3, 4, 5,  6, 7,  8,  9, 10, 11, 12, 13, 16 },  /* 4 */
    { 4, 0, 1,  2, 3, 4, 5,  6, 7,  8,  9, 10, 11, 12, 13, 16 },  /* 5 */
    { 4, 0, 1,  2, 3, 4, 5,  6, 7,  8,  9, 10, 11, 12, 13, 16 },  /* 6 */
    { 4, 0, 1,  2, 3, 4, 5,  6, 7,  8,  9, 10, 11, 12, 13, 16 },  /* 7 */
    { 4, 0, 1,  2, 3, 4, 5,  6, 7,  8,  9, 10, 11, 12, 13, 16 },  /* 8 */
    { 4, 0, 1,  2, 3, 4, 5,  6, 7,  8,  9, 10, 11, 12, 13, 16 },  /* 9 */
    { 4, 0, 1,  2, 3, 4, 5,  6, 7,  8,  9, 10, 11, 12, 13, 16 },  /* 10 */
    { 3, 0, 1,  2, 3, 4, 5, 16                                },  /* 11 */
    { 3, 0, 1,  2, 3, 4, 5, 16                                },  /* 12 */
    { 3, 0, 1,  2, 3, 4, 5, 16                                },  /* 13 */
    { 3, 0, 1,  2, 3, 4, 5, 16                                },  /* 14 */
    { 3, 0, 1,  2, 3, 4, 5, 16                                },  /* 15 */
    { 3, 0, 1,  2, 3, 4, 5, 16                                },  /* 16 */
    { 3, 0, 1,  2, 3, 4, 5, 16                                },  /* 17 */
    { 3, 0, 1,  2, 3, 4, 5, 16                                },  /* 18 */
    { 3, 0, 1,  2, 3, 4, 5, 16                                },  /* 19 */
    { 3, 0, 1,  2, 3, 4, 5, 16                                },  /* 20 */
    { 3, 0, 1,  2, 3, 4, 5, 16                                },  /* 21 */
    { 3, 0, 1,  2, 3, 4, 5, 16                                },  /* 22 */
    { 2, 0, 1, 16                                             },  /* 23 */
    { 2, 0, 1, 16                                             },  /* 24 */
    { 2, 0, 1, 16                                             },  /* 25 */
    { 2, 0, 1, 16                                             },  /* 26 */
    { 0                                   /* sblimit = 27 */  },  /* 27 */
    { 0                                                       },  /* 28 */
    { 0                                                       },  /* 29 */
    { 0                                                       },  /* 30 */
    { 0                                                       }   /* 31 */
  }, {
    /* table 1 */
    { 4, 0, 2,  4, 5, 6, 7,  8, 9, 10, 11, 12, 13, 14, 15, 16 },  /* 0 */
    { 4, 0, 2,  4, 5, 6, 7,  8, 9, 10, 11, 12, 13, 14, 15, 16 },  /* 1 */
    { 4, 0, 2,  4, 5, 6, 7,  8, 9, 10, 11, 12, 13, 14, 15, 16 },  /* 2 */
    { 4, 0, 1,  2, 3, 4, 5,  6, 7,  8,  9, 10, 11, 12, 13, 16 },  /* 3 */
    { 4, 0, 1,  2, 3, 4, 5,  6, 7,  8,  9, 10, 11, 12, 13, 16 },  /* 4 */
    { 4, 0, 1,  2, 3, 4, 5,  6, 7,  8,  9, 10, 11, 12, 13, 16 },  /* 5 */
    { 4, 0, 1,  2, 3, 4, 5,  6, 7,  8,  9, 10, 11, 12, 13, 16 },  /* 6 */
    { 4, 0, 1,  2, 3, 4, 5,  6, 7,  8,  9, 10, 11, 12, 13, 16 },  /* 7 */
    { 4, 0, 1,  2, 3, 4, 5,  6, 7,  8,  9, 10, 11, 12, 13, 16 },  /* 8 */
    { 4, 0, 1,  2, 3, 4, 5,  6, 7,  8,  9, 10, 11, 12, 13, 16 },  /* 9 */
    { 4, 0, 1,  2, 3, 4, 5,  6, 7,  8,  9, 10, 11, 12, 13, 16 },  /* 10 */
    { 3, 0, 1,  2, 3, 4, 5, 16                                },  /* 11 */
    { 3, 0, 1,  2, 3, 4, 5, 16                                },  /* 12 */
    { 3, 0, 1,  2, 3, 4, 5, 16                                },  /* 13 */
    { 3, 0, 1,  2, 3, 4, 5, 16                                },  /* 14 */
    { 3, 0, 1,  2, 3, 4, 5, 16                                },  /* 15 */
    { 3, 0, 1,  2, 3, 4, 5, 16                                },  /* 16 */
    { 3, 0, 1,  2, 3, 4, 5, 16                                },  /* 17 */
    { 3, 0, 1,  2, 3, 4, 5, 16                                },  /* 18 */
    { 3, 0, 1,  2, 3, 4, 5, 16                                },  /* 19 */
    { 3, 0, 1,  2, 3, 4, 5, 16                                },  /* 20 */
    { 3, 0, 1,  2, 3, 4, 5, 16                                },  /* 21 */
    { 3, 0, 1,  2, 3, 4, 5, 16                                },  /* 22 */
    { 2, 0, 1, 16                                             },  /* 23 */
    { 2, 0, 1, 16                                             },  /* 24 */
    { 2, 0, 1, 16                                             },  /* 25 */
    { 2, 0, 1, 16                                             },  /* 26 */
    { 2, 0, 1, 16                                             },  /* 27 */
    { 2, 0, 1, 16                                             },  /* 28 */
    { 2, 0, 1, 16                                             },  /* 29 */
    { 0                                   /* sblimit = 30 */  },  /* 30 */
    { 0                                                       }   /* 31 */
  }, {
    /* table 2 */
    { 4, 0, 1,  3, 4, 5, 6,  7, 8,  9, 10, 11, 12, 13, 14, 15 },  /* 0 */
    { 4, 0, 1,  3, 4, 5, 6,  7, 8,  9, 10, 11, 12, 13, 14, 15 },  /* 1 */
    { 3, 0, 1,  3, 4, 5, 6,  7                                },  /* 2 */
    { 3, 0, 1,  3, 4, 5, 6,  7                                },  /* 3 */
    { 3, 0, 1,  3, 4, 5, 6,  7                                },  /* 4 */
    { 3, 0, 1,  3, 4, 5, 6,  7                                },  /* 5 */
    { 3, 0, 1,  3, 4, 5, 6,  7                                },  /* 6 */
    { 3, 0, 1,  3, 4, 5, 6,  7                                },  /* 7 */
    { 0                                    /* sblimit = 8 */  },  /* 8 */
    { 0                                                       },  /* 9 */
    { 0                                                       },  /* 10 */
    { 0                                                       },  /* 11 */
    { 0                                                       },  /* 12 */
    { 0                                                       },  /* 13 */
    { 0                                                       },  /* 14 */
    { 0                                                       },  /* 15 */
    { 0                                                       },  /* 16 */
    { 0                                                       },  /* 17 */
    { 0                                                       },  /* 18 */
    { 0                                                       },  /* 19 */
    { 0                                                       },  /* 20 */
    { 0                                                       },  /* 21 */
    { 0                                                       },  /* 22 */
    { 0                                                       },  /* 23 */
    { 0                                                       },  /* 24 */
    { 0                                                       },  /* 25 */
    { 0                                                       },  /* 26 */
    { 0                                                       },  /* 27 */
    { 0                                                       },  /* 28 */
    { 0                                                       },  /* 29 */
    { 0                                                       },  /* 30 */
    { 0                                                       }   /* 31 */
  }, {
    /* table 3 */
    { 4, 0, 1,  3, 4, 5, 6,  7, 8,  9, 10, 11, 12, 13, 14, 15 },  /* 0 */
    { 4, 0, 1,  3, 4, 5, 6,  7, 8,  9, 10, 11, 12, 13, 14, 15 },  /* 1 */
    { 3, 0, 1,  3, 4, 5, 6,  7                                },  /* 2 */
    { 3, 0, 1,  3, 4, 5, 6,  7                                },  /* 3 */
    { 3, 0, 1,  3, 4, 5, 6,  7                                },  /* 4 */
    { 3, 0, 1,  3, 4, 5, 6,  7                                },  /* 5 */
    { 3, 0, 1,  3, 4, 5, 6,  7                                },  /* 6 */
    { 3, 0, 1,  3, 4, 5, 6,  7                                },  /* 7 */
    { 3, 0, 1,  3, 4, 5, 6,  7                                },  /* 8 */
    { 3, 0, 1,  3, 4, 5, 6,  7                                },  /* 9 */
    { 3, 0, 1,  3, 4, 5, 6,  7                                },  /* 10 */
    { 3, 0, 1,  3, 4, 5, 6,  7                                },  /* 11 */
    { 0                                   /* sblimit = 12 */  },  /* 12 */
    { 0                                                       },  /* 13 */
    { 0                                                       },  /* 14 */
    { 0                                                       },  /* 15 */
    { 0                                                       },  /* 16 */
    { 0                                                       },  /* 17 */
    { 0                                                       },  /* 18 */
    { 0                                                       },  /* 19 */
    { 0                                                       },  /* 20 */
    { 0                                                       },  /* 21 */
    { 0                                                       },  /* 22 */
    { 0                                                       },  /* 23 */
    { 0                                                       },  /* 24 */
    { 0                                                       },  /* 25 */
    { 0                                                       },  /* 26 */
    { 0                                                       },  /* 27 */
    { 0                                                       },  /* 28 */
    { 0                                                       },  /* 29 */
    { 0                                                       },  /* 30 */
    { 0                                                       }   /* 31 */
  }
};

struct quantclass {
  unsigned short nlevels;
  unsigned char group;
  unsigned char bits;
  fixed_t C;
  fixed_t D;
} const qc_table[17] = {
# include "qc_table.dat"
};

static
void II_samples(struct mad_bitptr *ptr,
		struct quantclass const *quantclass,
		fixed_t *output)
{
  unsigned int nb, s, sample[3];
  fixed_t requantized;

  if ((nb = quantclass->group)) {
    unsigned int c, nlevels;

    /* degrouping */
    c = mad_bit_read(ptr, quantclass->bits);
    nlevels = quantclass->nlevels;

    for (s = 0; s < 3; ++s) {
      sample[s] = c % nlevels;
      c /= nlevels;
    }
  }
  else {
    nb = quantclass->bits;

    for (s = 0; s < 3; ++s)
      sample[s] = mad_bit_read(ptr, nb);
  }

  for (s = 0; s < 3; ++s) {
    /* invert most significant bit, then scale sample to fixed format */

    requantized = ((fixed_t) (-0x80000000L ^ (sample[s] << (32 - nb)))) >> 3;

    /* requantize the sample */

    /* s'' = C * (s''' + D) */

    requantized = f_mul(quantclass->C, requantized + quantclass->D);

    /* s' = factor * s'' */
    /* (to be performed by caller) */

    output[s] = requantized;
  }
}

int mad_layer_II(struct mad_stream *stream, struct mad_frame *frame,
		 unsigned short const crc[2])
{
  struct mad_bitptr start;
  unsigned int table, sblimit, nbal, nch, bound, gr, ch, s, sb, index;
  unsigned char allocation[2][32], scfsi[2][32], scalefactor[2][32][3];
  fixed_t samples[3];

  nch = MAD_NUMCHANNELS(frame);

  switch (nch == 2 ? frame->bitrate >> 1 : frame->bitrate) {
  case 32000:
  case 48000:
    if (frame->samplefreq == 32000) {
      table   = 3;
      sblimit = 12;
    }
    else {
      table   = 2;
      sblimit = 8;
    }
    break;

  case 56000:
  case 64000:
  case 80000:
    table   = 0;
    sblimit = 27;
    break;

  default:
    if (frame->samplefreq == 48000) {
      table   = 0;
      sblimit = 27;
    }
    else {
      table   = 1;
      sblimit = 30;
    }
  }

  if (frame->mode == MAD_MODE_JOINT_STEREO)
    bound = 4 + frame->mode_ext * 4;
  else
    bound = 32;

  if (bound > sblimit)
    bound = sblimit;

  start = stream->ptr;

  /* decode bit allocations */

  for (sb = 0; sb < bound; ++sb) {
    nbal = bitalloc_table[table][sb][0];

    for (ch = 0; ch < nch; ++ch)
      allocation[ch][sb] = mad_bit_read(&stream->ptr, nbal);
  }

  for (sb = bound; sb < sblimit; ++sb) {
    nbal = bitalloc_table[table][sb][0];

    allocation[0][sb] = allocation[1][sb] = mad_bit_read(&stream->ptr, nbal);
  }

  /* decode scalefactor selection info */

  for (sb = 0; sb < sblimit; ++sb) {
    for (ch = 0; ch < nch; ++ch) {
      if (allocation[ch][sb])
	scfsi[ch][sb] = mad_bit_read(&stream->ptr, 2);
    }
  }

  /* check CRC word */

  if ((frame->flags & MAD_FLAG_PROTECTION) &&
      mad_crc(crc[0], start, mad_bit_length(&start, &stream->ptr)) != crc[1]) {
    stream->error = MAD_ERR_BADCRC;
    return -1;
  }

  /* decode scalefactors */

  for (sb = 0; sb < sblimit; ++sb) {
    for (ch = 0; ch < nch; ++ch) {
      if (allocation[ch][sb]) {
	scalefactor[ch][sb][0] = mad_bit_read(&stream->ptr, 6);

	switch (scfsi[ch][sb]) {
	case 2:
	  scalefactor[ch][sb][2] = scalefactor[ch][sb][1] =
	    scalefactor[ch][sb][0];
	  break;

	case 0:
	  scalefactor[ch][sb][1] = mad_bit_read(&stream->ptr, 6);
	  /* fall through */

	case 1:
	case 3:
	  scalefactor[ch][sb][2] = mad_bit_read(&stream->ptr, 6);
	}

	if (scfsi[ch][sb] & 1)
	  scalefactor[ch][sb][1] = scalefactor[ch][sb][scfsi[ch][sb] - 1];

	if (scalefactor[ch][sb][0] == 63 ||
	    scalefactor[ch][sb][1] == 63 ||
	    scalefactor[ch][sb][2] == 63) {
	  stream->error = MAD_ERR_BADSCALEFACTOR;
	  return -1;
	}
      }
    }
  }

  /* decode samples */

  for (gr = 0; gr < 12; ++gr) {
    for (sb = 0; sb < bound; ++sb) {
      for (ch = 0; ch < nch; ++ch) {
	if ((index = allocation[ch][sb])) {
	  index = bitalloc_table[table][sb][index];

	  II_samples(&stream->ptr, &qc_table[index], samples);

	  for (s = 0; s < 3; ++s) {
	    frame->sbsample[ch][3 * gr + s][sb] =
	      f_mul(samples[s], sf_table[scalefactor[ch][sb][gr / 4]]);
	  }
	}
	else {
	  for (s = 0; s < 3; ++s)
	    frame->sbsample[ch][3 * gr + s][sb] = 0;
	}
      }
    }

    for (sb = bound; sb < sblimit; ++sb) {
      if ((index = allocation[0][sb])) {
	index = bitalloc_table[table][sb][index];

	II_samples(&stream->ptr, &qc_table[index], samples);

	for (s = 0; s < 3; ++s) {
	  for (ch = 0; ch < nch; ++ch) {
	    frame->sbsample[ch][3 * gr + s][sb] =
	      f_mul(samples[s], sf_table[scalefactor[ch][sb][gr / 4]]);
	  }
	}
      }
      else {
	for (s = 0; s < 3; ++s) {
	  for (ch = 0; ch < nch; ++ch)
	    frame->sbsample[ch][3 * gr + s][sb] = 0;
	}
      }
    }

    for (sb = sblimit; sb < 32; ++sb) {
      for (s = 0; s < 3; ++s) {
	for (ch = 0; ch < nch; ++ch)
	  frame->sbsample[ch][3 * gr + s][sb] = 0;
      }
    }
  }

  return 0;
}
