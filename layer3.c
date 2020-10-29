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
 * $Id: layer3.c,v 1.9 2000/09/17 03:21:12 rob Exp $
 */

# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif

# ifdef DEBUG
#  include <stdio.h>
# endif

# include <stdlib.h>
# include <string.h>

# ifdef HAVE_LIMITS_H
#  include <limits.h>
# else
#  define CHAR_BIT  8
# endif

# include "fixed.h"
# include "bit.h"
# include "stream.h"
# include "frame.h"
# include "huffman.h"
# include "layer3.h"

/* --- Layer III ----------------------------------------------------------- */

enum {
  count1table_select	= 0x01,
  scalefac_scale	= 0x02,
  preflag		= 0x04,
  window_switching_flag	= 0x08,
  mixed_block_flag	= 0x10
};

struct sideinfo {
  unsigned int main_data_begin;
  unsigned int private_bits;

  unsigned char scfsi[2];

  struct granule {
    struct channel {
      /* from side info */
      unsigned short part2_3_length;
      unsigned short big_values;
      unsigned short global_gain;
      unsigned short scalefac_compress;

      unsigned char flags;
      unsigned char block_type;
      unsigned char table_select[3];
      unsigned char subblock_gain[3];
      unsigned char region0_count;
      unsigned char region1_count;

      /* from main_data */
      unsigned char scalefac_l[22];
      unsigned char scalefac_s[13][3];

      /* implied from decoding */
      unsigned short count1;
    } ch[2];
  } gr[2];
};

/*
 * scalefactor bit lengths
 * derived from section 2.4.2.7 of ISO/IEC 11172-3
 */
static
struct {
  unsigned short slen1;
  unsigned short slen2;
} const sflen[16] = {
  { 0, 0 }, { 0, 1 }, { 0, 2 }, { 0, 3 },
  { 3, 0 }, { 1, 1 }, { 1, 2 }, { 1, 3 },
  { 2, 1 }, { 2, 2 }, { 2, 3 }, { 3, 1 },
  { 3, 2 }, { 3, 3 }, { 4, 2 }, { 4, 3 }
};

/*
 * number of LSF scalefactor band values
 * derived from section 2.4.3.2 of ISO/IEC 13818-3
 */
static
unsigned short const nsfb_table[6][3][4] = {
  { {  6,  5,  5, 5 },
    {  9,  9,  9, 9 },
    {  6,  9,  9, 9 } },
  { {  6,  5,  7, 3 },
    {  9,  9, 12, 6 },
    {  6,  9, 12, 6 } },
  { { 11, 10,  0, 0 },
    { 18, 18,  0, 0 },
    { 15, 18,  0, 0 } },
  { {  7,  7,  7, 0 },
    { 12, 12, 12, 0 },
    {  6, 15, 12, 0 } },
  { {  6,  6,  6, 3 },
    { 12,  9,  9, 6 },
    {  6, 12,  9, 6 } },
  { {  8,  8,  5, 0 },
    { 15, 12,  9, 0 },
    {  6, 18,  9, 0 } }
};

/*
 * scalefactor band widths for long blocks
 * derived from Table B.8 of ISO/IEC 11172-3 and Table B.2 of ISO/IEC 13838-3
 */
static
unsigned short const sfwidth_l[6][22] = {
  /*    48 kHz */ {  4,  4,  4,  4,  4,  4,  6,  6,  6,   8,  10,
		    12, 16, 18, 22, 28, 34, 40, 46, 54,  54, 192 },
  /*  44.1 kHz */ {  4,  4,  4,  4,  4,  4,  6,  6,  8,   8,  10,
		    12, 16, 20, 24, 28, 34, 42, 50, 54,  76, 158 },
  /*    32 kHz */ {  4,  4,  4,  4,  4,  4,  6,  6,  8,  10,  12,
		    16, 20, 24, 30, 38, 46, 56, 68, 84, 102,  26 },

  /*    24 kHz */ {  6,  6,  6,  6,  6,  6,  8, 10, 12,  14,  16,
		    18, 22, 26, 32, 38, 46, 54, 62, 70,  76,  36 },
  /* 22.05 kHz */ {  6,  6,  6,  6,  6,  6,  8, 10, 12,  14,  16,
		    20, 24, 28, 32, 38, 46, 52, 60, 68,  58,  54 },
  /*    16 kHz */ {  6,  6,  6,  6,  6,  6,  8, 10, 12,  14,  16,
		    20, 24, 28, 32, 38, 46, 52, 60, 68,  58,  54 }
};

/*
 * scalefactor band widths for short blocks
 * derived from Table B.8 of ISO/IEC 11172-3 and Table B.2 of ISO/IEC 13838-3
 */
static
unsigned short const sfwidth_s[6][13] = {
  /*    48 kHz */ { 4, 4, 4, 4, 6,  6, 10, 12, 14, 16, 20, 26, 66 },
  /*  44.1 kHz */ { 4, 4, 4, 4, 6,  8, 10, 12, 14, 18, 22, 30, 56 },
  /*    32 kHz */ { 4, 4, 4, 4, 6,  8, 12, 16, 20, 26, 34, 42, 12 },

  /*    24 kHz */ { 4, 4, 4, 6, 8, 10, 12, 14, 18, 24, 32, 44, 12 },
  /* 22.05 kHz */ { 4, 4, 4, 6, 6,  8, 10, 14, 18, 26, 32, 42, 18 },
  /*    16 kHz */ { 4, 4, 4, 6, 8, 10, 12, 14, 18, 24, 30, 40, 18 }
};

/*
 * scalefactor band preemphasis (used only when preflag is set)
 * derived from Table B.6 of ISO/IEC 11172-3
 */
static
unsigned short const pretab[22] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 3, 3, 3, 2, 0
};

/*
 * table for requantization
 * rq_table[x].mantissa * 2^(rq_table[x].exponent) == x^(4/3)
 */
static
struct fixedfloat {
  unsigned long mantissa  : 27;
  unsigned short exponent :  5;
} const rq_table[8207] = {
# include "rq_table.dat"
};

/* fractional powers of two used during requantization */
static
mad_fixed_t const root_table[7] = {
  0x09837f05L /* 2^(-3/4) == 0.59460355750136 */,
  0x0b504f33L /* 2^(-2/4) == 0.70710678118655 */,
  0x0d744fcdL /* 2^(-1/4) == 0.84089641525371 */,
  0x10000000L /* 2^( 0/4) == 1.00000000000000 */,
  0x1306fe0aL /* 2^(+1/4) == 1.18920711500272 */,
  0x16a09e66L /* 2^(+2/4) == 1.41421356237310 */,
  0x1ae89f99L /* 2^(+3/4) == 1.68179283050743 */
};

/*
 * coefficients for aliasing reduction
 * derived from Table B.9 of ISO/IEC 11172-3
 *
 *  c[]  = { -0.6, -0.535, -0.33, -0.185, -0.095, -0.041, -0.0142, -0.0037 }
 * cs[i] =    1 / sqrt(1 + c[i]^2)
 * ca[i] = c[i] / sqrt(1 + c[i]^2)
 */
static
mad_fixed_t const cs[8] = {
  +0x0db84a81L /* +0.857492926 */, +0x0e1b9d7fL /* +0.881741997 */,
  +0x0f31adcfL /* +0.949628649 */, +0x0fbba815L /* +0.983314592 */,
  +0x0feda417L /* +0.995517816 */, +0x0ffc8fc8L /* +0.999160558 */,
  +0x0fff964cL /* +0.999899195 */, +0x0ffff8d3L /* +0.999993155 */
};

static
mad_fixed_t const ca[8] = {
  -0x083b5fe7L /* -0.514495755 */, -0x078c36d2L /* -0.471731969 */,
  -0x05039814L /* -0.313377454 */, -0x02e91dd1L /* -0.181913200 */,
  -0x0183603aL /* -0.094574193 */, -0x00a7cb87L /* -0.040965583 */,
  -0x003a2847L /* -0.014198569 */, -0x000f27b4L /* -0.003699975 */
};

/*
 * IMDCT coefficients for short blocks
 * derived from section 2.4.3.4.10.2 of ISO/IEC 11172-3
 *
 * imdct_s[i][k] = cos((PI / 24) * (2 * i + 7) * (2 * k + 1))
 */
static
mad_fixed_t const imdct_s[12][6] = {
# include "imdct_s.dat"
};

# if !defined(ASO_IMDCT)
/*
 * windowing coefficients for long blocks
 * derived from section 2.4.3.4.10.3 of ISO/IEC 11172-3
 *
 * window_l[i] = sin((PI / 36) * (i + 1/2))
 */
