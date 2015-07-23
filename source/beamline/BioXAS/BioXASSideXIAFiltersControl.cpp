#include "BioXASSideXIAFiltersControl.h"

BioXASSideXIAFiltersControl::BioXASSideXIAFiltersControl(const QString &name, QObject *parent) :
	CLSXIAFiltersControl(name, parent)
{

}

BioXASSideXIAFiltersControl::~BioXASSideXIAFiltersControl()
{

}

bool BioXASSideXIAFiltersControl::canMeasure() const
{
	return false;
}

bool BioXASSideXIAFiltersControl::canMove() const
{
	return false;
}

bool BioXASSideXIAFiltersControl::canStop() const
{
	return false;
}

bool BioXASSideXIAFiltersControl::validValue(double value) const
{
	return false;
}

bool BioXASSideXIAFiltersControl::validSetpoint(double value) const
{
	return false;
}

void BioXASSideXIAFiltersControl::updateConnected()
{

}

void BioXASSideXIAFiltersControl::updateValue()
{

}

AMAction3* BioXASSideXIAFiltersControl::createMoveAction(double setpoint)
{
	return 0;
}

