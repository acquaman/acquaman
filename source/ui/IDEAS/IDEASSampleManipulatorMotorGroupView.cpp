#include "IDEASSampleManipulatorMotorGroupView.h"


IDEASSampleManipulatorMotorGroupView::IDEASSampleManipulatorMotorGroupView(AMMotorGroupObject *motorGroupObject, QWidget *parent) :
    AMMotorGroupObjectView(motorGroupObject, parent)
{
	//motorGroupObject_ = motorGroupObject;

	jog_->setValue(1);
	jog_->setSingleStep(0.5);
	jog_->setDecimals(1);

	foreach(QDoubleSpinBox *cSP, controlSetpoints_)
		cSP->setDecimals(1);
}

IDEASSampleManipulatorMotorGroupView::~IDEASSampleManipulatorMotorGroupView()
{
}
