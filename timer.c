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
 * $Id: timer.c,v 1.4 2000/03/05 05:19:17 rob Exp $
 */

# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif

# include <stdio.h>

# include "timer.h"

void mad_timer_init(struct mad_timer *timer)
{
  timer->seconds    = 0;
  timer->parts36750 = 0;
}

void mad_timer_add(struct mad_timer *timer, struct mad_timer const *incr)
{
  timer->seconds    += incr->seconds;
  timer->parts36750 += incr->parts36750;

  timer->seconds    += timer->parts36750 / 36750;
  timer->parts36750 %= 36750;
}

void mad_timer_str(struct mad_timer const *timer,
		   char *dest, char const *format, int resolution)
{
  unsigned int hours, minutes, seconds, tenths;

  tenths = timer->parts36750 / 3675;

  switch (resolution) {
  case timer_hours:
    seconds = timer->seconds % 60;
    minutes = (timer->seconds / 60) % 60;
    hours   = (timer->seconds / 60) / 60;

    sprintf(dest, format, hours, minutes, seconds, tenths);
    break;

  case timer_minutes:
    seconds = timer->seconds % 60;
    minutes = timer->seconds / 60;

    sprintf(dest, format, minutes, seconds, tenths);
    break;

  case timer_seconds:
    seconds = timer->seconds;

    sprintf(dest, format, seconds, tenths);
    break;
  }
}
