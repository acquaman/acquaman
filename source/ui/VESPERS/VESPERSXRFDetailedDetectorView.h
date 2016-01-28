#ifndef VESPERSXRFDETAILEDDETECTORVIEW_H
#define VESPERSXRFDETAILEDDETECTORVIEW_H

#include "ui/beamline/AMXRFDetailedDetectorView.h"

#include "ui/dataman/AMChooseScanDialog.h"

class AMExportController;

class VESPERSXRFDetailedDetectorView : public AMXRFDetailedDetectorView
{
	Q_OBJECT

public:
	/// Constructor.  Takes a normal AMXRFDetector.
	VESPERSXRFDetailedDetectorView(AMXRFDetector *detector, QWidget *parent = 0);
	/// Destructor.
	virtual ~VESPERSXRFDetailedDetectorView(){}

//	/// Re-implementing to add the save button.
//	virtual void buildDetectorView();

//protected slots:
//	/// Starts the acquisition.  Calls acquire() but subclasses can reimplement if there is a more sofisticated start routine.
//	virtual void startAcquisition();

//	/// Handles bringing up and exporting the given XRF scans.
//	void onSaveButtonClicked();
//	/// Handles grabbing the scan and exporting it.
//	void exportScan();
//	/// Handles deleting the export controller.
//	void onExportControllerStateChanged(int state);

//protected:
//	/// Choose scan dialog.
//	AMChooseScanDialog *chooseScanDialog_;
//	/// The export controller.
//	AMExportController *exportController_;
};

#endif // VESPERSXRFDETAILEDDETECTORVIEW_H
