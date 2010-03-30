#include "AMDacqScanController.h"
#include <qdebug.h>

AMDacqScanController::AMDacqScanController(QObject *parent)
{

    running_ = FALSE;
    paused_ = FALSE;
    cancelled_ = FALSE;
    QEpicsAcqLocal *lAcq = new QEpicsAcqLocal((QWidget*)parent);
    advAcq_ = new QEpicsAdvAcq(lAcq);
    connect(advAcq_, SIGNAL(onStart()), this, SLOT(onStart()));
    connect(advAcq_, SIGNAL(onStop()), this, SLOT(onStop()));
    connect(advAcq_, SIGNAL(onPause(int)), this, SLOT(onPause(int)));
	connect(advAcq_, SIGNAL(sendCompletion(int)), this, SLOT(onSendCompletion(int)));

    /*
    qDebug() << "Start of dacqscancontroller constructor";

    acqMaster_t *master;
    master = new_acqMaster();
    acq_file_load("myScan.cfg", master);
    acqScan_t *sp = first_acqScan(master);
//    getValue(sp->scanName, "start", x)
    qDebug() << "with values of " << sp->acqControlList[0].startVal << " " << sp->acqControlList[0].deltaVal << " " << sp->acqControlList[0].finalVal;
    if( Standby_mode(master) == 1 && Run_mode(master) == 0)
        startMonitorTask(master);
    */
    /*
    QEpicsAcqLocal *lAcq = new QEpicsAcqLocal();
    QEpicsAdvAcq *myAcq = new QEpicsAdvAcq(lAcq);
    myAcq->setConfigFile("myScan.cfg");
    acqBaseOutput *abop = acqOutputHandlerFactory::new_acqOutput("SimpleText", "File");
    if( abop)
    {
        acqRegisterOutputHandler( myAcq->getMaster(), (acqKey_t) abop, &abop->handler);                // register the handler with the acquisition
        abop->setProperty( "File Template", "daveData.%03d.dat");                           // set the file name to be recorded to
    }

    for(int x = 0; x < myAcq->getNumRegions(); x++)
        qDebug() << "Start is " << myAcq->getStrStart(x) << " delta is " << myAcq->getStrDelta(x) << " end is " << myAcq->getStrEnd(x);
    myAcq->setStart(0, 285);
    myAcq->setDelta(0, 0.1);
    myAcq->setEnd(0, 286);
    for(int x = 0; x < myAcq->getNumRegions(); x++)
        qDebug() << "Start is " << myAcq->getStrStart(x) << " delta is " << myAcq->getStrDelta(x) << " end is " << myAcq->getStrEnd(x);
    myAcq->addRegion(1, 286.3, 0.3, 289, 1);
    for(int x = 0; x < myAcq->getNumRegions(); x++)
        qDebug() << "Start is " << myAcq->getStrStart(x) << " delta is " << myAcq->getStrDelta(x) << " end is " << myAcq->getStrEnd(x);
    myAcq->Start();

    qDebug() << "end of dacqscancontroller constructor";
    */
}

///// Sets a new scan configuration
//void AMDacqScanController::newConfigurationLoad(AMScanConfiguration &cfg)
//{
//}

/// Cancel scan if currently running or paused
void AMDacqScanController::cancel()
{
    advAcq_->Stop();
    cancelled_ = TRUE;
    emit cancelled();
}

void AMDacqScanController::onStart()
{
    running_ = TRUE;
	startTime_.start();
    emit started();
}

void AMDacqScanController::onStop()
{
    running_ = FALSE;
    if(cancelled_)
        cancelled_ = FALSE;
    else
        emit finished();
}

void AMDacqScanController::onPause(int mode)
{
    if(mode == 0){
        paused_ = TRUE;
        emit paused();
    }
    else if(mode == 1){
        paused_ = FALSE;
        emit resumed();
    }
}

void AMDacqScanController::onSendCompletion(int completion){
	double tc = ((double)startTime_.elapsed())/1000;
	double remaining = (completion != 0) ? (100*tc)/((double)completion) - tc : tc*100000;
	emit timeRemaining(remaining);
	emit progress(tc, tc+remaining);
}
