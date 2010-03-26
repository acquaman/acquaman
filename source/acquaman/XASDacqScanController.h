#ifndef XASDACQSCANCONTROLLER_H
#define XASDACQSCANCONTROLLER_H

#include "DacqScanController.h"
#include "XASScanConfiguration.h"

class XASDacqScanController : public DacqScanController
{
Q_OBJECT
public:
    explicit XASDacqScanController(QObject *parent = 0);

signals:

public slots:

};

#endif // XASDACQSCANCONTROLLER_H
