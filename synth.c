/*
 * mad - MPEG audio decoder
 * Copyright (C) 2000-2001 Robert Leslie
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
 * $Id: synth.c,v 1.12 2001/01/21 00:18:15 rob Exp $
 */

# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif

# include "global.h"

# include "fixed.h"
# include "frame.h"
# include "synth.h"

/*
 * NAME:	synth->init()
 * DESCRIPTION:	initialize synth struct
 */
void mad_synth_init(struct mad_synth *synth)
{
  synth->phase = 0;
  mad_synth_mute(synth);
}

/*
 * An optional optimization called here the Subband Synthesis Optimization
 * (SSO) improves the performance of subband synthesis at the expense of
 * accuracy.
 *
 * The idea is to simplify 32x32->64-bit multiplication to 32x32->32 such
 * that extra scaling and rounding are not necessary. This often allows the
 * compiler to use faster 32-bit multiply-accumulate instructions instead of
 * explicit 64-bit multiply, shift, and add instructions.
 *
 * SSO works like this: a full 32x32->64-bit multiply of two mad_fixed_t
 * values requires the result to be right-shifted 28 bits to be properly
 * scaled to the same fixed-point format. Right shifts can be applied at any
 * time to either operand or to the result, so the optimization involves
 * careful placement of these shifts to minimize the loss of accuracy.
 *
 * First, a 14-bit shift is applied with rounding at compile-time to the D[]
 * table of coefficients for the subband synthesis window. This only loses 2
 * bits of accuracy because the lower 12 bits are always zero. A second
 * 12-bit shift occurs after the DCT calculation. This loses 12 bits of
 * accuracy. Finally, a third 2-bit shift occurs just before the sample is
 * saved in the PCM buffer. 14 + 12 + 2 == 28 bits.
 */

/* FPM_APPROX without OPT_SSO will actually lose accuracy and performance */

# if defined(FPM_APPROX) && !defined(OPT_SSO)
#  define OPT_SSO
# endif

/* second SSO shift, with rounding */

# if defined(OPT_SSO)
#  define SHIFT(x)  (((x) + (1L << 11)) >> 12)
# else
#  define SHIFT(x)  (x)
# endif

/*
 * NAME:	dct32()
 * DESCRIPTION:	perform fast in[32]->out[32] DCT
 */
