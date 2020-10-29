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
 * $Id: synth.c,v 1.2 2000/03/05 07:31:55 rob Exp $
 */

# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif

# include "fixed.h"
# include "frame.h"
# include "synth.h"

# if !defined(DCT_FAST)
static
fixed_t const N[31][32] = {
# include "N.dat"
};
# endif

static
fixed_t const D[32][16] = {
# include "D.dat"
};

void mad_synth_init(struct mad_synth *synth)
{
  unsigned int i;

  for (i = 0; i < 256; ++i) {
    synth->filterout[0][0][i] = synth->filterout[0][1][i] =
    synth->filterout[1][0][i] = synth->filterout[1][1][i] = 0;
  }

  synth->slot = 0;
}

# if defined(DCT_FAST)
static
void dct32(fixed_t const in[32], fixed_t lo[16], fixed_t hi[16])
{
  fixed_t t0,   t1,   t2,   t3,   t4,   t5,   t6,   t7;
  fixed_t t8,   t9,   t10,  t11,  t12,  t13,  t14,  t15;
  fixed_t t16,  t17,  t18,  t19,  t20,  t21,  t22,  t23;
  fixed_t t24,  t25,  t26,  t27,  t28,  t29,  t30,  t31;
  fixed_t t32,  t33,  t34,  t35,  t36,  t37,  t38,  t39;
  fixed_t t40,  t41,  t42,  t43,  t44,  t45,  t46,  t47;
  fixed_t t48,  t49,  t50,  t51,  t52,  t53,  t54,  t55;
  fixed_t t56,  t57,  t58,  t59,  t60,  t61,  t62,  t63;
  fixed_t t64,  t65,  t66,  t67,  t68,  t69,  t70,  t71;
  fixed_t t72,  t73,  t74,  t75,  t76,  t77,  t78,  t79;
  fixed_t t80,  t81,  t82,  t83,  t84,  t85,  t86,  t87;
  fixed_t t88,  t89,  t90,  t91,  t92,  t93,  t94,  t95;
  fixed_t t96,  t97,  t98,  t99,  t100, t101, t102, t103;
  fixed_t t104, t105, t106, t107, t108, t109, t110, t111;
  fixed_t t112, t113, t114, t115, t116, t117, t118, t119;
  fixed_t t120, t121, t122, t123, t124, t125, t126, t127;
  fixed_t t128, t129, t130, t131, t132, t133, t134, t135;
  fixed_t t136, t137, t138, t139, t140, t141, t142, t143;
  fixed_t t144, t145, t146, t147, t148, t149, t150, t151;
  fixed_t t152, t153, t154, t155, t156, t157, t158, t159;
  fixed_t t160, t161, t162, t163, t164, t165, t166, t167;
  fixed_t t168, t169, t170, t171, t172, t173, t174, t175;
  fixed_t t176;

  /* costab[i] = cos(PI / (2 * 32) * i) */

  enum {
    costab1  = 0x0ffb10f2L,  /* 0.998795456 */
    costab2  = 0x0fec46d2L,  /* 0.995184727 */
    costab3  = 0x0fd3aac0L,  /* 0.989176510 */
    costab4  = 0x0fb14be8L,  /* 0.980785280 */
    costab5  = 0x0f853f7eL,  /* 0.970031253 */
    costab6  = 0x0f4fa0abL,  /* 0.956940336 */
    costab7  = 0x0f109082L,  /* 0.941544065 */
    costab8  = 0x0ec835e8L,  /* 0.923879533 */
    costab9  = 0x0e76bd7aL,  /* 0.903989293 */
    costab10 = 0x0e1c5979L,  /* 0.881921264 */
    costab11 = 0x0db941a3L,  /* 0.857728610 */
    costab12 = 0x0d4db315L,  /* 0.831469612 */
    costab13 = 0x0cd9f024L,  /* 0.803207531 */
    costab14 = 0x0c5e4036L,  /* 0.773010453 */
    costab15 = 0x0bdaef91L,  /* 0.740951125 */
    costab16 = 0x0b504f33L,  /* 0.707106781 */
    costab17 = 0x0abeb49aL,  /* 0.671558955 */
    costab18 = 0x0a267993L,  /* 0.634393284 */
    costab19 = 0x0987fbfeL,  /* 0.595699304 */
    costab20 = 0x08e39d9dL,  /* 0.555570233 */
    costab21 = 0x0839c3cdL,  /* 0.514102744 */
    costab22 = 0x078ad74eL,  /* 0.471396737 */
    costab23 = 0x06d74402L,  /* 0.427555093 */
    costab24 = 0x061f78aaL,  /* 0.382683432 */
    costab25 = 0x0563e69dL,  /* 0.336889853 */
    costab26 = 0x04a5018cL,  /* 0.290284677 */
    costab27 = 0x03e33f2fL,  /* 0.242980180 */
    costab28 = 0x031f1708L,  /* 0.195090322 */
    costab29 = 0x0259020eL,  /* 0.146730474 */
    costab30 = 0x01917a6cL,  /* 0.098017140 */
    costab31 = 0x00c8fb30L   /* 0.049067674 */
  };

  t0   = in[0]  + in[31];  t16  = f_mul(in[0]  - in[31], costab1);
  t1   = in[15] + in[16];  t17  = f_mul(in[15] - in[16], costab31);

  t41  = t16 + t17;
  t59  = f_mul(t16 - t17, costab2);
  t33  = t0  + t1;
  t50  = f_mul(t0  - t1,  costab2);

  t2   = in[7]  + in[24];  t18  = f_mul(in[7]  - in[24], costab15);
  t3   = in[8]  + in[23];  t19  = f_mul(in[8]  - in[23], costab17);

  t42  = t18 + t19;
  t60  = f_mul(t18 - t19, costab30);
  t34  = t2  + t3;
  t51  = f_mul(t2  - t3,  costab30);

  t4   = in[3]  + in[28];  t20  = f_mul(in[3]  - in[28], costab7);
  t5   = in[12] + in[19];  t21  = f_mul(in[12] - in[19], costab25);

  t43  = t20 + t21;
  t61  = f_mul(t20 - t21, costab14);
  t35  = t4  + t5;
  t52  = f_mul(t4  - t5,  costab14);

  t6   = in[4]  + in[27];  t22  = f_mul(in[4]  - in[27], costab9);
  t7   = in[11] + in[20];  t23  = f_mul(in[11] - in[20], costab23);

  t44  = t22 + t23;
  t62  = f_mul(t22 - t23, costab18);
  t36  = t6  + t7;
  t53  = f_mul(t6  - t7,  costab18);

  t8   = in[1]  + in[30];  t24  = f_mul(in[1]  - in[30], costab3);
  t9   = in[14] + in[17];  t25  = f_mul(in[14] - in[17], costab29);

  t45  = t24 + t25;
  t63  = f_mul(t24 - t25, costab6);
  t37  = t8  + t9;
  t54  = f_mul(t8  - t9,  costab6);

  t10  = in[6]  + in[25];  t26  = f_mul(in[6]  - in[25], costab13);
  t11  = in[9]  + in[22];  t27  = f_mul(in[9]  - in[22], costab19);

  t46  = t26 + t27;
  t64  = f_mul(t26 - t27, costab26);
  t38  = t10 + t11;
  t55  = f_mul(t10 - t11, costab26);

  t12  = in[2]  + in[29];  t28  = f_mul(in[2]  - in[29], costab5);
  t13  = in[13] + in[18];  t29  = f_mul(in[13] - in[18], costab27);

  t47  = t28 + t29;
  t65  = f_mul(t28 - t29, costab10);
  t39  = t12 + t13;
  t56  = f_mul(t12 - t13, costab10);

  t14  = in[5]  + in[26];  t30  = f_mul(in[5]  - in[26], costab11);
  t15  = in[10] + in[21];  t31  = f_mul(in[10] - in[21], costab21);

  t48  = t30 + t31;
  t66  = f_mul(t30 - t31, costab22);
  t40  = t14 + t15;
  t57  = f_mul(t14 - t15, costab22);

  t69  = t33 + t34;  t89  = f_mul(t33 - t34, costab4);
  t70  = t35 + t36;  t90  = f_mul(t35 - t36, costab28);
  t71  = t37 + t38;  t91  = f_mul(t37 - t38, costab12);
  t72  = t39 + t40;  t92  = f_mul(t39 - t40, costab20);
  t73  = t41 + t42;  t94  = f_mul(t41 - t42, costab4);
  t74  = t43 + t44;  t95  = f_mul(t43 - t44, costab28);
  t75  = t45 + t46;  t96  = f_mul(t45 - t46, costab12);
  t76  = t47 + t48;  t97  = f_mul(t47 - t48, costab20);

  t78  = t50 + t51;  t100 = f_mul(t50 - t51, costab4);
  t79  = t52 + t53;  t101 = f_mul(t52 - t53, costab28);
  t80  = t54 + t55;  t102 = f_mul(t54 - t55, costab12);
  t81  = t56 + t57;  t103 = f_mul(t56 - t57, costab20);

  t83  = t59 + t60;  t106 = f_mul(t59 - t60, costab4);
  t84  = t61 + t62;  t107 = f_mul(t61 - t62, costab28);
  t85  = t63 + t64;  t108 = f_mul(t63 - t64, costab12);
  t86  = t65 + t66;  t109 = f_mul(t65 - t66, costab20);

  t113 = t69  + t70;
  t114 = t71  + t72;
  t115 = t73  + t74;
  t116 = t75  + t76;

  t32  = t115 + t116;

  t118 = t78  + t79;
  t119 = t80  + t81;

  t58  = t118 + t119;

  t121 = t83  + t84;
  t122 = t85  + t86;

  t67  = t121 + t122;

  t49  = (t67 << 1) - t32;

  t125 = t89  + t90;
  t126 = t91  + t92;

  t93  = t125 + t126;

  t128 = t94  + t95;
  t129 = t96  + t97;

  t98  = t128 + t129;

  t68  = (t98 << 1) - t49;

  t132 = t100 + t101;
  t133 = t102 + t103;

  t104 = t132 + t133;

  t82  = (t104 << 1) - t58;

  t136 = t106 + t107;
  t137 = t108 + t109;

  t110 = t136 + t137;

  t87  = (t110 << 1) - t67;

  t77  = (t87 << 1) - t68;

  t141 = f_mul(t69 - t70, costab8);
  t142 = f_mul(t71 - t72, costab24);
  t143 = t141 + t142;
  t144 = f_mul(t73 - t74, costab8);
  t145 = f_mul(t75 - t76, costab24);
  t146 = t144 + t145;

  t88  = (t146 << 1) - t77;

  t148 = f_mul(t78 - t79, costab8);
  t149 = f_mul(t80 - t81, costab24);
  t150 = t148 + t149;

  t105 = (t150 << 1) - t82;

  t152 = f_mul(t83 - t84, costab8);
  t153 = f_mul(t85 - t86, costab24);
  t154 = t152 + t153;

  t111 = (t154 << 1) - t87;

  t99  = (t111 << 1) - t88;

  t157 = f_mul(t89 - t90, costab8);
  t158 = f_mul(t91 - t92, costab24);
  t159 = t157 + t158;

  t127 = (t159 << 1) - t93;

  t160 = (f_mul(t125 - t126, costab16) << 1) - t127;
  t161 = f_mul(t94 - t95, costab8);
  t162 = f_mul(t96 - t97, costab24);
  t163 = t161 + t162;

  t130 = (t163 << 1) - t98;

  t112 = (t130 << 1) - t99;

  t164 = (f_mul(t128 - t129, costab16) << 1) - t130;

  t166 = f_mul(t100 - t101, costab8);
  t167 = f_mul(t102 - t103, costab24);
  t168 = t166 + t167;

  t134 = (t168 << 1) - t104;

  t120 = (t134 << 1) - t105;

  t135 = (f_mul(t118 - t119, costab16) << 1) - t120;

  t169 = (f_mul(t132 - t133, costab16) << 1) - t134;

  t151 = (t169 << 1) - t135;

  t170 = (((f_mul(t148 - t149, costab16) << 1) - t150) << 1) - t151;
  t171 = f_mul(t106 - t107, costab8);
  t172 = f_mul(t108 - t109, costab24);
  t173 = t171 + t172;

  t138 = (t173 << 1) - t110;

  t123 = (t138 << 1) - t111;

  t139 = (f_mul(t121 - t122, costab16) << 1) - t123;

  t117 = (t123 << 1) - t112;

  t124 = (f_mul(t115 - t116, costab16) << 1) - t117;

  t131 = (t139 << 1) - t124;

  t140 = (t164 << 1) - t131;

  t174 = (f_mul(t136 - t137, costab16) << 1) - t138;

  t155 = (t174 << 1) - t139;

  t147 = (t155 << 1) - t140;

  t156 = (((f_mul(t144 - t145, costab16) << 1) - t146) << 1) - t147;

  t175 = (((f_mul(t152 - t153, costab16) << 1) - t154) << 1) - t155;

  t165 = (t175 << 1) - t156;

  t176 = (((((f_mul(t161 - t162, costab16) << 1) -
	     t163) << 1) - t164) << 1) - t165;

  /* the output order is convoluted to simplify later processing */

  /*  0 */ hi[15] = t113 + t114;
  /*  1 */ hi[14] = t32;
  /*  2 */ hi[13] = t58;
  /*  3 */ hi[12] = t49;
  /*  4 */ hi[11] = t93;
  /*  5 */ hi[10] = t68;
  /*  6 */ hi[9]  = t82;
  /*  7 */ hi[8]  = t77;
  /*  8 */ hi[7]  = t143;
  /*  9 */ hi[6]  = t88;
  /* 10 */ hi[5]  = t105;
  /* 11 */ hi[4]  = t99;
  /* 12 */ hi[3]  = t127;
  /* 13 */ hi[2]  = t112;
  /* 14 */ hi[1]  = t120;
  /* 15 */ hi[0]  = t117;

  /* 16 */ lo[0]  = f_mul(t113 - t114, costab16);
  /* 17 */ lo[1]  = t124;
  /* 18 */ lo[2]  = t135;
  /* 19 */ lo[3]  = t131;
  /* 20 */ lo[4]  = t160;
  /* 21 */ lo[5]  = t140;
  /* 22 */ lo[6]  = t151;
  /* 23 */ lo[7]  = t147;
  /* 24 */ lo[8]  = (f_mul(t141 - t142, costab16) << 1) - t143;
  /* 25 */ lo[9]  = t156;
  /* 26 */ lo[10] = t170;
  /* 27 */ lo[11] = t165;
  /* 28 */ lo[12] = (((f_mul(t157 - t158, costab16) << 1) - t159) << 1) - t160;
  /* 29 */ lo[13] = t176;
  /* 30 */ lo[14] = (((((f_mul(t166 - t167, costab16) << 1) -
			t168) << 1) - t169) << 1) - t170;
  /* 31 */ lo[15] = (((((((f_mul(t171 - t172, costab16) << 1) -
			  t173) << 1) - t174) << 1) - t175) << 1) - t176;

  /*
   * Totals:
   *  80 multiplies
   *  80 additions
   * 119 subtractions
   *  49 shifts
   */
}
# endif

