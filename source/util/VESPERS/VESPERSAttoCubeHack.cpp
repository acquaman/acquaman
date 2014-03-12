#include "VESPERSAttoCubeHack.h"
#include "beamline/AMPVControl.h"

 VESPERSAttoCubeHack::~VESPERSAttoCubeHack(){}
VESPERSAttoCubeHack::VESPERSAttoCubeHack(AMControl *Rz, QObject *parent)
	: QObject(parent)
{
	Rz_ = Rz;
	Rzfbk_ = new AMReadOnlyPVControl("RzFeedback", "SVM1607-2-B21-09:deg:fbk", this);
	connect(Rzfbk_, SIGNAL(valueChanged(double)), this, SLOT(onValueChanged()));
}

void VESPERSAttoCubeHack::onValueChanged()
{
	if (Rz_->isMoving() && fabs(Rzfbk_->value()-Rz_->value()) < 0.01)
		Rz_->stop();
}
