#include "REIXSXESScanConfiguration.h"

REIXSXESScanConfiguration::REIXSXESScanConfiguration(QObject *parent) :
	AMScanConfiguration(parent), mcpDetectorInfo_()
{
	gratingNumber_ = 0;
	centerEV_ = 400;
	defocusDistanceMm_ = 0;
	spectrometerCalibrationId_ = -1;
	shouldStartFromCurrentPosition_ = false;
}

// Returns a pointer to a newly-created copy of this scan configuration.  (It takes the role of a copy constructor, but is virtual so that our high-level classes can copy a scan configuration without knowing exactly what kind it is.)
AMScanConfiguration* REIXSXESScanConfiguration::createCopy() const {
	return new REIXSXESScanConfiguration(*this);	// can use the default auto-generated copy-constructor.
}

#include "acquaman/REIXS/REIXSXESScanController.h"

// Returns a pointer to a newly-created AMScanController that is appropriate for executing this scan configuration.
AMScanController* REIXSXESScanConfiguration::createController() {
	return new REIXSXESScanController(this);
}
