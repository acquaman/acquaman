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


#include "AMRange.h"

 AMRange::~AMRange(){}
AMRange::AMRange(double minimum, double maximum)
{
	isNull_ = false;
	minimum_ = minimum;
	maximum_ = maximum;
}

AMRange::AMRange(const AMRange &original)
{
	isNull_ = original.isNull();
	minimum_ = original.minimum();
	maximum_ = original.maximum();
}

AMRange::AMRange()
{
	isNull_ = true;
	minimum_ = -1.0;
	maximum_ = -1.0;
}

void AMRange::setRange(double newMinimum, double newMaximum)
{
	isNull_ = false;
	minimum_ = newMinimum;
	maximum_ = newMaximum;
}

void AMRange::setMinimum(double newValue)
{
	isNull_ = false;
	minimum_ = newValue;
}
void AMRange::setMaximum(double newValue)
{
	isNull_ = false;
	maximum_ = newValue;
}

AMRange &AMRange::operator =(const AMRange &other)
{
	isNull_ = other.isNull();
	minimum_ = other.minimum();
	maximum_ = other.maximum();

	return *this;
}

bool AMRange::operator ==(const AMRange &other) const
{
	return (isNull_ == other.isNull())
			&& (minimum_ == other.minimum())
			&& (maximum_ == other.maximum());
}

bool AMRange::operator !=(const AMRange &other) const
{
	return !(this->operator ==(other));
}
