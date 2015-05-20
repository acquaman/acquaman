#include "VESPERSXIAFilterControl.h"

VESPERSXIAFilterControl::VESPERSXIAFilterControl(const QString &name, QObject *parent)
	: AMPseudoMotorControl(name, " mm", parent)
{

}

VESPERSXIAFilterControl::~VESPERSXIAFilterControl()
{

}

bool VESPERSXIAFilterControl::canMeasure() const
{
}

bool VESPERSXIAFilterControl::canMove() const
{
}

bool VESPERSXIAFilterControl::canStop() const
{
}
