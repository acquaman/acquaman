#ifndef SGMFASTDACQSCANCONTROLLER_H
#define SGMFASTDACQSCANCONTROLLER_H

#include "acquaman/AMDacqScanController.h"
#include "SGMFastScanController.h"

class SGMFastDacqScanController : public AMDacqScanController, public SGMFastScanController
{
	Q_OBJECT
public:
	SGMFastDacqScanController(SGMFastScanConfiguration *cfg, QObject *parent = 0);

	bool isInitialized() { return (initialized_ && beamlineInitialized_);}

	virtual AMScan* scan() { return pScan();}

signals:
	void initialized();
	void reinitialized(bool removeScan);

public slots:
	void initialize();
	void reinitialize(bool removeScan);
	void start();

protected:
	bool event(QEvent *e);
	AMnDIndex toScanIndex(QMap<int, double> aeData);

protected slots:
	// Re-implementing to intercept finished() signal and do cleanup
	void onStop();
	// Re-implementing to incorporate initialization actions into progress
	void onSendCompletion(int completion);

	void onInitializationActionsSucceeded();
	void onInitializationActionsStageStarted(int stageIndex);
	void onInitializationActionsStageSucceeded(int stageIndex);
	void onInitializationActionsStageProgress(double elapsed, double total);

	void onScanFinished();

private:
	SGMFastScanConfiguration *pCfg() { return qobject_cast<SGMFastScanConfiguration*>(specificCfg_);}
	AMFastScan* pScan() { return qobject_cast<AMFastScan*>(specificScan_);}
};

#endif // SGMFASTDACQSCANCONTROLLER_H
