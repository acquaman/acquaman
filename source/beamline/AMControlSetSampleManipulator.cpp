#include "AMControlSetSampleManipulator.h"

#include "beamline/AMControlSet.h"

AMControlSetSampleManipulator::AMControlSetSampleManipulator(AMControlSet* controlSet)
{
	controlSet_ = controlSet;
}

AMControlInfoList AMControlSetSampleManipulator::position() const
{
	return controlSet_->toInfoList();
}

bool AMControlSetSampleManipulator::moveToPosition(const AMControlInfoList &newPosition)
{
	if(!controlSet_->isConnected())
		return false;

	controlSet_->setFromInfoList(newPosition);
	return true;
}
