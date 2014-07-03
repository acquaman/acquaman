/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#ifndef AMFONTSIZES_H
#define AMFONTSIZES_H

/// Provides pre-processor definitions of some standard font sizes, to account for the effective font size difference between Linux and Mac/Windows systems.  On most Linux, the default "regular" font is 10pt, vs. 12pt on Mac OS X.  (In the future, this might be better handled by replacing all style sheets embedded in the UI .cpp files with an application-wide stylesheet. This is an intermediate measure.)

#ifdef Q_OS_LINUX
#define AM_FONT_XLARGE 18
#define AM_FONT_XLARGE_ "18"
#define AM_FONT_LARGE 14
#define AM_FONT_LARGE_ "14"
#define AM_FONT_REGULAR 10
#define AM_FONT_REGULAR_ "10"
#define AM_FONT_SMALL 8
#define AM_FONT_SMALL_ "8"
#define AM_FONT_TINY 7
#define AM_FONT_TINY_ "7"
#else
#define AM_FONT_XLARGE 20
#define AM_FONT_XLARGE_ "20"
#define AM_FONT_LARGE 16
#define AM_FONT_LARGE_ "16"
#define AM_FONT_REGULAR 12
#define AM_FONT_REGULAR_ "12"
#define AM_FONT_SMALL 10
#define AM_FONT_SMALL_ "10"
#define AM_FONT_TINY 9
#define AM_FONT_TINY_ "9"
#endif

#endif // AMFONTSIZES_H
