#include "XRFDetectorInfo.h"

XRFDetectorInfo::XRFDetectorInfo(const QString &name, const QString &description, QObject *parent)
	: AMDetectorInfo(name, description, parent)
{
	setUnits("Counts");
}

XRFDetectorInfo::XRFDetectorInfo(const XRFDetectorInfo &original)
	: AMDetectorInfo(original.name(), original.description(), original.parent())
{
	retreiveAndSetProperties(original);
}

XRFDetectorInfo &XRFDetectorInfo::operator =(const XRFDetectorInfo &other)
{
	if (this != &other)
		retreiveAndSetProperties(other);
	return *this;
}

void XRFDetectorInfo::setMaximumEnergy(double energy)
{
	double newScale = energy*1000/size().i();
	maxEnergy_ = energy;

	for (int i = 0; i < roiInfoList()->count(); i++)
		(*roiInfoList())[i].setScale(newScale);

	setModified(true);
}