static
void matrixing(fixed_t lo[16], fixed_t hi[16], fixed_t const filter[32])
{
# if defined(DCT_FAST)

  dct32(filter, lo, hi);

  /*
    x[i]      =  x'[i + 16]  i = 0..15
    x[i + 17] = -x'[31 - i]  i = 0..15
    x[i + 32] = -x'[16 - i]  i = 0..15
    x[i + 48] = -x'[i]       i = 0..15
    x[16]     = 0
   */

# else

  /* generic matrix multiply */
  /* currently broken */

  register unsigned int i, k;
  register fixed_t *Vptr;
  register fixed_t const *fptr, *Nptr, *Vrefl;
  register fixed_t sum;

  Vptr = lo;
  Nptr = N[0];

  /* 0 <= i <= 15 */
  for (i = 0; i <= 15; ++i) {
    fptr = filter;

    sum = 0;
    for (k = 0; k < 32; ++k)
      sum += f_mul(*Nptr++, *fptr++);

    *Vptr++ = sum;
  }

# if 0
  /* i = 16 */
  ++Vptr;  /* = 0 */

  /* 17 <= i <= 32 */
  Vrefl = &V[15];
  for (i = 17; i <= 32; ++i)
    *Vptr++ = -*Vrefl--;
# endif

  Vptr = hi;

  /* 33 <= i <= 47 */
  for (i = 33; i <= 47; ++i) {
    fptr = filter;

    sum = 0;
    for (k = 0; k < 32; ++k)
      sum -= f_mul(*Nptr++, *fptr++);

    *Vptr++ = sum;
  }

  /* i = 48 */
  fptr = filter;
  sum = 0;
  for (k = 0; k < 32; ++k)
    sum += *fptr++;

  *Vptr++ = sum;

# if 0
  /* 49 <= i <= 63 */
  Vrefl = &V[47];
  for (i = 49; i <= 63; ++i)
    *Vptr++ = *Vrefl--;
# endif

# endif
}

