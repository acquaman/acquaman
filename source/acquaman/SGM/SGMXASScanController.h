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
	AMXASScan *specificScan_;

private:
	SGMXASScanConfiguration **_pCfg_;
	AMXASScan **_pScan_;

	SGMXASScanConfiguration* pCfg_() { return *_pCfg_;}
	AMXASScan* pScan_() { return *_pScan_;}
};

#endif // AMSGMXASSCANCONTROLLER_H
