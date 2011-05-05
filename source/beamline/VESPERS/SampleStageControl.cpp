#include "SampleStageControl.h"

SampleStageControl::SampleStageControl(AMPVwStatusControl *horiz, AMPVwStatusControl *vert, AMPVwStatusControl *norm, AMReadOnlyPVControl *xMotor, AMReadOnlyPVControl *yMotor, AMReadOnlyPVControl *zMotor, QObject *parent)
	: QObject(parent)
{
	// The limits.
	xLow_ = 0;
	yLow_= 0;
	zLow_ = 0;
	xHigh_ = 0;
	yHigh_ = 0;
	zHigh_ = 0;

	// The motor controls
	horiz_ = horiz;
	vert_ = vert;
	norm_ = norm;
	xMotor_ = xMotor;
	yMotor_ = yMotor;
	zMotor_ = zMotor;
}

void SampleStageControl::moveHorizontal(double setpoint)
{

}

void SampleStageControl::moveVertical(double setpoint)
{

}

void SampleStageControl::moveNormal(double setpoint)
{

}
