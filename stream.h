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
 * $Id: stream.h,v 1.1 2000/08/02 05:48:52 rob Exp $
 */

# ifndef STREAM_H
# define STREAM_H

# include "bit.h"

struct mad_stream {
  unsigned char const *buffer;		/* input bitstream buffer */
  unsigned char const *bufend;		/* end of buffer */
  unsigned long skiplen;		/* bytes to skip before next frame */

  int sync;				/* stream sync found */
  unsigned int freerate;		/* free bitrate (fixed) */

  unsigned char const *this_frame;	/* start of current frame */
  unsigned char const *next_frame;	/* start of next frame */
  struct mad_bitptr ptr;		/* current processing bit pointer */

  struct mad_bitptr anc_ptr;		/* ancillary bits pointer */
  unsigned int anc_bitlen;		/* number of ancillary bits */

  unsigned char (*main_data)[1935];	/* Layer III main_data */
  unsigned int md_len;			/* bytes in main_data */

  int error;				/* error code (see below) */
};

# define MAD_ERR_BUFLEN		0x0001	/* input buffer too small (or EOF) */
# define MAD_ERR_BUFPTR		0x0002	/* invalid (null) buffer pointer */

# define MAD_ERR_NOMEM		0x0031	/* not enough memory */

# define MAD_ERR_LOSTSYNC	0x0101	/* lost synchronization */
# define MAD_ERR_BADLAYER	0x0102	/* reserved header layer value */
# define MAD_ERR_BADBITRATE	0x0103	/* forbidden bitrate value */
# define MAD_ERR_BADSAMPLEFREQ	0x0104	/* reserved sample frequency value */
# define MAD_ERR_BADEMPHASIS	0x0105	/* reserved emphasis value */

# define MAD_ERR_BADCRC		0x0201	/* CRC check failed */
# define MAD_ERR_BADBITALLOC	0x0211	/* forbidden bit allocation value */
# define MAD_ERR_BADSCALEFACTOR	0x0221	/* bad scalefactor index */
# define MAD_ERR_BADFRAMELEN	0x0231	/* bad frame length */
# define MAD_ERR_BADBIGVALUES	0x0232	/* bad big_values count */
# define MAD_ERR_BADBLOCKTYPE	0x0233	/* reserved block_type */
# define MAD_ERR_BADDATAPTR	0x0234	/* bad main_data_begin pointer */
# define MAD_ERR_BADDATALEN	0x0235	/* bad main data length */
# define MAD_ERR_BADPART3LEN	0x0236	/* bad audio data length */
# define MAD_ERR_BADHUFFTABLE	0x0237	/* bad Huffman table select */
# define MAD_ERR_BADSTEREO	0x0238	/* incompatible block_type for M/S */

# define MAD_RECOVERABLE(error)	((error) & 0xff00)

void mad_stream_init(struct mad_stream *);
void mad_stream_finish(struct mad_stream *);

void mad_stream_buffer(struct mad_stream *,
		       unsigned char const *, unsigned long);
void mad_stream_skip(struct mad_stream *, unsigned long);

int mad_stream_sync(struct mad_stream *);

# endif

