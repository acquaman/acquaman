#include "BioXASCarbonFilterFarm.h"
#include "util/AMErrorMonitor.h"

BioXASCarbonFilterFarm::BioXASCarbonFilterFarm(const QString &name, const QString &pvBase, QObject *parent) :
	QObject(parent)
{
	// Initialize local variables.

	name_ = name;
	connected_ = false;
	filter_ = 0;

	// Create components.


}
