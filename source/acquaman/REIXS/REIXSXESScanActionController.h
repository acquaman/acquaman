#ifndef REIXSXESSCANACTIONCONTROLLER_H
#define REIXSXESSCANACTIONCONTROLLER_H

#include "acquaman/AMScanActionController.h"
#include "acquaman/SGM/SGMFastScanConfiguration2013.h"
#include "dataman/AMUser.h"
#include "actions3/AMAction3.h"

class QTimer;

class AMListAction3;
class REIXSXESScanConfiguration;

class REIXSXESScanActionController : public AMScanActionController
{
Q_OBJECT
public:
	REIXSXESScanActionController(REIXSXESScanConfiguration* configuration, QObject *parent = 0);

public slots:
	virtual void skip(const QString &command);

protected slots:
	void onDetectorAcquisitionSucceeded();

	/// Called when the data values of the detector image change (during a scan)
	void onNewImageValues();
	/// Called to save the current raw data to disk
	void saveRawData();

	/// Fills the scan meta-data (scan name, number, sampleId) either automatically, or from the pre-set configuration values.
	void initializeScanMetaData();
	void onInitializationActionsListSucceeded();


protected:
	virtual bool initializeImplementation();
	virtual bool startImplementation();
	virtual void cancelImplementation();

	AMAction3* createInitializationActions();


protected:
	REIXSXESScanConfiguration *configuration_;
	QTimer *updateTimer_;
	AMListAction3 *xesActionsInitializationList_;


};

#endif // REIXSXESSCANACTIONCONTROLLER_H
