#ifndef XASDACQSCANCONTROLLER_H
#define XASDACQSCANCONTROLLER_H

#include "AMDacqScanController.h"
//#include "AMXASScanConfiguration.h"
#include "AMSGMXASScanController.h"
//#include "beamline/AMSGMBeamline.h"

class AMXASDacqScanController : public AMDacqScanController, public AMSGMXASScanController
{
Q_OBJECT
public:
    explicit AMXASDacqScanController(AMXASScanConfiguration *xasSCfg, QObject *parent = 0);

//    bool isInitialized() { return initialized_;}

signals:
    void initialized();

public slots:
    void initialize();
    void start(){
        advAcq_->setConfigFile("myScan.cfg");
        //advAcq_->clearRegions();
        //advAcq_->addRegion(0, xasSCfg_->start(0), xasSCfg_->delta(0), xasSCfg_->end(0), 1.0 );
        advAcq_->setStart(0, xasSCfg_->start(0));
        advAcq_->setDelta(0, xasSCfg_->delta(0));
        advAcq_->setEnd(0, xasSCfg_->end(0));
        AMDacqScanController::start();
    }

//protected:
//    bool initialized_;
};

#endif // XASDACQSCANCONTROLLER_H
