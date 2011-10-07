#include "AMIonChamberInfo.h"

AMIonChamberInfo::AMIonChamberInfo(const QString &name, const QString &description, QObject *parent)
	: AMDetectorInfo(name, description, parent)
{
}

AMIonChamberInfo::AMIonChamberInfo(const AMIonChamberInfo &original)
	: AMDetectorInfo(original.name(), original.description(), original.parent())
{
	retreiveAndSetProperties(original);
}
