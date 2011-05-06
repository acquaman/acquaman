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

	// Scalers.
	sx_ = 0;
	sy_ = 0;
	sz_ = 0;

	// The motor controls
	horiz_ = horiz;
	vert_ = vert;
	norm_ = norm;
	xMotor_ = xMotor;
	yMotor_ = yMotor;
	zMotor_ = zMotor;

	connect(horiz_, SIGNAL(connected(bool)), this, SIGNAL(connected(bool)));
	connect(vert_, SIGNAL(connected(bool)), this, SIGNAL(connected(bool)));
	connect(norm_, SIGNAL(connected(bool)), this, SIGNAL(connected(bool)));

	connect(horiz_, SIGNAL(movingChanged(bool)), this, SIGNAL(movingChanged(bool)));
	connect(vert_, SIGNAL(movingChanged(bool)), this, SIGNAL(movingChanged(bool)));
	connect(norm_, SIGNAL(movingChanged(bool)), this, SIGNAL(movingChanged(bool)));

	connect(horiz_, SIGNAL(writePVValueChanged(double)), this, SIGNAL(horizontalSetpointChanged(double)));
	connect(vert_, SIGNAL(writePVValueChanged(double)), this, SIGNAL(verticalSetpointChanged(double)));
	connect(norm_, SIGNAL(writePVValueChanged(double)), this, SIGNAL(normalSetpointChanged(double)));
}

void SampleStageControl::moveHorizontal(double setpoint)
{
	if (validNewXPosition(setpoint))
		horiz_->move(setpoint);
}

void SampleStageControl::moveVertical(double setpoint)
{
	if (validNewYPosition(setpoint) && validNewZPosition(setpoint))
		vert_->move(setpoint);
}

void SampleStageControl::moveNormal(double setpoint)
{
	if (validNewYPosition(setpoint) && validNewZPosition(setpoint))
		norm_->move(setpoint);
}

bool SampleStageControl::validNewXPosition(double setpoint)
{
	// Turn the setpoint into counts.  This currently is hard coded.
	if (xLow_ == xHigh_)
		return true;

	int spCount = (int)setpoint*10000*sx_;

	if (spCount > xLow_ && spCount < xHigh_)
		return true;

	return false;
}

bool SampleStageControl::validNewYPosition(double setpoint)
{
	// Turn the setpoint into counts.  This currently is the coded.
	if (yLow_ == yHigh_)
		return true;

	int spCount = (int)setpoint*10000*sy_;

	if (spCount > yLow_ && spCount < yHigh_)
		return true;

	return false;
}

bool SampleStageControl::validNewZPosition(double setpoint)
{
	// Turn the setpoint into counts.  This currently is hard coded.
	if (zLow_ == zHigh_)
		return true;

	int spCount = (int)setpoint*10000*sz_;

	if (spCount > zLow_ && spCount < zHigh_)
		return true;

	return false;
}
