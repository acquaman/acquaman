#include "PIDLoopControl.h"

PIDLoopControl::PIDLoopControl(AMPVControl *pidX, AMPVControl *pidY, AMPVControl *pidZ, QObject *parent)
	: QObject(parent)
{
	x_ = pidX;
	y_ = pidY;
	z_ = pidZ;
}
