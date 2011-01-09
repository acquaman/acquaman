#ifndef ACQMAN_SGMXASDACQSCANCONTROLLER_H
#define ACQMAN_SGMXASDACQSCANCONTROLLER_H

#include "acquaman/AMDacqScanController.h"
//#include "AMXASScanConfiguration.h"
#include "SGMXASScanController.h"
//#include "beamline/SGMBeamline.h"

class SGMXASDacqScanController : public AMDacqScanController, public SGMXASScanController
{
Q_OBJECT
public:
	explicit SGMXASDacqScanController(SGMXASScanConfiguration *cfg, QObject *parent = 0);

	bool isInitialized() { return (initialized_ && beamlineInitialized_);}

	virtual AMScan* scan() {return pScan_();}

signals:
	void initialized();
	void reinitialized(bool removeScan);

public slots:
	void initialize();
	void reinitialize(bool removeScan);
	void start();

//	AMXASScan* scan(){ return pScan_();}

//protected:
//	virtual void reinitialize();


protected:
	/*
	bool event(QEvent *e);
	*/
	AMnDIndex toScanIndex(QMap<int, double> aeData);

private:
	/// \todo Why the double pointers?
	SGMXASScanConfiguration **_pCfg_;
	/// \todo Why the double pointer system?
	AMXASScan **_pScan_;

	SGMXASScanConfiguration *pCfg_() { return *_pCfg_;}
	AMXASScan* pScan_() { return *_pScan_;}
};

#endif // ACQMAN_SGMXASDACQSCANCONTROLLER_H
