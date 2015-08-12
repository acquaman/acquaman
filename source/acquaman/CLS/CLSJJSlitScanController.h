#ifndef CLSJJSLITSCANCONTROLLER_H
#define CLSJJSLITSCANCONTROLLER_H

#include "acquaman/AMGenericStepScanController.h"

#include "acquaman/CLS/CLSJJSlitScanConfiguration.h"

class CLSJJSlitScanController : public AMGenericStepScanController
{
    Q_OBJECT

public:
	/// Constructor.
	explicit CLSJJSlitScanController(CLSJJSlitScanConfiguration *configuration, QObject *parent = 0);
	/// Destructor.
	virtual ~CLSJJSlitScanController();

protected:
	/// Creates and returns the initialization actions, to be executed before the scan starts.
	virtual AMAction3* createInitializationActions();
	/// Creates and returns the cleanup actions, to be executed after the scan ends.
	virtual AMAction3* createCleanupActions();

	/// Adds extra analyzed data sources.
	virtual void buildScanControllerImplementation();

protected:
	/// The scan configuration.
	CLSJJSlitScanConfiguration *jjSlitScanConfiguration_;

	/// The JJ slits being scanned.
	CLSJJSlits *jjSlits_;
};

#endif // CLSJJSLITSCANCONTROLLER_H
