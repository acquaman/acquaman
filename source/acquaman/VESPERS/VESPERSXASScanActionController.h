#ifndef VESPERSXASSCANACTIONCONTROLLER_H
#define VESPERSXASSCANACTIONCONTROLLER_H

#include "acquaman/AMRegionScanActionController.h"
#include "acquaman/VESPERS/VESPERSEXAFSScanConfiguration.h"

/// VESPERS specific subclass for X-ray Absorption Spectroscopy.
class VESPERSXASScanActionController : public AMRegionScanActionController
{
	Q_OBJECT

public:
	/// Constructor.  Builds a runnable scan action controller for the VESPERS beamline.
	VESPERSXASScanActionController(VESPERSEXAFSScanConfiguration *configuration, QObject *parent = 0);

protected:
	/// Reimplemented to provide actions that will setup the beamline for optimized operation of the XAS scan.
	AMAction3* createInitializationActions();
	/// Reimplemented to put the beamline in a good state after a scan has been completed.
	AMAction3* createCleanupActions();

	/// Adds anything extra (eg: analysis blocks) to the scan before it's started.
	virtual void buildScanControllerImplementation() {}

protected:
	/// Specific scan configuration with all the VESPERS specific information inside.
	VESPERSEXAFSScanConfiguration *configuration_;
};

#endif // VESPERSXASSCANACTIONCONTROLLER_H
