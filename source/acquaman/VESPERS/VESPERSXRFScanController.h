#ifndef VESPERSXRFSCANCONTROLLER_H
#define VESPERSXRFSCANCONTROLLER_H

#include "acquaman/VESPERS/VESPERSXRFScanConfiguration.h"
#include "acquaman/AMScanController.h"
#include "beamline/VESPERS/XRFDetector.h"
#include "dataman/VESPERS/AMXRFScan.h"

class VESPERSXRFScanController : public AMScanController
{
	Q_OBJECT

public:
	/// Default constructor.
	VESPERSXRFScanController(VESPERSXRFScanConfiguration *scanConfig, QObject *parent = 0);

public slots:
	/// Finishes the scan prematurely.  Still a valid scan.
	void finish() { detector_->stop(); onDetectorAcquisitionFinished(); }

protected slots:
	/// Helper slot to finish up a scan.
	void onDetectorAcquisitionFinished();
	/// Slot that updates the data stored in the scan.
	void onDetectorAcquisitionUpdate();
	/// Helper slot that emits the current progress of the scan.
	void onProgressUpdate();
	/// Checks to see if the scan has finished.  If it is, then the finish scan sequence is started.
	void onStatusChanged();

protected:
	/// Initializes the scan
	virtual bool initializeImplementation() { setInitialized(); return true; }
	/// Starts current scan.
	virtual bool startImplementation();
	/// Cancels current scan.  Treated as finishing early.
	virtual void cancelImplementation() { setCancelled(); }
	/// Saves the data after a scan is stopped.
	void saveData();

	// Member variables.
	/// XRF detector.
	XRFDetector *detector_;
	/// An XRF scan.
	AMXRFScan *scan_;
};

#endif // VESPERSXRFSCANCONTROLLER_H
