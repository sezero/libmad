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
 * $Id: timer.h,v 1.6 2000/03/19 06:43:39 rob Exp $
 */

# ifndef TIMER_H
# define TIMER_H

struct mad_timer {
  unsigned long seconds;		/* whole seconds */
  unsigned int parts36750;		/* 1/36750 seconds */
};

void mad_timer_init(struct mad_timer *);

# define mad_timer_finish(timer)  /* nothing */

void mad_timer_add(struct mad_timer *, struct mad_timer const *);
void mad_timer_str(struct mad_timer const *, char *, char const *, int);

# define mad_timer_seconds(timer)	((timer)->seconds)

# define MAD_TIMER_HOURS	0x0002
# define MAD_TIMER_MINUTES	0x0001
# define MAD_TIMER_SECONDS	0x0000

# define MAD_TIMER_DECISECONDS	0x0003
# define MAD_TIMER_CENTISECONDS	0x0004
# define MAD_TIMER_MILLISECONDS	0x0005

# endif

