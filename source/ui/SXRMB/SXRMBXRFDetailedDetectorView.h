#ifndef SXRMBXRFDETAILEDDETECTORVIEW_H
#define SXRMBXRFDETAILEDDETECTORVIEW_H

#include "ui/beamline/AMXRFDetailedDetectorView.h"
#include "beamline/SXRMB/SXRMBBrukerDetector.h"

class SXRMBXRFDetailedDetectorView : public AMXRFDetailedDetectorView
{
	Q_OBJECT

public:
	/// Constructor.  Builds a more detailed view for SXRMBBrukerDetector.
	SXRMBXRFDetailedDetectorView(SXRMBBrukerDetector *detector, QWidget *parent = 0);
	/// Destructor.
	virtual ~SXRMBXRFDetailedDetectorView();

	/// enable the deadtimeLabel
	void enableDeadTimeDisplay();

protected slots:
	/// Handles updating the dead time label.
	virtual void onDeadTimeChanged();

};

#endif // SXRMBXRFDETAILEDDETECTORVIEW_H