static
mad_fixed_t const window_l[36] = {
  0x00b2aa3eL /* 0.043619387 */, 0x0216a2a2L /* 0.130526192 */,
  0x03768962L /* 0.216439614 */, 0x04cfb0e2L /* 0.300705800 */,
  0x061f78aaL /* 0.382683432 */, 0x07635284L /* 0.461748613 */,
  0x0898c779L /* 0.537299608 */, 0x09bd7ca0L /* 0.608761429 */,
  0x0acf37adL /* 0.675590208 */, 0x0bcbe352L /* 0.737277337 */,
  0x0cb19346L /* 0.793353340 */, 0x0d7e8807L /* 0.843391446 */,

  0x0e313245L /* 0.887010833 */, 0x0ec835e8L /* 0.923879533 */,
  0x0f426cb5L /* 0.953716951 */, 0x0f9ee890L /* 0.976296007 */,
  0x0fdcf549L /* 0.991444861 */, 0x0ffc19fdL /* 0.999048222 */,
  0x0ffc19fdL /* 0.999048222 */, 0x0fdcf549L /* 0.991444861 */,
  0x0f9ee890L /* 0.976296007 */, 0x0f426cb5L /* 0.953716951 */,
  0x0ec835e8L /* 0.923879533 */, 0x0e313245L /* 0.887010833 */,

  0x0d7e8807L /* 0.843391446 */, 0x0cb19346L /* 0.793353340 */,
  0x0bcbe352L /* 0.737277337 */, 0x0acf37adL /* 0.675590208 */,
  0x09bd7ca0L /* 0.608761429 */, 0x0898c779L /* 0.537299608 */,
  0x07635284L /* 0.461748613 */, 0x061f78aaL /* 0.382683432 */,
  0x04cfb0e2L /* 0.300705800 */, 0x03768962L /* 0.216439614 */,
  0x0216a2a2L /* 0.130526192 */, 0x00b2aa3eL /* 0.043619387 */,
};
# endif  /* ASO_IMDCT */

/*
 * windowing coefficients for short blocks
 * derived from section 2.4.3.4.10.3 of ISO/IEC 11172-3
 *
 * window_s[i] = sin((PI / 12) * (i + 1/2))
 */
static
mad_fixed_t const window_s[12] = {
  0x0216a2a2L /* 0.130526192 */, 0x061f78aaL /* 0.382683432 */,
  0x09bd7ca0L /* 0.608761429 */, 0x0cb19346L /* 0.793353340 */,
  0x0ec835e8L /* 0.923879533 */, 0x0fdcf549L /* 0.991444861 */,
  0x0fdcf549L /* 0.991444861 */, 0x0ec835e8L /* 0.923879533 */,
  0x0cb19346L /* 0.793353340 */, 0x09bd7ca0L /* 0.608761429 */,
  0x061f78aaL /* 0.382683432 */, 0x0216a2a2L /* 0.130526192 */,
};

/*
 * coefficients for intensity stereo processing
 * derived from section 2.4.3.4.9.3 of ISO/IEC 11172-3
 *
 * is_ratio[i] = tan(i * (PI / 12))
 * is_table[i] = is_ratio[i] / (1 + is_ratio[i])
 */
static
mad_fixed_t const is_table[7] = {
  0x00000000L /* 0.000000000 */,
  0x0361962fL /* 0.211324865 */,
  0x05db3d74L /* 0.366025404 */,
  0x08000000L /* 0.500000000 */,
  0x0a24c28cL /* 0.633974596 */,
  0x0c9e69d1L /* 0.788675135 */,
  0x10000000L /* 1.000000000 */
};

/*
 * coefficients for LSF intensity stereo processing
 * derived from section 2.4.3.2 of ISO/IEC 13838-3
 *
 * lsf_is_table[0][i] = (1 / sqrt(sqrt(2)))^(i + 1)
 * lsf_is_table[1][i] = (1 / sqrt(2))^(i + 1)
 */
static
mad_fixed_t const lsf_is_table[2][3] = {
  {
    0x0d744fcdL /* 0.840896415 */,
    0x0b504f33L /* 0.707106781 */,
    0x09837f05L /* 0.594603558 */
  }, {
    0x0b504f33L /* 0.707106781 */,
    0x08000000L /* 0.500000000 */,
    0x05a8279aL /* 0.353553391 */
  }
};

/*
 * NAME:	III_sideinfo()
 * DESCRIPTION:	decode frame side information from a bitstream
 */
static
int III_sideinfo(struct mad_bitptr *ptr, unsigned int nch,
		 int lsf, struct sideinfo *si,
		 unsigned int *md_bitlen, unsigned int *priv_bitlen)
{
  unsigned int ngr, gr, ch, i;
  int result = 0;

  *md_bitlen   = 0;
  *priv_bitlen = lsf ? ((nch == 1) ? 1 : 2) : ((nch == 1) ? 5 : 3);

  si->main_data_begin = mad_bit_read(ptr, lsf ? 8 : 9);
  si->private_bits    = mad_bit_read(ptr, *priv_bitlen);

  if (lsf)
    ngr = 1;
  else {
    ngr = 2;

    for (ch = 0; ch < nch; ++ch)
      si->scfsi[ch] = mad_bit_read(ptr, 4);
  }

  for (gr = 0; gr < ngr; ++gr) {
    struct granule *granule = &si->gr[gr];

    for (ch = 0; ch < nch; ++ch) {
      struct channel *channel = &granule->ch[ch];

      channel->part2_3_length    = mad_bit_read(ptr, 12);
      channel->big_values        = mad_bit_read(ptr, 9);
      channel->global_gain       = mad_bit_read(ptr, 8);
      channel->scalefac_compress = mad_bit_read(ptr, lsf ? 9 : 4);

      *md_bitlen += channel->part2_3_length;

      if (channel->big_values > 288 && result == 0)
	result = MAD_ERR_BADBIGVALUES;

      channel->flags = 0;

      if (mad_bit_read(ptr, 1)) {
	channel->flags |= window_switching_flag;

	channel->block_type = mad_bit_read(ptr, 2);
	if (channel->block_type == 0 && result == 0)
	  result = MAD_ERR_BADBLOCKTYPE;

	channel->region0_count = 7;
	channel->region1_count = 36;

	if (mad_bit_read(ptr, 1))
	  channel->flags |= mixed_block_flag;
	else if (channel->block_type == 2)
	  channel->region0_count = 8;

	for (i = 0; i < 2; ++i)
	  channel->table_select[i] = mad_bit_read(ptr, 5);

# ifdef DEBUG
	channel->table_select[2] = 4;  /* not used */
# endif

	for (i = 0; i < 3; ++i)
	  channel->subblock_gain[i] = mad_bit_read(ptr, 3);
      }
      else {
	channel->block_type = 0;

	for (i = 0; i < 3; ++i)
	  channel->table_select[i] = mad_bit_read(ptr, 5);

	channel->region0_count = mad_bit_read(ptr, 4);
	channel->region1_count = mad_bit_read(ptr, 3);
      }

      /* [preflag,] scalefac_scale, count1table_select */
      channel->flags |= mad_bit_read(ptr, lsf ? 2 : 3);
    }
  }

  return result;
}

/*
 * NAME:	III_lsf_scalefactors()
 * DESCRIPTION:	decode channel scalefactors for LSF from a bitstream
 */
