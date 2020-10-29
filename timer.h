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
 * $Id: timer.h,v 1.4 2000/03/05 07:31:55 rob Exp $
 */

# ifndef TIMER_H
# define TIMER_H

enum {
  timer_hours,
  timer_minutes,
  timer_seconds
};

struct mad_timer {
  unsigned long seconds;		/* whole seconds */
  unsigned int parts36750;		/* 1/36750 seconds */
};

void mad_timer_init(struct mad_timer *);
void mad_timer_add(struct mad_timer *, struct mad_timer const *);
void mad_timer_str(struct mad_timer const *, char *, char const *, int);

# endif

