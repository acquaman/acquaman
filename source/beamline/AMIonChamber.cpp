#include "AMIonChamber.h"

AMIonChamber::AMIonChamber(QObject *parent)
	: AMIonChamberInfo(""), AMDetector("")
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