static
unsigned int III_lsf_scalefactors(struct mad_bitptr *ptr, int mode_extension,
				  unsigned int ch, struct channel *channel)
{
  struct mad_bitptr start;
  unsigned int scalefac_compress, index, slen[4], part, n, sfb, w;
  unsigned short const *nsfb;
  unsigned short scalefac[36];

  start = *ptr;

  scalefac_compress = channel->scalefac_compress;
  index = (channel->block_type == 2) ?
    ((channel->flags & mixed_block_flag) ? 2 : 1) : 0;

  if (!((mode_extension & 0x1) && ch == 1)) {
    if (scalefac_compress < 400) {
      slen[0] = (scalefac_compress >> 4) / 5;
      slen[1] = (scalefac_compress >> 4) % 5;
      slen[2] = (scalefac_compress % 16) >> 2;
      slen[3] =  scalefac_compress %  4;

      nsfb = nsfb_table[0][index];
    }
    else if (scalefac_compress < 500) {
      scalefac_compress -= 400;

      slen[0] = (scalefac_compress >> 2) / 5;
      slen[1] = (scalefac_compress >> 2) % 5;
      slen[2] =  scalefac_compress %  4;
      slen[3] = 0;

      nsfb = nsfb_table[1][index];
    }
    else {
      scalefac_compress -= 500;

      slen[0] = scalefac_compress / 3;
      slen[1] = scalefac_compress % 3;
      slen[2] = 0;
      slen[3] = 0;

      channel->flags |= preflag;

      nsfb = nsfb_table[2][index];
    }
  }
  else {  /* (mode_extension & 0x1) && ch == 1 */
    scalefac_compress >>= 1;

    if (scalefac_compress < 180) {
      slen[0] =  scalefac_compress / 36;
      slen[1] = (scalefac_compress % 36) / 6;
      slen[2] = (scalefac_compress % 36) % 6;
      slen[3] = 0;

      nsfb = nsfb_table[3][index];
    }
    else if (scalefac_compress < 244) {
      scalefac_compress -= 180;

      slen[0] = (scalefac_compress % 64) >> 4;
      slen[1] = (scalefac_compress % 16) >> 2;
      slen[2] =  scalefac_compress %  4;
      slen[3] = 0;

      nsfb = nsfb_table[4][index];
    }
    else {
      scalefac_compress -= 244;

      slen[0] = scalefac_compress / 3;
      slen[1] = scalefac_compress % 3;
      slen[2] = 0;
      slen[3] = 0;

      nsfb = nsfb_table[5][index];
    }
  }

  n = 0;
  for (part = 0; part < 4; ++part) {
    for (sfb = 0; sfb < nsfb[part]; ++sfb)
      scalefac[n++] = mad_bit_read(ptr, slen[part]);
  }

  n = 0;
  if (channel->block_type == 2) {
    if (channel->flags & mixed_block_flag) {
      for (sfb = 0; sfb < 6; ++sfb)
	channel->scalefac_l[sfb] = scalefac[n++];

      for (sfb = 3; sfb < 12; ++sfb) {
	for (w = 0; w < 3; ++w)
	  channel->scalefac_s[sfb][w] = scalefac[n++];
      }
    }
    else {  /* !(channel->flags & mixed_block_flag) */
      for (sfb = 0; sfb < 12; ++sfb) {
	for (w = 0; w < 3; ++w)
	  channel->scalefac_s[sfb][w] = scalefac[n++];
      }
    }

    for (w = 0; w < 3; ++w)
      channel->scalefac_s[12][w] = 0;
  }
  else {  /* channel->block_type != 2 */
    for (sfb = 0; sfb < 21; ++sfb)
      channel->scalefac_l[sfb] = scalefac[n++];

    channel->scalefac_l[21] = 0;
  }

  return mad_bit_length(&start, ptr);
}

/*
 * NAME:	III_scalefactors()
 * DESCRIPTION:	decode channel scalefactors of one granule from a bitstream
 */
static
unsigned int III_scalefactors(struct mad_bitptr *ptr, struct channel *channel,
			      struct channel const *gr0ch, unsigned int scfsi)
{
  struct mad_bitptr start;
  unsigned int sfb, w;
  unsigned int slen1, slen2;

  start = *ptr;

  slen1 = sflen[channel->scalefac_compress].slen1;
  slen2 = sflen[channel->scalefac_compress].slen2;

  if (channel->block_type == 2) {
    if (channel->flags & mixed_block_flag) {
      for (sfb = 0; sfb < 8; ++sfb)
	channel->scalefac_l[sfb] = mad_bit_read(ptr, slen1);

      for (sfb = 3; sfb < 6; ++sfb) {
	for (w = 0; w < 3; ++w)
	  channel->scalefac_s[sfb][w] = mad_bit_read(ptr, slen1);
      }
    }
    else {  /* !(channel->flags & mixed_block_flag) */
      for (sfb = 0; sfb < 6; ++sfb) {
	for (w = 0; w < 3; ++w)
	  channel->scalefac_s[sfb][w] = mad_bit_read(ptr, slen1);
      }
    }

    for (sfb = 6; sfb < 12; ++sfb) {
      for (w = 0; w < 3; ++w)
	channel->scalefac_s[sfb][w] = mad_bit_read(ptr, slen2);
    }

    for (w = 0; w < 3; ++w)
      channel->scalefac_s[12][w] = 0;
  }
  else {  /* channel->block_type != 2 */
    if (scfsi & 0x8) {
      for (sfb = 0; sfb < 6; ++sfb)
	channel->scalefac_l[sfb] = gr0ch->scalefac_l[sfb];
    }
    else {
      for (sfb = 0; sfb < 6; ++sfb)
	channel->scalefac_l[sfb] = mad_bit_read(ptr, slen1);
    }

    if (scfsi & 0x4) {
      for (sfb = 6; sfb < 11; ++sfb)
	channel->scalefac_l[sfb] = gr0ch->scalefac_l[sfb];
    }
    else {
      for (sfb = 6; sfb < 11; ++sfb)
	channel->scalefac_l[sfb] = mad_bit_read(ptr, slen1);
    }

    if (scfsi & 0x2) {
      for (sfb = 11; sfb < 16; ++sfb)
	channel->scalefac_l[sfb] = gr0ch->scalefac_l[sfb];
    }
    else {
      for (sfb = 11; sfb < 16; ++sfb)
	channel->scalefac_l[sfb] = mad_bit_read(ptr, slen2);
    }

    if (scfsi & 0x1) {
      for (sfb = 16; sfb < 21; ++sfb)
	channel->scalefac_l[sfb] = gr0ch->scalefac_l[sfb];
    }
    else {
      for (sfb = 16; sfb < 21; ++sfb)
	channel->scalefac_l[sfb] = mad_bit_read(ptr, slen2);
    }

    channel->scalefac_l[21] = 0;
  }

  return mad_bit_length(&start, ptr);
}

/*
 * NAME:	III_requantize()
 * DESCRIPTION:	requantize one sample
 */
static
mad_fixed_t III_requantize(signed int value, signed int exp, unsigned int scf,
			   struct channel const *channel)
{
  mad_fixed_t requantized;
  signed int frac;
  struct fixedfloat const *power;

  /*
   * long blocks:
   * xr[i] = sign(is[i]) * abs(is[i])^(4/3) *
   *         2^((1/4) * (global_gain - 210)) *
   *         2^-(scalefac_multiplier *
   *             (scalefac_l[sfb] + preflag * pretab[sfb]))
   *
   * short blocks:
   * xr[i] = sign(is[i]) * abs(is[i])^(4/3) *
   *         2^((1/4) * (global_gain - 210 - 8 * subblock_gain[w])) *
   *         2^-(scalefac_multiplier * scalefac_s[sfb][w])
   *
   * where:
   * scalefac_multiplier = (scalefac_scale + 1) / 2
   */

  scf <<= (channel->flags & scalefac_scale) ? 2 : 1;
  exp  += (signed) channel->global_gain - 210 - (signed) scf;

  frac = exp % 4;
  exp /= 4;

  if (value < 0) {
    power = &rq_table[-value];
    requantized = -power->mantissa;
  }
  else {
    power = &rq_table[value];
    requantized = power->mantissa;
  }

  exp += power->exponent;

  if (exp < 0) {
    if (-exp >= sizeof(mad_fixed_t) * CHAR_BIT) {
      /* underflow */
      requantized = 0;
    }
    else
      requantized >>= -exp;
  }
  else {
    if (exp >= 5) {
      /* overflow */
# ifdef DEBUG
      fprintf(stderr, "requantize overflow (%f * 2^%d)\n",
	      mad_f_todouble(requantized), exp);
# endif
      requantized = (requantized < 0) ? MAD_F_MIN : MAD_F_MAX;
    }
    else
      requantized <<= exp;
  }

  return frac ? mad_f_mul(requantized, root_table[3 + frac]) : requantized;
}

/*
 * NAME:	III_requantize_l()
 * DESCRIPTION:	requantize one sample from a long block
 */
static
mad_fixed_t III_requantize_l(signed int value, unsigned int sfb,
			     struct channel const *channel)
{
  unsigned int scf;

  /* channel->block_type != 2 ||
     (l < 36 && (channel->flags & mixed_block_flag)) */

  scf = channel->scalefac_l[sfb];
  if (channel->flags & preflag)
    scf += pretab[sfb];

  return III_requantize(value, 0, scf, channel);
}

/*
 * NAME:	III_requantize_s()
 * DESCRIPTION:	requantize one sample from a short block
 */
static
mad_fixed_t III_requantize_s(signed int value, unsigned int sfb,
			     unsigned int window,
			     struct channel const *channel)
{
  signed int exp;
  unsigned int scf;

  /* channel->block_type == 2 &&
     (l >= 36 || !(channel->flags & mixed_block_flag)) */

  exp = (signed) channel->subblock_gain[window] * 8;
  scf = channel->scalefac_s[sfb][window];

  return III_requantize(value, -exp, scf, channel);
}

/* we must take care that sz >= bits and sz < sizeof(long) lest bits == 0 */
# define MASK(cache, sz, bits)	\
    (((cache) >> ((sz) - (bits))) & ((1 << (bits)) - 1))
