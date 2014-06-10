#ifndef VESPERSENERGYSCANACTIONCONTROLLER_H
#define VESPERSENERGYSCANACTIONCONTROLLER_H

#include "acquaman/AMStepScanActionController.h"
#include "acquaman/VESPERS/VESPERSEnergyScanConfiguration.h"
#include "acquaman/VESPERS/VESPERSScanController.h"

/// VESPERS specific subclass for XRD energy scans.
class VESPERSEnergyScanActionController : public AMStepScanActionController, public VESPERSScanController
{
	Q_OBJECT

public:
	/// Constructor.  Builds a runnable scan action controller for the VESPERS beamline.
	VESPERSEnergyScanActionController(VESPERSEnergyScanConfiguration *configuration, QObject *parent = 0);
	/// Destructor.
	virtual ~VESPERSEnergyScanActionController();

protected slots:
	/// Helper slot that handles the progress update.
	void onScanTimerUpdate();

protected:
	/// Reimplemented to provide actions that will setup the beamline for optimized operation of the XAS scan.
	AMAction3* createInitializationActions();
	/// Reimplemented to put the beamline in a good state after a scan has been completed.
	AMAction3* createCleanupActions();

	/// Adds anything extra (eg: analysis blocks) to the scan before it's started.
	virtual void buildScanControllerImplementation();

	/// Specific scan configuration with all the VESPERS specific information inside.
	VESPERSEnergyScanConfiguration *configuration_;

	/// The energy of the monochromator before the scan started.
	double originalEnergy_;
	/// Timer used for determining the elapsed time for a scan.
	QTimer elapsedTime_;
	/// Number of seconds since the timer started.
	double secondsElapsed_;
	/// Number of seconds total for the scan to complete (estimate).
	double secondsTotal_;
};

#endif // VESPERSENERGYSCANACTIONCONTROLLER_H
