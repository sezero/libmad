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
 * $Id: synth.h,v 1.2 2000/09/17 18:52:18 rob Exp $
 */

# ifndef MAD_SYNTH_H
# define MAD_SYNTH_H

# include "fixed.h"
# include "frame.h"

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

