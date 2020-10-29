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
 * $Id: decoder.h,v 1.3 2000/09/17 18:52:18 rob Exp $
 */

# ifndef MAD_DECODER_H
# define MAD_DECODER_H

# include "stream.h"
# include "frame.h"
# include "synth.h"

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

  void *cb_data;

  int (*input_func)(void *, struct mad_stream *);
  int (*filter_func)(void *, struct mad_frame *);
  int (*output_func)(void *, struct mad_frame const *,
		     struct mad_synth const *);
  int (*error_func)(void *, struct mad_stream *, struct mad_frame *);
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

void mad_decoder_init(struct mad_decoder *, void *,
		      int (*)(void *, struct mad_stream *),
		      int (*)(void *, struct mad_frame *),
		      int (*)(void *, struct mad_frame const *,
			      struct mad_synth const *),
		      int (*)(void *, struct mad_stream *,
			      struct mad_frame *));
int mad_decoder_finish(struct mad_decoder *);

int mad_decoder_run(struct mad_decoder *, int);
int mad_decoder_command(struct mad_decoder *, union mad_control *);

# define MAD_DECODER_SYNC	0x0000
# define MAD_DECODER_ASYNC	0x0001

# define MAD_DECODER_CONTINUE	0x0000
# define MAD_DECODER_STOP	0x0010
# define MAD_DECODER_BREAK	0x0011
# define MAD_DECODER_IGNORE	0x0020

# endif

