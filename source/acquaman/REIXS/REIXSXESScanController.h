#ifndef REIXSXESSCANCONTROLLER_H
#define REIXSXESSCANCONTROLLER_H

#include "acquaman/AMScanController.h"
#include "acquaman/REIXS/REIXSXESScanConfiguration.h"
#include "dataman/AMXESScan.h"
#include <QTimer>

class AMBeamlineControlSetMoveAction;



/// This is the implementation of AMScanController that executes a REIXS XES Scan, based on the parameters in a given REIXSXESScanConfiguration.
class REIXSXESScanController : public AMScanController
{
	Q_OBJECT
public:
	explicit REIXSXESScanController(REIXSXESScanConfiguration* configuration, QObject *parent = 0);

signals:

protected:
	/// Start scan running if not currently running or paused
	virtual bool startImplementation();
	/// Cancel scan if currently running or paused
	virtual void cancelImplementation();
	/// Called before starting to satisfy any prerequisites (ie: setting up the beamline, setting up files, etc.)
	virtual bool initializeImplementation();


protected slots:
	/// Called when the move to the starting position succeeds
	void onInitialSetupMoveSucceeded();
	/// Called if the move to the starting position fails.
	void onInitialSetupMoveFailed();

	/// Called when the data values of the detector image change (during a scan)
	void onNewImageValues();

	/// Called at every interval of the scanProgressTimer_ to report on the scan progress and check if we've reached the timeout duration.
	void onScanProgressCheck();
	/// Called to clean up when the scan is finished.
	void onScanFinished();

	/// Called to save the current raw data to disk
	void saveRawData();

protected:
	/// \todo Move to AMScanController? Time that scan was started
	QTime startTime_;
	/// 1 second timer to check on progress of scan
	QTimer scanProgressTimer_;


	/// This move action is used to implement the initial move of the spectrometer into position
	AMBeamlineControlSetMoveAction* initialMoveAction_;


private:
	/// Shortcut to cast the scan configuration (AMScanController's generalCfg_) back into the REIXSXESScanConfiguration that we know it is.
	REIXSXESScanConfiguration* pCfg() { return qobject_cast<REIXSXESScanConfiguration*>(generalCfg_); }

	/// Shortcut to cast the scan object (AMScanController's generalScan_ back into the AMXESScan that we know it is.
	AMXESScan* pScan() { return qobject_cast<AMXESScan*>(generalScan_); }

};

#endif // REIXSXESSCANCONTROLLER_H
