#include "XRFDetectorInfo.h"

XRFDetectorInfo::XRFDetectorInfo(const QString &name, const QString &description, QObject *parent)
	: AMDetectorInfo(name, description, parent)
{
	setUnits("Counts");
}
