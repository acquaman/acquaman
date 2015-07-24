#include "BioXASXIAFilters.h"
#include "util/AMErrorMonitor.h"
#include "actions3/AMActionSupport.h"

BioXASXIAFilters::BioXASXIAFilters(const QString &name, QObject *parent) :
	BioXASBeamlineComponent(name, parent)
{
	// Initialize member variables.

	filtersControl_ = 0;

	filter1_ = 0;
	filter2_ = 0;
	filter3_ = 0;
	filter4_ = 0;
}

BioXASXIAFilters::~BioXASXIAFilters()
{

}

bool BioXASXIAFilters::isConnected() const
{
	bool isConnected = (
				filtersControl_ && filtersControl_->isConnected()
				);

	return isConnected;
}
