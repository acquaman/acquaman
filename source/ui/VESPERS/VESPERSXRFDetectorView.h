#ifndef VESPERSXRFDETECTORVIEW_H
#define VESPERSXRFDETECTORVIEW_H

#include "ui/beamline/AMXRFDetailedDetectorView.h"

#include "actions3/actions/AMScanAction.h"
#include "acquaman/VESPERS/VESPERSXRFScanConfiguration.h"

/// This class extends the AMXRFDetailedDetectorView to always save acquisitions started by the view.
class VESPERSXRFDetectorView : public AMXRFDetailedDetectorView
{
	Q_OBJECT

public:
	/// Constructor.
	VESPERSXRFDetectorView(AMXRFDetector *detector, QWidget *parent = 0);

protected slots:
	/// Starts the acquisition.  Calls acquire() but subclasses can reimplement if there is a more sofisticated start routine.
	virtual void startAcquisition();
	/// Cancles the acquisition.  Calls cancel() but subclasses can reimplement if they need a more thorough cancel.
	virtual void cancelAcquisition();
	/// Handles the action cleanup after it's finished.  This is called regardless of the final state.
	void onScanActionFinished();

protected:
	/// The scan action that runs the XRF scan controller.
	AMScanAction *scanAction_;
	/// Holds the VESPERS XRF scan configuration.
	VESPERSXRFScanConfiguration *scanConfiguration_;
};

#endif // VESPERSXRFDETECTORVIEW_H
