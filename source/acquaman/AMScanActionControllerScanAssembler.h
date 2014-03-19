#ifndef AMSCANACTIONCONTROLLERSCANASSEMBLER_H
#define AMSCANACTIONCONTROLLERSCANASSEMBLER_H

#include "beamline/AMDetectorSet.h"
#include "beamline/AMControlSet.h"
#include "dataman/AMScanAxis.h"

class AMScanActionControllerScanAssembler : public QObject
{
	Q_OBJECT

public:
	AMScanActionControllerScanAssembler(QObject *parent = 0);
	virtual ~AMScanActionControllerScanAssembler();

	bool insertAxis(int index, AMControl *axisControl, AMScanAxis *scanAxis);
	bool appendAxis(AMControl *axisControl, AMScanAxis *scanAxis);
	bool addDetector(AMDetector *detector);

	void generateActionTree();

signals:
	void actionTreeGenerated(AMAction3 *actionTree);

protected:
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

protected:
	AMDetectorSet *detectors_;
	AMControlSet *controls_;
	QList<AMScanAxis*> axes_;

	AMAction3 *actionTree_;
};

#endif // AMSCANACTIONCONTROLLERSCANASSEMBLER_H
