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
	explicit SGMXASDacqScanController(AMXASScanConfiguration *xasSCfg, QObject *parent = 0);

	bool isInitialized() { return (initialized_ && beamlineInitialized_);}

signals:
	void initialized();

public slots:
	void initialize();
	void start(){
//		advAcq_->setConfigFile("myScan.cfg");
		advAcq_->setConfigFile("/home/reixs/beamline/programming/acquaman/myScan.cfg");
		//advAcq_->clearRegions();
		//advAcq_->addRegion(0, xasSCfg_->start(0), xasSCfg_->delta(0), xasSCfg_->end(0), 1.0 );
		advAcq_->setStart(0, xasSCfg_->start(0));
		advAcq_->setDelta(0, xasSCfg_->delta(0));
		advAcq_->setEnd(0, xasSCfg_->end(0));
		AMDacqScanController::start();
	}
};

#endif // ACQMAN_SGMXASDACQSCANCONTROLLER_H
