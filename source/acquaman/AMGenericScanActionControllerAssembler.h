#ifndef AMGENERICSCANACTIONCONTROLLERASSEMBLER_H
#define AMGENERICSCANACTIONCONTROLLERASSEMBLER_H

#include "acquaman/AMScanActionControllerScanAssembler.h"

/// Builds the list of actions that can run an entire scan.  Has the base code for all simple scans.
class AMGenericScanActionControllerAssembler : public AMScanActionControllerScanAssembler
{
	Q_OBJECT

public:
	/// Constructor.
	AMGenericScanActionControllerAssembler(QObject *parent = 0);
	/// Destructor.
	virtual ~AMGenericScanActionControllerAssembler() {}

protected:
	/// Does all the work of building the action tree.
	virtual bool generateActionTreeImplmentation();

	/// Method that determines what type of AMScanAxis needs to be created and then passes that off to the appropriate method.
	AMAction3* generateActionTreeForAxis(AMControl *axisControl, AMScanAxis *scanAxis);
	/// Method that builds a very generic start, delta, end, constant dwell time step axis.
	AMAction3* generateActionTreeForStepAxis(AMControl *axisControl, AMScanAxis *stepScanAxis);
	/// Method that builds a section of an action based on the given AMScanAxisRegion.
	AMAction3* generateActionTreeForStepAxisRegion(AMControl *axisControl, AMScanAxisRegion *stepScanAxisRegion, bool isFinalRegion);
	/// Method that builds a continuous move axis with the given control and axis.
	AMAction3* generateActionTreeForContinuousMoveAxis(AMControl *axisControl, AMScanAxis *continuiousMoveScanAxis);
	/// Method that builds a continuous dwell axis with the given control and axis.
	AMAction3* generateActionTreeForContinuousDwellAxis(AMControl *axisControl, AMScanAxis *continuousDwellScanAxis);
	/// Method that determines what kind of acquisition trigger is required and then creates a list to ensure that all detectors are triggered appropriately.
	AMAction3* generateActionListForDetectorAcquisition();
	/// Method that generates a list of dwell triggers to ensure that all detectors are told to acquire.  Uses the prefilled detector list.
	AMAction3* generateActionListForStepDetectorAcquisition();
	/// Method that generates a list of dwell triggers for continuous dwell acquisitions.
	AMAction3* generateActionListForContinuousDetectorAcquisition();
	/// Method that creates the actions that are required to prepare the detectors to be properly triggered.
	AMAction3* generateActionListForDetectorInitialization();
	/// Method that creates the actions that are required to properly clean up the detector to be used for further acquisition.
	AMAction3* generateActionListForDetectorCleanup();
	/// Method that finds all the placeholder actions that are used to build the action tree effectively.
	QList<AMAction3*> findInsertionPoints(AMAction3 *action);
};

#endif // AMGENERICSCANACTIONCONTROLLERASSEMBLER_H
