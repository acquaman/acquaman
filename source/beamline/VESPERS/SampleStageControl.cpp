#include "SampleStageControl.h"

SampleStageControl::SampleStageControl(AMControl *horiz, AMControl *vert, AMControl *norm, QObject *parent)
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
	sx_ = 1;
	sy_ = 1;
	sz_ = 1;

	// The motor controls
	horiz_ = horiz;
	vert_ = vert;
	norm_ = norm;

	connect(horiz_, SIGNAL(connected(bool)), this, SIGNAL(connected(bool)));
	connect(vert_, SIGNAL(connected(bool)), this, SIGNAL(connected(bool)));
	connect(norm_, SIGNAL(connected(bool)), this, SIGNAL(connected(bool)));

	connect(horiz_, SIGNAL(movingChanged(bool)), this, SIGNAL(movingChanged(bool)));
	connect(vert_, SIGNAL(movingChanged(bool)), this, SIGNAL(movingChanged(bool)));
	connect(norm_, SIGNAL(movingChanged(bool)), this, SIGNAL(movingChanged(bool)));

	connect(horiz_, SIGNAL(setpointChanged(double)), this, SIGNAL(horizontalSetpointChanged(double)));
	connect(vert_, SIGNAL(setpointChanged(double)), this, SIGNAL(verticalSetpointChanged(double)));
	connect(norm_, SIGNAL(setpointChanged(double)), this, SIGNAL(normalSetpointChanged(double)));
}

bool SampleStageControl::setMotors(AMControl *horiz, AMControl *vert, AMControl *norm)
{
	// Check to see if the motors that were passed in are valid.
	if (horiz == 0 || vert == 0 || norm == 0)
		return false;

	disconnect(horiz_, SIGNAL(connected(bool)), this, SIGNAL(connected(bool)));
	disconnect(vert_, SIGNAL(connected(bool)), this, SIGNAL(connected(bool)));
	disconnect(norm_, SIGNAL(connected(bool)), this, SIGNAL(connected(bool)));

	disconnect(horiz_, SIGNAL(movingChanged(bool)), this, SIGNAL(movingChanged(bool)));
	disconnect(vert_, SIGNAL(movingChanged(bool)), this, SIGNAL(movingChanged(bool)));
	disconnect(norm_, SIGNAL(movingChanged(bool)), this, SIGNAL(movingChanged(bool)));

	disconnect(horiz_, SIGNAL(setpointChanged(double)), this, SIGNAL(horizontalSetpointChanged(double)));
	disconnect(vert_, SIGNAL(setpointChanged(double)), this, SIGNAL(verticalSetpointChanged(double)));
	disconnect(norm_, SIGNAL(setpointChanged(double)), this, SIGNAL(normalSetpointChanged(double)));

	horiz_ = h;
	vert_ = v;
	norm_ = n;

	sx_ = 1;
	sy_ = 1;
	sz_ = 1;

	connect(horiz_, SIGNAL(connected(bool)), this, SIGNAL(connected(bool)));
	connect(vert_, SIGNAL(connected(bool)), this, SIGNAL(connected(bool)));
	connect(norm_, SIGNAL(connected(bool)), this, SIGNAL(connected(bool)));

	connect(horiz_, SIGNAL(movingChanged(bool)), this, SIGNAL(movingChanged(bool)));
	connect(vert_, SIGNAL(movingChanged(bool)), this, SIGNAL(movingChanged(bool)));
	connect(norm_, SIGNAL(movingChanged(bool)), this, SIGNAL(movingChanged(bool)));

	connect(horiz_, SIGNAL(writePVValueChanged(double)), this, SIGNAL(horizontalSetpointChanged(double)));
	connect(vert_, SIGNAL(writePVValueChanged(double)), this, SIGNAL(verticalSetpointChanged(double)));
	connect(norm_, SIGNAL(writePVValueChanged(double)), this, SIGNAL(normalSetpointChanged(double)));

	return true;
}

void SampleStageControl::moveHorizontal(double setpoint)
{
	if (validNewXPosition(setpoint))
		horiz_->move(setpoint);
	else
		emit moveError(horiz_->name());
}

void SampleStageControl::moveVertical(double setpoint)
{
	if (validNewYPosition(setpoint) && validNewZPosition(setpoint))
		vert_->move(setpoint);
	else
		emit moveError(vert_->name());
}

void SampleStageControl::moveNormal(double setpoint)
{
	if (validNewYPosition(setpoint) && validNewZPosition(setpoint))
		norm_->move(setpoint);
	else
		emit moveError(norm_->name());
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
	// Turn the setpoint into counts.  This currently is hard coded.
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