# define MASK1BIT(cache, sz)  \
    ((cache) & (1 << ((sz) - 1)))

/*
 * NAME:	III_huffdecode()
 * DESCRIPTION:	decode Huffman code words of one channel of one granule
 */
static
int III_huffdecode(struct mad_bitptr *ptr, signed short is[576],
		   struct channel *channel, unsigned int sfreqi,
		   unsigned int part2_length)
{
  struct mad_bitptr peek;
  signed int bits_left, cachesz;
  unsigned int l;
  unsigned long bitcache;

  bits_left = (signed) channel->part2_3_length - (signed) part2_length;
  if (bits_left < 0)
    return MAD_ERR_BADPART3LEN;

  peek = *ptr;
  mad_bit_skip(ptr, bits_left);

  /* align bit reads to byte boundaries */
  cachesz  = mad_bit_bitsleft(&peek);
  cachesz += (7 + (24 - cachesz)) & ~7;

  bitcache   = mad_bit_read(&peek, cachesz);
  bits_left -= cachesz;

  l = 0;

  /* big_values */
  {
    unsigned int region, rcount, pcount;
    unsigned short const *fwidth;
    struct hufftable const *entry;
    union huffpair const *table;
    unsigned int linbits, startbits, big_values;

    if (channel->block_type == 2) {
      fwidth = &sfwidth_s[sfreqi][3];

      pcount = 1 + (36 / 2);
      rcount = 1;
    }
    else {
      fwidth = &sfwidth_l[sfreqi][0];

      pcount = 1 + (*fwidth++ / 2);
      rcount = channel->region0_count + 1;
    }

    entry     = &mad_huff_pair_table[channel->table_select[region = 0]];
    table     = entry->table;
    linbits   = entry->linbits;
    startbits = entry->startbits;

    if (table == 0)
      return MAD_ERR_BADHUFFTABLE;

    big_values = channel->big_values;

    while (big_values-- && cachesz + bits_left > 0) {
      union huffpair const *pair;
      unsigned int clumpsz;
      signed int x, y;

      /* change table if region boundary */

      if (--pcount == 0) {
	if (--rcount == 0) {
	  if (region == 0 && !(channel->flags & window_switching_flag)) {
	    pcount = *fwidth++ / 2;
	    rcount = channel->region1_count + 1;
	  }
	  else
	    pcount = 576 / 2;  /* all remaining */

	  entry     = &mad_huff_pair_table[channel->table_select[++region]];
	  table     = entry->table;
	  linbits   = entry->linbits;
	  startbits = entry->startbits;

	  if (table == 0)
	    return MAD_ERR_BADHUFFTABLE;
	}
	else {
	  pcount = *fwidth++ / 2;
	  if (channel->block_type == 2)
	    pcount *= 3;
	}
      }

      /* hcod (0..19) */

      if (cachesz < 19) {
	unsigned int bits;

	bits       = (12 + (19 - cachesz)) & ~7;
	bitcache   = (bitcache << bits) | mad_bit_read(&peek, bits);
	cachesz   += bits;
	bits_left -= bits;
      }

      clumpsz = startbits;
      pair    = &table[MASK(bitcache, cachesz, clumpsz)];

      while (!pair->final) {
	cachesz -= clumpsz;

	clumpsz = pair->ptr.bits;
	pair    = &table[pair->ptr.offset + MASK(bitcache, cachesz, clumpsz)];
      }

      cachesz -= pair->value.hlen;

      /* x (0..14) */

      if (cachesz < 14) {
	bitcache   = (bitcache << 16) | mad_bit_read(&peek, 16);
	cachesz   += 16;
	bits_left -= 16;
      }

      x = pair->value.x;
      if (x == 15 && linbits) {
	x += MASK(bitcache, cachesz, linbits);
	cachesz -= linbits;
      }

      if (x && MASK1BIT(bitcache, cachesz--))
	x = -x;

      is[l++] = x;

      /* y (0..14) */

      if (cachesz < 14) {
	bitcache   = (bitcache << 16) | mad_bit_read(&peek, 16);
	cachesz   += 16;
	bits_left -= 16;
      }

      y = pair->value.y;
      if (y == 15 && linbits) {
	y += MASK(bitcache, cachesz, linbits);
	cachesz -= linbits;
      }

      if (y && MASK1BIT(bitcache, cachesz--))
	y = -y;

      is[l++] = y;
    }
  }

  if (cachesz + bits_left < 0) {
# if 1 && defined(DEBUG)
    fprintf(stderr, "huffman big_values overrun (%d bits)\n",
	    -(cachesz + bits_left));
# endif

    is[l - 2] = 0;
    is[l - 1] = 0;

    channel->count1 = 0;
  }
  else {  /* count1 */
    union huffquad const *table;
    unsigned int count1 = 0;

    table = mad_huff_quad_table[channel->flags & count1table_select];

    while (cachesz + bits_left > 0 && l <= 572) {
      union huffquad const *quad;
      signed int v, w, x, y;

      /* hcod (1..6) */

      if (cachesz < 10) {
	bitcache   = (bitcache << 16) | mad_bit_read(&peek, 16);
	cachesz   += 16;
	bits_left -= 16;
      }

      quad = &table[MASK(bitcache, cachesz, 4)];

      /* quad tables guaranteed to have at most one extra lookup */
      if (!quad->final) {
	cachesz -= 4;

	quad = &table[quad->ptr.offset +
		      MASK(bitcache, cachesz, quad->ptr.bits)];
      }

      cachesz -= quad->value.hlen;

      /* v (0..1) */

      v = quad->value.v;
      if (v && MASK1BIT(bitcache, cachesz--))
	v = -v;

      is[l++] = v;

      /* w (0..1) */

      w = quad->value.w;
      if (w && MASK1BIT(bitcache, cachesz--))
	w = -w;

      is[l++] = w;

      /* x (0..1) */

      x = quad->value.x;
      if (x && MASK1BIT(bitcache, cachesz--))
	x = -x;

      is[l++] = x;

      /* y (0..1) */

      y = quad->value.y;
      if (y && MASK1BIT(bitcache, cachesz--))
	y = -y;

      is[l++] = y;

      ++count1;
    }

    if (cachesz + bits_left < 0) {
# if 0 && defined(DEBUG)
      fprintf(stderr, "huffman count1 overrun (%d bits)\n",
	      -(cachesz + bits_left));
# endif

      /* technically the bitstream is misformatted, but apparently
	 some encoders are just a bit sloppy with stuffing bits */

      is[l - 4] = 0;
      is[l - 3] = 0;
      is[l - 2] = 0;
      is[l - 1] = 0;

      --count1;
    }

    channel->count1 = count1;
  }

# if 0 && defined(DEBUG)
  if (bits_left < 0)
    fprintf(stderr, "read %d bits too many\n", -bits_left);
  else if (cachesz + bits_left > 0)
    fprintf(stderr, "%d stuffing bits\n", cachesz + bits_left);
# endif

  /* rzero */
  while (l < 576) {
    is[l++] = 0;
    is[l++] = 0;
  }

  return 0;
}

# undef MASK
# undef MASK1BIT

/*
 * NAME:	III_reorder()
 * DESCRIPTION:	reorder frequency lines of a short block into subband order
 */
static
void III_reorder(mad_fixed_t xr[576], struct channel const *channel,
		 unsigned short const sfwidth[13])
{
  mad_fixed_t tmp[32][3][6];
  unsigned int sb, l, sfb, f, w, sbw[3], sw[3];

  if (channel->flags & mixed_block_flag)
    sb = 2, sfb = 3;
  else
    sb = 0, sfb = 0;

  for (w = 0; w < 3; ++w) {
    sbw[w] = sb;
    sw[w]  = 0;
  }

  f = sfwidth[sfb];
  w = 0;

  for (l = 18 * sb; l < 576; ++l) {
    tmp[sbw[w]][w][sw[w]++] = xr[l];

    if (sw[w] == 6) {
      sw[w] = 0;
      ++sbw[w];
    }

    if (--f == 0) {
      if (w++ == 2) {
	w = 0;
	++sfb;
      }

      f = sfwidth[sfb];
    }
  }

  memcpy(&xr[18 * sb], &tmp[sb], (576 - 18 * sb) * sizeof(mad_fixed_t));
}

/*
 * NAME:	III_stereo()
 * DESCRIPTION:	perform joint stereo processing on a granule
 */
static
int III_stereo(mad_fixed_t xr[2][576], struct granule *granule,
	       struct mad_frame *frame, unsigned int sfreqi)
{
  int i_stereo, ms_stereo;
  unsigned int bound;

