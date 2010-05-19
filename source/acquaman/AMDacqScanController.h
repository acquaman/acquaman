#ifndef ACQMAN_DACQSCANCONTROLLER_H
#define ACQMAN_DACQSCANCONTROLLER_H

#include <QTime>
#include <QStringList>

#include "AMErrorMonitor.h"

#include "AMScanController.h"
#include "qdebug.h"

#include "dacq3_2/OutputHandler/acqFactory.h"
#include "AMAcqScanOutput.h"
#include "dataman/AMXASScan.h"
#include "dacq3_2/qepicsadvacq.h"

class AMDacqScanController : public AMScanController
{
Q_OBJECT
public:
	AMDacqScanController(QObject *parent = 0);

public slots:
//    /// Sets a new scan configuration
//    virtual void newConfigurationLoad(AMScanConfiguration &cfg);
	/// Start scan running if not currently running or paused
	virtual void start(){
		qDebug() << "Start of dacq controller start";
		if(initialized_){
		//	acqBaseOutput *abop = acqOutputHandlerFactory::new_acqOutput("SimpleText", "File");
			acqBaseOutput *abop = acqOutputHandlerFactory::new_acqOutput("AMScan", "File");
			if( abop)
			{
				acqRegisterOutputHandler( advAcq_->getMaster(), (acqKey_t) abop, &abop->handler);                // register the handler with the acquisition
				abop->setProperty( "File Template", "daveData.%03d.dat");                           // set the file name to be recorded to
				QStringList scanDetectors;
				scanDetectors << "eV_Fbk" << "reading" << "tfy" << "pgt";
				curScan_ = new AMXASScan(scanDetectors);
				curScan_->addChannel("eV", "eV");
				curScan_->addChannel("Jitter", "eV_Fbk");
				curScan_->addChannel("TEY", "reading");
				curScan_->addChannel("TFY", "tfy");
				curScan_->addChannel("PGT", "pgt");
				((AMAcqScanOutput*)abop)->setScan(curScan_);
			}
			qDebug() << "Just before sending start to library in dacq controller start";
			advAcq_->Start();
			qDebug() << "Just after sending start to library in dacq controller start";
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
	AMXASScan *curScan_;

protected slots:
	void onStart();
	void onStop();
	void onPause(int mode);
	void onSendCompletion(int completion);
};

#endif // ACQMAN_DACQSCANCONTROLLER_H
