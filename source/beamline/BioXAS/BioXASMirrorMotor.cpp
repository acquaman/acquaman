#include "BioXASMirrorMotor.h"

BioXASMirrorMotor::BioXASMirrorMotor(const QString &name, const QString &baseName, const QString &description, bool hasEncoder, double xPosition, double yPosition, double tolerance, double moveStartTimeoutSeconds, QObject *parent, const QString &pvUnitFieldName) :
	CLSMAXvMotor(name, baseName, description, hasEncoder, tolerance, moveStartTimeoutSeconds, parent, pvUnitFieldName)
{
	// Initialize member variables.

	xPosition_ = 0;
	yPosition_ = 0;

	// Current settings.

	setXPosition(xPosition);
	setYPosition(yPosition);
}

BioXASMirrorMotor::~BioXASMirrorMotor()
{

}

AMControl::FailureExplanation BioXASMirrorMotor::setXPosition(double newPosition)
{
	AMControl::FailureExplanation result = AMControl::OtherFailure;

	if (xPosition_ != newPosition) {
		xPosition_ = newPosition;
		result = AMControl::NoFailure;
		emit xPositionChanged(xPosition_);
	}

	return result;
}

AMControl::FailureExplanation BioXASMirrorMotor::setYPosition(double newPosition)
{
	AMControl::FailureExplanation result = AMControl::OtherFailure;

	if (yPosition_ != newPosition) {
		yPosition_ = newPosition;
		result = AMControl::NoFailure;
		emit yPositionChanged(yPosition_);
	}

	return result;
}
