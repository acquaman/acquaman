#include "PGMMonoGratingSelectionControl.h"

#include "beamline/AMPVControl.h"
#include "actions3/AMActionSupport.h"

PGMMonoGratingSelectionControl::PGMMonoGratingSelectionControl(QObject *parent)
	: AMEnumeratedControl("Grating Selection", QString(), parent)
{
	gratingSelectionPVControl_ = new AMPVwStatusControl("Grating Selection PV", "PGM_mono:choice", "PGM_mono:choice", "BL1611-ID-2:status", QString(), this);

	addOption(Low, "Low");
	addOption(Medium, "Medium");
	addOption(High, "High");
	addOption(Unknown, "Unknown", true);

	setAllowsMovesWhileMoving(false);
	setContextKnownDescription("Grating Selection Control");

	updateStates();

	addChildControl(gratingSelectionPVControl_);
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

	return gratingValue == 1 || gratingValue == 2 || gratingValue == 3 || gratingValue == -1;
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
	return AMActionSupport::buildControlMoveAction(gratingSelectionPVControl_, setpoint, false, false);
}

int PGMMonoGratingSelectionControl::currentIndex() const
{
	if (!isConnected()) {

		return -1;
	}

	int intValue = int(gratingSelectionPVControl_->value());

	// The PV uses 3 for unknown, the enumerated control interface uses -1
	if (intValue == 4) {
		return -1;
	}

	// Else we can just map the PV value to our enum
	return Grating(intValue);
}

