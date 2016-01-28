#ifndef AMXRFSCANCONTROLLER_H
#define AMXRFSCANCONTROLLER_H

#include "acquaman/AMScanController.h"

#include "acquaman/AMXRFScanConfiguration.h"
#include "beamline/AMXRFDetector.h"

/// Builds a controller that encapsulates a single acquisition and then saves the data and stores a record in the database.
class AMXRFScanController : public AMScanController
{
	Q_OBJECT

public:
	/// Constructor.
	AMXRFScanController(AMXRFScanConfiguration *configuration, QObject *parent = 0);
	/// Destructor.
	virtual ~AMXRFScanController();

protected slots:
	/// Helper slot to finish up a scan.
	void onDetectorAcquisitionFinished();
	/// Helper slot that emits the current progress of the scan.
	void onProgressUpdate();
	/// Checks to see if the scan has finished.  If it is, then the finish scan sequence is started.
	void onStatusChanged();

protected:
	/// Initializes the scan
//	virtual bool initializeImplementation();
	/// Starts current scan.
//	virtual bool startImplementation();
	/// Cancels current scan.  Treated as finishing early.
//	virtual void cancelImplementation();
	/// Cancels current scan. Treated as finishing early.
//	virtual void stopImplementation(const QString &command);
	/// Saves the data after a scan is stopped.
	void saveData();

	/// The pointer to the detector that we're using.
	AMXRFDetector *detector_;
};

#endif // AMXRFSCANCONTROLLER_H
