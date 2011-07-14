#include "SampleStageControl.h"

SampleStageControl::SampleStageControl(AMControl *horiz, AMControl *vert, AMControl *norm, QObject *parent)
	: QObject(parent)
{
	// The limits.
	xRange_ = qMakePair(0, 0);
	yRange_ = qMakePair(0, 0);
	zRange_ = qMakePair(0, 0);

	// The calibration to convert from mm to counts.
	xCalibration_ = 10000;
	yCalibration_ = 10000;
	zCalibration_ = 10000;

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
	// Turn the setpoint into counts.
	if (xRange_.first == xRange_.second)
		return true;

	int spCount = (int)setpoint*xCalibration_*sx_;

	if (spCount > xRange_.first && spCount < xRange_.second)
		return true;

	return false;
}

bool SampleStageControl::validNewYPosition(double setpoint)
{
	// Turn the setpoint into counts.
	if (yRange_.first == yRange_.second)
		return true;

	int spCount = (int)setpoint*yCalibration_*sy_;

	if (spCount > yRange_.first && spCount < yRange_.second)
		return true;

	return false;
}

bool SampleStageControl::validNewZPosition(double setpoint)
{
	// Turn the setpoint into counts.
	if (zRange_.first == zRange_.second)
		return true;

	int spCount = (int)setpoint*zCalibration_*sz_;

	if (spCount > zRange_.first && spCount < zRange_.second)
		return true;

	return false;
}
