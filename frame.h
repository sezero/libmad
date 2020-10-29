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
 * $Id: frame.h,v 1.5 2000/04/22 04:36:50 rob Exp $
 */

# ifndef FRAME_H
# define FRAME_H

# include "fixed.h"
# include "timer.h"
# include "stream.h"

struct mad_frame {
  int layer;				/* audio layer (1, 2, or 3) */
  int mode;				/* channel mode (see below) */
  int mode_ext;				/* additional mode info */
  int emphasis;				/* de-emphasis to use (see below) */

  unsigned int bitrate;			/* stream bitrate (bps) */
  unsigned int sfreq;			/* sampling frequency (Hz) */

  struct mad_timer duration;		/* audio playing time of frame */

  int flags;				/* flags and private bits (below) */

  mad_fixed_t sbsample[2][36][32];	/* synthesis subband filter samples */
  mad_fixed_t (*overlap)[2][32][18];	/* Layer III block overlap data */
};

# define MAD_NUMCHANNELS(frame)		((frame)->mode ? 2 : 1)
# define MAD_NUMSBSAMPLES(frame)	((frame)->layer == 1 ? 12 : 36)

# define MAD_MODE_SINGLE_CHANNEL	0
# define MAD_MODE_DUAL_CHANNEL		1
# define MAD_MODE_JOINT_STEREO		2
# define MAD_MODE_STEREO		3

# define MAD_EMPH_NONE		0	/* no emphasis */
# define MAD_EMPH_50_15_MS	1	/* 50/15 microseconds */
# define MAD_EMPH_CCITT_J_17	3	/* CCITT J.17 */

# define MAD_FLAG_PROTECTION	0x0100	/* frame has CRC protection */
# define MAD_FLAG_COPYRIGHT	0x0200	/* frame is copyright */
# define MAD_FLAG_ORIGINAL	0x0400	/* frame is original (else copy) */
# define MAD_FLAG_PADDING	0x0800	/* frame has additional slot */

# define MAD_FLAG_I_STEREO	0x1000	/* uses intensity joint stereo */
# define MAD_FLAG_MS_STEREO	0x2000	/* uses middle/side joint stereo */

# define MAD_FLAG_PRIVATE	0x0040	/* header private bit */
# define MAD_FLAG_III_PRIVATE	0x001f	/* Layer III private bits */
# define MAD_FLAG_III_5BITPRIV	0x0020	/* 5 bits in III private (else 3) */

void mad_frame_init(struct mad_frame *);
void mad_frame_finish(struct mad_frame *);

void mad_frame_mute(struct mad_frame *);

int mad_frame_header(struct mad_frame *, struct mad_stream *,
		     unsigned short *);
int mad_frame_decode(struct mad_frame *, struct mad_stream *);

# endif