  i_stereo  = frame->mode_ext & 0x1;
  ms_stereo = frame->mode_ext & 0x2;

  bound = 576;

  /* intensity stereo */

  if (i_stereo) {
    struct channel const *right_ch = &granule->ch[1];
    unsigned int zero_part, i, sfb_l, sfb_s, f, w, n;
    mad_fixed_t const *lsf_scale;

    frame->flags |= MAD_FLAG_I_STEREO;

    zero_part = 576 - right_ch->big_values * 2 - right_ch->count1 * 4;

# if 0 && defined(DEBUG)
    fprintf(stderr, "intensity_stereo zero_part:%u, block_type:%u\n",
	    zero_part, right_ch->block_type);
# endif

    sfb_l = (right_ch->block_type == 2) ?
      ((frame->flags & MAD_FLAG_LSF_EXT) ? 6 : 8) : 22;
    sfb_s = 13;

    while (bound > 0) {
      if (right_ch->block_type != 2 ||
	  (bound <= 36 && (right_ch->flags & mixed_block_flag))) {
	/* long blocks */
	f = sfwidth_l[sfreqi][sfb_l - 1];

	if (zero_part < f)
	  break;
	else {
	  zero_part -= f;
	  bound     -= f;
	  --sfb_l;
	}
      }
      else {
	/* short blocks */
	f = sfwidth_s[sfreqi][sfb_s - 1] * 3;

	if (zero_part < f)
	  break;
	else {
	  zero_part -= f;
	  bound     -= f;
	  --sfb_s;
	}
      }
    }

    f = 0;
    w = 0;
    n = 0;

    /* intensity_scale */
    lsf_scale = lsf_is_table[right_ch->scalefac_compress & 0x1];

    for (i = bound; i < 576; ++i) {
      unsigned int is_pos = 7;

      if (f-- == 0) {
	if (w-- == 0) {
	  if (right_ch->block_type != 2 ||
	      (i < 36 && (right_ch->flags & mixed_block_flag))) {
	    w = 0;
	    f = sfwidth_l[sfreqi][sfb_l] - 1;
	    is_pos = right_ch->scalefac_l[sfb_l++];
	  }
	  else {
	    w = 2;
	    f = n = sfwidth_s[sfreqi][sfb_s] - 1;
	    is_pos = right_ch->scalefac_s[sfb_s++][0];
	  }
	}
	else {
	  f = n;
	  is_pos = right_ch->scalefac_s[sfb_s - 1][2 - w];
	}
      }

      if (is_pos < 7) {
	mad_fixed_t left;

	left = xr[0][i];

	if (frame->flags & MAD_FLAG_LSF_EXT) {
	  if (is_pos == 0)
	    xr[1][i] = left;
	  else if (is_pos & 0x1) {
	    xr[0][i] = mad_f_mul(left, lsf_scale[(is_pos - 1) / 2]);
	    xr[1][i] = left;
	  }
	  else
	    xr[1][i] = mad_f_mul(left, lsf_scale[(is_pos - 1) / 2]);
	}
	else {
	  xr[0][i] = mad_f_mul(left, is_table[is_pos]);
	  xr[1][i] = mad_f_mul(left, is_table[6 - is_pos]);
	}
      }
# if defined(OPT_ISKLUGE)
      else if (ms_stereo) {
	mad_fixed_t m, s;

	/*
	 * There seems to be some discrepancy with respect to simultaneous
	 * MS and intensity stereo encoding. The ISO/IEC 11172-3 standard
	 * clearly states that "...the scalefactor band in which the last
	 * non-zero (right channel) frequency line occurs is the last
	 * scalefactor band to which the MS_stereo equations apply" when
	 * intensity stereo is also enabled. "Above this bound intensity
	 * stereo may be applied if enabled in the header."
	 *
	 * However, the ISO/IEC 13818-3 standard states, "As in ISO/IEC
	 * 11172-3 scalefactor bands with an illegal intensity position
	 * have to be decoded according to the MS equations as defined in
	 * ISO/IEC 11172-3 if MS stereo is enabled, or both channels
	 * independently if MS stereo is not enabled."
	 *
	 * The ISO reference source and many other decoders are apparently
	 * interpreting this with mixed results. It's possible many
	 * encoders have a questionable implementation as well, but it
	 * happens to go unnoticed with the same decoders.
	 *
	 * It's not clear what the right answer is. For now, the intensity
	 * stereo bands can also optionally be processed with MS for
	 * is_pos values outside the intensity stereo range, which seems
	 * to be what at least one other decoder implementation is doing.
	 * This does not necessarily produce optimal results, but it is
	 * better than no processing for many bitstreams.
	 */

	m = xr[0][i];
	s = xr[1][i];

	xr[0][i] = mad_f_mul(m + s, root_table[1]); /* l = (m + s) / sqrt(2) */
	xr[1][i] = mad_f_mul(m - s, root_table[1]); /* r = (m - s) / sqrt(2) */
      }
# endif  /* OPT_ISKLUGE */
    }
  }

  /* middle/side stereo */

  if (ms_stereo) {
    unsigned int i;

    frame->flags |= MAD_FLAG_MS_STEREO;

    if (granule->ch[0].block_type != granule->ch[1].block_type)
      return MAD_ERR_BADSTEREO;

    i = bound;
    while (i--) {
      mad_fixed_t m, s;

      m = xr[0][i];
      s = xr[1][i];

      xr[0][i] = mad_f_mul(m + s, root_table[1]);  /* l = (m + s) / sqrt(2) */
      xr[1][i] = mad_f_mul(m - s, root_table[1]);  /* r = (m - s) / sqrt(2) */
    }
  }

  return 0;
}

/*
 * NAME:	III_aliasreduce()
 * DESCRIPTION:	perform frequency line alias reduction
 */
static inline
void III_aliasreduce(mad_fixed_t xr[576])
{
  unsigned int l, i;

  for (l = 18; l < 576; l += 18) {
    for (i = 0; i < 8; ++i) {
      mad_fixed_t a, b;

      a = xr[l - 1 - i];
      b = xr[l     + i];

      xr[l - 1 - i] = mad_f_mul(a, cs[i]) - mad_f_mul(b, ca[i]);
      xr[l     + i] = mad_f_mul(b, cs[i]) + mad_f_mul(a, ca[i]);
    }
  }
}

# if defined(ASO_IMDCT)
void III_imdct_l(mad_fixed_t const [18], mad_fixed_t [36], unsigned int);
# else
/*
 * NAME:	imdct36
 * DESCRIPTION:	perform X[18]->x[36] IMDCT
 */
