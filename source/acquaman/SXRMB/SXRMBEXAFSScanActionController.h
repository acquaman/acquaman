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

protected:
	/// Reimplemented to provide actions that will setupd the beamine for optimzed operation of the XAS scan.
	AMAction3* createInitializationActions();
	/// Reimplemented to put the beamline in a good state after a scan has been completed.
	AMAction3* createCleanupActions();
	/// Re-implemented due to the extra complexity of setting up IDEAS.
	virtual void cancelImplementation();
	/// Reimplemented to populate scan initial conditions
	void onInitializationActionsListSucceeded();

	/// Sets the scan axis and adds anything extra.
	virtual void buildScanControllerImplementation();
	/// Reimplemented for EXAFS capabilities.  Creates the scan assembler that builds all the actions used to run the scan.
	virtual void createScanAssembler();


protected:
	SXRMBEXAFSScanConfiguration *configuration_;
};

#endif // SXRMBEXAFSSCANACTIONCONTROLLER_H
