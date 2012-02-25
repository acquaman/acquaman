#include "REIXSMoveToSampleTransferPositionActionInfo.h"

REIXSMoveToSampleTransferPositionActionInfo::REIXSMoveToSampleTransferPositionActionInfo(QObject *parent) :
	AMActionInfo("Move to Sample Transfer Position", "Move the REIXS sample manipulator to the transfer position", ":/32x32/media-eject.png", parent)
{
	positions_.append(AMControlInfo("sampleX", 0));
	positions_.append(AMControlInfo("sampleY", 0));
	positions_.append(AMControlInfo("sampleZ", 0));
	positions_.append(AMControlInfo("sampleTheta", 0));
}

REIXSMoveToSampleTransferPositionActionInfo::REIXSMoveToSampleTransferPositionActionInfo(const REIXSMoveToSampleTransferPositionActionInfo &other) :
	AMActionInfo(other)
{
	positions_.setValuesFrom(other.positions_);
}