void mad_synthesis(struct mad_frame *frame, struct mad_synth *synth)
{
  unsigned int nch, ns, ch, s, sb;
  fixed_t *pcmptr;

  nch = MAD_NUMCHANNELS(frame);
  ns  = MAD_NUMSAMPLES(frame);

  for (s = 0; s < ns; ++s) {
    unsigned int slot;
    unsigned int even, odd;
    unsigned int even14, even12, even10, even8, even6, even4, even2;
    unsigned int odd14, odd12, odd10, odd8, odd6, odd4, odd2;

    slot = synth->slot;
    synth->slot = (slot + 1) & 0xf;

    even = slot & 0xe;
    odd  = ((slot - 1) & 0xf) | 0x1;

    even14 = (even + 14) & 0xf;
    even12 = (even + 12) & 0xf;
    even10 = (even + 10) & 0xf;
    even8  = (even + 8)  & 0xf;
    even6  = (even + 6)  & 0xf;
    even4  = (even + 4)  & 0xf;
    even2  = (even + 2)  & 0xf;

    odd14  = (odd  + 14) & 0xf;
    odd12  = (odd  + 12) & 0xf;
    odd10  = (odd  + 10) & 0xf;
    odd8   = (odd  + 8)  & 0xf;
    odd6   = (odd  + 6)  & 0xf;
    odd4   = (odd  + 4)  & 0xf;
    odd2   = (odd  + 2)  & 0xf;

    for (ch = 0; ch < nch; ++ch) {
      register fixed_t *evenp, *oddp, *even2p;
      register fixed_t const *Dptr;
      register fixed_t sum;

      evenp = even2p = synth->filterout[ch][0];
      oddp = synth->filterout[ch][1];

      matrixing(&evenp[16 * slot], &oddp[16 * slot], frame->sbsample[ch][s]);

      pcmptr = &synth->pcmout[ch][s * 32];

      if (slot & 1)
	evenp  += 16;
      else {
	oddp   += 16;
	even2p += 16;
      }

      /* calculate 32 samples */

      Dptr = D[0];

# if defined(FPM_MACC)
      {
	register fixed64hi_t hi = 0;
	register fixed64lo_t lo = 0;

	f_macc(hi, lo, evenp[0],    Dptr[even]);
	f_macc(hi, lo, evenp[32],   Dptr[even14]);
	f_macc(hi, lo, evenp[64],   Dptr[even12]);
	f_macc(hi, lo, evenp[96],   Dptr[even10]);
	f_macc(hi, lo, evenp[128],  Dptr[even8]);
	f_macc(hi, lo, evenp[160],  Dptr[even6]);
	f_macc(hi, lo, evenp[192],  Dptr[even4]);
	f_macc(hi, lo, evenp[224],  Dptr[even2]);

	sum = f_scale64(hi, lo);

	hi = 0;
	lo = 0;

	f_macc(hi, lo, even2p[0],   Dptr[odd]);
	f_macc(hi, lo, even2p[32],  Dptr[odd14]);
	f_macc(hi, lo, even2p[64],  Dptr[odd12]);
	f_macc(hi, lo, even2p[96],  Dptr[odd10]);
	f_macc(hi, lo, even2p[128], Dptr[odd8]);
	f_macc(hi, lo, even2p[160], Dptr[odd6]);
	f_macc(hi, lo, even2p[192], Dptr[odd4]);
	f_macc(hi, lo, even2p[224], Dptr[odd2]);

	sum -= f_scale64(hi, lo);
      }
# else
      sum  = f_mul(evenp[0],    Dptr[even]);
      sum += f_mul(evenp[32],   Dptr[even14]);
      sum += f_mul(evenp[64],   Dptr[even12]);
      sum += f_mul(evenp[96],   Dptr[even10]);
      sum += f_mul(evenp[128],  Dptr[even8]);
      sum += f_mul(evenp[160],  Dptr[even6]);
      sum += f_mul(evenp[192],  Dptr[even4]);
      sum += f_mul(evenp[224],  Dptr[even2]);

      sum -= f_mul(even2p[0],   Dptr[odd]);
      sum -= f_mul(even2p[32],  Dptr[odd14]);
      sum -= f_mul(even2p[64],  Dptr[odd12]);
      sum -= f_mul(even2p[96],  Dptr[odd10]);
      sum -= f_mul(even2p[128], Dptr[odd8]);
      sum -= f_mul(even2p[160], Dptr[odd6]);
      sum -= f_mul(even2p[192], Dptr[odd4]);
      sum -= f_mul(even2p[224], Dptr[odd2]);
# endif

      *pcmptr++ = sum;
      ++evenp;

      for (sb = 1; sb < 16; ++sb) {
	Dptr = D[sb];

# if defined(FPM_MACC)
	{
	  register fixed64hi_t hi = 0;
	  register fixed64lo_t lo = 0;

	  f_macc(hi, lo, evenp[0],   Dptr[even]);
	  f_macc(hi, lo, evenp[32],  Dptr[even14]);
	  f_macc(hi, lo, evenp[64],  Dptr[even12]);
	  f_macc(hi, lo, evenp[96],  Dptr[even10]);
	  f_macc(hi, lo, evenp[128], Dptr[even8]);
	  f_macc(hi, lo, evenp[160], Dptr[even6]);
	  f_macc(hi, lo, evenp[192], Dptr[even4]);
	  f_macc(hi, lo, evenp[224], Dptr[even2]);

	  sum = f_scale64(hi, lo);

	  hi = 0;
	  lo = 0;

	  f_macc(hi, lo, oddp[0],    Dptr[odd]);
	  f_macc(hi, lo, oddp[32],   Dptr[odd14]);
	  f_macc(hi, lo, oddp[64],   Dptr[odd12]);
	  f_macc(hi, lo, oddp[96],   Dptr[odd10]);
	  f_macc(hi, lo, oddp[128],  Dptr[odd8]);
	  f_macc(hi, lo, oddp[160],  Dptr[odd6]);
	  f_macc(hi, lo, oddp[192],  Dptr[odd4]);
	  f_macc(hi, lo, oddp[224],  Dptr[odd2]);

	  sum -= f_scale64(hi, lo);
	}
# else
	sum  = f_mul(evenp[0],   Dptr[even]);
	sum += f_mul(evenp[32],  Dptr[even14]);
	sum += f_mul(evenp[64],  Dptr[even12]);
	sum += f_mul(evenp[96],  Dptr[even10]);
	sum += f_mul(evenp[128], Dptr[even8]);
	sum += f_mul(evenp[160], Dptr[even6]);
	sum += f_mul(evenp[192], Dptr[even4]);
	sum += f_mul(evenp[224], Dptr[even2]);

	sum -= f_mul(oddp[0],    Dptr[odd]);
	sum -= f_mul(oddp[32],   Dptr[odd14]);
	sum -= f_mul(oddp[64],   Dptr[odd12]);
	sum -= f_mul(oddp[96],   Dptr[odd10]);
	sum -= f_mul(oddp[128],  Dptr[odd8]);
	sum -= f_mul(oddp[160],  Dptr[odd6]);
	sum -= f_mul(oddp[192],  Dptr[odd4]);
	sum -= f_mul(oddp[224],  Dptr[odd2]);
# endif

	*pcmptr++ = sum;
	++evenp, ++oddp;
      }

      /* V[16] == 0 */

      Dptr = D[16];

# if defined(FPM_MACC)
      {
	register fixed64hi_t hi = 0;
	register fixed64lo_t lo = 0;

	f_macc(hi, lo, oddp[0],   Dptr[odd]);
	f_macc(hi, lo, oddp[32],  Dptr[odd14]);
	f_macc(hi, lo, oddp[64],  Dptr[odd12]);
	f_macc(hi, lo, oddp[96],  Dptr[odd10]);
	f_macc(hi, lo, oddp[128], Dptr[odd8]);
	f_macc(hi, lo, oddp[160], Dptr[odd6]);
	f_macc(hi, lo, oddp[192], Dptr[odd4]);
	f_macc(hi, lo, oddp[224], Dptr[odd2]);

	sum = f_scale64(hi, lo);
      }
# else
      sum  = f_mul(oddp[0],   Dptr[odd]);
      sum += f_mul(oddp[32],  Dptr[odd14]);
      sum += f_mul(oddp[64],  Dptr[odd12]);
      sum += f_mul(oddp[96],  Dptr[odd10]);
      sum += f_mul(oddp[128], Dptr[odd8]);
      sum += f_mul(oddp[160], Dptr[odd6]);
      sum += f_mul(oddp[192], Dptr[odd4]);
      sum += f_mul(oddp[224], Dptr[odd2]);
# endif

      *pcmptr++ = -sum;
      --evenp, --oddp;

      for (sb = 17; sb < 32; ++sb) {
	Dptr = D[sb];

# if defined(FPM_MACC)
	{
	  register fixed64hi_t hi = 0;
	  register fixed64lo_t lo = 0;

	  f_macc(hi, lo, evenp[0],   Dptr[even]);
	  f_macc(hi, lo, evenp[32],  Dptr[even14]);
	  f_macc(hi, lo, evenp[64],  Dptr[even12]);
	  f_macc(hi, lo, evenp[96],  Dptr[even10]);
	  f_macc(hi, lo, evenp[128], Dptr[even8]);
	  f_macc(hi, lo, evenp[160], Dptr[even6]);
	  f_macc(hi, lo, evenp[192], Dptr[even4]);
	  f_macc(hi, lo, evenp[224], Dptr[even2]);

	  f_macc(hi, lo, oddp[0],    Dptr[odd]);
	  f_macc(hi, lo, oddp[32],   Dptr[odd14]);
	  f_macc(hi, lo, oddp[64],   Dptr[odd12]);
	  f_macc(hi, lo, oddp[96],   Dptr[odd10]);
	  f_macc(hi, lo, oddp[128],  Dptr[odd8]);
	  f_macc(hi, lo, oddp[160],  Dptr[odd6]);
	  f_macc(hi, lo, oddp[192],  Dptr[odd4]);
	  f_macc(hi, lo, oddp[224],  Dptr[odd2]);

	  sum = f_scale64(hi, lo);
	}
# else
	sum  = f_mul(evenp[0],   Dptr[even]);
	sum += f_mul(evenp[32],  Dptr[even14]);
	sum += f_mul(evenp[64],  Dptr[even12]);
	sum += f_mul(evenp[96],  Dptr[even10]);
	sum += f_mul(evenp[128], Dptr[even8]);
	sum += f_mul(evenp[160], Dptr[even6]);
	sum += f_mul(evenp[192], Dptr[even4]);
	sum += f_mul(evenp[224], Dptr[even2]);

	sum += f_mul(oddp[0],    Dptr[odd]);
	sum += f_mul(oddp[32],   Dptr[odd14]);
	sum += f_mul(oddp[64],   Dptr[odd12]);
	sum += f_mul(oddp[96],   Dptr[odd10]);
	sum += f_mul(oddp[128],  Dptr[odd8]);
	sum += f_mul(oddp[160],  Dptr[odd6]);
	sum += f_mul(oddp[192],  Dptr[odd4]);
	sum += f_mul(oddp[224],  Dptr[odd2]);
# endif

	*pcmptr++ = -sum;
	--evenp, --oddp;
      }
    }
  }

  synth->pcmlen = 32 * ns;
}
