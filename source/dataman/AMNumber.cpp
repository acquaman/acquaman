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


#include "AMNumber.h"

/// Default constructor creates a Null AMNumber
 AMNumber::~AMNumber(){}
AMNumber::AMNumber() {
	type_ = Integer;
	state_ = Null;
}

/// Constructs an invalid number with a given State. (\c state should not be Valid, otherwise it will be changed to Null.  This constructor is useful, for ex, in returning an OutOfBoundsError number from an array accessed outside its range.
AMNumber::AMNumber(State errorState) {
	state_ = errorState;
	if(state_ == Valid)
		state_ = Null;
	type_ = Integer;
}

/// Conversion constructor from int
AMNumber::AMNumber(int fromInt) {
	type_ = Integer;
	value_.i = fromInt;
	state_ = Valid;
}

/// Conversion constructor from double
AMNumber::AMNumber(double fromDouble) {
	type_ = FloatingPoint;
	value_.d = fromDouble;
	state_ = Valid;
}

/// Assignment operator from double
AMNumber& AMNumber::operator=(double fromDouble) {
	type_ = FloatingPoint;
	value_.d = fromDouble;
	state_ = Valid;

	return *this;
}

/// Assignment operator from int
AMNumber& AMNumber::operator=(int fromInt) {
	type_ = Integer;
	value_.i = fromInt;
	state_ = Valid;

	return *this;
}

QString AMNumber::toString(QChar format, int precision) {
	if(!isValid())
		return "[X]";
	if(type_ == Integer)
		return QString("%1").arg(value_.i);
	else
		return QString("%1").arg(value_.d, 0, format.toAscii(), precision);
}

QString AMNumber::toString(int precision)
{
	// This may be removed once toString usage is standardized across all exporters.
	if(precision <= 0)
		precision = 19;

	QChar format = 'g';
	return AMNumber::toString(format, precision);
}
