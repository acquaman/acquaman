#include "CLSAmptekSDD123DetectorInfo.h"

CLSAmptekSDD123DetectorInfo::CLSAmptekSDD123DetectorInfo(const QString &name, const QString &description, QObject *parent) :
	AMDetectorInfo(name, description, parent)
{
	setUnits("Counts");
}

CLSAmptekSDD123DetectorInfo::CLSAmptekSDD123DetectorInfo(const CLSAmptekSDD123DetectorInfo &original) :
	AMDetectorInfo(original.name(), original.description(), original.parent())
{
	retreiveAndSetProperties(original);
}

CLSAmptekSDD123DetectorInfo& CLSAmptekSDD123DetectorInfo::operator =(const CLSAmptekSDD123DetectorInfo &other)
{
	if(this != &other)
		retreiveAndSetProperties(other);
	return *this;
}
