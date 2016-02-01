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

	/// Method that builds all the general aspects, such as measurements and raw data sources, and the file writer capabilities for the scan controller.
	virtual void buildScanController();

protected:
	/// Creates the scan assembler that builds all the actions used to run the scan.
	virtual void createScanAssembler();
	/// Method that prefills the scan's raw data store for 2D and higher dimensionality scans.
	virtual void prefillScanPoints();
	/// Adds anything extra (eg: analysis blocks) to the scan before it's started.
	virtual void buildScanControllerImplementation();

	/// The timed line scan configuration.
	VESPERSTimedLineScanConfiguration *timedConfiguration_;
};

#endif // VESPERSTIMEDLINESCANACTIONCONTROLLER_H
