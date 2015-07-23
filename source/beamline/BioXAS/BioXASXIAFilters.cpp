#include "BioXASXIAFilters.h"
#include "util/AMErrorMonitor.h"
#include "actions3/AMActionSupport.h"

BioXASXIAFilters::BioXASXIAFilters(const QString &name, QObject *parent) :
	BioXASBeamlineComponent(name, parent)
{
	// Initialize local variables.

	connected_ = false;

	// As of right now, the subclass is responsible for instantiating the filter controls and making the appropriate connections.

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
				filter1_ && filter1_->isConnected() &&
				filter2_ && filter2_->isConnected() &&
				filter3_ && filter3_->isConnected() &&
				filter4_ && filter4_->isConnected()
				);

	return isConnected;
}
