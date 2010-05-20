#ifndef AMSGMXASSCANCONTROLLER_H
#define AMSGMXASSCANCONTROLLER_H

#include "beamline/SGMBeamline.h"
#include "SGMXASScanConfiguration.h"
#include "dataman/AMXASScan.h"

class SGMXASScanController
{
public:
	SGMXASScanController(SGMXASScanConfiguration *cfg);

	bool isBeamlineInitialized() { return beamlineInitialized_;}
	virtual bool beamlineInitialize();

protected:
	SGMXASScanConfiguration *specificCfg_;
	bool beamlineInitialized_;
	AMXASScan *scan_;

private:
	SGMXASScanConfiguration *pCfg_;
};

#endif // AMSGMXASSCANCONTROLLER_H
