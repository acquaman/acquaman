#include "AMIonChamber.h"

AMIonChamber::AMIonChamber(const QString &name, const QString &description, QObject *parent)
	: AMIonChamberInfo(name, description, parent), AMDetector(name)
{
}

void AMIonChamber::toInfo()
{
	AMIonChamberInfo info(name());
	info.setVoltagRange();
}

bool AMIonChamber::setFromInfo(const AMDetectorInfo *info)
{
	AMIonChamberInfo *info = qobject_cast<AMIonChamberInfo *>(info);

	if (!info)
		return false;

	setVoltagRange(info->voltageRange());

	return true;
}

bool AMIonChamber::setFromIonChamberInfo(const AMIonChamberInfo &info)
{
	setVoltagRange(info.voltageRange());

	return true;
}
