#ifndef VESPERSTIMEDLINESCANACTIONCONTROLLER_H
#define VESPERSTIMEDLINESCANACTIONCONTROLLER_H

#include "acquaman/VESPERS/VESPERSSpatialLineScanActionController.h"
#include "acquaman/VESPERS/VESPERSTimedLineScanConfiguration.h"

class VESPERSTimedLineScanActionController : public VESPERSSpatialLineScanActionController
{
	Q_OBJECT

public:
	/// Constructor.
	VESPERSTimedLineScanActionController(VESPERSTimedLineScanConfiguration *configuration, QObject *parent = 0);
	/// Destructor.
	~VESPERSTimedLineScanActionController();
};

#endif // VESPERSTIMEDLINESCANACTIONCONTROLLER_H
