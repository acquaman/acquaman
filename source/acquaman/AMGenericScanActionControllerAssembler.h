#ifndef AMGENERICSCANACTIONCONTROLLERASSEMBLER_H
#define AMGENERICSCANACTIONCONTROLLERASSEMBLER_H

#include "acquaman/AMScanActionControllerScanAssembler.h"

class AMGenericScanActionControllerAssembler : public AMScanActionControllerScanAssembler
{
	Q_OBJECT

public:
	/// Constructor.
	AMGenericScanActionControllerAssembler(QObject *parent = 0);

protected:
	/// Does all the work of building the action tree.
	virtual bool generateActionTreeImplmentation();

	AMAction3* generateActionTreeForAxis(AMControl *axisControl, AMScanAxis *scanAxis);
	AMAction3* generateActionTreeForStepAxis(AMControl *axisControl, AMScanAxis *stepScanAxis);
	AMAction3* generateActionTreeForStepAxisRegion(AMControl *axisControl, AMScanAxisRegion *stepScanAxisRegion, bool isFinalRegion);
	AMAction3* generateActionTreeForContinuousMoveAxis(AMControl *axisControl, AMScanAxis *continuiousMoveScanAxis);
	AMAction3* generateActionTreeForContinuousDwellAxis(AMControl *axisControl, AMScanAxis *continuousDwellScanAxis);

	AMAction3* generateActionListForDetectorAcquisition();
	AMAction3* generateActionListForStepDetectorAcquisition();
	AMAction3* generateActionListForContinuousDetectorAcquisition();

	AMAction3* generateActionListForDetectorInitialization();
	AMAction3* generateActionListForDetectorCleanup();

	QList<AMAction3*> findInsertionPoints(AMAction3 *action);
};

#endif // AMGENERICSCANACTIONCONTROLLERASSEMBLER_H
