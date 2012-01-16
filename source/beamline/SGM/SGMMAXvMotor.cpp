#include "SGMMAXvMotor.h"

#include "actions/AMBeamlineControlMoveAction.h"
#include "actions/AMBeamlineControlStopAction.h"

SGMMAXvMotor::SGMMAXvMotor(const QString &name, const QString &baseName, const QString &description, bool hasEncoder, double tolerance, double moveStartTimeoutSeconds, QObject *parent)
	: CLSMAXvMotor(name, baseName, description, hasEncoder, tolerance, moveStartTimeoutSeconds, parent)
{
	encoderCalibrationAbsoluteOffset_ = new AMPVControl(name+"EncoderCalibrationAbsoluteOffset", baseName+":enc:absOffset", baseName+":enc:absOffset", QString(), this, 1);
	encoderType_ = new AMPVControl(name+"EncoderType", baseName+":encoderType:sp", baseName+":encoderType", QString(), this, 0.1);
	encoderEncoding_ = new AMPVControl(name+"EncoderEncoding", baseName+":absEncodingType", baseName+":absEncodingType", QString(), this, 0.1);
	actualRetries_ = new AMReadOnlyPVControl(name+"ActualRetries", baseName+":retry:fbk", this);

	connect(encoderCalibrationAbsoluteOffset_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)));
	connect(encoderCalibrationAbsoluteOffset_, SIGNAL(valueChanged(double)), this, SIGNAL(encoderCalibrationAbsoluteOffsetChanged(double)));
	connect(encoderType_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)));
	connect(encoderType_, SIGNAL(valueChanged(double)), this, SLOT(onEncoderTypeChanged(double)));
	connect(encoderEncoding_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)));
	connect(encoderEncoding_, SIGNAL(valueChanged(double)), this, SLOT(onEncoderEncodingChanged(double)));
	connect(actualRetries_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)));
	connect(actualRetries_, SIGNAL(valueChanged(double)), this, SIGNAL(actualRetriesChanged(double)));
}

bool SGMMAXvMotor::isConnected() const
{
	bool overallConnected = CLSMAXvMotor::isConnected();
	bool extrasConnected = true;

	if (hasEncoder_){

		extrasConnected = encoderCalibrationAbsoluteOffset_->isConnected()
				&& encoderType_->isConnected()
				&& encoderEncoding_->isConnected()
				&& actualRetries_->isConnected();
	}

	return overallConnected && extrasConnected;
}

double SGMMAXvMotor::encoderCalibrationAbsoluteOffset() const{
	if(isConnected())
		return encoderCalibrationAbsoluteOffset_->value();
	return 0.0;
}

SGMMAXvMotor::EncoderType SGMMAXvMotor::encoderType() const{
	if(!isConnected())
		return SGMMAXvMotor::EncoderTypeError;
	if(!hasEncoder_)
		return SGMMAXvMotor::EncoderNone;
	int enumChoice = (int)encoderType_->value();
	switch(enumChoice){
	case 0:
		return SGMMAXvMotor::EncoderIncrememtal;
		break;
	case 1:
		return SGMMAXvMotor::EncoderAbsolute;
		break;
	case 2:
		return SGMMAXvMotor::EncoderPotentiometer;
		break;
	default:
		return SGMMAXvMotor::EncoderTypeError;
		break;
	}
}

SGMMAXvMotor::EncoderEncoding SGMMAXvMotor::encoderEncoding() const{
	if(!isConnected())
		return SGMMAXvMotor::EncodingError;
	if(!hasEncoder_)
		return SGMMAXvMotor::EncodingNone;
	int enumChoice = (int)encoderEncoding_->value();
	switch(enumChoice){
	case 0:
		return SGMMAXvMotor::EncodingBinary;
		break;
	case 1:
		return SGMMAXvMotor::EncodingGreyCode;
		break;
	default:
		return SGMMAXvMotor::EncodingError;
		break;
	}
}

double SGMMAXvMotor::actualRetries() const{
	if(isConnected())
		return actualRetries_->value();
	return 0.5;
}

AMBeamlineActionItem* SGMMAXvMotor::createEncoderCalibrationAbsoluteOffsetAction(double encoderCalibrationAbsoluteOffset){
	if(!isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(encoderCalibrationAbsoluteOffset_);
	action->setSetpoint(encoderCalibrationAbsoluteOffset);
	return action;
}

AMBeamlineActionItem* SGMMAXvMotor::createEncoderTypeAction(SGMMAXvMotor::EncoderType encoderType){
	if(!isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(encoderType_);
	action->setSetpoint(encoderType);
	return action;
}

AMBeamlineActionItem* SGMMAXvMotor::createEncoderEncodingAction(SGMMAXvMotor::EncoderEncoding encoderEncoding){
	if(!isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(encoderEncoding_);
	action->setSetpoint(encoderEncoding);
	return action;
}

void SGMMAXvMotor::setEncoderCalibrationAbsoluteOffset(double encoderCalibrationAbsoluteOffset){
	if(isConnected())
		encoderCalibrationAbsoluteOffset_->move(encoderCalibrationAbsoluteOffset);
}

void SGMMAXvMotor::setEncoderType(SGMMAXvMotor::EncoderType encoderType){
	if(isConnected())
		encoderType_->move(encoderType);
}

void SGMMAXvMotor::setEncoderEncoding(SGMMAXvMotor::EncoderEncoding encoderEncoding){
	if(isConnected())
		encoderEncoding_->move(encoderEncoding);
}

void SGMMAXvMotor::onEncoderTypeChanged(double value){
	int enumChoice = (int)value;
	switch(enumChoice){
	case 0:
		emit encoderTypeChanged(SGMMAXvMotor::EncoderIncrememtal);
		break;
	case 1:
		emit encoderTypeChanged(SGMMAXvMotor::EncoderAbsolute);
		break;
	case 2:
		emit encoderTypeChanged(SGMMAXvMotor::EncoderPotentiometer);
		break;
	default:
		emit encoderTypeChanged(SGMMAXvMotor::EncoderTypeError);
		break;
	}
}

void SGMMAXvMotor::onEncoderEncodingChanged(double value){
	int enumChoice = (int)value;
	switch(enumChoice){
	case 0:
		emit encoderEncodingChanged(SGMMAXvMotor::EncodingBinary);
		break;
	case 1:
		emit encoderEncodingChanged(SGMMAXvMotor::EncodingGreyCode);
		break;
	default:
		emit encoderEncodingChanged(SGMMAXvMotor::EncodingError);
		break;
	}
}
