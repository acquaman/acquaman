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

signals:
	void initialized();

public slots:
	void initialize();
	void start(){
		advAcq_->setConfigFile("/home/reixs/beamline/programming/acquaman/devConfigurationFiles/defaultEnergy.cfg");
		foreach(QString str, pScan_()->detectors()){
			qDebug() << "Trying to add detector " << str << " to .cfg file as " << SGMBeamline::sgm()->pvName(str) << " in slot " << advAcq_->getNumRecords();
			if(advAcq_->addRecord(advAcq_->getNumRecords(), SGMBeamline::sgm()->pvName(str), true, false, 0))
				qDebug() << "Added successfully";
			else
				qDebug() << "Add failed";
		}
		//advAcq_->clearRegions();
		for(int x = 0; x < pCfg_()->count(); x++){
			if(advAcq_->getNumRegions() == x)
				advAcq_->addRegion(x, pCfg_()->start(x), pCfg_()->delta(x), pCfg_()->end(x), 1);
			else{
				advAcq_->setStart(x, pCfg_()->start(x));
				advAcq_->setDelta(x, pCfg_()->delta(x));
				advAcq_->setEnd(x, pCfg_()->end(x));
			}
		}
		generalScan_ = specificScan_;
		AMDacqScanController::start();
	}

private:
	SGMXASScanConfiguration **_pCfg_;
	AMXASScan **_pScan_;

	SGMXASScanConfiguration *pCfg_() { return *_pCfg_;}
	AMXASScan* pScan_() { return *_pScan_;}
};

#endif // ACQMAN_SGMXASDACQSCANCONTROLLER_H
