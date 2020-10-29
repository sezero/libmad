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
 * $Id: synth.h,v 1.3 2000/03/05 18:11:34 rob Exp $
 */

# ifndef SYNTH_H
# define SYNTH_H

# include "frame.h"

struct mad_synth {
  fixed_t filterout[2][2][256];		/* polyphase filterbank outputs */
  unsigned short slot;			/* current processing slot */

  unsigned short pcmlen;		/* number of PCM samples */
  fixed_t pcmout[2][1152];		/* PCM sample outputs */
};

void mad_synth_init(struct mad_synth *);

void mad_synthesis(struct mad_frame *, struct mad_synth *);

# endif

