#ifndef REIXSXESSCANACTIONCONTROLLER_H
#define REIXSXESSCANACTIONCONTROLLER_H

#include "acquaman/AMRegionScanActionController.h"
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
 	virtual ~REIXSXESScanActionController();
	REIXSXESScanActionController(REIXSXESScanConfiguration* configuration, QObject *parent = 0);


public slots:
	virtual void skip(const QString &command);

	/// Method that builds all the general aspects, such as measurements and raw data sources, and the file writer capabilities for the scan controller.
	virtual void buildScanController();

protected slots:
	void onDetectorAcquisitionSucceeded();
	void onDetectorAcquisitionFailed();

	/// Called when the data values of the detector image change (during a scan)
	void onNewImageValues();
	/// Called to save the current raw data to disk
	void saveRawData();

	/// Fills the scan meta-data (scan name, number, sampleId) either automatically, or from the pre-set configuration values.
	void initializeScanMetaData();
	void onInitializationActionsListSucceeded();


protected:
	/// Reimplemented to provide actions that will setup the beamline for optimized operation of the XAS scan.
	/// Reimplemented to put the beamline in a good state after a scan has been completed.
	/// Adds anything extra (eg: analysis blocks) to the scan before it's started.
	virtual void buildScanControllerImplementation();

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
