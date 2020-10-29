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
 * $Id: mad.h,v 1.4 2000/03/05 07:31:55 rob Exp $
 */

# ifndef MAD_H
# define MAD_H

# include "bit.h"
# include "stream.h"
# include "frame.h"

unsigned short mad_crc(unsigned short, struct mad_bitptr, unsigned int);
int mad_decode(struct mad_stream *, struct mad_frame *);

# endif

