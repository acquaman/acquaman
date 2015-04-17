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

protected:
	/// Creates the scan assembler that builds all the actions used to run the scan.
	virtual void createScanAssembler();

	/// The timed line scan configuration.
	VESPERSTimedLineScanConfiguration *timedConfiguration_;
};

#endif // VESPERSTIMEDLINESCANACTIONCONTROLLER_H
