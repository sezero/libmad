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
 * $Id: frame.c,v 1.6 2000/09/24 17:49:38 rob Exp $
 */

# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif

# include <stdlib.h>

# include "bit.h"
# include "stream.h"
# include "frame.h"
# include "timer.h"
# include "layer12.h"
# include "layer3.h"

static
unsigned long const bitrate_table[5][15] = {
  /* MPEG 1 */
  { 0,  32000,  64000,  96000, 128000, 160000, 192000, 224000,  /* Layer I   */
       256000, 288000, 320000, 352000, 384000, 416000, 448000 },
  { 0,  32000,  48000,  56000,  64000,  80000,  96000, 112000,  /* Layer II  */
       128000, 160000, 192000, 224000, 256000, 320000, 384000 },
  { 0,  32000,  40000,  48000,  56000,  64000,  80000,  96000,  /* Layer III */
       112000, 128000, 160000, 192000, 224000, 256000, 320000 },

  /* MPEG 2 LSF */
  { 0,  32000,  48000,  56000,  64000,  80000,  96000, 112000,  /* Layer I   */
       128000, 144000, 160000, 176000, 192000, 224000, 256000 },
  { 0,   8000,  16000,  24000,  32000,  40000,  48000,  56000,  /* Layers    */
        64000,  80000,  96000, 112000, 128000, 144000, 160000 } /* II & III  */
};

static
unsigned int const sfreq_table[3] = { 44100, 48000, 32000 };

static
int (*const decoder_table[3])(struct mad_stream *, struct mad_frame *,
			      unsigned short const [2]) = {
  mad_layer_I,
  mad_layer_II,
  mad_layer_III
};

/*
 * NAME:	frame->init()
 * DESCRIPTION:	initialize frame struct
 */
void mad_frame_init(struct mad_frame *frame)
{
  frame->layer    = 0;
  frame->mode     = 0;
  frame->mode_ext = 0;
  frame->emphasis = 0;

  frame->bitrate  = 0;
  frame->sfreq    = 0;

  frame->flags    = 0;
  frame->private  = 0;

  mad_timer_init(&frame->duration);

  mad_frame_mute(frame);
  frame->overlap = 0;
}

/*
 * NAME:	frame->finish()
 * DESCRIPTION:	deallocate any dynamic memory associated with frame
 */
void mad_frame_finish(struct mad_frame *frame)
{
  if (frame->overlap) {
    free(frame->overlap);
    frame->overlap = 0;
  }

  mad_timer_finish(&frame->duration);
}

/*
 * NAME:	frame->mute()
 * DESCRIPTION:	zero all subband values so the frame becomes silent
 */
void mad_frame_mute(struct mad_frame *frame)
{
  unsigned int s, sb;

  for (s = 0; s < 36; ++s) {
    for (sb = 0; sb < 32; ++sb)
      frame->sbsample[0][s][sb] =
      frame->sbsample[1][s][sb] = 0;
  }
}

/*
 * NAME:	frame->header()
 * DESCRIPTION:	read the next frame header from the stream
 */
