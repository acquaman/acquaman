#include "AMStepScanConfiguration.h"

AMStepScanConfiguration::AMStepScanConfiguration(QObject *parent)
	: AMScanConfiguration(parent)
{
}

AMStepScanConfiguration::AMStepScanConfiguration(const AMStepScanConfiguration &original)
	: AMScanConfiguration()
{

}

AMStepScanConfiguration::~AMStepScanConfiguration()
{

}

AMDbObjectList AMStepScanConfiguration::dbReadScanAxes()
{

}

void AMStepScanConfiguration::dbLoadScanAxes(const AMDbObjectList &newScanAxes)
{

}
