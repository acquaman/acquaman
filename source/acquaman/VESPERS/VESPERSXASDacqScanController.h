#ifndef VESPERSXASDACQSCANCONTROLLER_H
#define VESPERSXASDACQSCANCONTROLLER_H

#include "acquaman/AMDacqScanController.h"
#include "acquaman/VESPERS/VESPERSXASScanConfiguration.h"
#include "dataman/AMXASScan.h"

#define VESPERSXASDACQSCANCONTROLLER_CANT_INTIALIZE 77001
#define VESPERSXASDACQSCANCONTROLLER_CANT_START_BL_SCANNING 77002
#define VESPERSXASDACQSCANCONTROLLER_CANT_START_DETECTOR_SOURCE_MISMATCH 77003
#define VESPERSXASDACQSCANCONTROLLER_CANT_START_NO_CFG_FILE 77004

class VESPERSXASDacqScanController : public AMDacqScanController
{
	Q_OBJECT

public:
	VESPERSXASDacqScanController(VESPERSXASScanConfiguration *cfg, QObject *parent = 0);

	virtual AMScan *scan() { return 0; }

protected slots:
	void onInitializationActionsSucceeded();
	void onInitializationActionsFailed(int explanation);
	void onInitializationActionsProgress(double elapsed, double total);

protected:
	bool initializeImplementation();
	bool startImplementation();
	AMnDIndex toScanIndex(QMap<int, double> aeData);

	VESPERSXASScanConfiguration *config_;
	AMXASScan *xasScan_;

};

#endif // VESPERSXASDACQSCANCONTROLLER_H
