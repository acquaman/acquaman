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


#ifndef AMAXISINFO_H
#define AMAXISINFO_H

#include <QString>
#include "dataman/AMNumber.h"

class AMAxisInfo
{
public:
	/// Constructor.
	AMAxisInfo(const QString& mName, long mSize, const QString& mDescription = QString(), const QString& mUnits = QString() );
	/// Destructor.
	virtual ~AMAxisInfo();
	/// Size: number of elements along the axis
	long size;
	/// A unique (programmer-specific) name for the axis
	QString name;
	/// A human-readable description
	QString description, units;
	/// Indicates that the independent values along the axis are uniformly spaced with a constant starting value and increment
	bool isUniform;
	/// Relevent only when isUniform is true... these provide a fast way to compute the axis' independent values
	AMNumber start, increment;

	/// Include a reference to the axis' array of independent values here?

};

#endif // AMAXISINFO_H