static inline
void imdct36(mad_fixed_t const X[18], mad_fixed_t x[36])
{
  mad_fixed_t t0,  t1,  t2,  t3,  t4,  t5;
  mad_fixed_t t6,  t7,  t8,  t9,  t10, t11;
  mad_fixed_t t12, t13, t14, t15, t16, t17;

  t6 =
    mad_f_mul(X[4],  0x0ec835e8L) +
    mad_f_mul(X[13], 0x061f78aaL);

  t0 = t6 +
    (t7 = mad_f_mul((t16 = X[1] - X[10]), -0x061f78aaL)) +
    (t8 = mad_f_mul((t17 = X[7] + X[16]), -0x0ec835e8L));

  x[7] = t0 +
    mad_f_mul((t10 = X[0] - X[11] - X[12]),  0x0216a2a2L) +
    mad_f_mul((t11 = X[2] - X[9]  - X[14]),  0x09bd7ca0L) +
    mad_f_mul((t12 = X[3] - X[8]  - X[15]), -0x0cb19346L) +
    mad_f_mul((t13 = X[5] - X[6]  - X[17]), -0x0fdcf549L);
  x[10] = -x[7];

  x[19] = x[34] = -t6 + -t7 + -t8 +
    mad_f_mul(t10, -0x0cb19346L) +
    mad_f_mul(t11,  0x0fdcf549L) +
    mad_f_mul(t12,  0x0216a2a2L) +
    mad_f_mul(t13, -0x09bd7ca0L);

  t14 = X[0] - X[3] + X[8] - X[11] - X[12] + X[15];
  t15 = X[2] + X[5] - X[6] - X[9]  - X[14] - X[17];

  x[22] = x[31] = t0 +
    mad_f_mul(t14, -0x0ec835e8L) +
    mad_f_mul(t15,  0x061f78aaL);

  t1 = t6 +
    mad_f_mul(X[1],  -0x09bd7ca0L) +
    mad_f_mul(X[7],   0x0216a2a2L) +
    mad_f_mul(X[10], -0x0fdcf549L) +
    mad_f_mul(X[16],  0x0cb19346L);

  x[6] = t1 +
    mad_f_mul(X[0],   0x03768962L) +
    mad_f_mul(X[2],   0x0e313245L) +
    mad_f_mul(X[3],  -0x0ffc19fdL) +
    mad_f_mul(X[5],  -0x0acf37adL) +
    mad_f_mul(X[6],   0x04cfb0e2L) +
    mad_f_mul(X[8],  -0x0898c779L) +
    mad_f_mul(X[9],   0x0d7e8807L) +
    mad_f_mul(X[11],  0x0f426cb5L) +
    mad_f_mul(X[12], -0x0bcbe352L) +
    mad_f_mul(X[14],  0x00b2aa3eL) +
    mad_f_mul(X[15], -0x07635284L) +
    mad_f_mul(X[17], -0x0f9ee890L);
  x[11] = -x[6];

  x[23] = x[30] = t1 +
    mad_f_mul(X[0],  -0x0f426cb5L) +
    mad_f_mul(X[2],  -0x00b2aa3eL) +
    mad_f_mul(X[3],   0x0898c779L) +
    mad_f_mul(X[5],   0x0f9ee890L) +
    mad_f_mul(X[6],   0x0acf37adL) +
    mad_f_mul(X[8],  -0x07635284L) +
    mad_f_mul(X[9],  -0x0e313245L) +
    mad_f_mul(X[11], -0x0bcbe352L) +
    mad_f_mul(X[12], -0x03768962L) +
    mad_f_mul(X[14],  0x0d7e8807L) +
    mad_f_mul(X[15],  0x0ffc19fdL) +
    mad_f_mul(X[17],  0x04cfb0e2L);

  x[18] = x[35] = -t1 +
    mad_f_mul(X[0],  -0x0bcbe352L) +
    mad_f_mul(X[2],   0x0d7e8807L) +
    mad_f_mul(X[3],  -0x07635284L) +
    mad_f_mul(X[5],   0x04cfb0e2L) +
    mad_f_mul(X[6],   0x0f9ee890L) +
    mad_f_mul(X[8],  -0x0ffc19fdL) +
    mad_f_mul(X[9],  -0x00b2aa3eL) +
    mad_f_mul(X[11],  0x03768962L) +
    mad_f_mul(X[12], -0x0f426cb5L) +
    mad_f_mul(X[14],  0x0e313245L) +
    mad_f_mul(X[15],  0x0898c779L) +
    mad_f_mul(X[17], -0x0acf37adL);

  t9 =
    mad_f_mul(X[4],   0x061f78aaL) +
    mad_f_mul(X[13], -0x0ec835e8L);

  t2 = t9 +
    mad_f_mul(X[1],  -0x0cb19346L)  +
    mad_f_mul(X[7],   0x0fdcf549L)  +
    mad_f_mul(X[10],  0x0216a2a2L)  +
    mad_f_mul(X[16], -0x09bd7ca0L);

  x[5] = t2 +
    mad_f_mul(X[0],   0x04cfb0e2L) +
    mad_f_mul(X[2],   0x0ffc19fdL) +
    mad_f_mul(X[3],  -0x0d7e8807L) +
    mad_f_mul(X[5],   0x03768962L) +
    mad_f_mul(X[6],  -0x0bcbe352L) +
    mad_f_mul(X[8],  -0x0e313245L) +
    mad_f_mul(X[9],   0x07635284L) +
    mad_f_mul(X[11], -0x0acf37adL) +
    mad_f_mul(X[12],  0x0f9ee890L) +
    mad_f_mul(X[14],  0x0898c779L) +
    mad_f_mul(X[15],  0x00b2aa3eL) +
    mad_f_mul(X[17],  0x0f426cb5L);
  x[12] = -x[5];

  x[0] = t2 +
    mad_f_mul(X[0],   0x0acf37adL) +
    mad_f_mul(X[2],  -0x0898c779L) +
    mad_f_mul(X[3],   0x0e313245L) +
    mad_f_mul(X[5],  -0x0f426cb5L) +
    mad_f_mul(X[6],  -0x03768962L) +
    mad_f_mul(X[8],   0x00b2aa3eL) +
    mad_f_mul(X[9],  -0x0ffc19fdL) +
    mad_f_mul(X[11],  0x0f9ee890L) +
    mad_f_mul(X[12], -0x04cfb0e2L) +
    mad_f_mul(X[14],  0x07635284L) +
    mad_f_mul(X[15],  0x0d7e8807L) +
    mad_f_mul(X[17], -0x0bcbe352L);
  x[17] = -x[0];

  x[24] = x[29] = t2 +
    mad_f_mul(X[0],  -0x0f9ee890L) +
    mad_f_mul(X[2],  -0x07635284L) +
    mad_f_mul(X[3],  -0x00b2aa3eL) +
    mad_f_mul(X[5],   0x0bcbe352L) +
    mad_f_mul(X[6],   0x0f426cb5L) +
    mad_f_mul(X[8],   0x0d7e8807L) +
    mad_f_mul(X[9],   0x0898c779L) +
    mad_f_mul(X[11], -0x04cfb0e2L) +
    mad_f_mul(X[12], -0x0acf37adL) +
    mad_f_mul(X[14], -0x0ffc19fdL) +
    mad_f_mul(X[15], -0x0e313245L) +
    mad_f_mul(X[17], -0x03768962L);

  t3 = t9 +
    mad_f_mul(X[1],  -0x0216a2a2L) +
    mad_f_mul(X[7],  -0x09bd7ca0L) +
    mad_f_mul(X[10],  0x0cb19346L) +
    mad_f_mul(X[16],  0x0fdcf549L);

  x[8] = t3 +
    mad_f_mul(X[0],   0x00b2aa3eL) +
    mad_f_mul(X[2],   0x03768962L) +
    mad_f_mul(X[3],  -0x04cfb0e2L) +
    mad_f_mul(X[5],  -0x07635284L) +
    mad_f_mul(X[6],   0x0898c779L) +
    mad_f_mul(X[8],   0x0acf37adL) +
    mad_f_mul(X[9],  -0x0bcbe352L) +
    mad_f_mul(X[11], -0x0d7e8807L) +
    mad_f_mul(X[12],  0x0e313245L) +
    mad_f_mul(X[14],  0x0f426cb5L) +
    mad_f_mul(X[15], -0x0f9ee890L) +
    mad_f_mul(X[17], -0x0ffc19fdL);
  x[9] = -x[8];

  x[21] = x[32] = t3 +
    mad_f_mul(X[0],  -0x0e313245L) +
    mad_f_mul(X[2],   0x0bcbe352L) +
    mad_f_mul(X[3],   0x0f9ee890L) +
    mad_f_mul(X[5],  -0x0898c779L) +
    mad_f_mul(X[6],  -0x0ffc19fdL) +
    mad_f_mul(X[8],   0x04cfb0e2L) +
    mad_f_mul(X[9],   0x0f426cb5L) +
    mad_f_mul(X[11], -0x00b2aa3eL) +
    mad_f_mul(X[12], -0x0d7e8807L) +
    mad_f_mul(X[14], -0x03768962L) +
    mad_f_mul(X[15],  0x0acf37adL) +
    mad_f_mul(X[17],  0x07635284L);

  x[20] = x[33] = -t3 +
    mad_f_mul(X[0],  -0x0d7e8807L) +
    mad_f_mul(X[2],   0x0f426cb5L) +
    mad_f_mul(X[3],   0x0acf37adL) +
    mad_f_mul(X[5],  -0x0ffc19fdL) +
    mad_f_mul(X[6],  -0x07635284L) +
    mad_f_mul(X[8],   0x0f9ee890L) +
    mad_f_mul(X[9],   0x03768962L) +
    mad_f_mul(X[11], -0x0e313245L) +
    mad_f_mul(X[12],  0x00b2aa3eL) +
    mad_f_mul(X[14],  0x0bcbe352L) +
    mad_f_mul(X[15], -0x04cfb0e2L) +
    mad_f_mul(X[17], -0x0898c779L);

  t4 = -t9 +
    mad_f_mul(t16, -0x0ec835e8L) +
    mad_f_mul(t17,  0x061f78aaL);

  x[4] = t4 +
    mad_f_mul(t14, 0x061f78aaL) +
    mad_f_mul(t15, 0x0ec835e8L);
  x[13] = -x[4];

  x[1] = t4 +
    mad_f_mul(t10,  0x09bd7ca0L) +
    mad_f_mul(t11, -0x0216a2a2L) +
    mad_f_mul(t12,  0x0fdcf549L) +
    mad_f_mul(t13, -0x0cb19346L);
  x[16] = -x[1];

  x[25] = x[28] = t4 +
    mad_f_mul(t10, -0x0fdcf549L) +
    mad_f_mul(t11, -0x0cb19346L) +
    mad_f_mul(t12, -0x09bd7ca0L) +
    mad_f_mul(t13, -0x0216a2a2L);

  t5 = -t6 +
    mad_f_mul(X[1],  -0x0fdcf549L) +
    mad_f_mul(X[7],  -0x0cb19346L) +
    mad_f_mul(X[10], -0x09bd7ca0L) +
    mad_f_mul(X[16], -0x0216a2a2L);

  x[2] = t5 +
    mad_f_mul(X[0],   0x0898c779L) +
    mad_f_mul(X[2],   0x04cfb0e2L) +
    mad_f_mul(X[3],   0x0bcbe352L) +
    mad_f_mul(X[5],   0x00b2aa3eL) +
    mad_f_mul(X[6],   0x0e313245L) +
    mad_f_mul(X[8],  -0x03768962L) +
    mad_f_mul(X[9],   0x0f9ee890L) +
    mad_f_mul(X[11], -0x07635284L) +
    mad_f_mul(X[12],  0x0ffc19fdL) +
    mad_f_mul(X[14], -0x0acf37adL) +
    mad_f_mul(X[15],  0x0f426cb5L) +
    mad_f_mul(X[17], -0x0d7e8807L);
  x[15] = -x[2];

  x[3] = t5 +
    mad_f_mul(X[0],   0x07635284L) +
    mad_f_mul(X[2],   0x0acf37adL) +
    mad_f_mul(X[3],   0x03768962L) +
    mad_f_mul(X[5],   0x0d7e8807L) +
    mad_f_mul(X[6],  -0x00b2aa3eL) +
    mad_f_mul(X[8],   0x0f426cb5L) +
    mad_f_mul(X[9],  -0x04cfb0e2L) +
    mad_f_mul(X[11],  0x0ffc19fdL) +
    mad_f_mul(X[12], -0x0898c779L) +
    mad_f_mul(X[14],  0x0f9ee890L) +
    mad_f_mul(X[15], -0x0bcbe352L) +
    mad_f_mul(X[17],  0x0e313245L);
  x[14] = -x[3];

  x[26] = x[27] = t5 +
    mad_f_mul(X[0],  -0x0ffc19fdL) +
    mad_f_mul(X[2],  -0x0f9ee890L) +
    mad_f_mul(X[3],  -0x0f426cb5L) +
    mad_f_mul(X[5],  -0x0e313245L) +
    mad_f_mul(X[6],  -0x0d7e8807L) +
    mad_f_mul(X[8],  -0x0bcbe352L) +
    mad_f_mul(X[9],  -0x0acf37adL) +
    mad_f_mul(X[11], -0x0898c779L) +
    mad_f_mul(X[12], -0x07635284L) +
    mad_f_mul(X[14], -0x04cfb0e2L) +
    mad_f_mul(X[15], -0x03768962L) +
    mad_f_mul(X[17], -0x00b2aa3eL);
}

