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
 * $Id: timer.c,v 1.2 2000/09/07 22:29:38 rob Exp $
 */

# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif

# include <stdio.h>

# include "timer.h"

/*
 * NAME:	timer->init()
 * DESCRIPTION:	initialize timer struct
 */
void mad_timer_init(struct mad_timer *timer)
{
  timer->seconds    = 0;
  timer->parts36750 = 0;
}

/*
 * NAME:	timer->add()
 * DESCRIPTION:	add one timer to another
 */
void mad_timer_add(struct mad_timer *timer, struct mad_timer const *incr)
{
  timer->seconds    += incr->seconds;
  timer->parts36750 += incr->parts36750;

  /* reduce parts36750 */
  timer->seconds    += timer->parts36750 / 36750;
  timer->parts36750 %= 36750;
}

/*
 * NAME:	timer->str()
 * DESCRIPTION:	write a string representation of a timer using a template
 */
void mad_timer_str(struct mad_timer const *timer,
		   char *dest, char const *format, int resolution)
{
  unsigned int hours, minutes, seconds, tenths;

  seconds = timer->seconds;
  tenths  = timer->parts36750 / 3675;

  switch (resolution) {
  case MAD_TIMER_HOURS:
    minutes = seconds / 60;
    hours   = minutes / 60;

    sprintf(dest, format, hours, minutes % 60, seconds % 60, tenths);
    break;

  case MAD_TIMER_MINUTES:
    minutes = seconds / 60;

    sprintf(dest, format, minutes, seconds % 60, tenths);
    break;

  case MAD_TIMER_SECONDS:
    sprintf(dest, format, seconds, tenths);
    break;
  }
}

/*
 * NAME:	timer->count()
 * DESCRIPTION:	return timer value in selected units
 */
unsigned long mad_timer_count(struct mad_timer const *timer, int units)
{
  switch (units) {
  case MAD_TIMER_HOURS:
    return timer->seconds / 60 / 60;
  case MAD_TIMER_MINUTES:
    return timer->seconds / 60;
  case MAD_TIMER_SECONDS:
    return timer->seconds;
  case MAD_TIMER_DECISECONDS:
    return timer->seconds *   10 + timer->parts36750 / 3675;
  case MAD_TIMER_CENTISECONDS:
    return timer->seconds *  100 + timer->parts36750 /  368;
  case MAD_TIMER_MILLISECONDS:
    return timer->seconds * 1000 + timer->parts36750 /   37;
  }

  return 0;
}
