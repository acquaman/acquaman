#ifndef SGMXASSCANACTIONCONTROLLER_H
#define SGMXASSCANACTIONCONTROLLER_H

#include "acquaman/AMRegionScanActionController.h"
#include "acquaman/SGM/SGMXASScanConfiguration2013.h"

/// SGM specific subclass for X-ray Absorption Spectroscopy.
class SGMXASScanActionController : public AMRegionScanActionController
{
	Q_OBJECT

public:
	/// Constructor.  Builds a runnable scan action controller for the SGM beamline.
 	virtual ~SGMXASScanActionController();
	SGMXASScanActionController(SGMXASScanConfiguration2013 *cfg, QObject *parent = 0);

protected:
	/// Reimplemented to provide actions that will setupd the beamine for optimzed operation of the XAS scan.
	AMAction3* createInitializationActions();
	/// Reimplemented to put the beamline in a good state after a scan has been completed.
	AMAction3* createCleanupActions();

	/// Sets the scan axis and adds anything extra.
	virtual void buildScanControllerImplementation();

protected:
	/// Specific scan configuration with all the SGM specific information inside.
	SGMXASScanConfiguration2013 *configuration_;

	/// Helper function to generate the notes for an SGMXAS scan
	QString buildNotes();
};

#endif // SGMXASSCANACTIONCONTROLLER_H
