#ifndef SGMFASTSCANCONTROLLER_H
#define SGMFASTSCANCONTROLLER_H

#include "dataman/AMFastScan.h"
#include "SGMFastScanConfiguration.h"
#include "analysis/AM1DExpressionAB.h"

class SGMFastScanController
{
public:
	SGMFastScanController(SGMFastScanConfiguration *cfg);

	bool isBeamlineInitialized();
	virtual bool beamlineInitialize();
	virtual void reinitialize();

protected:
	SGMFastScanConfiguration *specificCfg_;
	AMBeamlineParallelActionsList *initializationActions_;
	bool beamlineInitialized_;
	AMFastScan *specificScan_;

private:
	SGMFastScanConfiguration* pCfg() { return qobject_cast<SGMFastScanConfiguration*>(specificCfg_);}
	AMFastScan* pScan() { return qobject_cast<AMFastScan*>(specificScan_);}
};

#endif // SGMFASTSCANCONTROLLER_H