int mad_frame_header(struct mad_frame *frame, struct mad_stream *stream,
		     unsigned short *crc)
{
  register unsigned char const *ptr, *end;
  struct mad_bitptr header;
  unsigned int pad_slot, index;

  ptr = stream->next_frame;
  end = stream->bufend;

  if (ptr == 0) {
    stream->error = MAD_ERR_BUFPTR;
    goto fail;
  }

  /* stream skip */
  if (stream->skiplen) {
    if (!stream->sync)
      ptr = stream->this_frame;

    if (end - ptr < stream->skiplen) {
      stream->skiplen   -= end - ptr;
      stream->next_frame = end;

      stream->error = MAD_ERR_BUFLEN;
      goto fail;
    }

    ptr += stream->skiplen;
    stream->skiplen = 0;

    stream->sync = 1;
  }

 sync:
  /* synchronize */
  if (stream->sync) {
    if (end - ptr < 4) {
      stream->next_frame = ptr;

      stream->error = MAD_ERR_BUFLEN;
      goto fail;
    }
    else if (!(ptr[0] == 0xff && (ptr[1] & 0xf0) == 0xf0)) {
      /* mark point where frame sync word was expected */
      stream->this_frame = ptr;
      stream->next_frame = ptr + 1;

      stream->error = MAD_ERR_LOSTSYNC;
      goto fail;
    }
  }
  else {
    mad_bit_init(&stream->ptr, ptr);

    if (mad_stream_sync(stream) == -1) {
      if (end - stream->next_frame >= 4)
	stream->next_frame = end - 4;

      stream->error = MAD_ERR_BUFLEN;
      goto fail;
    }

    ptr = mad_bit_nextbyte(&stream->ptr);
  }

  /* begin processing */
  stream->this_frame = ptr;
  stream->next_frame = ptr + 1;  /* possibly bogus sync word */

  /* skip syncword */
  mad_bit_init(&header, ptr + 1);
  mad_bit_skip(&header, 4);

  frame->flags   = 0;
  frame->private = 0;

  /* ID */
  if (mad_bit_read(&header, 1) == 0)
    frame->flags |= MAD_FLAG_LSF_EXT;

  /* layer */
  frame->layer = (4 - mad_bit_read(&header, 2)) & 3;

  if (frame->layer == 0) {
    stream->error = MAD_ERR_BADLAYER;
    goto fail;
  }

  /* protection_bit */
  if (mad_bit_read(&header, 1) == 0) {
    frame->flags |= MAD_FLAG_PROTECTION;

    if (crc)
      *crc = mad_bit_crc(header, 16, 0xffff);
  }

  /* bitrate_index */
  index = mad_bit_read(&header, 4);
  if (index == 15) {
    stream->error = MAD_ERR_BADBITRATE;
    goto fail;
  }

  if (frame->flags & MAD_FLAG_LSF_EXT)
    frame->bitrate = bitrate_table[3 + (frame->layer >> 1)][index];
  else
    frame->bitrate = bitrate_table[frame->layer - 1][index];

  /* sampling_frequency */
  index = mad_bit_read(&header, 2);
  if (index == 3) {
    stream->error = MAD_ERR_BADSAMPLEFREQ;
    goto fail;
  }

  frame->sfreq = sfreq_table[index];

  if (frame->flags & MAD_FLAG_LSF_EXT)
    frame->sfreq /= 2;

  /* padding_bit */
  pad_slot = mad_bit_read(&header, 1);
  if (pad_slot)
    frame->flags |= MAD_FLAG_PADDING;

  /* private_bit */
  if (mad_bit_read(&header, 1))
    frame->private |= MAD_PRIV_HEADER;

  /* mode */
  frame->mode = 3 - mad_bit_read(&header, 2);

  /* mode_extension */
  frame->mode_ext = mad_bit_read(&header, 2);

  /* copyright */
  if (mad_bit_read(&header, 1))
    frame->flags |= MAD_FLAG_COPYRIGHT;

  /* original/copy */
  if (mad_bit_read(&header, 1))
    frame->flags |= MAD_FLAG_ORIGINAL;

  /* emphasis */
  frame->emphasis = mad_bit_read(&header, 2);

  if (frame->emphasis == 2) {
    stream->error = MAD_ERR_BADEMPHASIS;
    goto fail;
  }

  /* set stream pointer */
  stream->ptr = header;

  /* calculate beginning of next frame */
  if (frame->bitrate == 0 && stream->sync)
    frame->bitrate = stream->freerate;

  if (frame->bitrate) {
    unsigned int N;

    if (frame->layer == 1)
      N = ((12 * frame->bitrate / frame->sfreq) + pad_slot) * 4;
    else {
      unsigned int slots_per_frame;

      slots_per_frame =
	(frame->layer == 3 && (frame->flags & MAD_FLAG_LSF_EXT)) ? 72 : 144;

      N = (slots_per_frame * frame->bitrate / frame->sfreq) + pad_slot;
    }

    /* verify there is enough data left in buffer to decode this frame */
    if (N + 4 > end - stream->this_frame) {
      stream->next_frame = stream->this_frame;

      stream->error = MAD_ERR_BUFLEN;
      goto fail;
    }

    stream->next_frame = stream->this_frame + N;

    if (!stream->sync) {
      /* check that a valid frame header follows this frame */

      ptr = stream->next_frame;
      if (!(ptr[0] == 0xff && (ptr[1] & 0xf0) == 0xf0)) {
	ptr = stream->next_frame = stream->this_frame + 1;
	goto sync;
      }

      stream->sync = 1;
    }
  }

  /* calculate frame duration */
  mad_timer_set(&frame->duration, 0,
		32 * MAD_NSBSAMPLES(frame), frame->sfreq);

  return 0;

 fail:
  stream->sync = 0;

  return -1;
}

