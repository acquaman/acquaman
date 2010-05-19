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
		for(int x = 0; x < xasSCfg_->count(); x++){
			if(advAcq_->getNumRegions() == x)
				advAcq_->addRegion(x, xasSCfg_->start(x), xasSCfg_->delta(x), xasSCfg_->end(x), 1);
			else{
				qDebug() << "Setting start, delta, end in controller start loop";
				advAcq_->setStart(x, xasSCfg_->start(x));
				advAcq_->setDelta(x, xasSCfg_->delta(x));
				advAcq_->setEnd(x, xasSCfg_->end(x));
				qDebug() << "Done setting start, delta, end in controller start loop";
			}
		}
		qDebug() << "Done SGMXAS controller start loop";
		AMDacqScanController::start();
	}
};

#endif // ACQMAN_SGMXASDACQSCANCONTROLLER_H
