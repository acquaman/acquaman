/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").
Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef AMLIKELY_H
#define AMLIKELY_H

/// Macros to use as hints for branch prediction in if statements:
#ifdef __GNUC__
/// Tells the compiler that the expression \c x is likely to be true.  Provides hints for branch optimization.
#define AM_LIKELY(x)       __builtin_expect((x),1)
/// Tells the compiler that the expression \c x is not likely to be true.  Provies hints for branch optimization.
#define AM_UNLIKELY(x)     __builtin_expect((x),0)
#else
#define AM_LIKELY(x)       (x)
#define AM_UNLIKELY(x)     (x)
#endif

#endif // AMLIKELY_H
