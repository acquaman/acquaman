#ifndef XRFDETECTOR_H
#define XRFDETECTOR_H

#include "dataman/VESPERS/XRFDetectorInfo.h"
#include "beamline/AMDetector.h"

class XRFDetector : public XRFDetectorInfo, public AMDetector
{
	Q_OBJECT
public:
	/// Constructor for teh win.
	XRFDetector(QString name);
};

#endif // XRFDETECTOR_H
