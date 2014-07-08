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


#ifndef VESPERSMONOCHROMATOR_H
#define VESPERSMONOCHROMATOR_H

#include <QObject>

#include "beamline/AMControl.h"
#include "actions3/AMAction3.h"

/*!
  This class encapsulates the different PVs that make up the monochromator in the VESPERS beamline.  It provides an abstraction from the Mono Application that is
  currently used on the beamline.
  */
class VESPERSMonochromator : public QObject
{
	Q_OBJECT
public:
	/// Constructor.
	virtual ~VESPERSMonochromator();
	explicit VESPERSMonochromator(QObject *parent = 0);

	/// Returns the energy setpoint.
	double Eo() const { return Eo_->value(); }
	/// Returns the desired energy setpoint.
	double Ea() const { return energy_->setpoint(); }
	/// Returns the relative energy movement from Eo.
	double delE() const { return delE_->value(); }
	/// Returns the current energy feedback.
	double energy() const { return energy_->value(); }
	/// Returns the current energy feedback expressed in K-space.
	double KSetpoint() const { return K_->setpoint(); }
	/// Returns the current energy expressed in K-space.
	double KFeedback() const { return K_->value(); }
	/// Returns the current calibration offset angle.
	double offsetAngle() const { return offsetAngle_->value(); }
	/// Returns whether scanning is allowed or not.
	bool allowScanning() const { return ((int)allowScan_->value() == 1); }
	/// Returns true if the encoder is using eV for its read back precision and false if using keV.
	bool usingeV() const { return ((int)encoder_->value() == 1); }

	/// Returns the E0 control.
	AMControl *EoControl() const { return Eo_; }
	/// Returns the Ea control.
	AMControl *EaControl() const { return energy_; }
	/// Returns the delta E control.
	AMControl *delEControl() const { return delE_; }
	/// Returns the K control.
	AMControl *KControl() const { return K_; }

	/// Returns a newly created action to move the Eo control.  Returns 0 if the control is not connected.
	AMAction3 *createEoAction(double energy);
	/// Returns a newly created action to move the Ea control.  Returns 0 if the control is not connected.
	AMAction3 *createEaAction(double energy);
	/// Returns a newly created action to move the delE control.  Returns 0 if the control is not connected.
	AMAction3 *createDelEAction(double energy);
	/// Returns a newly created action to move the K control.  Returns 0 if the control is not connected.
	AMAction3 *createKAction(double k);
	/// Returns a newly created action to move the offset angle control.  Returns 0 if the control is not connected.
	AMAction3 *createOffsetAngleAction(double angle);
	/// Returns a newly created action to move the allowScanning control.  Returns 0 if the control is not connected.
	AMAction3 *createAllowScanningAction(bool allow);
	/// Returns a newly created action to move the usingeV control.  Returns 0 if the control is not connected.
	AMAction3 *createUsingeVAction(bool useeV);

signals:
	/// Notifier that the energy setpoint has changed.  Passes the new energy.
	void EoChanged(double);
	/// Notifier that the desired energy setpoint has changed.  Passes the new energy.
	void EaChanged(double);
	/// Notifier that the relative energy value has changed.  Passes the new value.
	void delEChanged(double);
	/// Notifier that the energy feedback value has changed.  Passes the new energy.
	void energyChanged(double);
	/// Notifier that the K setpoint value has changed.  Passes the new value.
	void KSetpointChanged(double);
	/// Notifier that the K feedback value has changed.  Passes the new value.
	void KFeedbackChanged(double);
	/// Notifer that the energy calibration angle offset has changed.  Passes the new value.
	void offsetAngleChanged(double);
	/// Notifier that the monochromator is allowed to scan or not.  Passes the new state.
	void allowScanChanged(bool);
	/// Notifier that the encoder for the energy feedback has been changed (either eV or keV).  Passes the new state where true means eV, false means keV.
	void usingeVChanged(bool);

public slots:
	/// Sets the energy setpoint.
	void setEo(double val) { Eo_->move(val); }
	/// Sets the desired energy setpoint.
	void setEa(double val) { energy_->move(val); }
	/// Sets the relative energy.
	void setDelE(double val) { delE_->move(val); }
	/// Sets the K setpoint.
	void setKSetpoint(double val) { K_->move(val); }
	/// Sets the energy calibration offset angle.
	void setOffsetAngle(double val) { offsetAngle_->move(val); }
	/// Sets the monochromator to scan or not.  True allows scanning, false does not.
	void setAllowScanning(bool allow) { allowScan_->move((allow == true) ? 1.0 : 0.0); }
	/// Sets the encoder to use eV or keV depending of the given value.  True is eV, false is keV.
	void setUsingeV(bool eV) { encoder_->move((eV == true) ? 1.0 : 0.0); }

protected slots:
	/// Helper slot that emits the correct bool based on the value of allowScan.
	void onAllowScanChanged(double allowScan) { emit allowScanChanged(((int)allowScan == 1) ? true : false); }
	/// Helper slot that emits the correct bool based on the value of useeV.
	void onEncoderChanged(double useeV) { emit usingeVChanged(((int)useeV == 1) ? true : false); }

protected:
	/// The energy setpoint.
	AMControl *Eo_;
	/// The desired energy.
	AMControl *energy_;
	/// The relative movement in energy from Eo.
	AMControl *delE_;
	/// The current energy in K-space.
	AMControl *K_;
	/// The energy calibration offset angle.
	AMControl *offsetAngle_;
	/// The switch for whether the monochromator is allowed to scan.
	AMControl *allowScan_;
	/// The control that tells the mono app whether to use eV or keV for encoder precision.
	AMControl *encoder_;
};

#endif // VESPERSMONOCHROMATOR_H