/*
 * NAME:	III_imdct_l()
 * DESCRIPTION:	perform IMDCT and windowing for long blocks
 */
static
void III_imdct_l(mad_fixed_t const X[18], mad_fixed_t z[36],
		 unsigned int block_type)
{
  unsigned int i;

  /* IMDCT */

  imdct36(X, z);

  /* windowing */

  switch (block_type) {
  case 0:  /* normal window */
    for (i =  0; i < 36; ++i) z[i] = mad_f_mul(z[i], window_l[i]);
    break;

  case 1:  /* start block */
    for (i =  0; i < 18; ++i) z[i] = mad_f_mul(z[i], window_l[i]);
    /*  (i = 18; i < 24; ++i) z[i] unchanged */
    for (i = 24; i < 30; ++i) z[i] = mad_f_mul(z[i], window_s[i - 18]);
    for (i = 30; i < 36; ++i) z[i] = 0;
    break;

  case 3:  /* stop block */
    for (i =  0; i <  6; ++i) z[i] = 0;
    for (i =  6; i < 12; ++i) z[i] = mad_f_mul(z[i], window_s[i - 6]);
    /*  (i = 12; i < 18; ++i) z[i] unchanged */
    for (i = 18; i < 36; ++i) z[i] = mad_f_mul(z[i], window_l[i]);
    break;
  }
}
# endif  /* ASO_IMDCT */

/*
 * NAME:	III_imdct_s()
 * DESCRIPTION:	perform IMDCT and windowing for short blocks
 */
static
void III_imdct_s(mad_fixed_t const X[18], mad_fixed_t z[36])
{
  unsigned int w, i, k;
  mad_fixed_t y[3][12];

  /* IMDCT and windowing */

  for (w = 0; w < 3; ++w) {
    for (i = 0; i < 12; ++i) {
      register mad_fixed_t sum;

      sum = 0;

      for (k = 0; k < 6; ++k)
	sum += mad_f_mul(X[6 * w + k], imdct_s[i][k]);

      y[w][i] = mad_f_mul(sum, window_s[i]);
    }
  }

  /* overlapping and concatenation */

  for (i =  0; i <  6; ++i) z[i] = 0;
  for (i =  6; i < 12; ++i) z[i] = y[0][i - 6];
  for (i = 12; i < 18; ++i) z[i] = y[0][i - 6] + y[1][i - 12];
  for (i = 18; i < 24; ++i) z[i] =               y[1][i - 12] + y[2][i - 18];
  for (i = 24; i < 30; ++i) z[i] =                              y[2][i - 18];
  for (i = 30; i < 36; ++i) z[i] = 0;
}

/*
 * NAME:	III_overlap()
 * DESCRIPTION:	perform overlap-add of windowed IMDCT outputs
 */
static
void III_overlap(mad_fixed_t const output[36], mad_fixed_t overlap[18],
		 mad_fixed_t sample[18][32], unsigned int sb)
{
  unsigned int i;

  for (i = 0; i < 18; ++i) {
    sample[i][sb] = output[i] + overlap[i];
    overlap[i]    = output[i + 18];
  }
}

/*
 * NAME:	III_overlap_z()
 * DESCRIPTION:	perform "overlap-add" of zero IMDCT outputs
 */
static inline
void III_overlap_z(mad_fixed_t overlap[18],
		   mad_fixed_t sample[18][32], unsigned int sb)
{
  unsigned int i;

  for (i = 0; i < 18; ++i) {
    sample[i][sb] = overlap[i];
    overlap[i]    = 0;
  }
}

/*
 * NAME:	III_freqinver()
 * DESCRIPTION:	perform subband frequency inversion for odd sample lines
 */
static
void III_freqinver(mad_fixed_t sample[18][32], unsigned int sb)
{
  unsigned int i;

  for (i = 1; i < 18; i += 2)
    sample[i][sb] = -sample[i][sb];
}

/*
 * NAME:	III_decode()
 * DESCRIPTION:	decode frame main_data
 */
static
int III_decode(struct mad_bitptr *ptr, struct mad_frame *frame,
		struct sideinfo *si, unsigned int nch)
{
  unsigned int sfreqi, ngr, gr, sfbcut;

  /* 48000 => 0, 44100 => 1, 32000 => 2,
     24000 => 3, 22050 => 4, 16000 => 5 */
  sfreqi = ((frame->sfreq >>  7) & 0x000f) +
           ((frame->sfreq >> 15) & 0x0001) - 8;

  /* scalefactors, Huffman decoding, requantization */

  if (frame->flags & MAD_FLAG_LSF_EXT)
    ngr = 1, sfbcut = 6;
  else
    ngr = 2, sfbcut = 8;

