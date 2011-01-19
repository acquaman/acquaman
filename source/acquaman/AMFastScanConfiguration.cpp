#include "AMFastScanConfiguration.h"

AMFastScanConfiguration::AMFastScanConfiguration(QObject *parent) :
    AMScanConfiguration(parent)
{
	start_ = -1;
	end_ = -1;
}
