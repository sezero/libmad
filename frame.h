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
 * $Id: frame.h,v 1.5 2000/10/25 21:52:32 rob Exp $
 */

# ifndef MAD_FRAME_H
# define MAD_FRAME_H

# include "fixed.h"
# include "timer.h"
# include "stream.h"

enum mad_layer {
  MAD_LAYER_I   = 1,			/* Layer I */
  MAD_LAYER_II  = 2,			/* Layer II */
  MAD_LAYER_III = 3			/* Layer III */
};

enum mad_mode {
  MAD_MODE_SINGLE_CHANNEL = 0,		/* single channel */
  MAD_MODE_DUAL_CHANNEL	  = 1,		/* dual channel */
  MAD_MODE_JOINT_STEREO	  = 2,		/* joint (MS/intensity) stereo */
  MAD_MODE_STEREO	  = 3		/* normal LR stereo */
};

enum mad_emphasis {
  MAD_EMPHASIS_NONE	  = 0,		/* no emphasis */
  MAD_EMPHASIS_50_15_MS	  = 1,		/* 50/15 microseconds */
  MAD_EMPHASIS_CCITT_J_17 = 3		/* CCITT J.17 */
};

struct mad_frame {
  enum mad_layer layer;			/* audio layer (1, 2, or 3) */
  enum mad_mode mode;			/* channel mode (see above) */
  int mode_ext;				/* additional mode info */
  enum mad_emphasis emphasis;		/* de-emphasis to use (see above) */

  unsigned long bitrate;		/* stream bitrate (bps) */
  unsigned int sfreq;			/* sampling frequency (Hz) */

  unsigned int crc_header;		/* header CRC partial checksum */
  unsigned int crc_check;		/* target CRC final checksum */

  int flags;				/* flags (below) */
  int private;				/* private bits (below) */

  mad_timer_t duration;			/* audio playing time of frame */

  mad_fixed_t sbsample[2][36][32];	/* synthesis subband filter samples */
  mad_fixed_t (*overlap)[2][32][18];	/* Layer III block overlap data */
};

# define MAD_NCHANNELS(frame)		((frame)->mode ? 2 : 1)
# define MAD_NSBSAMPLES(frame)  \
  ((frame)->layer == MAD_LAYER_I ? 12 :  \
   (((frame)->layer == MAD_LAYER_III &&  \
     ((frame)->flags & MAD_FLAG_LSF_EXT)) ? 18 : 36))

enum {
  MAD_FLAG_NPRIVATE_III	  = 0x0007,	/* number of Layer III private bits */
  MAD_FLAG_INCOMPLETE	  = 0x0008,	/* header but not data is decoded */

  MAD_FLAG_PROTECTION	  = 0x0010,	/* frame has CRC protection */
  MAD_FLAG_COPYRIGHT	  = 0x0020,	/* frame is copyright */
  MAD_FLAG_ORIGINAL	  = 0x0040,	/* frame is original (else copy) */
  MAD_FLAG_PADDING	  = 0x0080,	/* frame has additional slot */

  MAD_FLAG_I_STEREO	  = 0x0100,	/* uses intensity joint stereo */
  MAD_FLAG_MS_STEREO	  = 0x0200,	/* uses middle/side joint stereo */

  MAD_FLAG_LSF_EXT	  = 0x1000,	/* lower sampling freq. extension */
  MAD_FLAG_MC_EXT	  = 0x2000	/* multichannel audio extension */
};

enum {
  MAD_PRIVATE_HEADER	  = 0x0100,	/* header private bit */
  MAD_PRIVATE_III	  = 0x001f	/* Layer III private bits (up to 5) */
};

void mad_frame_init(struct mad_frame *);
void mad_frame_finish(struct mad_frame *);

void mad_frame_mute(struct mad_frame *);

int mad_frame_header(struct mad_frame *, struct mad_stream *);
int mad_frame_decode(struct mad_frame *, struct mad_stream *);

# endif
