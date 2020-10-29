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
 * $Id: bit.c,v 1.5 2000/03/03 02:24:33 rob Exp $
 */

# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif

# ifdef HAVE_LIMITS_H
#  include <limits.h>
# else
#  define CHAR_BIT  8
# endif

# include "bit.h"

void mad_bit_init(struct mad_bitptr *bits, unsigned char const *byte)
{
  bits->byte  = byte;
  bits->cache = 0;
  bits->left  = CHAR_BIT;
}

unsigned int mad_bit_length(struct mad_bitptr const *begin,
			    struct mad_bitptr const *end)
{
  return begin->left +
    CHAR_BIT * (end->byte - (begin->byte + 1)) + (CHAR_BIT - end->left);
}

/*
 * NAME:	bit->byte()
 * DESCRIPTION:	return pointer to next unprocessed byte
 */
unsigned char const *mad_bit_byte(struct mad_bitptr const *bits)
{
  return bits->left == CHAR_BIT ? bits->byte : bits->byte + 1;
}

void mad_bit_seek(struct mad_bitptr *bits, unsigned int len)
{
  bits->byte += len / CHAR_BIT;
  bits->left -= len % CHAR_BIT;

  if (bits->left > CHAR_BIT) {
    bits->byte++;
    bits->left += CHAR_BIT;
  }

  if (bits->left < CHAR_BIT)
    bits->cache = *bits->byte;
}

unsigned long mad_bit_read(struct mad_bitptr *bits, unsigned int len)
{
  register unsigned long value;

  if (bits->left == CHAR_BIT)
    bits->cache = *bits->byte;

  if (len < bits->left) {
    value = (bits->cache & ((1 << bits->left) - 1)) >> (bits->left - len);
    bits->left -= len;

    return value;
  }

  if (len == bits->left) {
    value = bits->cache & ((1 << len) - 1);

    bits->byte++;
    bits->left = CHAR_BIT;

    return value;
  }

  /* remaining bits in current byte */
  value = bits->cache & ((1 << bits->left) - 1);
  len  -= bits->left;

  bits->byte++;
  bits->left = CHAR_BIT;

  while (len >= CHAR_BIT) {
    value = (value << CHAR_BIT) | *bits->byte++;
    len  -= CHAR_BIT;
  }

  if (len > 0) {
    bits->cache = *bits->byte;

    value = (value << len) | (bits->cache >> (CHAR_BIT - len));
    bits->left -= len;
  }

  return value;
}
