#ifndef XASDACQSCANCONTROLLER_H
#define XASDACQSCANCONTROLLER_H

#include "DacqScanController.h"
#include "XASScanConfiguration.h"
#include "beamline/SGMBeamline.h"

class XASDacqScanController : public DacqScanController
{
Q_OBJECT
public:
    explicit XASDacqScanController(XASScanConfiguration *xasSCfg, QObject *parent = 0);

    bool isInitialized() { return initialized_;}

signals:
    void initialized();

public slots:
    void initialize();

protected:
    bool initialized_;
};

#endif // XASDACQSCANCONTROLLER_H
