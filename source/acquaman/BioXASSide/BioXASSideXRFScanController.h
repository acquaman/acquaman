#ifndef BIOXASSIDEXRFSCANCONTROLLER_H
#define BIOXASSIDEXRFSCANCONTROLLER_H

#include "acquaman/BioXASSide/BioXASSideXRFScanConfiguration.h"
#include "acquaman/AMScanController.h"
#include "beamline/AMXRFDetector.h"

class BioXASSideXRFScanController : public AMScanController
{
	Q_OBJECT

public:
	/// Default constructor.
	BioXASSideXRFScanController(BioXASSideXRFScanConfiguration *configuration, QObject *parent = 0);
	/// Destructor.
	virtual ~BioXASSideXRFScanController();

protected slots:
	/// Helper slot to finish up a scan.
	void onDetectorAcquisitionFinished();
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
	/// Cancels current scan. Treated as finishing early.
	virtual void stopImplementation(const QString &command) { Q_UNUSED(command); setFinished(); }
	/// Saves the data after a scan is stopped.
	void saveData();

	/// The pointer to the detector that we're using.
	AMXRFDetector *detector_;
};

#endif // BIOXASSIDEXRFSCANCONTROLLER_H
