#ifndef SXRMBEXAFSSCANACTIONCONTROLLER_H
#define SXRMBEXAFSSCANACTIONCONTROLLER_H

#include "acquaman/AMStepScanActionController.h"

class SXRMBEXAFSScanConfiguration;

class SXRMBEXAFSScanActionController : public AMStepScanActionController
{
Q_OBJECT
public:
	/// Constructor takes an SXRMB EXAFS scan configuration
	SXRMBEXAFSScanActionController(SXRMBEXAFSScanConfiguration *configuration, QObject *parent = 0);

	/// Destructor
	virtual ~SXRMBEXAFSScanActionController();

signals:

public slots:

protected slots:
	/// Handles updating time elapsed for scan and progress bar
	void onScanTimerUpdate();

protected:
	/// Reimplemented to provide actions that will setupd the beamine for optimzed operation of the XAS scan.
	AMAction3* createInitializationActions();
	/// Reimplemented to put the beamline in a good state after a scan has been completed.
	AMAction3* createCleanupActions();
	/// Re-implemented due to the extra complexity of setting up IDEAS.
	virtual void cancelImplementation();

	/// Sets the scan axis and adds anything extra.
	virtual void buildScanControllerImplementation();
	/// Reimplemented for EXAFS capabilities.  Creates the scan assembler that builds all the actions used to run the scan.
	virtual void createScanAssembler();


protected:
	/// Our scan configuration
	SXRMBEXAFSScanConfiguration *configuration_;

	/// Timer used for determinging the elasped time for a scan.
	QTimer elapsedTime_;
	/// Number of seconds since the timer started.
	double secondsElapsed_;
	/// Number of seconds total for the scan to complete (estimate).
	double secondsTotal_;
};

#endif // SXRMBEXAFSSCANACTIONCONTROLLER_H
