#ifndef REIXSXASSCANCONTROLLER_H
#define REIXSXASSCANCONTROLLER_H

#include "acquaman/AMSA1DScanController.h"
#include "acquaman/REIXS/REIXSXASScanConfiguration.h"

/// Re-implements AMSA1DScanController to add analysis blocks for normalized TEY and TFY spectra.
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
	void createAnalysisBlocks();

};

#endif // REIXSXASSCANCONTROLLER_H
