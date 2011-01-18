#ifndef SGMFASTDACQSCANCONTROLLER_H
#define SGMFASTDACQSCANCONTROLLER_H

#include "acquaman/AMDacqScanController.h"
#include "dataman/AMFastScan.h"
#include "analysis/AM1DExpressionAB.h"
#include "SGMFastScanConfiguration.h"

//class SGMFastScanController;
class SGMFastScanController
{
public:
	SGMFastScanController(SGMFastScanConfiguration *cfg);

	bool isBeamlineInitialized();
	virtual bool beamlineInitialize();
	virtual void reinitialize();

protected:
	SGMFastScanConfiguration *specificCfg_;
	bool beamlineInitialized_;
	AMFastScan *specificScan_;

private:
	SGMFastScanConfiguration* pCfg() { return qobject_cast<SGMFastScanConfiguration*>(specificCfg_);}
	AMFastScan* pScan() { return qobject_cast<AMFastScan*>(specificScan_);}
};

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

private:
//	SGMFastScanConfiguration *pCfg() { return qobject_cast<SGMFastScanConfiguration*>(generalCfg_);}
//	AMFastScan* pScan() { return qobject_cast<AMFastScan*>(generalScan_);}
	SGMFastScanConfiguration *pCfg() { return qobject_cast<SGMFastScanConfiguration*>(specificCfg_);}
	AMFastScan* pScan() { return qobject_cast<AMFastScan*>(specificScan_);}
};


/*
class SGMFastScanController
{
public:
	SGMFastScanController(SGMFastScanConfiguration *cfg);

	bool isBeamlineInitialized();
	virtual bool beamlineInitialize();
	virtual void reinitialize();

protected:
	SGMFastScanConfiguration *specificCfg_;
	bool beamlineInitialized_;
	AMFastScan *specificScan_;

private:
	SGMFastScanConfiguration* pCfg() { return qobject_cast<SGMFastScanConfiguration*>(generalCfg_);}
	AMFastScan* pScan() { return qobject_cast<AMFastScan*>(generalScan);}
};
*/

#endif // SGMFASTDACQSCANCONTROLLER_H
