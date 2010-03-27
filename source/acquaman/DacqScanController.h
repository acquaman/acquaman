#ifndef ACQMAN_DACQSCANCONTROLLER_H
#define ACQMAN_DACQSCANCONTROLLER_H

#include "ScanController.h"

#include "dacq3_2/OutputHandler/acqFactory.h"
#include "dacq3_2/qepicsadvacq.h"

class DacqScanController : public ScanController
{
Q_OBJECT
public:
    DacqScanController(QObject *parent = 0);

public slots:
    /// Sets a new scan configuration
    virtual void newConfigurationLoad(ScanConfiguration &cfg);
    /// Start scan running if not currently running or paused
    virtual void start(){ advAcq_->Start();}
    /// Cancel scan if currently running or paused
    virtual void cancel();
    /// Pause scan if currently running
    virtual void pause() {advAcq_->Pause(0);}
    /// Resume scan if currently paused
    virtual void resume(){ advAcq_->Pause(1);}

protected:
    QEpicsAdvAcq *advAcq_;
    bool cancelled_;

protected slots:
    void onStart();
    void onStop();
    void onPause(int mode);
};

#endif // DACQSCANCONTROLLER_H
