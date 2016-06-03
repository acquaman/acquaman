#ifndef PGMXASSCANCONTROLLER_H
#define PGMXASSCANCONTROLLER_H

#include "acquaman/AMGenericStepScanController.h"

#include "acquaman/PGM/PGMXASScanConfiguration.h"

/// The scan controller for XAS on PGM
class PGMXASScanController : public AMGenericStepScanController
{
	Q_OBJECT

public:
	/// Constructor.
	PGMXASScanController(PGMXASScanConfiguration *configuration, QObject *parent = 0);
	/// Destructor.
	virtual ~PGMXASScanController(){}

signals:

public slots:

protected:
	/// The configuration.
	PGMXASScanConfiguration *configuration_;
};

#endif // PGMXASSCANCONTROLLER_H
