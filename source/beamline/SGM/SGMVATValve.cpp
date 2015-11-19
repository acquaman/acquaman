#include "SGMVATValve.h"

SGMVATValve::SGMVATValve(const QString &name, QObject *parent) :
	SGMControl(name, "", parent)
{
	position_ = new AMSinglePVControl("vatValvePosition", "VVR1611-4-I10-11:ctrl:posn", this);
	addChildControl(position_);

	speed_ = new AMSinglePVControl("vatValveSpeed", "VVR1611-4-I10-11:valveSpeed", this);
	addChildControl(speed_);
}

SGMVATValve::~SGMVATValve()
{

}
