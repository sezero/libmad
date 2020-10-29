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
 * $Id: frame.c,v 1.1 2000/03/05 05:19:16 rob Exp $
 */

# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif

# include "frame.h"
# include "timer.h"

void mad_frame_init(struct mad_frame *frame)
{
  unsigned int ch, sb, i;

  frame->layer      = 0;
  frame->mode       = 0;
  frame->mode_ext   = 0;
  frame->emphasis   = 0;

  frame->bitrate    = 0;
  frame->samplefreq = 0;

  mad_timer_init(&frame->duration);

  frame->flags      = 0;

  mad_frame_mute(frame);

  for (ch = 0; ch < 2; ++ch) {
    for (sb = 0; sb < 32; ++sb) {
      for (i = 0; i < 18; ++i)
	frame->overlap[ch][sb][i] = 0;
    }
  }
}

void mad_frame_mute(struct mad_frame *frame)
{
  unsigned int s, sb;

  for (s = 0; s < 36; ++s) {
    for (sb = 0; sb < 32; ++sb)
      frame->sbsample[0][s][sb] = frame->sbsample[1][s][sb] = 0;
  }
}
