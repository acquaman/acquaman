#include "AMAxisInfo.h"

#include "dataman/AMNumber.h"

AMAxisInfo::AMAxisInfo(const QString& mName, int mSize, const QString& mDescription, const QString& mUnits)
{
	name = mName;
	size = mSize;
	description = mDescription;
	units = mUnits;
	isUniform = false;
}
