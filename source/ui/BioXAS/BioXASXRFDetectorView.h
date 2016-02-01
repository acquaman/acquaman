#ifndef BIOXASXRFDETECTORVIEW_H
#define BIOXASXRFDETECTORVIEW_H

#include "ui/beamline/AMXspress3XRFDetectorView.h"

class BioXASXRFDetectorView : public AMXspress3XRFDetectorView
{
	Q_OBJECT

public:
	/// Constructor.
	BioXASXRFDetectorView(AMXspress3XRFDetector *detector, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASXRFDetectorView(){}

protected slots:
	/// Starts the acquisition.  Calls acquire() but subclasses can reimplement if there is a more sofisticated start routine.
	virtual void startAcquisition();
};

#endif // BIOXASXRFDETECTORVIEW_H
