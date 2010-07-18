#include "AMXASScanConfiguration.h"

/// Constructor simply passes arguments up to AMScanConfiguration constructor.
AMXASScanConfiguration::AMXASScanConfiguration(QObject *parent) : AMScanConfiguration(parent)
{
	regions_ = new AMXASRegionsList(this);
	connect(regions_, SIGNAL(regionsChanged()), this, SIGNAL(regionsChanged()));
}

