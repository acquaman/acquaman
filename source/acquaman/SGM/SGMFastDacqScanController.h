#ifndef SGMFASTDACQSCANCONTROLLER_H
#define SGMFASTDACQSCANCONTROLLER_H

#include "acquaman/AMDacqScanController.h"
#include "SGMFastScanController.h"

class SGMFastDacqScanController : public AMDacqScanController, public SGMFastScanController
{
	Q_OBJECT
public:
	explicit SGMFastDacqScanController(SGMFastScanConfiguration *cfg, QObject *parent = 0);

	virtual AMScan* scan() { return pScan();}

protected:
	bool initializeImplementation();
	bool startImplementation();
	void cancelImplementation();

protected:
	bool event(QEvent *e);
	AMnDIndex toScanIndex(QMap<int, double> aeData);

	double lastProgress_;
	int initializationStagesComplete_;
	bool dacqRunUpStarted_;
	bool dacqRunUpCompleted_;
	bool dacqRunCompleted_;
	QTimer *fastScanTimer_;
	int timerSeconds_;

protected slots:
	// Re-implementing to intercept finished() signal and do cleanup
	void onDacqStop();
	// Re-implementing to incorporate initialization actions into progress
	void onDacqSendCompletion(int completion);
	// Re-implementing to incorporate initialization actions into progress
	void onDacqState(const QString &state);

	void onInitializationActionsSucceeded();
	void onInitializationActionsStageSucceeded(int stageIndex);
	void onFastScanTimerTimeout();

	void calculateProgress(double elapsed, double total);

	void onScanFinished();

private:
	SGMFastScanConfiguration *pCfg() { return qobject_cast<SGMFastScanConfiguration*>(specificCfg_);}
	AMFastScan* pScan() { return qobject_cast<AMFastScan*>(specificScan_);}
};

#endif // SGMFASTDACQSCANCONTROLLER_H
