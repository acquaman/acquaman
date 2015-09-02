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
  * \brief A class which represents a Max V Motor which contains aspects specific
  * to SGM.
  *
  * Due to the fact that not all motors at the CLS use the same version of the
  * MaxV driver, SGM needs to have its own class for the motor to add those features
  * which its version of the driver contains, which others don't.
  */
class SGMMAXvMotor : public CLSMAXvMotor
{
	Q_OBJECT

public:

	/*!
	  * An enumeration of all the different encoder types used by MaxVMotors.
	  */
	enum EncoderType {
		EncoderIncrememtal = 0,		// An incremental encoder
		EncoderAbsolute = 1,		// An absolute encoder
		EncoderPotentiometer = 2,	// A potentiometer encoder
		EncoderNone = 3,			// No encoder
		EncoderTypeError = 4		// There is an error determinging the encoder type
	};

	/*!
	  * An enumeration of all the different types of encoding the encoders user
	  * on MaxVMotors.
	  */
	enum EncoderEncoding {
		EncodingBinary = 0,		// Encoder uses binary encoding
		EncodingGreyCode = 1,	// Encoder uses greycode encoding
		EncodingNone = 2,		// No encoder, so no encoding
		EncodingError = 3		// There is an error determining the encoding type
	};


	/*!
	  * Creates an instance of an SGMMAXvMotor with the provided data.
	  * \param name ~ A unique description of this motor.
	  * \param baseName ~ The base of the PV name (if the motor status was
	  * "SMTR16114I1022:status" then the base is "SMTR16114I1022").
	  * \param description ~ A human readable description for this motor.
	  * \param hasEncoder ~ Whether or not the motor has an encoder.
	  * \param tolerance ~ The tolerance that will be used to determine if moves
	  * succeed in getting close enough to be considered a success.
	  * \param moveStartTimeoutSeconds ~ An optional number of seconds to wait
	  * for the motor to move before something is considered amis. If none is
	  * provided 2.0 seconds is used.
	  * \param parent ~ An optional QObject parent. If none is provided the
	  * SGMMAXvMotor will have no parent.
	  */
	SGMMAXvMotor(const QString &name,
				 const QString &baseName,
				 const QString &description,
				 bool hasEncoder,
				 double tolerance,
				 double moveStartTimeoutSeconds = 2.0,
				 QObject *parent = 0);

	/*!
	  * Virtual destructor for the SGMMAXvMotor.
	  */
	virtual ~SGMMAXvMotor() {}

	/*!
	  * Whether all process variables for this motor are considered connected.
	  */
	virtual bool isConnected() const;

	/*!
	  * The absolute encoder calibration offset, or 0 if the motor isn't connected.
	  */
	double encoderCalibrationAbsoluteOffset() const;

	/*!
	  * The encoder type in use, or EncoderTypeError if the motor isn't connected.
	  */
	SGMMAXvMotor::EncoderType encoderType() const;

	/*!
	  * The encoder encoding type in use, or EncodingError if the motor isn't
	  * connected.
	  */
	SGMMAXvMotor::EncoderEncoding encoderEncoding() const;

	/*!
	  * The actual retried value, or 0.5 if the motor isn't connected.
	  */
	double actualRetries() const;

	/*!
	  * Creates and returns an action to change to encoder calibration absolute
	  * offset. If the motor is not connected, 0 will be returned.
	  * \param encoderCalibrationAbsoluteOffset ~ The value which the returned
	  * action will be configured to set the offset to.
	  */
	AMAction3* createEncoderCalibrationAbsoluteOffsetAction(double encoderCalibrationAbsoluteOffset);

	/*!
	  * Creates and returns an action to change the encoder type. If the motor
	  * is not connected, 0 will be returned.
	  * \param encoderType ~ The value which the returned action will be
	  * configured to set the encoder type to.
	  */
	AMAction3* createEncoderTypeAction(SGMMAXvMotor::EncoderType encoderType);

	/// Returns a newly created action to change the encoder encoding. Returns 0 if the control is not connected.
	/*!
	  * Creates and returns an action to change the encoder encoding. If the motor
	  * is not yet connected, 0 will be returned.
	  * \param encoderEncoding ~ The value which the returned action will be
	  * configured to set the encoder encoding to.
	  */
	AMAction3* createEncoderEncodingAction(SGMMAXvMotor::EncoderEncoding encoderEncoding);

public slots:

	/*!
	 * Sets the absolute encoder calibration offset.
	 */
	void setEncoderCalibrationAbsoluteOffset(double absoluteEncoderCalibrationOffset);

	/*!
	  * Sets the encoder type to use.
	  */
	void setEncoderType(SGMMAXvMotor::EncoderType encoderType);

	/*!
	  * Sets the encoding type to be used by the encoder.
	  */
	void setEncoderEncoding(SGMMAXvMotor::EncoderEncoding encoderEncoding);

signals:
	/*!
	  * Signal indicating that the absolute encoder calibration offset has
	  * been altered.
	  * \param newCalibration ~ The value to which the offset has been set.
	  */
	void encoderCalibrationAbsoluteOffsetChanged(double newCalibration);

	/*!
	  * Signal indicating that the encoder type has been altered
	  * \param encoderType ~ The value to which the encoder type has been set.
	  */
	void encoderTypeChanged(SGMMAXvMotor::EncoderType encoderType);

	/*!
	  * Signal indicating that the encoding type used by the encoder has been
	  * altered.
	  * \param encoderEncoding ~ The value to which the encoder encoding has
	  * been set.
	  */
	void encoderEncodingChanged(SGMMAXvMotor::EncoderEncoding encoderEncoding);

protected slots:
	/*!
	  * Handles signals indicating that the encoder type has been altered.
	  * \param value ~ A double value containing the encoder type in code form.
	  */
	void onEncoderTypeChanged(double value);

	/*!
	  * Handles signals indicating that the encoding type used by the encoder
	  * has been altered.
	  * \param value ~ A double value containing the encodering type used by
	  * the encoder in code form.
	  */
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
