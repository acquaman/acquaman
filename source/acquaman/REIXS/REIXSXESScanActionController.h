#ifndef REIXSXESSCANACTIONCONTROLLER_H
#define REIXSXESSCANACTIONCONTROLLER_H

#include "acquaman/AMScanActionController.h"
#include "acquaman/SGM/SGMFastScanConfiguration2013.h"
#include "dataman/AMUser.h"
#include "actions3/AMAction3.h"

class QTimer;

class REIXSXESScanConfiguration;

class REIXSXESScanActionController : public AMScanActionController
{
public:
	REIXSXESScanActionController(REIXSXESScanConfiguration* configuration, QObject *parent = 0);

protected slots:
	void onDetectorAcquisitionSucceeded();

	/// Called when the data values of the detector image change (during a scan)
	void onNewImageValues();
	/// Called to save the current raw data to disk
	void saveRawData();

protected:
	virtual bool initializeImplementation();
	virtual bool startImplementation();
	virtual void cancelImplementation();

protected:
	REIXSXESScanConfiguration *configuration_;
	QTimer *updateTimer_;
};

#endif // REIXSXESSCANACTIONCONTROLLER_H
