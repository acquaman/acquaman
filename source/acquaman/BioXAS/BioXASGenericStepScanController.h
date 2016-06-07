#ifndef BIOXASGENERICSTEPSCANCONTROLLER_H
#define BIOXASGENERICSTEPSCANCONTROLLER_H

#include "acquaman/AMGenericStepScanController.h"
#include "acquaman/BioXAS/BioXASScanController.h"
#include "acquaman/BioXAS/BioXASGenericStepScanConfiguration.h"

class BioXASGenericStepScanController : public AMGenericStepScanController, public BioXASScanController
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASGenericStepScanController(BioXASGenericStepScanConfiguration *configuration, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASGenericStepScanController();

protected:
	/// Adds anything extra (eg: analysis blocks) to the scan before it's started.
	virtual void buildScanControllerImplementation();
};

#endif // BIOXASGENERICSTEPSCANCONTROLLER_H
