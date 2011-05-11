#include "PIDLoopControl.h"

PIDLoopControl::PIDLoopControl(QString name, AMControl *pidX, AMControl *pidY, AMControl *pidZ, QObject *parent)
	: QObject(parent)
{
	name_ = name;

	// Need read/write control.
	x_ = qobject_cast<AMPVControl *>(pidX);
	y_ = qobject_cast<AMPVControl *>(pidY);
	z_ = qobject_cast<AMPVControl *>(pidZ);

	connect(x_, SIGNAL(valueChanged(double)), this, SLOT(onXStateChanged()));
	connect(y_, SIGNAL(valueChanged(double)), this, SLOT(onYStateChanged()));
	connect(z_, SIGNAL(valueChanged(double)), this, SLOT(onZStateChanged()));
}
