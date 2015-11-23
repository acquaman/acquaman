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


#ifndef SGMMAXVMOTOR_H
#define SGMMAXVMOTOR_H

#include "beamline/CLS/CLSMAXvMotor.h"

/*!
  Due to the fact that not all motors at the CLS use the same version of the MaxV driver, SGM needs to have its own class for the motor because
  it uses some options which don't exist on (what we will call "standard", but that is entirely arbitrary) VESPESR.  Because there appears to be
  less in the VESPERS MaxV driver we will assume that it is the base model and therefore SGM has extra pieces added in.
  */
class SGMMAXvMotor : public CLSMAXvMotor
{
	Q_OBJECT

public:

	enum EncoderType {
		EncoderIncrememtal = 0,		///< An incremental encoder
		EncoderAbsolute = 1,		///< An absolute encoder
		EncoderPotentiometer = 2,	///< A potentiometer encoder
		EncoderNone = 3,		///< No encoder
		EncoderTypeError = 4		///< There is an error determinging the encoder type
	};

	enum EncoderEncoding {
		EncodingBinary = 0,	///< Encoder uses binary encoding
		EncodingGreyCode = 1,	///< Encoder uses greycode encoding
		EncodingNone = 2,	///< No encoder, so no encoding
		EncodingError = 3	///< There is an error determining the encoding type
	};

	/// Constructor.  Adds more controls to the standard MaxV motor.
	/*!
	  \param name A unique description of this motor
	  \param baseName The base of the PV name (if the motor status was "SMTR16114I1022:status" then the base is "SMTR16114I1022")
	  \param description A human readable description for this motor
	  \param hasEncoder Should be set to true if the motor has an encoder
	  \param tolerance The tolerance that will be used to determine if moves succeed in getting close enough to be considered a success
	  \param moveStartTimeout How long the motor has to start moving before something is considered amis
	  \param parent QObject parent class
	  */
 	virtual ~SGMMAXvMotor();
	SGMMAXvMotor(const QString &name, const QString &baseName, const QString &description, bool hasEncoder, double tolerance, double moveStartTimeoutSeconds = 2.0, QObject *parent = 0);

	/// Indicates that all process variables for this motor are connected
	virtual bool isConnected() const;

	/// Returns the absolute encoder calibration offset. Returns 0 if the motor isn't connected yet.
	double encoderCalibrationAbsoluteOffset() const;
	/// Returns the encoder type in use. Returns EncoderTypeError if the motor isn't connected yet.
	SGMMAXvMotor::EncoderType encoderType() const;
	/// Returns the encoding type in use. Returns EncodingError if the motor isn't connected yet.
	SGMMAXvMotor::EncoderEncoding encoderEncoding() const;
	/// Returns the actual retries value. Returns 0.5 if the motor isn't connected yet.  This is because actual retries is actually given in integers.
	double actualRetries() const;

	/// Returns a newly created action to change the encoder calibration absolute offset. Returns 0 if the control is not connected.
	AMAction3* createEncoderCalibrationAbsoluteOffsetAction(double encoderCalibrationAbsoluteOffset);
	/// Returns a newly created action to change the encoder type. Returns 0 if the control is not connected.
	AMAction3* createEncoderTypeAction(SGMMAXvMotor::EncoderType encoderType);
	/// Returns a newly created action to change the encoder encoding. Returns 0 if the control is not connected.
	AMAction3* createEncoderEncodingAction(SGMMAXvMotor::EncoderEncoding encoderEncoding);

public slots:

	/// Sets the absolute encoder calibration offset
	void setEncoderCalibrationAbsoluteOffset(double absoluteEncoderCalibrationOffset);
	/// Sets the encoder type to use
	void setEncoderType(SGMMAXvMotor::EncoderType encoderType);
	/// Sets the encoding to use
	void setEncoderEncoding(SGMMAXvMotor::EncoderEncoding encoderEncoding);

signals:
	/// Emitted when the absolute encoder calibration offset changes
	void encoderCalibrationAbsoluteOffsetChanged(double newCalibration);
	/// Emitted when the encoder type in use changes
	void encoderTypeChanged(SGMMAXvMotor::EncoderType encoderType);
	/// Emitted when the encoder encoding in use changes
	void encoderEncodingChanged(SGMMAXvMotor::EncoderEncoding encoderEncoding);

protected slots:
	/// Handles changes in the encoder type control
	void onEncoderTypeChanged(double value);

	/// Handles changes in the encoder encoding
	void onEncoderEncodingChanged(double value);

protected:
	/// Read-write control for the absolute encoder calibration offset
	AMPVControl *encoderCalibrationAbsoluteOffset_;
	/// Read-write control for the encoder type
	AMPVControl *encoderType_;
	/// Read-write control for the encoder encoding
	AMPVControl *encoderEncoding_;
	/// Readonly control for the actual retries value
	AMReadOnlyPVControl *actualRetries_;

};

#endif // SGMMAXVMOTOR_H
