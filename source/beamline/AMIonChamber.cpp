#include "AMIonChamber.h"

AMIonChamber::AMIonChamber(const QString &name, const QString &description, QObject *parent)
	: AMIonChamberInfo(name, description, parent), AMDetector(name)
{
}

bool AMIonChamber::setFromInfo(const AMDetectorInfo *info)
{
	const AMIonChamberInfo *ionChamberInfo = qobject_cast<const AMIonChamberInfo *>(info);

	if (!ionChamberInfo)
		return false;

	setVoltagRange(ionChamberInfo->voltageRange());

	return true;
}

bool AMIonChamber::setFromIonChamberInfo(const AMIonChamberInfo &info)
{
	setVoltagRange(info.voltageRange());

	return true;
}
