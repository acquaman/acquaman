#ifndef XASDACQSCANCONTROLLER_H
#define XASDACQSCANCONTROLLER_H

#include "AMDacqScanController.h"
#include "AMXASScanConfiguration.h"
#include "beamline/AMSGMBeamline.h"

class AMXASDacqScanController : public AMDacqScanController
{
Q_OBJECT
public:
    explicit AMXASDacqScanController(AMXASScanConfiguration *xasSCfg, QObject *parent = 0);

    bool isInitialized() { return initialized_;}

signals:
    void initialized();

public slots:
    void initialize();

protected:
    bool initialized_;
};

#endif // XASDACQSCANCONTROLLER_H
