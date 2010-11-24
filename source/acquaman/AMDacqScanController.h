#ifndef ACQMAN_DACQSCANCONTROLLER_H
#define ACQMAN_DACQSCANCONTROLLER_H

#include <QTime>
#include <QStringList>
#include <QDir>

#include "AMErrorMonitor.h"

#include "AMScanController.h"
#include "qdebug.h"

#include "dacq3_2/OutputHandler/acqFactory.h"
#include "AMAcqScanSpectrumOutput.h"
#include "dacq3_2/qepicsadvacq.h"

class AMDacqScanController : public AMScanController
{
Q_OBJECT
public:
	AMDacqScanController(AMScanConfiguration *cfg, QObject *parent = 0);

public slots:
//    /// Sets a new scan configuration
//    virtual void newConfigurationLoad(AMScanConfiguration &cfg);
	/// Start scan running if not currently running or paused
	virtual void start();
	/// Cancel scan if currently running or paused
	virtual void cancel();
	/// Pause scan if currently running
	virtual void pause() { advAcq_->Pause(1);}
	/// Resume scan if currently paused
	virtual void resume(){ advAcq_->Pause(0);}

protected:
	bool event(QEvent *e);

protected:
	QEpicsAdvAcq *advAcq_;
	bool cancelled_;
	QTime startTime_;

protected slots:
	void onStart();
	void onStop();
	void onPause(int mode);
	void onSendCompletion(int completion);

private:
	AMScanConfiguration **_pCfg_;
	AMScan **_pScan_;

	AMScanConfiguration *pCfg_() { return *_pCfg_;}
	AMScan *pScan_() { return *_pScan_;}

	// removed: testing function: void play2d();
};

#endif // ACQMAN_DACQSCANCONTROLLER_H