  for (gr = 0; gr < ngr; ++gr) {
    struct granule *granule = &si->gr[gr];
    static mad_fixed_t xr[2][576];
    unsigned int ch;
    int error;

    for (ch = 0; ch < nch; ++ch) {
      struct channel *channel = &granule->ch[ch];
      static signed short is[576];
      unsigned int part2_length, sfb, i, f, w;

      if (frame->flags & MAD_FLAG_LSF_EXT)
	part2_length = III_lsf_scalefactors(ptr, frame->mode_ext, ch, channel);
      else {
	part2_length = III_scalefactors(ptr, channel, &si->gr[0].ch[ch],
					gr == 0 ? 0 : si->scfsi[ch]);
      }

      error = III_huffdecode(ptr, is, channel, sfreqi, part2_length);
      if (error)
	return error;

      i = 0;

      /* subbands 0-1 */

      if (channel->block_type != 2 || (channel->flags & mixed_block_flag)) {
	/* long blocks sfb 0-7 (0-5 LSF) */
	for (sfb = 0; sfb < sfbcut; ++sfb) {
	  for (f = sfwidth_l[sfreqi][sfb]; f--; ++i)
	    xr[ch][i] = is[i] ? III_requantize_l(is[i], sfb, channel) : 0;
	}
      }
      else {
	/* short blocks sfb 0-2 */
	for (sfb = 0; sfb < 3; ++sfb) {
	  for (w = 0; w < 3; ++w) {
	    for (f = sfwidth_s[sfreqi][sfb]; f--; ++i)
	      xr[ch][i] = is[i] ? III_requantize_s(is[i], sfb, w, channel) : 0;
	  }
	}
      }

      /* subbands 2-31 */

      if (channel->block_type != 2) {
	/* long blocks sfb 8-21 (6-21 LSF) */
	for (sfb = sfbcut; sfb < 22; ++sfb) {
	  for (f = sfwidth_l[sfreqi][sfb]; f--; ++i)
	    xr[ch][i] = is[i] ? III_requantize_l(is[i], sfb, channel) : 0;
	}
      }
      else {  /* channel->block_type == 2 */
	/* short blocks sfb 3-12 */
	for (sfb = 3; sfb < 13; ++sfb) {
	  for (w = 0; w < 3; ++w) {
	    for (f = sfwidth_s[sfreqi][sfb]; f--; ++i)
	      xr[ch][i] = is[i] ? III_requantize_s(is[i], sfb, w, channel) : 0;
	  }
	}
      }
    }

    /* joint stereo processing */

    if (frame->mode == MAD_MODE_JOINT_STEREO) {
      error = III_stereo(xr, granule, frame, sfreqi);
      if (error)
	return error;
    }

    /* reordering, alias reduction, IMDCT, overlap-add, frequency inversion */

    for (ch = 0; ch < nch; ++ch) {
      struct channel const *channel = &granule->ch[ch];
      mad_fixed_t (*sample)[32] = &frame->sbsample[ch][18 * gr];
      unsigned int sb, l, i, sblimit;
      mad_fixed_t output[36];

      if (channel->block_type == 2)
	III_reorder(xr[ch], channel, sfwidth_s[sfreqi]);
      else
	III_aliasreduce(xr[ch]);

      l = 0;

      /* subbands 0-1 */

      if (channel->block_type != 2 || (channel->flags & mixed_block_flag)) {
	/* long blocks */
	for (sb = 0; sb < 2; ++sb, l += 18) {
	  III_imdct_l(&xr[ch][l], output, channel->block_type);
	  III_overlap(output, (*frame->overlap)[ch][sb], sample, sb);
	}
      }
      else {
	/* short blocks */
	for (sb = 0; sb < 2; ++sb, l += 18) {
	  III_imdct_s(&xr[ch][l], output);
	  III_overlap(output, (*frame->overlap)[ch][sb], sample, sb);
	}
      }

      III_freqinver(sample, 1);

      /* (nonzero) subbands 2-31 */

      i = 576;
      while (i > 36 && xr[ch][i - 1] == 0)
	--i;

      sblimit = 32 - (576 - i) / 18;

      if (channel->block_type != 2) {
	/* long blocks */
	for (sb = 2; sb < sblimit; ++sb, l += 18) {
	  III_imdct_l(&xr[ch][l], output, channel->block_type);
	  III_overlap(output, (*frame->overlap)[ch][sb], sample, sb);

	  if (sb & 1)
	    III_freqinver(sample, sb);
	}
      }
      else {
	/* short blocks */
	for (sb = 2; sb < sblimit; ++sb, l += 18) {
	  III_imdct_s(&xr[ch][l], output);
	  III_overlap(output, (*frame->overlap)[ch][sb], sample, sb);

	  if (sb & 1)
	    III_freqinver(sample, sb);
	}
      }

      /* (zero) remaining subbands */

      for (sb = sblimit; sb < 32; ++sb) {
	III_overlap_z((*frame->overlap)[ch][sb], sample, sb);

	if (sb & 1)
	  III_freqinver(sample, sb);
      }
    }
  }

  return 0;
}

/*
 * NAME:	layer->III()
 * DESCRIPTION:	decode a single Layer III frame
 */
int mad_layer_III(struct mad_stream *stream, struct mad_frame *frame,
		  unsigned short const crc[2])
{
  unsigned int nch, private_bitlen;
  unsigned int main_data_bitlen, main_data_length, sideinfo_length;
  unsigned int frame_space, frame_used, frame_free;
  struct mad_bitptr ptr;
  struct sideinfo si;
  int error, result = 0;

  /* allocate Layer III dynamic structures */

  if (stream->main_data == 0) {
    stream->main_data = malloc(1935);
    if (stream->main_data == 0) {
      stream->error = MAD_ERR_NOMEM;
      return -1;
    }
  }

  if (frame->overlap == 0) {
    frame->overlap = calloc(2 * 32 * 18, sizeof(mad_fixed_t));
    if (frame->overlap == 0) {
      stream->error = MAD_ERR_NOMEM;
      return -1;
    }
  }

  nch = MAD_NCHANNELS(frame);
  sideinfo_length = (frame->flags & MAD_FLAG_LSF_EXT) ?
    (nch == 1 ? 9 : 17) : (nch == 1 ? 17 : 32);

  /* check frame sanity */

  if (stream->next_frame - mad_bit_nextbyte(&stream->ptr) <
      (signed int) sideinfo_length) {
    stream->error = MAD_ERR_BADFRAMELEN;
    stream->md_len = 0;
    return -1;
  }

  /* check CRC word */

  if ((frame->flags & MAD_FLAG_PROTECTION) &&
      mad_bit_crc(stream->ptr, sideinfo_length * CHAR_BIT, crc[0]) != crc[1]) {
    stream->error = MAD_ERR_BADCRC;
    result = -1;
  }

  /* decode frame side information */

  error = III_sideinfo(&stream->ptr, nch, frame->flags & MAD_FLAG_LSF_EXT,
		       &si, &main_data_bitlen, &private_bitlen);
  if (error && result == 0) {
    stream->error = error;
    result = -1;
  }

  frame->flags   |= private_bitlen;
  frame->private |= si.private_bits;

  /* find main_data */

  frame_space      = stream->next_frame - mad_bit_nextbyte(&stream->ptr);
  main_data_length = (main_data_bitlen + (CHAR_BIT - 1)) / CHAR_BIT;

  if (si.main_data_begin == 0) {
    ptr = stream->ptr;
    stream->md_len = 0;

    frame_used = main_data_length;
  }
  else {
    if (si.main_data_begin > stream->md_len) {
      if (result == 0) {
	stream->error = MAD_ERR_BADDATAPTR;
	result = -1;
      }

      frame_used = 0;
    }
    else {
      mad_bit_init(&ptr,
		   &(*stream->main_data)[stream->md_len] - si.main_data_begin);

      if (main_data_length > si.main_data_begin) {
	if (main_data_length - si.main_data_begin > frame_space) {
	  if (result == 0) {
	    stream->error = MAD_ERR_BADDATALEN;
	    result = -1;
	  }

	  frame_used = 0;
	}
	else {
	  memcpy(&(*stream->main_data)[stream->md_len],
		 mad_bit_nextbyte(&stream->ptr),
		 main_data_length - si.main_data_begin);
	  stream->md_len += main_data_length - si.main_data_begin;

	  frame_used = main_data_length - si.main_data_begin;
	}
      }
      else
	frame_used = 0;
    }
  }

  frame_free = frame_space - frame_used;

  /* improve free bitrate discovery */
  if (frame->bitrate == 0 && frame_used)
    mad_bit_skip(&stream->ptr, frame_used * CHAR_BIT);

# if 0 && defined(DEBUG)
  fprintf(stderr, "main_data_begin:%u, main_data_length:%u, "
	  "frame_space:%u, md_len:%u      \n",
	  si.main_data_begin, main_data_length, frame_space, stream->md_len);
# endif

  /* decode main_data */

  if (result == 0) {
    error = III_decode(&ptr, frame, &si, nch);
    if (error) {
      stream->error = error;
      result = -1;
    }
  }

  /* preload main_data buffer with up to 511 bytes for next frame(s) */

  if (frame_free >= 511) {
    memcpy(*stream->main_data, stream->next_frame - 511, 511);
    stream->md_len = 511;
  }
  else {
    if (main_data_length < si.main_data_begin) {
      unsigned int extra;

      extra = si.main_data_begin - main_data_length;
      if (extra + frame_free > 511)
	extra = 511 - frame_free;

      memmove(*stream->main_data,
	      *stream->main_data + stream->md_len - extra,
	      extra);

      stream->md_len = extra;
    }
    else
      stream->md_len = 0;

    memcpy(*stream->main_data + stream->md_len,
	   stream->next_frame - frame_free, frame_free);
    stream->md_len += frame_free;
  }

  /* designate ancillary bits */
  /* FIXME */
  stream->anc_bitlen = 0;

  return result;
}
