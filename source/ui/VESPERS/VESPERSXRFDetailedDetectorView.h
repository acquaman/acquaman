#ifndef VESPERSXRFDETAILEDDETECTORVIEW_H
#define VESPERSXRFDETAILEDDETECTORVIEW_H

#include "ui/beamline/AMXRFDetailedDetectorView.h"

class VESPERSXRFDetailedDetectorView : public AMXRFDetailedDetectorView
{
	Q_OBJECT

public:
	/// Constructor.  Takes a normal AMXRFDetector.
	VESPERSXRFDetailedDetectorView(AMXRFDetector *detector, QWidget *parent = 0);
	/// Destructor.
	virtual ~VESPERSXRFDetailedDetectorView(){}

protected slots:
	/// Starts the acquisition.  Calls acquire() but subclasses can reimplement if there is a more sofisticated start routine.
	virtual void startAcquisition();

};

#endif // VESPERSXRFDETAILEDDETECTORVIEW_H
