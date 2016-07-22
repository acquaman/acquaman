#include "PGMMonoGratingSelectionControl.h"

#include "beamline/AMPVControl.h"
#include "actions3/AMActionSupport.h"

PGMMonoGratingSelectionControl::PGMMonoGratingSelectionControl(QObject *parent)
	: AMEnumeratedControl("Grating Selection", QString(), parent)
{
	gratingSelectionPVControl_ = new AMPVwStatusControl("Grating Selection PV", "PGM_mono:choice", "PGM_mono:choice", "BL1611-ID-2:status", QString(), this);
	addChildControl(gratingSelectionPVControl_);

	addOption(Low, "Low");
	addOption(Medium, "Medium");
	addOption(High, "High");

	setAllowsMovesWhileMoving(false);
	setContextKnownDescription("Grating Selection Control");

	updateStates();
}

bool PGMMonoGratingSelectionControl::shouldMeasure() const
{
	return true;
}

bool PGMMonoGratingSelectionControl::shouldMove() const
{
	return true;
}

bool PGMMonoGratingSelectionControl::shouldStop() const
{
	return false;
}

bool PGMMonoGratingSelectionControl::canMeasure() const
{
	return shouldMeasure() && gratingSelectionPVControl_ && gratingSelectionPVControl_->canMeasure();
}

bool PGMMonoGratingSelectionControl::canMove() const
{
	return shouldMove() && gratingSelectionPVControl_ && gratingSelectionPVControl_->canMove();
}

bool PGMMonoGratingSelectionControl::canStop() const
{
	return shouldStop() && gratingSelectionPVControl_ && gratingSelectionPVControl_->canStop();
}

bool PGMMonoGratingSelectionControl::validValue(double value) const
{
	int gratingValue = int(value);

	return gratingValue == Low || gratingValue == Medium || gratingValue == High || gratingValue == Unknown;
}

bool PGMMonoGratingSelectionControl::validSetpoint(double value) const
{
	Grating gratingValue = Grating(value);

	return gratingValue == Low || gratingValue == Medium || gratingValue == High;
}

void PGMMonoGratingSelectionControl::updateConnected()
{
	setConnected(gratingSelectionPVControl_ && gratingSelectionPVControl_->isConnected());
}

void PGMMonoGratingSelectionControl::updateMoving()
{
	setIsMoving(isConnected() && gratingSelectionPVControl_->isMoving());
}

AMAction3 *PGMMonoGratingSelectionControl::createMoveAction(double setpoint)
{
	AMAction3 *result = 0;

	if (int(setpoint) == Low)
		result = AMActionSupport::buildControlMoveAction(gratingSelectionPVControl_, PGMMONOGRATINGSELECTIONCONTROL_PV_LOW);
	else if (int(setpoint) == Medium)
		result = AMActionSupport::buildControlMoveAction(gratingSelectionPVControl_, PGMMONOGRATINGSELECTIONCONTROL_PV_MEDIUM);
	else if (int(setpoint) == High)
		result = AMActionSupport::buildControlMoveAction(gratingSelectionPVControl_, PGMMONOGRATINGSELECTIONCONTROL_PV_HIGH);

	return result;
}

int PGMMonoGratingSelectionControl::currentIndex() const
{
	int result = enumNames().indexOf("Unknown");

	if (canMeasure()) {
		int intValue = int(gratingSelectionPVControl_->value());

		if (intValue == PGMMONOGRATINGSELECTIONCONTROL_PV_LOW)
			result = Low;
		else if (intValue == PGMMONOGRATINGSELECTIONCONTROL_PV_MEDIUM)
			result = Medium;
		else if (intValue == PGMMONOGRATINGSELECTIONCONTROL_PV_HIGH)
			result = High;
	}

	return result;
}

