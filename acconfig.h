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
 * $Id: acconfig.h,v 1.3 2000/09/17 18:52:18 rob Exp $
 */

/*****************************************************************************
 * Definitions selected automatically by `configure'                         *
 *****************************************************************************/
@TOP@

/* Define to optimize for speed over accuracy. */
#undef OPT_SPEED

/* Define to optimize for accuracy over speed. */
#undef OPT_ACCURACY

/* Define to enable a Layer III intensity stereo kluge. */
#undef OPT_ISKLUGE

/* Define to enable a fast subband synthesis approximation optimization. */
#undef OPT_SSO

/* Define to enable diagnostic debugging support. */
#undef DEBUG

/* Define to disable debugging assertions. */
#undef NDEBUG

/* Define to enable experimental code. */
#undef EXPERIMENTAL

@BOTTOM@

/*****************************************************************************
 * End of automatically configured definitions                               *
 *****************************************************************************/

# if defined(OPT_SPEED) && defined(OPT_ACCURACY)
#  error "cannot optimize for both speed and accuracy"
# endif

# if defined(OPT_SPEED) && !defined(OPT_SSO) && !defined(FPM_ARM)
#  define OPT_SSO 1
# endif

# if defined(DEBUG) && defined(NDEBUG)
#  error "cannot define both DEBUG and NDEBUG"
# endif

