#include "SGMVATValve.h"
#include "source/beamline/AMPVControl.h"
#include "beamline/SGM/SGMVATValveState.h"

SGMVATValve::SGMVATValve(const QString &deviceName, const QString &baseName, QObject *parent) :
	AMConnectedControl(deviceName, "", parent)
{
	position_ = new AMPVControl(name(), baseName+":position", baseName+":ctrl:posn", "", this);

	speed_ = new AMSinglePVControl(name(), baseName+":valveSpeed", this);

	state_ = new SGMVATValveState(name(), this);
	state_->setPosition(position_);
	state_->setSpeed(speed_);
}

SGMVATValve::~SGMVATValve()
{

}
