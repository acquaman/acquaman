#ifndef SXRMBXRFDETAILEDDETECTORVIEW_H
#define SXRMBXRFDETAILEDDETECTORVIEW_H

#include "beamline/SXRMB/SXRMBBrukerDetector.h"

#include "ui/beamline/AMXRFDetailedDetectorView.h"
#include "ui/dataman/AMChooseScanDialog.h"

class AMExportController;

class SXRMBXRFDetailedDetectorView : public AMXRFDetailedDetectorView
{
	Q_OBJECT

public:
	/// Constructor.  Builds a more detailed view for SXRMBBrukerDetector.
	SXRMBXRFDetailedDetectorView(AMXRFDetector *detector, QWidget *parent = 0);
	/// Destructor.
	virtual ~SXRMBXRFDetailedDetectorView();

	/// Re-implementing to add the save button.
	virtual void buildDetectorView();

protected slots:
	/// Starts the acquisition.  Calls acquire() but subclasses can reimplement if there is a more sofisticated start routine.
	virtual void startAcquisition();

	/// Handles bringing up and exporting the given XRF scans.
	void onSaveButtonClicked();
	/// Handles grabbing the scan and exporting it.
	void exportScan();
	/// Handles deleting the export controller.
	void onExportControllerStateChanged(int state);

private:
	/// Choose scan dialog.
	AMChooseScanDialog *chooseScanDialog_;
	/// The export controller.
	AMExportController *exportController_;

};

#endif // SXRMBXRFDETAILEDDETECTORVIEW_H