static
void dct32(mad_fixed_t const in[32], unsigned int slot,
	   mad_fixed_t lo[16][8], mad_fixed_t hi[16][8])
{
  mad_fixed_t t0,   t1,   t2,   t3,   t4,   t5,   t6,   t7;
  mad_fixed_t t8,   t9,   t10,  t11,  t12,  t13,  t14,  t15;
  mad_fixed_t t16,  t17,  t18,  t19,  t20,  t21,  t22,  t23;
  mad_fixed_t t24,  t25,  t26,  t27,  t28,  t29,  t30,  t31;
  mad_fixed_t t32,  t33,  t34,  t35,  t36,  t37,  t38,  t39;
  mad_fixed_t t40,  t41,  t42,  t43,  t44,  t45,  t46,  t47;
  mad_fixed_t t48,  t49,  t50,  t51,  t52,  t53,  t54,  t55;
  mad_fixed_t t56,  t57,  t58,  t59,  t60,  t61,  t62,  t63;
  mad_fixed_t t64,  t65,  t66,  t67,  t68,  t69,  t70,  t71;
  mad_fixed_t t72,  t73,  t74,  t75,  t76,  t77,  t78,  t79;
  mad_fixed_t t80,  t81,  t82,  t83,  t84,  t85,  t86,  t87;
  mad_fixed_t t88,  t89,  t90,  t91,  t92,  t93,  t94,  t95;
  mad_fixed_t t96,  t97,  t98,  t99,  t100, t101, t102, t103;
  mad_fixed_t t104, t105, t106, t107, t108, t109, t110, t111;
  mad_fixed_t t112, t113, t114, t115, t116, t117, t118, t119;
  mad_fixed_t t120, t121, t122, t123, t124, t125, t126, t127;
  mad_fixed_t t128, t129, t130, t131, t132, t133, t134, t135;
  mad_fixed_t t136, t137, t138, t139, t140, t141, t142, t143;
  mad_fixed_t t144, t145, t146, t147, t148, t149, t150, t151;
  mad_fixed_t t152, t153, t154, t155, t156, t157, t158, t159;
  mad_fixed_t t160, t161, t162, t163, t164, t165, t166, t167;
  mad_fixed_t t168, t169, t170, t171, t172, t173, t174, t175;
  mad_fixed_t t176;

  /* costab[i] = cos(PI / (2 * 32) * i) */

  enum {
    costab1  = MAD_F(0x0ffb10f2),  /* 0.998795456 */
    costab2  = MAD_F(0x0fec46d2),  /* 0.995184727 */
    costab3  = MAD_F(0x0fd3aac0),  /* 0.989176510 */
    costab4  = MAD_F(0x0fb14be8),  /* 0.980785280 */
    costab5  = MAD_F(0x0f853f7e),  /* 0.970031253 */
    costab6  = MAD_F(0x0f4fa0ab),  /* 0.956940336 */
    costab7  = MAD_F(0x0f109082),  /* 0.941544065 */
    costab8  = MAD_F(0x0ec835e8),  /* 0.923879533 */
    costab9  = MAD_F(0x0e76bd7a),  /* 0.903989293 */
    costab10 = MAD_F(0x0e1c5979),  /* 0.881921264 */
    costab11 = MAD_F(0x0db941a3),  /* 0.857728610 */
    costab12 = MAD_F(0x0d4db315),  /* 0.831469612 */
    costab13 = MAD_F(0x0cd9f024),  /* 0.803207531 */
    costab14 = MAD_F(0x0c5e4036),  /* 0.773010453 */
    costab15 = MAD_F(0x0bdaef91),  /* 0.740951125 */
    costab16 = MAD_F(0x0b504f33),  /* 0.707106781 */
    costab17 = MAD_F(0x0abeb49a),  /* 0.671558955 */
    costab18 = MAD_F(0x0a267993),  /* 0.634393284 */
    costab19 = MAD_F(0x0987fbfe),  /* 0.595699304 */
    costab20 = MAD_F(0x08e39d9d),  /* 0.555570233 */
    costab21 = MAD_F(0x0839c3cd),  /* 0.514102744 */
    costab22 = MAD_F(0x078ad74e),  /* 0.471396737 */
    costab23 = MAD_F(0x06d74402),  /* 0.427555093 */
    costab24 = MAD_F(0x061f78aa),  /* 0.382683432 */
    costab25 = MAD_F(0x0563e69d),  /* 0.336889853 */
    costab26 = MAD_F(0x04a5018c),  /* 0.290284677 */
    costab27 = MAD_F(0x03e33f2f),  /* 0.242980180 */
    costab28 = MAD_F(0x031f1708),  /* 0.195090322 */
    costab29 = MAD_F(0x0259020e),  /* 0.146730474 */
    costab30 = MAD_F(0x01917a6c),  /* 0.098017140 */
    costab31 = MAD_F(0x00c8fb30)   /* 0.049067674 */
  };

  t0   = in[0]  + in[31];  t16  = mad_f_mul(in[0]  - in[31], costab1);
  t1   = in[15] + in[16];  t17  = mad_f_mul(in[15] - in[16], costab31);

  t41  = t16 + t17;
  t59  = mad_f_mul(t16 - t17, costab2);
  t33  = t0  + t1;
  t50  = mad_f_mul(t0  - t1,  costab2);

  t2   = in[7]  + in[24];  t18  = mad_f_mul(in[7]  - in[24], costab15);
  t3   = in[8]  + in[23];  t19  = mad_f_mul(in[8]  - in[23], costab17);

  t42  = t18 + t19;
  t60  = mad_f_mul(t18 - t19, costab30);
  t34  = t2  + t3;
  t51  = mad_f_mul(t2  - t3,  costab30);

  t4   = in[3]  + in[28];  t20  = mad_f_mul(in[3]  - in[28], costab7);
  t5   = in[12] + in[19];  t21  = mad_f_mul(in[12] - in[19], costab25);

  t43  = t20 + t21;
  t61  = mad_f_mul(t20 - t21, costab14);
  t35  = t4  + t5;
  t52  = mad_f_mul(t4  - t5,  costab14);

  t6   = in[4]  + in[27];  t22  = mad_f_mul(in[4]  - in[27], costab9);
  t7   = in[11] + in[20];  t23  = mad_f_mul(in[11] - in[20], costab23);

  t44  = t22 + t23;
  t62  = mad_f_mul(t22 - t23, costab18);
  t36  = t6  + t7;
  t53  = mad_f_mul(t6  - t7,  costab18);

  t8   = in[1]  + in[30];  t24  = mad_f_mul(in[1]  - in[30], costab3);
  t9   = in[14] + in[17];  t25  = mad_f_mul(in[14] - in[17], costab29);

  t45  = t24 + t25;
  t63  = mad_f_mul(t24 - t25, costab6);
  t37  = t8  + t9;
  t54  = mad_f_mul(t8  - t9,  costab6);

  t10  = in[6]  + in[25];  t26  = mad_f_mul(in[6]  - in[25], costab13);
  t11  = in[9]  + in[22];  t27  = mad_f_mul(in[9]  - in[22], costab19);

  t46  = t26 + t27;
  t64  = mad_f_mul(t26 - t27, costab26);
  t38  = t10 + t11;
  t55  = mad_f_mul(t10 - t11, costab26);

  t12  = in[2]  + in[29];  t28  = mad_f_mul(in[2]  - in[29], costab5);
  t13  = in[13] + in[18];  t29  = mad_f_mul(in[13] - in[18], costab27);

  t47  = t28 + t29;
  t65  = mad_f_mul(t28 - t29, costab10);
  t39  = t12 + t13;
  t56  = mad_f_mul(t12 - t13, costab10);

  t14  = in[5]  + in[26];  t30  = mad_f_mul(in[5]  - in[26], costab11);
  t15  = in[10] + in[21];  t31  = mad_f_mul(in[10] - in[21], costab21);

  t48  = t30 + t31;
  t66  = mad_f_mul(t30 - t31, costab22);
  t40  = t14 + t15;
  t57  = mad_f_mul(t14 - t15, costab22);

  t69  = t33 + t34;  t89  = mad_f_mul(t33 - t34, costab4);
  t70  = t35 + t36;  t90  = mad_f_mul(t35 - t36, costab28);
  t71  = t37 + t38;  t91  = mad_f_mul(t37 - t38, costab12);
  t72  = t39 + t40;  t92  = mad_f_mul(t39 - t40, costab20);
  t73  = t41 + t42;  t94  = mad_f_mul(t41 - t42, costab4);
  t74  = t43 + t44;  t95  = mad_f_mul(t43 - t44, costab28);
  t75  = t45 + t46;  t96  = mad_f_mul(t45 - t46, costab12);
  t76  = t47 + t48;  t97  = mad_f_mul(t47 - t48, costab20);

  t78  = t50 + t51;  t100 = mad_f_mul(t50 - t51, costab4);
  t79  = t52 + t53;  t101 = mad_f_mul(t52 - t53, costab28);
  t80  = t54 + t55;  t102 = mad_f_mul(t54 - t55, costab12);
  t81  = t56 + t57;  t103 = mad_f_mul(t56 - t57, costab20);

  t83  = t59 + t60;  t106 = mad_f_mul(t59 - t60, costab4);
  t84  = t61 + t62;  t107 = mad_f_mul(t61 - t62, costab28);
  t85  = t63 + t64;  t108 = mad_f_mul(t63 - t64, costab12);
  t86  = t65 + t66;  t109 = mad_f_mul(t65 - t66, costab20);

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

  t141 = mad_f_mul(t69 - t70, costab8);
  t142 = mad_f_mul(t71 - t72, costab24);
  t143 = t141 + t142;
  t144 = mad_f_mul(t73 - t74, costab8);
  t145 = mad_f_mul(t75 - t76, costab24);
  t146 = t144 + t145;

  t88  = (t146 << 1) - t77;

  t148 = mad_f_mul(t78 - t79, costab8);
  t149 = mad_f_mul(t80 - t81, costab24);
  t150 = t148 + t149;

  t105 = (t150 << 1) - t82;

  t152 = mad_f_mul(t83 - t84, costab8);
  t153 = mad_f_mul(t85 - t86, costab24);
  t154 = t152 + t153;

  t111 = (t154 << 1) - t87;

  t99  = (t111 << 1) - t88;

  t157 = mad_f_mul(t89 - t90, costab8);
  t158 = mad_f_mul(t91 - t92, costab24);
  t159 = t157 + t158;

  t127 = (t159 << 1) - t93;

  t160 = (mad_f_mul(t125 - t126, costab16) << 1) - t127;
  t161 = mad_f_mul(t94 - t95, costab8);
  t162 = mad_f_mul(t96 - t97, costab24);
  t163 = t161 + t162;

  t130 = (t163 << 1) - t98;

  t112 = (t130 << 1) - t99;

  t164 = (mad_f_mul(t128 - t129, costab16) << 1) - t130;

  t166 = mad_f_mul(t100 - t101, costab8);
  t167 = mad_f_mul(t102 - t103, costab24);
  t168 = t166 + t167;

  t134 = (t168 << 1) - t104;

  t120 = (t134 << 1) - t105;

  t135 = (mad_f_mul(t118 - t119, costab16) << 1) - t120;

  t169 = (mad_f_mul(t132 - t133, costab16) << 1) - t134;

  t151 = (t169 << 1) - t135;

  t170 = (((mad_f_mul(t148 - t149, costab16) << 1) - t150) << 1) - t151;
  t171 = mad_f_mul(t106 - t107, costab8);
  t172 = mad_f_mul(t108 - t109, costab24);
  t173 = t171 + t172;

  t138 = (t173 << 1) - t110;

  t123 = (t138 << 1) - t111;

  t139 = (mad_f_mul(t121 - t122, costab16) << 1) - t123;

  t117 = (t123 << 1) - t112;

  t124 = (mad_f_mul(t115 - t116, costab16) << 1) - t117;

  t131 = (t139 << 1) - t124;

  t140 = (t164 << 1) - t131;

  t174 = (mad_f_mul(t136 - t137, costab16) << 1) - t138;

  t155 = (t174 << 1) - t139;

  t147 = (t155 << 1) - t140;

  t156 = (((mad_f_mul(t144 - t145, costab16) << 1) - t146) << 1) - t147;

  t175 = (((mad_f_mul(t152 - t153, costab16) << 1) - t154) << 1) - t155;

  t165 = (t175 << 1) - t156;

  t176 = (((((mad_f_mul(t161 - t162, costab16) << 1) -
	     t163) << 1) - t164) << 1) - t165;

  /* the output order is convoluted to simplify later processing */

  /*  0 */ hi[15][slot] = SHIFT(t113 + t114);
  /*  1 */ hi[14][slot] = SHIFT(t32);
  /*  2 */ hi[13][slot] = SHIFT(t58);
  /*  3 */ hi[12][slot] = SHIFT(t49);
  /*  4 */ hi[11][slot] = SHIFT(t93);
  /*  5 */ hi[10][slot] = SHIFT(t68);
  /*  6 */ hi[ 9][slot] = SHIFT(t82);
  /*  7 */ hi[ 8][slot] = SHIFT(t77);
  /*  8 */ hi[ 7][slot] = SHIFT(t143);
  /*  9 */ hi[ 6][slot] = SHIFT(t88);
  /* 10 */ hi[ 5][slot] = SHIFT(t105);
  /* 11 */ hi[ 4][slot] = SHIFT(t99);
  /* 12 */ hi[ 3][slot] = SHIFT(t127);
  /* 13 */ hi[ 2][slot] = SHIFT(t112);
  /* 14 */ hi[ 1][slot] = SHIFT(t120);
  /* 15 */ hi[ 0][slot] = SHIFT(t117);

  /* 16 */ lo[ 0][slot] = SHIFT(mad_f_mul(t113 - t114, costab16));
  /* 17 */ lo[ 1][slot] = SHIFT(t124);
  /* 18 */ lo[ 2][slot] = SHIFT(t135);
  /* 19 */ lo[ 3][slot] = SHIFT(t131);
  /* 20 */ lo[ 4][slot] = SHIFT(t160);
  /* 21 */ lo[ 5][slot] = SHIFT(t140);
  /* 22 */ lo[ 6][slot] = SHIFT(t151);
  /* 23 */ lo[ 7][slot] = SHIFT(t147);
  /* 24 */ lo[ 8][slot] = SHIFT((mad_f_mul(t141 - t142, costab16) << 1) -
				t143);
  /* 25 */ lo[ 9][slot] = SHIFT(t156);
  /* 26 */ lo[10][slot] = SHIFT(t170);
  /* 27 */ lo[11][slot] = SHIFT(t165);
  /* 28 */ lo[12][slot] = SHIFT((((mad_f_mul(t157 - t158, costab16) << 1) -
				  t159) << 1) - t160);
  /* 29 */ lo[13][slot] = SHIFT(t176);
  /* 30 */ lo[14][slot] = SHIFT((((((mad_f_mul(t166 - t167, costab16) << 1) -
				    t168) << 1) - t169) << 1) - t170);
  /* 31 */ lo[15][slot] = SHIFT((((((((mad_f_mul(t171 - t172, costab16) << 1) -
				      t173) << 1) - t174) << 1) -
				  t175) << 1) - t176);

  /*
   * Totals:
   *  80 multiplies
   *  80 additions
   * 119 subtractions
   *  49 shifts (not counting SSO)
   */
}

