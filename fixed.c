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
 * $Id: fixed.c,v 1.1 2000/08/02 05:48:51 rob Exp $
 */

# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif

# include "fixed.h"

# if !defined(FPM_MACRO)
mad_fixed_t mad_f_mul(mad_fixed_t x, mad_fixed_t y)
{
  int neg;
  unsigned long A, B, C, D;
  mad_fixed_t prod;

  /* this is accurate but extremely slow */

  neg = (x < 0 && y > 0) ||
        (x > 0 && y < 0);

  if (x < 0)
    x = -x;
  if (y < 0)
    y = -y;

  A = (x >> 16) & 0xffff;
  B = (x >>  0) & 0xffff;
  C = (y >> 16) & 0xffff;
  D = (y >>  0) & 0xffff;

  prod = ((A * C) << 4) + ((A * D + B * C + ((B * D) >> 16)) >> 12);

  return neg ? -prod : prod;
}
# endif
