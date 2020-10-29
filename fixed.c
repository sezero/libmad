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
 * $Id: fixed.c,v 1.8 2000/04/22 04:36:50 rob Exp $
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

# ifdef DEBUG
mad_fixed_t mad_f_abs(mad_fixed_t x)
{
  return (x < 0) ? -x : x;
}

mad_fixed_t mad_f_tofixed(double d)
{
  mad_fixed_t f;
  int neg, i;
  double part;

  neg = d < 0.0;
  if (neg)
    d = -d;

  f = 0;
  part = 4.0;

  for (i = 0; i < 31; ++i) {
    f <<= 1;

    if (d >= part) {
      d -= part;
      f |= 1;
    }

    part /= 2.0;
  }

  if (d >= part)
    ++f;

  return neg ? -f : f;
}

double mad_f_todouble(mad_fixed_t f)
{
  double d, part;
  int neg, i;

  neg = f < 0;
  if (neg)
    f = -f;

  d = 0.0;
  part = 4.0;

  for (i = 0; i < 31; ++i) {
    if (f & 0x40000000L)
      d += part;

    part /= 2.0;
    f <<= 1;
  }

  return neg ? -d : d;
}
# endif
