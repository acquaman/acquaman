#include "CLSCrossHairGeneratorControl.h"

CLSCrossHairGeneratorControl::CLSCrossHairGeneratorControl(const QString &baseVideoChannelPVName, const QString &baseCrossHairPVName, QObject *parent)
	: QObject(parent)
{
	horizontalPosition1_ = new AMSinglePVControl("Horizontal Position 1", QString("%1:posn:h0").arg(baseCrossHairPVName), this, 0.5);
	horizontalPosition2_ = new AMSinglePVControl("Horizontal Position 2", QString("%1:posn:h1").arg(baseCrossHairPVName), this, 0.5);
	verticalPosition1_ = new AMSinglePVControl("Vertical Position 1", QString("%1:posn:v0").arg(baseCrossHairPVName), this, 0.5);
	verticalPosition2_ = new AMSinglePVControl("Vertical Position 2", QString("%1:posn:v1").arg(baseCrossHairPVName), this, 0.5);

	horizontalType1_ = new AMSinglePVControl("Horizontal Type 1", QString("%1:type:h0").arg(baseCrossHairPVName), this, 0.5);
	horizontalType2_ = new AMSinglePVControl("Horizontal Type 2", QString("%1:type:h1").arg(baseCrossHairPVName), this, 0.5);
	verticalType1_ = new AMSinglePVControl("Vertical Position 1", QString("%1:type:v0").arg(baseCrossHairPVName), this, 0.5);
	verticalType2_ = new AMSinglePVControl("Vertical Position 2", QString("%1:type:v1").arg(baseCrossHairPVName), this, 0.5);

	displayState_ = new AMSinglePVControl("Display State", QString("%1:dpl:on").arg(baseCrossHairPVName), this, 0.5);
	boxState_ = new AMSinglePVControl("Box State", QString("%1:box:on").arg(baseCrossHairPVName), this, 0.5);
	intensity_ = new AMSinglePVControl("Intensity", QString("%1:ints:adj").arg(baseCrossHairPVName), this, 0.5);

	channel_ = new AMPVControl("Channel", QString("%1:channel:fbk").arg(baseVideoChannelPVName), QString("%1:channel").arg(baseVideoChannelPVName), QString(), this, 0.5);

	connect(horizontalPosition1_, SIGNAL(valueChanged(double)), this, SIGNAL(horizontalPosition1Changed()));
	connect(horizontalPosition2_, SIGNAL(valueChanged(double)), this, SIGNAL(horizontalPosition2Changed()));
	connect(verticalPosition1_, SIGNAL(valueChanged(double)), this, SIGNAL(verticalPosition1Changed()));
	connect(verticalPosition2_, SIGNAL(valueChanged(double)), this, SIGNAL(verticalPosition2Changed()));

	connect(horizontalType1_, SIGNAL(valueChanged(double)), this, SIGNAL(horizontalType1Changed()));
	connect(horizontalType2_, SIGNAL(valueChanged(double)), this, SIGNAL(horizontalType2Changed()));
	connect(verticalType1_, SIGNAL(valueChanged(double)), this, SIGNAL(verticalType1Changed()));
	connect(verticalType2_, SIGNAL(valueChanged(double)), this, SIGNAL(verticalType2Changed()));

	connect(displayState_, SIGNAL(valueChanged(double)), this, SIGNAL(displayStateChanged()));
	connect(boxState_, SIGNAL(valueChanged(double)), this, SIGNAL(boxStateChanged()));
	connect(intensity_, SIGNAL(valueChanged(double)), this, SIGNAL(intensityChanged()));

	connect(channel_, SIGNAL(valueChanged(double)), this, SIGNAL(channelChanged()));
}

CLSCrossHairGeneratorControl::~CLSCrossHairGeneratorControl()
{

}

int CLSCrossHairGeneratorControl::horizontalPosition1() const
{
	return int(horizontalPosition1_->value());
}

int CLSCrossHairGeneratorControl::horizontalPosition2() const
{
	return int(horizontalPosition2_->value());
}

int CLSCrossHairGeneratorControl::verticalPosition1() const
{
	return int(verticalPosition1_->value());
}

int CLSCrossHairGeneratorControl::verticalPosition2() const
{
	return int(verticalPosition2_->value());
}

int CLSCrossHairGeneratorControl::horizontalType1() const
{
	return int(horizontalType1_->value());
}

int CLSCrossHairGeneratorControl::horizontalType2() const
{
	return int(horizontalType2_->value());
}

int CLSCrossHairGeneratorControl::verticalType1() const
{
	return int(verticalType1_->value());
}

int CLSCrossHairGeneratorControl::verticalType2() const
{
	return int(verticalType2_->value());
}

bool CLSCrossHairGeneratorControl::displayState() const
{
	return (int(displayState_->value()) == 1);
}

bool CLSCrossHairGeneratorControl::boxState() const
{
	return (int(boxState_->value()) == 1);
}

int CLSCrossHairGeneratorControl::intensity() const
{
	return int(intensity_->value());
}

int CLSCrossHairGeneratorControl::channel() const
{
	return int(channel_->value());
}

void CLSCrossHairGeneratorControl::setHorizontalPosition1(int value)
{
	if (horizontalPosition1() != value)
		horizontalPosition1_->move(value);
}

void CLSCrossHairGeneratorControl::setHorizontalPosition2(int value)
{
	if (horizontalPosition2() != value)
		horizontalPosition2_->move(value);
}

void CLSCrossHairGeneratorControl::setVerticalPosition1(int value)
{
	if (verticalPosition1() != value)
		verticalPosition1_->move(value);
}

void CLSCrossHairGeneratorControl::setVerticalPosition2(int value)
{
	if (verticalPosition2() != value)
		verticalPosition2_->move(value);
}

void CLSCrossHairGeneratorControl::setHorizontalType1(int value)
{
	if (horizontalType1() != value)
		horizontalType1_->move(value);
}

void CLSCrossHairGeneratorControl::setHorizontalType2(int value)
{
	if (horizontalType2() != value)
		horizontalType2_->move(value);
}

void CLSCrossHairGeneratorControl::setVerticalType1(int value)
{
	if (verticalType1() != value)
		verticalType1_->move(value);
}

void CLSCrossHairGeneratorControl::setVerticalType2(int value)
{
	if (verticalType2() != value)
		verticalType2_->move(value);
}

void CLSCrossHairGeneratorControl::setDisplayState(bool state)
{
	if (displayState() != state)
		displayState_->move(state ? 1.0 : 0.0);
}

void CLSCrossHairGeneratorControl::setBoxState(bool state)
{
	if (boxState() != state)
		boxState_->move(state ? 1.0 : 0.0);
}

void CLSCrossHairGeneratorControl::setIntensity(int value)
{
	if (intensity() != value)
		intensity_->move(value);
}

void CLSCrossHairGeneratorControl::setChannel(int value)
{
	if (channel() != value)
		channel_->move(value);
}
