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

protected slots:
	/// Helper slot to finish up a scan.
	void onDetectorAcquisitionFinished();
	/// Slot that updates the data stored in the scan.
	void onDetectorAcquisitionUpdate();

protected:
	/// Initializes the scan
	virtual void initializeImplementation() { setInitialized(); }
	/// Starts current scan.
	virtual void startImplementation();
	/// Cancels current scan.  Treated as finishing early.
	virtual void cancelImplementation() { onDetectorAcquisitionFinished(); }

	// Member variables.
	/// XRF detector.
	XRFDetector *detector_;
	/// An XRF scan.
	AMXRFScan *scan_;
};

#endif // VESPERSXRFSCANCONTROLLER_H
