#include "REIXSXESMCPDetectorInfo.h"

REIXSXESMCPDetectorInfo::REIXSXESMCPDetectorInfo(const QString& name, const QString& description, QObject *parent) :
	AMDetectorInfo(name, description, parent)
{
	setUnits("counts");
}
