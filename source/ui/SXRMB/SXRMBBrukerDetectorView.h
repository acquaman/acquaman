#ifndef SXRMBBRUKERDETECTORVIEW_H
#define SXRMBBRUKERDETECTORVIEW_H

#include "ui/beamline/AMXRFDetailedDetectorView.h"

#include "beamline/SXRMB/SXRMBBrukerDetector.h"

class SXRMBBrukerDetectorView : public AMXRFDetailedDetectorView
{
	Q_OBJECT

public:
	/// Constructor.
	SXRMBBrukerDetectorView(SXRMBBrukerDetector *detector, QWidget *parent = 0);
	/// Destructor.
	virtual ~SXRMBBrukerDetectorView();

	/// Enable the dead time label even though the bruker doesn't have a dead time in the same way the other XRF detectors do.
	void enableDeadTimeDisplay();
};

#endif // SXRMBBRUKERDETECTORVIEW_H
