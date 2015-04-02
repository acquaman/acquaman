#include "CLSStandardsWheel.h"

CLSStandardsWheel::CLSStandardsWheel(const QString &name, const QString &basePVName, QObject *parent)
	: QObject(parent)
{
	wheel_ = new CLSMAXvMotor(name, basePVName, name, true, 0.1, 2.0, this, ":deg");
}

CLSStandardsWheel::~CLSStandardsWheel()
{

}

void CLSStandardsWheel::moveToIndex(int index)
{
	if (index < 12 && index >= 0)
		wheel_->move(indexToDegrees(index));
}

double CLSStandardsWheel::indexToDegrees(int index) const
{
	return double(index*30.0);
}
