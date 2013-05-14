#ifndef VESPERSPILATUSCCDDETECTORVIEW_H
#define VESPERSPILATUSCCDDETECTORVIEW_H

#include "ui/VESPERS/VESPERSCCDDetectorView.h"
#include "beamline/VESPERS/VESPERSPilatusCCDDetector.h"

/// This class extends the basic detector view and adds in some file system information and enables copying all the images to the aurora drive.
class VESPERSPilatusCCDDetectorView : public VESPERSCCDDetectorView
{
	Q_OBJECT

public:
	/// Constructor.  Extends the standard CCD detector view.
	Q_INVOKABLE explicit VESPERSPilatusCCDDetectorView(VESPERSPilatusCCDDetector *detector, QWidget *parent = 0);
};

#endif // VESPERSPILATUSCCDDETECTORVIEW_H
