#ifndef ACQMAN_DACQSCANCONTROLLER_H
#define ACQMAN_DACQSCANCONTROLLER_H

#include <QTime>
#include <QStringList>
#include <QDir>

#include "AMErrorMonitor.h"

#include "AMScanController.h"
#include "qdebug.h"

#include "dacq3_2/OutputHandler/acqFactory.h"
#include "AMAcqScanOutput.h"
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
	virtual void start(){
		if(initialized_){
		//	acqBaseOutput *abop = acqOutputHandlerFactory::new_acqOutput("SimpleText", "File");
		//	acqBaseOutput *abop = acqOutputHandlerFactory::new_acqOutput("AMScan", "File");
		//	acqBaseOutput *abop = acqOutputHandlerFactory::new_acqOutput("Text", "File");
			acqBaseOutput *abop = acqOutputHandlerFactory::new_acqOutput("AMScanSpectrum", "File");
			if( abop)
			{
				acqRegisterOutputHandler( advAcq_->getMaster(), (acqKey_t) abop, &abop->handler);                // register the handler with the acquisition
				abop->setProperty( "File Template", pCfg_()->fileName().toStdString());
				abop->setProperty( "File Path", pCfg_()->filePath().toStdString());

				qDebug() << "Just before set scan for spectrumOutput";
				((AMAcqScanSpectrumOutput*)abop)->setScan(pScan_());
				advAcq_->Start();
			}
			else
				AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, -1, "AMDacqScanController: could not create output handler."));
			/*
			  Moved into abop check, why was it out here?
			qDebug() << "Just before sending start to library in dacq controller start";
			advAcq_->Start();
			qDebug() << "Just after sending start to library in dacq controller start";
			*/
		}
		else
			AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, -1, "AMDacqScanController: attempted start on uninitialized controller."));
	}
	/// Cancel scan if currently running or paused
	virtual void cancel();
	/// Pause scan if currently running
	virtual void pause() {advAcq_->Pause(0);}
	/// Resume scan if currently paused
	virtual void resume(){ advAcq_->Pause(1);}

protected:
	QEpicsAdvAcq *advAcq_;
	bool cancelled_;
	QTime startTime_;
//	AMScan *curScan_;

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
};

#endif // ACQMAN_DACQSCANCONTROLLER_H