/*
 * NAME:	free_bitrate()
 * DESCRIPTION:	attempt to discover the bitstream's free bitrate
 */
static
int free_bitrate(struct mad_stream *stream, struct mad_frame *frame)
{
  struct mad_bitptr keep_ptr;
  unsigned long rate = 0;
  unsigned int pad_slot, slots_per_frame;
  unsigned char const *ptr = 0;

  keep_ptr = stream->ptr;

  pad_slot = (frame->flags & MAD_FLAG_PADDING) ? 1 : 0;
  slots_per_frame =
    (frame->layer == 3 && (frame->flags & MAD_FLAG_LSF_EXT)) ? 72 : 144;

  while (mad_stream_sync(stream) == 0) {
    struct mad_stream peek_stream;
    struct mad_frame peek_frame;

    peek_stream = *stream;
    peek_frame  = *frame;

    peek_stream.sync       = 1;
    peek_stream.next_frame = mad_bit_nextbyte(&stream->ptr);

    if (mad_frame_header(&peek_frame, &peek_stream, 0) == 0 &&
	peek_frame.layer == frame->layer &&
	peek_frame.sfreq == frame->sfreq) {
      unsigned int N;

      ptr = mad_bit_nextbyte(&stream->ptr);

      N = ptr - stream->this_frame;

      if (frame->layer == 1) {
	rate = (unsigned long) frame->sfreq *
	  (N - 4 * pad_slot + 4) / 48 / 1000;
      }
      else {
	rate = (unsigned long) frame->sfreq *
	  (N - pad_slot + 1) / slots_per_frame / 1000;
      }

      break;
    }

    mad_bit_skip(&stream->ptr, 8);
  }

  stream->ptr = keep_ptr;

  if (rate == 0) {
    stream->error = MAD_ERR_LOSTSYNC;
    return -1;
  }

  stream->sync       = 1;
  stream->freerate   = rate * 1000;
  stream->next_frame = ptr;

  frame->bitrate     = stream->freerate;

# ifdef DEBUG
  fprintf(stderr, "free bitrate == %lu\n", rate * 1000);
# endif

  return 0;
}

/*
 * NAME:	frame->decode()
 * DESCRIPTION:	decode a single frame from a bitstream
 */
int mad_frame_decode(struct mad_frame *frame, struct mad_stream *stream)
{
  unsigned short crc[2];

  if (mad_frame_header(frame, stream, &crc[0]) == -1)
    goto fail;

  /* crc_check */
  if (frame->flags & MAD_FLAG_PROTECTION)
    crc[1] = mad_bit_read(&stream->ptr, 16);

  /* find free bitrate */
  if (frame->bitrate == 0 &&
      free_bitrate(stream, frame) == -1)
    goto fail;

  /* decode frame */
  if (decoder_table[frame->layer - 1](stream, frame, crc) == -1) {
    if (!MAD_RECOVERABLE(stream->error))
      stream->next_frame = stream->this_frame;

    goto fail;
  }

  /* designate ancillary bits */
  if (frame->layer != 3) {
    struct mad_bitptr next_frame;

    mad_bit_init(&next_frame, stream->next_frame);

    stream->anc_ptr    = stream->ptr;
    stream->anc_bitlen = mad_bit_length(&stream->ptr, &next_frame);
  }

  return 0;

 fail:
  stream->anc_bitlen = 0;
  return -1;
}
