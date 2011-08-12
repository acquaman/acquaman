#ifndef VESPERSMONOCHROMATOR_H
#define VESPERSMONOCHROMATOR_H

#include <QObject>

#include <beamline/AMProcessVariable.h>

/*!
  This class encapsulates the different PVs that make up the monochromator in the VESPERS beamline.  It provides an abstraction from the Mono Application that is
  currently used on the beamline.
  */
class VESPERSMonochromator : public QObject
{
	Q_OBJECT
public:
	/// Constructor.
	explicit VESPERSMonochromator(QObject *parent = 0);

	/// Returns the energy setpoint.
	double Eo() const { return Eo_->getDouble(); }
	/// Returns the desired energy setpoint.
	double Ea() const { return Ea_->getDouble(); }
	/// Returns the relative energy movement from Eo.
	double delE() const { return delE_->getDouble(); }
	/// Returns the current energy feedback.
	double energy() const { return Efbk_->getDouble(); }
	/// Returns the current energy feedback expressed in K-space.
	double KSetpoint() const { return Ksp_->getDouble(); }
	/// Returns the current energy expressed in K-space.
	double KFeedback() const { return Kfbk_->getDouble(); }
	/// Returns the current calibration offset angle.
	double offsetAngle() const { return offsetAngle_->getDouble(); }
	/// Returns whether scanning is allowed or not.
	bool allowScanning() const { return (allowScan_->getInt() == 1) ? true : false; }
	/// Returns true if the encoder is using eV for its read back precision and false if using keV.
	bool usingeV() const { return (encoder_->getInt() == 1) ? true : false; }

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
	void setEo(double val) { Eo_->setValue(val); }
	/// Sets the desired energy setpoint.
	void setEa(double val) { Ea_->setValue(val); }
	/// Sets the relative energy.
	void setDelE(double val) { delE_->setValue(val); }
	/// Sets the K setpoint.
	void setKSetpoint(double val) { Ksp_->setValue(val); }
	/// Sets the energy calibration offset angle.
	void setOffsetAngle(double val) { offsetAngle_->setValue(val); }
	/// Sets the monochromator to scan or not.  True allows scanning, false does not.
	void setAllowScanning(bool allow) { allowScan_->setValue((allow == true) ? 1 : 0); }
	/// Sets the encoder to use eV or keV depending of the given value.  True is eV, false is keV.
	void setUsingeV(bool eV) { encoder_->setValue((eV == true) ? 1 : 0); }

protected slots:
	/// Helper slot that emits the correct bool based on the value of allowScan.
	void onAllowScanChanged(int allowScan) { emit allowScanChanged((allowScan == 1) ? true : false); }
	/// Helper slot that emits the correct bool based on the value of useeV.
	void onEncoderChanged(int useeV) { emit usingeVChanged((useeV == 1) ? true : false); }

protected:
	/// The energy setpoint.
	AMProcessVariable *Eo_;
	/// The desired energy setpoint.
	AMProcessVariable *Ea_;
	/// The relative movement in energy from Eo.
	AMProcessVariable *delE_;
	/// The energy feedback in eV.
	AMProcessVariable *Efbk_;
	/// The current energy setpoint in K-space.
	AMProcessVariable *Ksp_;
	/// The current energy feedback in K-space.
	AMProcessVariable *Kfbk_;
	/// The energy calibration offset angle.
	AMProcessVariable *offsetAngle_;
	/// The switch for whether the monochromator is allowed to scan.
	AMProcessVariable *allowScan_;
	/// The PV that tells the mono app whether to use eV or keV for encoder precision.
	AMProcessVariable *encoder_;
};

#endif // VESPERSMONOCHROMATOR_H
