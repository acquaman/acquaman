/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").
Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "SGMMAXvMotor.h"

#include "actions3/AMActionSupport.h"
#include "actions3/actions/AMControlStopAction.h"

SGMMAXvMotor::SGMMAXvMotor(const QString &name,
						   const QString &baseName,
						   const QString &description,
						   bool hasEncoder,
						   double tolerance,
						   double moveStartTimeoutSeconds,
						   QObject *parent)
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

AMAction3* SGMMAXvMotor::createEncoderCalibrationAbsoluteOffsetAction(double encoderCalibrationAbsoluteOffset){
	if(!isConnected())
		return 0;

	return AMActionSupport::buildControlMoveAction(encoderCalibrationAbsoluteOffset_, encoderCalibrationAbsoluteOffset);
}

AMAction3* SGMMAXvMotor::createEncoderTypeAction(SGMMAXvMotor::EncoderType encoderType){
	if(!isConnected())
		return 0;

	return AMActionSupport::buildControlMoveAction(encoderType_, encoderType);
}

AMAction3* SGMMAXvMotor::createEncoderEncodingAction(SGMMAXvMotor::EncoderEncoding encoderEncoding){
	if(!isConnected())
		return 0;

	return AMActionSupport::buildControlMoveAction(encoderEncoding_, encoderEncoding);
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
