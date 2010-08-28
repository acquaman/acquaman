#ifndef ACQMAN_SGMXASDACQSCANCONTROLLER_H
#define ACQMAN_SGMXASDACQSCANCONTROLLER_H

#include "acquaman/AMDacqScanController.h"
//#include "AMXASScanConfiguration.h"
#include "SGMXASScanController.h"
//#include "beamline/SGMBeamline.h"

class SGMXASDacqScanController : public AMDacqScanController, public SGMXASScanController
{
Q_OBJECT
public:
	explicit SGMXASDacqScanController(SGMXASScanConfiguration *cfg, QObject *parent = 0);

	bool isInitialized() { return (initialized_ && beamlineInitialized_);}

	virtual AMScan* scan() {return pScan_();}

signals:
	void initialized();

public slots:
	void initialize();
	void start();

//	AMXASScan* scan(){ return pScan_();}

private:
	SGMXASScanConfiguration **_pCfg_;
	AMXASScan **_pScan_;

	SGMXASScanConfiguration *pCfg_() { return *_pCfg_;}
	AMXASScan* pScan_() { return *_pScan_;}
};

#endif // ACQMAN_SGMXASDACQSCANCONTROLLER_H