# undef SHIFT

/* third SSO shift and/or D[] optimization preshift */

# if defined(OPT_SSO)
#  undef  MAD_F_HAVEMLA
#  define MUL(x, y)		((x) * (y))
#  define PRESHIFT(x)		((MAD_F(x) + (1L << 13)) >> 14)
#  define SHIFT(x)		((x) >> 2)
# elif defined(MAD_F_SCALEBITS)
#  undef  MAD_F_SCALEBITS
#  define MAD_F_SCALEBITS	(MAD_F_FRACBITS - 12)
#  define MLA(hi, lo, x, y)	mad_f_mla(&(hi), &(lo), (x), (y))
#  define MUL(x, y)		mad_f_mul((x), (y))
#  define PRESHIFT(x)		(MAD_F(x) >> 12)
#  define SHIFT(x)		(x)
# else
#  define MLA(hi, lo, x, y)	mad_f_mla(&(hi), &(lo), (x), (y))
#  define MUL(x, y)		mad_f_mul((x), (y))
#  define PRESHIFT(x)		MAD_F(x)
#  define SHIFT(x)		(x)
# endif

static
mad_fixed_t const D[17][16] = {
# include "D.dat"
};

/*
 * NAME:	synth->frame()
 * DESCRIPTION:	perform PCM synthesis of frame subband samples
 */
