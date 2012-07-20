#ifndef REIXSXASSCANCONTROLLER_H
#define REIXSXASSCANCONTROLLER_H

#include "acquaman/AMSA1DScanController.h"
#include "acquaman/REIXS/REIXSXASScanConfiguration.h"

/// Re-implements AMSA1DScanController to add analysis blocks for normalized TEY and TFY spectra, and disable/re-enable the settling time and tolerance for REIXSBrokenMonoControl.
class REIXSXASScanController : public AMSA1DScanController
{
    Q_OBJECT
public:
	explicit REIXSXASScanController(REIXSXASScanConfiguration* configuration, QObject *parent = 0);
	virtual ~REIXSXASScanController();

signals:

public slots:

protected:
	/// Re-implemented from AMSA1DScanController to add analysis blocks for normalization.
	virtual void createAnalysisBlocks();

	/// Re-implemented to use the three-step move control for the first move, and then the direct energy PV after that.
	virtual AMControl* control();

//	/// Re-implemented to disable the settling time and widen the tolerance for REIXSBrokenMonoControl before starting the scan.
//	virtual bool customInitializeImplementation();
//	/// Re-implemented to restore the settling time and re-set the tolerance for REIXSBrokenMonoControl after the scan is over or cancelled.
//	virtual void customCleanupImplementation();
//	/// Used to restore the existing values
//	double repeatMoveSettlingTime_, singleMoveSettlingTime_, tolerance_;



};

#endif // REIXSXASSCANCONTROLLER_H
