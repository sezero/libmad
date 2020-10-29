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
 * $Id: timer.c,v 1.4 2000/09/24 17:49:38 rob Exp $
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
  timer->seconds  = 0;
  timer->fraction = 0;
}

/*
 * NAME:	reduce()
 * DESCRIPTION:	carry timer fraction into seconds
 */
static
void reduce(struct mad_timer *timer)
{
  timer->seconds  += timer->fraction / MAD_TIMER_FRACPARTS;
  timer->fraction %= MAD_TIMER_FRACPARTS;
}

/*
 * NAME:	timer->set()
 * DESCRIPTION:	set timer to specific value
 */
void mad_timer_set(struct mad_timer *timer, unsigned long seconds,
		   unsigned long fraction, unsigned long fracparts)
{
  timer->seconds = seconds;

  if (fraction == 0)
    fracparts = 0;
  else if (fracparts == 0) {
    fracparts = fraction;
    fraction  = 1;
  }

  switch (fracparts) {
  case 0:
    timer->fraction = 0;
    break;

  case MAD_TIMER_FRACPARTS:
    timer->fraction = fraction;
    break;

  case 16000:
    timer->fraction = fraction * (MAD_TIMER_FRACPARTS / 16000);
    break;

  case 22050:
    timer->fraction = fraction * (MAD_TIMER_FRACPARTS / 22050);
    break;

  case 24000:
    timer->fraction = fraction * (MAD_TIMER_FRACPARTS / 24000);
    break;

  case 32000:
    timer->fraction = fraction * (MAD_TIMER_FRACPARTS / 32000);
    break;

  case 44100:
    timer->fraction = fraction * (MAD_TIMER_FRACPARTS / 44100);
    break;

  case 48000:
    timer->fraction = fraction * (MAD_TIMER_FRACPARTS / 48000);
    break;

  default:
    /* reduce fraction to avoid overflow */
    while (fraction % 2 == 0 && fracparts % 2 == 0)
      fraction /= 2, fracparts /= 2;
    while (fraction % 3 == 0 && fracparts % 3 == 0)
      fraction /= 3, fracparts /= 3;
    while (fraction % 5 == 0 && fracparts % 5 == 0)
      fraction /= 5, fracparts /= 5;
    while (fraction % 7 == 0 && fracparts % 7 == 0)
      fraction /= 7, fracparts /= 7;

    if (fracparts < MAD_TIMER_FRACPARTS) {
      timer->fraction = fraction * (MAD_TIMER_FRACPARTS / fracparts) +
	fraction * (MAD_TIMER_FRACPARTS % fracparts) / fracparts;
    }
    else {
      timer->fraction = MAD_TIMER_FRACPARTS * (fraction / fracparts) +
	MAD_TIMER_FRACPARTS * (fraction % fracparts) / fracparts;
    }
  }

  if (timer->fraction >= MAD_TIMER_FRACPARTS)
    reduce(timer);
}

/*
 * NAME:	timer->add()
 * DESCRIPTION:	add one timer to another
 */
void mad_timer_add(struct mad_timer *timer, struct mad_timer const *incr)
{
  timer->seconds  += incr->seconds;
  timer->fraction += incr->fraction;

  if (timer->fraction >= MAD_TIMER_FRACPARTS)
    reduce(timer);
}

/*
 * NAME:	timer->compare()
 * DESCRIPTION:	indicate relative order of two timers
 */
int mad_timer_compare(struct mad_timer const *timer1,
		      struct mad_timer const *timer2)
{
  signed long diff;

  diff = timer1->seconds - timer2->seconds;
  if (diff < 0)
    return -1;
  else if (diff > 0)
    return +1;

  diff = timer1->fraction - timer2->fraction;
  if (diff < 0)
    return -1;
  else if (diff > 0)
    return +1;

  return 0;
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
  tenths  = timer->fraction / (MAD_TIMER_FRACPARTS / 10);

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

  default:
    /* unsupported resolution */
    *dest = 0;
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
    return timer->seconds * 10 +
      timer->fraction / (MAD_TIMER_FRACPARTS / 10);

  case MAD_TIMER_CENTISECONDS:
    return timer->seconds * 100 +
      timer->fraction / (MAD_TIMER_FRACPARTS / 100);

  case MAD_TIMER_MILLISECONDS:
    return timer->seconds * 1000 +
      timer->fraction / (MAD_TIMER_FRACPARTS / 1000);
  }

  /* unsupported units */
  return 0;
}

/*
 * NAME:	timer->fraction()
 * DESCRIPTION:	return fractional part of timer in arbitrary terms
 */
unsigned long mad_timer_fraction(struct mad_timer const *timer,
				 unsigned long fracparts)
{
  switch (fracparts) {
  case 0:
    return MAD_TIMER_FRACPARTS / timer->fraction;

  case MAD_TIMER_FRACPARTS:
    return timer->fraction;

  case 16000:
    return timer->fraction * 16000 / MAD_TIMER_FRACPARTS;

  case 22050:
    return timer->fraction * 22050 / MAD_TIMER_FRACPARTS;

  case 24000:
    return timer->fraction * 24000 / MAD_TIMER_FRACPARTS;

  case 32000:
    return timer->fraction * 32000 / MAD_TIMER_FRACPARTS;

  case 44100:
    return timer->fraction * 44100 / MAD_TIMER_FRACPARTS;

  case 48000:
    return timer->fraction * 48000 / MAD_TIMER_FRACPARTS;

  default:
    return timer->fraction * fracparts / MAD_TIMER_FRACPARTS;
  }
}
