#ifndef AMTIMEDSCANACTIONCONTROLLERASSEMBLER_H
#define AMTIMEDSCANACTIONCONTROLLERASSEMBLER_H

#include "acquaman/AMScanActionControllerScanAssembler.h"

class AMTimedScanActionControllerAssembler : public AMScanActionControllerScanAssembler
{
	Q_OBJECT

public:
	/// Constructor.
	AMTimedScanActionControllerAssembler(double acquisitionTime, double timePerAcquisition, int iterations, QObject *parent = 0);
	/// Destructor.
	virtual ~AMTimedScanActionControllerAssembler();

protected:
	/// Does all the work of building the action tree.
	virtual bool generateActionTreeImplmentation();

	/// Method that determines what kind of acquisition trigger is required and then creates a list to ensure that all detectors are triggered appropriately.
	AMAction3* generateActionListForDetectorAcquisition();
	/// Method that creates the actions that are required to prepare the detectors to be properly triggered.
	AMAction3* generateActionListForDetectorInitialization();
	/// Method that creates the actions that are required to properly clean up the detector to be used for further acquisition.
	AMAction3* generateActionListForDetectorCleanup();

	/// Acquisition time.
	double acquisitionTime_;
	/// The overall time per acquisition.
	double timePerAcquisition_;
	/// The number of times we'll repeat this process.
	int iterations_;
};

#endif // AMTIMEDSCANACTIONCONTROLLERASSEMBLER_H
