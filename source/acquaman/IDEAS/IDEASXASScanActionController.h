#ifndef IDEASXASSCANACTIONCONTROLLER_H
#define IDEASXASSCANACTIONCONTROLLER_H

#include "acquaman/AMRegionScanActionController.h"
#include "acquaman/IDEAS/IDEASXASScanConfiguration.h"

class IDEASXASScanActionController : public AMRegionScanActionController
{
	Q_OBJECT

public:
	/// Constructor.  Builds a runnable scan action controller for the SGM beamline.
	IDEASXASScanActionController(IDEASXASScanConfiguration *cfg, QObject *parent = 0);
	virtual ~IDEASXASScanActionController();

protected:
	/// Reimplemented to provide actions that will setupd the beamine for optimzed operation of the XAS scan.
	AMAction3* createInitializationActions();
	/// Reimplemented to put the beamline in a good state after a scan has been completed.
	AMAction3* createCleanupActions();

	/// Sets the scan axis and adds anything extra.
	virtual void buildScanControllerImplementation();

protected:
	/// Specific scan configuration with all the SGM specific information inside.
	IDEASXASScanConfiguration *configuration_;
};

#endif // IDEASXASSCANACTIONCONTROLLER_H
