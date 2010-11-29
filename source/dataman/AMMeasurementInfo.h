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

	/// A unique, variable-style name for this measurement
	QString name;
	/// A human-readable description of this measurement
	QString description;
	/// Units of the measurement values themselves (ex: for a counting detector, could be "counts". For a current meter, could be "mA".)
	QString units;
	/// Number, size, and description of each axis of this measurement's dimensionality.  (For a scalar-value (0D) detector, this is just an empty list)
	QList<AMAxisInfo> axes;

	int rank() const { return axes.count(); }

	AMnDIndex size() const {
		AMnDIndex s;
		for(int i=0; i<axes.count(); i++)
			s.append(axes.at(i).size);
		return s;
	}
};


#endif // AMMEASUREMENTINFO_H