void mad_synth_frame(struct mad_synth *synth, struct mad_frame const *frame)
{
  unsigned int nch, ns, ch, s, phase = 0;

  nch = MAD_NCHANNELS(&frame->header);
  ns  = MAD_NSBSAMPLES(&frame->header);

  for (ch = 0; ch < nch; ++ch) {
    mad_fixed_t *pcm1, *pcm2;
    mad_fixed_t (*even)[2][16][8], (*odd)[2][16][8];
    mad_fixed_t const (*sbsample)[36][32];

    phase = synth->phase;
    pcm1  = synth->pcm.samples[ch];

    even = &synth->filter[ch][0];
    odd  = &synth->filter[ch][1];

    sbsample = &frame->sbsample[ch];

    for (s = 0; s < ns; ++s) {
      unsigned int pe14, pe12, pe10, pe08, pe06, pe04, pe02, pe00;
      unsigned int po14, po12, po10, po08, po06, po04, po02, po00;
      register mad_fixed_t (*fe)[8], (*fx)[8], (*fo)[8];
      register mad_fixed_t const (*Dptr)[16];
      register mad_fixed_t sum1, sum2;
      unsigned int sb;

      dct32((*sbsample)[s], phase >> 1,
	    (*even)[~phase & 1], (*odd)[~phase & 1]);

      pe00 = phase & ~0x1;
      pe02 = (pe00 + 2) % 16;
      pe04 = (pe02 + 2) % 16;
      pe06 = (pe04 + 2) % 16;
      pe08 = (pe06 + 2) % 16;
      pe10 = (pe08 + 2) % 16;
      pe12 = (pe10 + 2) % 16;
      pe14 = (pe12 + 2) % 16;

      po00 = ((phase - 1) & 0xf) | 0x1;
      po02 = (po00 + 2) % 16;
      po04 = (po02 + 2) % 16;
      po06 = (po04 + 2) % 16;
      po08 = (po06 + 2) % 16;
      po10 = (po08 + 2) % 16;
      po12 = (po10 + 2) % 16;
      po14 = (po12 + 2) % 16;

      /* calculate 32 samples */

      fe = &(*even)[~phase & 1][0];
      fx = &(*even)[ phase & 1][0];
      fo =  &(*odd)[ phase & 1][0];

      Dptr = &D[0];

# if defined(MAD_F_HAVEMLA)
      {
	mad_fixed64hi_t hi1, hi2;
	mad_fixed64lo_t lo1, lo2;

	hi1 = lo1 = hi2 = lo2 = 0;

	MLA(hi1, lo1, (*fe)[0], (*Dptr)[pe00]);
	MLA(hi1, lo1, (*fe)[1], (*Dptr)[pe14]);
	MLA(hi1, lo1, (*fe)[2], (*Dptr)[pe12]);
	MLA(hi1, lo1, (*fe)[3], (*Dptr)[pe10]);
	MLA(hi1, lo1, (*fe)[4], (*Dptr)[pe08]);
	MLA(hi1, lo1, (*fe)[5], (*Dptr)[pe06]);
	MLA(hi1, lo1, (*fe)[6], (*Dptr)[pe04]);
	MLA(hi1, lo1, (*fe)[7], (*Dptr)[pe02]);

	MLA(hi2, lo2, (*fx)[0], (*Dptr)[po00]);
	MLA(hi2, lo2, (*fx)[1], (*Dptr)[po14]);
	MLA(hi2, lo2, (*fx)[2], (*Dptr)[po12]);
	MLA(hi2, lo2, (*fx)[3], (*Dptr)[po10]);
	MLA(hi2, lo2, (*fx)[4], (*Dptr)[po08]);
	MLA(hi2, lo2, (*fx)[5], (*Dptr)[po06]);
	MLA(hi2, lo2, (*fx)[6], (*Dptr)[po04]);
	MLA(hi2, lo2, (*fx)[7], (*Dptr)[po02]);

	sum1 = mad_f_scale64(hi1, lo1) -
	       mad_f_scale64(hi2, lo2);
      }
# else
      {
	register mad_fixed_t sub1;

	sum1  = MUL((*fe)[0], (*Dptr)[pe00]);
	sum1 += MUL((*fe)[1], (*Dptr)[pe14]);
	sum1 += MUL((*fe)[2], (*Dptr)[pe12]);
	sum1 += MUL((*fe)[3], (*Dptr)[pe10]);
	sum1 += MUL((*fe)[4], (*Dptr)[pe08]);
	sum1 += MUL((*fe)[5], (*Dptr)[pe06]);
	sum1 += MUL((*fe)[6], (*Dptr)[pe04]);
	sum1 += MUL((*fe)[7], (*Dptr)[pe02]);

	sub1  = MUL((*fx)[0], (*Dptr)[po00]);
	sub1 += MUL((*fx)[1], (*Dptr)[po14]);
	sub1 += MUL((*fx)[2], (*Dptr)[po12]);
	sub1 += MUL((*fx)[3], (*Dptr)[po10]);
	sub1 += MUL((*fx)[4], (*Dptr)[po08]);
	sub1 += MUL((*fx)[5], (*Dptr)[po06]);
	sub1 += MUL((*fx)[6], (*Dptr)[po04]);
	sub1 += MUL((*fx)[7], (*Dptr)[po02]);

	sum1 -= sub1;
      }
# endif

      *pcm1++ = SHIFT(sum1);

      pcm2 = pcm1 + 30;

      for (sb = 1; sb < 16; ++sb) {
	++fe;
	++Dptr;

	/* D[32 - sb][i] == -D[sb][15 - i] */

# if defined(MAD_F_HAVEMLA)
	{
	  mad_fixed64hi_t hi1, hi2;
	  mad_fixed64lo_t lo1, lo2;

	  hi1 = lo1 = hi2 = lo2 = 0;

	  MLA(hi1, lo1, (*fe)[0], (*Dptr)[     pe00]);
	  MLA(hi2, lo2, (*fe)[0], (*Dptr)[15 - pe00]);
	  MLA(hi1, lo1, (*fe)[1], (*Dptr)[     pe14]);
	  MLA(hi2, lo2, (*fe)[1], (*Dptr)[15 - pe14]);
	  MLA(hi1, lo1, (*fe)[2], (*Dptr)[     pe12]);
	  MLA(hi2, lo2, (*fe)[2], (*Dptr)[15 - pe12]);
	  MLA(hi1, lo1, (*fe)[3], (*Dptr)[     pe10]);
	  MLA(hi2, lo2, (*fe)[3], (*Dptr)[15 - pe10]);

	  MLA(hi1, lo1, (*fe)[4], (*Dptr)[     pe08]);
	  MLA(hi2, lo2, (*fe)[4], (*Dptr)[15 - pe08]);
	  MLA(hi1, lo1, (*fe)[5], (*Dptr)[     pe06]);
	  MLA(hi2, lo2, (*fe)[5], (*Dptr)[15 - pe06]);
	  MLA(hi1, lo1, (*fe)[6], (*Dptr)[     pe04]);
	  MLA(hi2, lo2, (*fe)[6], (*Dptr)[15 - pe04]);
	  MLA(hi1, lo1, (*fe)[7], (*Dptr)[     pe02]);
	  MLA(hi2, lo2, (*fe)[7], (*Dptr)[15 - pe02]);

	  sum1 = mad_f_scale64(hi1, lo1);

	  hi1 = lo1 = 0;

	  MLA(hi1, lo1, (*fo)[0], (*Dptr)[     po00]);
	  MLA(hi2, lo2, (*fo)[0], (*Dptr)[15 - po00]);
	  MLA(hi1, lo1, (*fo)[1], (*Dptr)[     po14]);
	  MLA(hi2, lo2, (*fo)[1], (*Dptr)[15 - po14]);
	  MLA(hi1, lo1, (*fo)[2], (*Dptr)[     po12]);
	  MLA(hi2, lo2, (*fo)[2], (*Dptr)[15 - po12]);
	  MLA(hi1, lo1, (*fo)[3], (*Dptr)[     po10]);
	  MLA(hi2, lo2, (*fo)[3], (*Dptr)[15 - po10]);

	  MLA(hi1, lo1, (*fo)[4], (*Dptr)[     po08]);
	  MLA(hi2, lo2, (*fo)[4], (*Dptr)[15 - po08]);
	  MLA(hi1, lo1, (*fo)[5], (*Dptr)[     po06]);
	  MLA(hi2, lo2, (*fo)[5], (*Dptr)[15 - po06]);
	  MLA(hi1, lo1, (*fo)[6], (*Dptr)[     po04]);
	  MLA(hi2, lo2, (*fo)[6], (*Dptr)[15 - po04]);
	  MLA(hi1, lo1, (*fo)[7], (*Dptr)[     po02]);
	  MLA(hi2, lo2, (*fo)[7], (*Dptr)[15 - po02]);

	  sum2  = mad_f_scale64(hi2, lo2);
	  sum1 -= mad_f_scale64(hi1, lo1);
	}
# else
	{
	  register mad_fixed_t sub1;

	  sum1  = MUL((*fe)[0], (*Dptr)[     pe00]);
	  sum2  = MUL((*fe)[0], (*Dptr)[15 - pe00]);
	  sum1 += MUL((*fe)[1], (*Dptr)[     pe14]);
	  sum2 += MUL((*fe)[1], (*Dptr)[15 - pe14]);
	  sum1 += MUL((*fe)[2], (*Dptr)[     pe12]);
	  sum2 += MUL((*fe)[2], (*Dptr)[15 - pe12]);
	  sum1 += MUL((*fe)[3], (*Dptr)[     pe10]);
	  sum2 += MUL((*fe)[3], (*Dptr)[15 - pe10]);

	  sum1 += MUL((*fe)[4], (*Dptr)[     pe08]);
	  sum2 += MUL((*fe)[4], (*Dptr)[15 - pe08]);
	  sum1 += MUL((*fe)[5], (*Dptr)[     pe06]);
	  sum2 += MUL((*fe)[5], (*Dptr)[15 - pe06]);
	  sum1 += MUL((*fe)[6], (*Dptr)[     pe04]);
	  sum2 += MUL((*fe)[6], (*Dptr)[15 - pe04]);
	  sum1 += MUL((*fe)[7], (*Dptr)[     pe02]);
	  sum2 += MUL((*fe)[7], (*Dptr)[15 - pe02]);

	  sub1  = MUL((*fo)[0], (*Dptr)[     po00]);
	  sum2 += MUL((*fo)[0], (*Dptr)[15 - po00]);
	  sub1 += MUL((*fo)[1], (*Dptr)[     po14]);
	  sum2 += MUL((*fo)[1], (*Dptr)[15 - po14]);
	  sub1 += MUL((*fo)[2], (*Dptr)[     po12]);
	  sum2 += MUL((*fo)[2], (*Dptr)[15 - po12]);
	  sub1 += MUL((*fo)[3], (*Dptr)[     po10]);
	  sum2 += MUL((*fo)[3], (*Dptr)[15 - po10]);

	  sub1 += MUL((*fo)[4], (*Dptr)[     po08]);
	  sum2 += MUL((*fo)[4], (*Dptr)[15 - po08]);
	  sub1 += MUL((*fo)[5], (*Dptr)[     po06]);
	  sum2 += MUL((*fo)[5], (*Dptr)[15 - po06]);
	  sub1 += MUL((*fo)[6], (*Dptr)[     po04]);
	  sum2 += MUL((*fo)[6], (*Dptr)[15 - po04]);
	  sub1 += MUL((*fo)[7], (*Dptr)[     po02]);
	  sum2 += MUL((*fo)[7], (*Dptr)[15 - po02]);

	  sum1 -= sub1;
	}
# endif

	*pcm1++ = SHIFT(sum1);
	*pcm2-- = SHIFT(sum2);

	++fo;
      }

      ++Dptr;

# if defined(MAD_F_HAVEMLA)
      {
	mad_fixed64hi_t hi;
	mad_fixed64lo_t lo;

	hi = lo = 0;

	MLA(hi, lo, (*fo)[0], (*Dptr)[po00]);
	MLA(hi, lo, (*fo)[1], (*Dptr)[po14]);
	MLA(hi, lo, (*fo)[2], (*Dptr)[po12]);
	MLA(hi, lo, (*fo)[3], (*Dptr)[po10]);
	MLA(hi, lo, (*fo)[4], (*Dptr)[po08]);
	MLA(hi, lo, (*fo)[5], (*Dptr)[po06]);
	MLA(hi, lo, (*fo)[6], (*Dptr)[po04]);
	MLA(hi, lo, (*fo)[7], (*Dptr)[po02]);

	sum1 = mad_f_scale64(hi, lo);
      }
# else
      sum1  = MUL((*fo)[0], (*Dptr)[po00]);
      sum1 += MUL((*fo)[1], (*Dptr)[po14]);
      sum1 += MUL((*fo)[2], (*Dptr)[po12]);
      sum1 += MUL((*fo)[3], (*Dptr)[po10]);
      sum1 += MUL((*fo)[4], (*Dptr)[po08]);
      sum1 += MUL((*fo)[5], (*Dptr)[po06]);
      sum1 += MUL((*fo)[6], (*Dptr)[po04]);
      sum1 += MUL((*fo)[7], (*Dptr)[po02]);
# endif

      *pcm1 = SHIFT(-sum1);
      pcm1 += 16;

      phase = (phase + 1) % 16;
    }
  }

  synth->phase      = phase;
  synth->pcm.length = 32 * ns;
}

/*
 * NAME:	synth->mute()
 * DESCRIPTION:	zero all polyphase filterbank values, resetting synthesis
 */
void mad_synth_mute(struct mad_synth *synth)
{
  unsigned int ch, s, v;

  for (ch = 0; ch < 2; ++ch) {
    for (s = 0; s < 16; ++s) {
      for (v = 0; v < 8; ++v) {
	synth->filter[ch][0][0][s][v] = synth->filter[ch][0][1][s][v] =
	synth->filter[ch][1][0][s][v] = synth->filter[ch][1][1][s][v] = 0;
      }
    }
  }
}
