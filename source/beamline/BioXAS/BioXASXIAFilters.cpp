#include "BioXASXIAFilters.h"
#include "util/AMErrorMonitor.h"
#include "actions3/AMActionSupport.h"

BioXASXIAFilters::BioXASXIAFilters(QObject *parent) :
	QObject(parent)
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

void BioXASXIAFilters::setConnected(bool isConnected)
{
	if (connected_ != isConnected) {
		connected_ = isConnected;
		emit connectedChanged(connected_);
	}
}

void BioXASXIAFilters::onConnectedChanged()
{
	bool filter1OK = (filter1_ && filter1_->isConnected());
	bool filter2OK = (filter2_ && filter2_->isConnected());
	bool filter3OK = (filter3_ && filter3_->isConnected());
	bool filter4OK = (filter4_ && filter4_->isConnected());

	setConnected(filter1OK && filter2OK && filter3OK && filter4OK);
}
