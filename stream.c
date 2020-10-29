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
 * $Id: stream.c,v 1.1 2000/03/05 05:19:17 rob Exp $
 */

# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif

# include "stream.h"
# include "bit.h"

void mad_stream_init(struct mad_stream *stream)
{
  stream->buffer     = 0;
  stream->bufend     = 0;

  stream->sync       = 0;
  stream->freerate   = 0;

  stream->this_frame = 0;
  stream->next_frame = 0;
  mad_bit_init(&stream->ptr, 0);

  mad_bit_init(&stream->anc_ptr, 0);
  stream->anc_bitlen = 0;

  stream->md_len     = 0;

  stream->error      = 0;
}

void mad_stream_buffer(struct mad_stream *stream,
		       unsigned char const *buf, unsigned long len)
{
  stream->buffer = buf;
  stream->bufend = buf + len;

  stream->this_frame = 0;
  stream->next_frame = buf;

  stream->sync = 0;

  mad_bit_init(&stream->ptr, 0);
}
