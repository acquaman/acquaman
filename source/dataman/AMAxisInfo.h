#ifndef AMAXISINFO_H
#define AMAXISINFO_H

#include <QString>
#include "dataman/AMNumber.h"

class AMAxisInfo
{
public:
	AMAxisInfo(const QString& mName, int mSize, const QString& mDescription = QString(), const QString& mUnits = QString() );
	/// Size / number of elements along the axis
	int size;
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
