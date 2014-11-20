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


#include "AMActionInfo3.h"

AMActionInfo3::AMActionInfo3(const QString &shortDescription, const QString & longDescription, const QString &iconFileName, QObject *parent)
	: AMDbObject(parent),
	  longDescription_(longDescription),
	  iconFileName_(iconFileName)
{
	setName(shortDescription);
	setShortDescription(shortDescription);
	expectedDuration_ = -1;
	canCopy_ = true;
}

AMActionInfo3::AMActionInfo3(const AMActionInfo3& other)
	: AMDbObject(),
	  longDescription_(other.longDescription_),
	  iconFileName_(other.iconFileName_)
{
	setName(other.name());
	setShortDescription(other.shortDescription());
	expectedDuration_ = other.expectedDuration_;
	canCopy_ = other.canCopy();
}

