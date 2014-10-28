#ifndef SXRMB2DSCANACTIONCONTROLLER_H
#define SXRMB2DSCANACTIONCONTROLLER_H

#include "acquaman/AMStepScanActionController.h"
#include "acquaman/SXRMB/SXRMB2DMapScanConfiguration.h"

class SXRMB2DScanActionController : public AMStepScanActionController
{
	Q_OBJECT

public:
	/// Constructor.  Builds a runnable scan action controller for the SXRMB beamline for 2D mapping.
	SXRMB2DScanActionController(SXRMB2DMapScanConfiguration *configuration, QObject *parent = 0);
	/// Destructor.
	virtual ~SXRMB2DScanActionController() {}

protected:
	/// Reimplementned to provide actions that will setup the beamline for optimized operation of the 2D scan.
	AMAction3 *createInitializationActions();
	/// Reimplemented to put the beamline in a good state after a scan has been completed.
	AMAction3 *createCleanupActions();

	/// Adds anything extra (eg: analysis blocks) to the scan before it's started.
	virtual void buildScanControllerImplementation();
	/// Creates the axis order for higher dimensional scans.  The default order is the axis order, but if a different order is desired, then you should reimplement this in subclasses.
	virtual void createAxisOrderMap();

	/// Specific scan configuration with all the VESPERS specific information inside.
	SXRMB2DMapScanConfiguration *configuration_;
};

#endif // SXRMB2DSCANACTIONCONTROLLER_H
