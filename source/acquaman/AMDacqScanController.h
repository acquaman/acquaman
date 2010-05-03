#ifndef ACQMAN_DACQSCANCONTROLLER_H
#define ACQMAN_DACQSCANCONTROLLER_H

#include <QTime>

#include "AMScanController.h"
#include "qdebug.h"

#include "dacq3_2/OutputHandler/acqFactory.h"
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
		if(initialized_){
//            acqBaseOutput *abop = acqOutputHandlerFactory::new_acqOutput("SimpleText", "File");
			acqBaseOutput *abop = acqOutputHandlerFactory::new_acqOutput("AMScan", "File");
			if( abop)
			{
				acqRegisterOutputHandler( advAcq_->getMaster(), (acqKey_t) abop, &abop->handler);                // register the handler with the acquisition
				abop->setProperty( "File Template", "daveData.%03d.dat");                           // set the file name to be recorded to
			}
			advAcq_->Start();
		}
		else
			qDebug() << "Hold your horses!";
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

protected slots:
	void onStart();
	void onStop();
	void onPause(int mode);
	void onSendCompletion(int completion);
};

#endif // ACQMAN_DACQSCANCONTROLLER_H
