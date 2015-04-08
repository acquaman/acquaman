#ifndef BIOXASSOLLERSLITSCANCONTROLLER_H
#define BIOXASSOLLERSLITSCANCONTROLLER_H

#include "acquaman/AMStepScanActionController.h"
#include "acquaman/BioXAS/BioXASSollerSlitScanConfiguration.h"

class BioXASSollerSlitScanController : public AMStepScanActionController
{
	Q_OBJECT

public:
	/// Constructor.
	BioXASSollerSlitScanController(BioXASSollerSlitScanConfiguration *configuration, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSollerSlitScanController();

protected:
	/// Reimplemented to provide actions that will setupd the beamine for optimzed operation of the XAS scan.
	AMAction3* createInitializationActions();
	/// Reimplemented to put the beamline in a good state after a scan has been completed.
	AMAction3* createCleanupActions();

	/// Sets the scan axis and adds anything extra.
	virtual void buildScanControllerImplementation();

	/// The configuration.
	BioXASSollerSlitScanConfiguration *configuration_;
};

#endif // BIOXASSOLLERSLITSCANCONTROLLER_H
