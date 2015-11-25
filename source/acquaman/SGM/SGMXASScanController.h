#ifndef SGMXASSCANCONTROLLER_H
#define SGMXASSCANCONTROLLER_H

#include "acquaman/AMGenericContinuousScanController.h"
#include "acquaman/SGM/SGMXASScanConfiguration.h"

/// This class sets up, runs and manages an XAS continuous scan for the SGM beamline.
class SGMXASScanController : public AMGenericContinuousScanController
{
	Q_OBJECT

public:
	/// Constructor.  Takes an SGMXASScanConfiguration.
	SGMXASScanController(SGMXASScanConfiguration *configuration, QObject *parent = 0);
	/// Destructor.
	virtual ~SGMXASScanController();

signals:

public slots:

protected:
	/// Reimplemented to provide actions that will setup the beamline for optimized operation of the XAS scan.
	AMAction3 *createInitializationActions();
	/// Reimplemented to put the beamline in a good state after a scan has been completed.
	AMAction3 *createCleanupActions();

	/// Specific SGM XAS scan configuration.
	SGMXASScanConfiguration *sgmXASConfiguration_;
};

#endif // SGMXASSCANCONTROLLER_H
