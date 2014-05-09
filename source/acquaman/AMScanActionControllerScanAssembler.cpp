#include "AMScanActionControllerScanAssembler.h"

AMScanActionControllerScanAssembler::~AMScanActionControllerScanAssembler(){}

AMScanActionControllerScanAssembler::AMScanActionControllerScanAssembler(QObject *parent) :
	QObject(parent)
{
	detectors_ = new AMDetectorSet(this);
	controls_ = new AMControlSet(this);
	actionTree_ = 0; //NULL
}

bool AMScanActionControllerScanAssembler::insertAxis(int index, AMControl *axisControl, AMScanAxis *scanAxis)
{
	if(!scanAxis)
		return false;
	if((axisControl && controls_->contains(axisControl->name())) || axes_.contains(scanAxis))
		return false;
	if(index < 0 || index > controls_->count())
		return false;
	if(controls_->count() != axes_.count())
		return false;

	if (axisControl)
		controls_->insert(index, axisControl, axisControl->name());

	else
		controls_->insert(index, axisControl, "");

	axes_.insert(index, scanAxis);

	return true;
}

bool AMScanActionControllerScanAssembler::appendAxis(AMControl *axisControl, AMScanAxis *scanAxis)
{
	return insertAxis(controls_->count(), axisControl, scanAxis);
}

bool AMScanActionControllerScanAssembler::addDetector(AMDetector *detector)
{
	return detectors_->addDetector(detector);
}

bool AMScanActionControllerScanAssembler::generateActionTree()
{
	if (generateActionTreeImplmentation()){

		emit actionTreeGenerated(actionTree_);
		return true;
	}

	return false;
}
