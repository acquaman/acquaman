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


#ifndef AMMEASUREMENTINFO_H
#define AMMEASUREMENTINFO_H

#include <QString>
#include <QList>
#include "dataman/AMAxisInfo.h"
#include "dataman/AMnDIndex.h"

/// AMMeasurementInfo is lightweight value class, used by AMDataStore and AMDetectorInfo, etc. to describe the dimensionality and nature of a detector's measurement (rank, size, name, description, axis parameters)
class AMMeasurementInfo {
public:
	/// Create a measurement description by specifying the axes that it has. The default creates a 0-dimensional (scalar) measurement.
	AMMeasurementInfo(const QString& mName, const QString& mDescription, const QString& mUnits = QString(), const QList<AMAxisInfo>& mAxes = QList<AMAxisInfo>()) {
		name = mName;
		description = mDescription;
		units = mUnits;
		axes = mAxes;
	}
	/// Destructor.
	virtual ~AMMeasurementInfo(){}

	/// A unique, variable-style name for this measurement
	QString name;
	/// A human-readable description of this measurement
	QString description;
	/// Units of the measurement values themselves (ex: for a counting detector, could be "counts". For a current meter, could be "mA".)
	QString units;
	/// Number, size, and description of each axis of this measurement's dimensionality.  (For a scalar-value (0D) detector, this is just an empty list)
	QList<AMAxisInfo> axes;

	/// Returns the number of axes (ie: the number of dimensions in this measurement)
	int inline rank() const { return axes.count(); }

	/// Returns the length of all the axes
	AMnDIndex inline size() const {
		int rank = axes.count();
		AMnDIndex s(rank, AMnDIndex::DoNotInit);
		for(int i=0; i<rank; ++i)
			s[i] = axes.at(i).size;
		return s;
	}

	/// Returns the length of the specified axis \c axisId.  (\c axisId is assumed to be >= 0 and < rank().)
	long inline size(int axisId) const {
		return axes.at(axisId).size;
	}

	/// Returns the number of points this measurement spans (A scalar value is "1" point, a 1D Detector is the same as its dimension, higher-D detectors are the products of their dimensions)
	long inline spanSize() const {
		long aSize = 1;
		for(int i=axes.count()-1; i>=0; --i)
			aSize *= axes.at(i).size;
		return aSize;
	}
};


#endif // AMMEASUREMENTINFO_H
