/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef VESPERSENDSTATION_H
#define VESPERSENDSTATION_H

#include <QObject>
#include <QMap>

#include "beamline/AMPVControl.h"

class VESPERSEndstation : public QObject
{
	Q_OBJECT
public:
	/// The constructor.  Builds and encapsulates many of the controls used for the endstation.  Requires the motor that is part of the sample stage for normal movements.
	explicit VESPERSEndstation(AMControl *normal, QObject *parent = 0);

	/// Returns the state of the endstation XIA shutter.
	bool shutterState() const { return (int)filterShutterLower_->value() == 1 ? true : false; }
	/// Returns a bool on the state of the power of the microscope.  True for powered, false for un-powered.
	bool laserPowered() { return (int)laserPower_->value() == 1 ? true : false; }
	/// Returns whether the microscope is in the home position.
	bool microscopeInHomePosition(double value) { return controlWithinTolerance(microscopeControl_, value, softLimits_.value(microscopeControl_).second); }
	/// Overloaded function.  Uses current value.
	bool microscopeInHomePosition() { return controlWithinTolerance(microscopeControl_, microscopeControl_->value(), softLimits_.value(microscopeControl_).second); }
	/// Returns whether the CCD is in the home position.
	bool ccdInHomePosition(double value) { return controlWithinTolerance(ccdControl_, value, softLimits_.value(ccdControl_).second); }
	/// Overloaded function.  Uses current value.
	bool ccdInHomePosition() { return controlWithinTolerance(ccdControl_, ccdControl_->value(), softLimits_.value(ccdControl_).second); }
	/// Enables/Disables the microscope PV by blocking signals.
	void lightEnabled(bool on) { micLightPV_->blockSignals(!on); }
	/// Returns the current control being focused on.
	AMControl *current() const { return current_; }
	/// Returns a control based on the name.  Returns 0 if invalid.
	AMControl *control(QString name) const;
	/// Returns the soft limits for the given control (if any) as a QPair<double, double>.
	QPair<double, double> getLimits(AMControl *control) { return qMakePair(softLimits_.value(control).first, softLimits_.value(control).second); }
	/// Returns the microscope setpoint names as a QPair<QString, QString>.
	QPair<QString, QString> microscopeNames() const { return microscopeNames_; }

signals:
	/// Notifier that the endstation shutter has changed.  Returns the state.
	void shutterChanged(bool);
	/// Notifier that the laser power changed.
	void laserPoweredChanged();
	/// Notifier that the filter index has changed.  This directly relates to the thickness of the filters in the same way as setFilterThickness (0 = 0um and 16 = 800 um).
	void filterThicknessChanged(int);
	/// Notifier that the light intensity has changed.
	void lightIntensityChanged(int);
	/// The current control has either been updated or changed.  Passes the pointer to the control.
	void currentControlChanged(AMControl *);
	/// The CCD feedback motor position changed notification.
	void ccdFbkChanged(double);
	/// The focus feeback motor position changed notification.
	void focusFbkChanged(double);
	/// The microscope feedback motor position changed notification.
	void microscopeFbkChanged(double);
	/// The single element motor position changed notification.
	void singleElFbkChanged(double);
	/// The four element motor position changed notification.
	void fourElFbkChanged(double);

public slots:
	/// Sets the current control that should be focused on.  It will find the correct control based on the name.  Control is set to 0 if invalid name is given.
	void setCurrent(QString name);
	/// Resets the pseudo-motor positions.
	void resetPseudoMotors() { resetPseudoMotors_->setValue(1); }
	/// Toggles the current state of the laser power.
	void toggleLaserPower() { toggleControl(laserPower_); emit laserPoweredChanged(); }
	/// Loads the config file and then sets up the soft limits.
	bool loadConfiguration();
	/// Changes the filters placed in the beamline based on \code index.  The valid numbers are 0 - 16; 0 um to 800 um in 50 um intervals.
	void setFilterThickness(int index);
	/// Opens/Closes the shutter.
	void setShutterState(bool state);
	/// Convenience slot.  Opens the endstation shutter.
	void openShutter() { setShutterState(true); }
	/// Convenience slot.  Closes the endstation shutter.
	void closeShutter() { setShutterState(false); }
	/// Set the value for the microscope.  Must be between 0 and 100.
	void setLightIntensity(int intensity) { if (micLightPV_->getInt() != intensity) micLightPV_->setValue(intensity); }

protected slots:
	/// Helper slot that emits the right signal based on the current state of filterLower.
	void onShutterChanged(double val) { emit shutterChanged((int)val == 1 ? true : false); }
	/// Determines if the filters are all connected.  They shouldn't be accessed until they are all connected.
	void onFiltersConnected();
	/// Helper slot that emits a signal with the current index related to the thickness of the filters.
	void onFiltersChanged();
	/// Handles updating the control window if the configuration changes.
	void updateControl(AMControl *control);

protected:
	/// Returns whether the \code control \code value is within tolerance of \code position.
	bool controlWithinTolerance(AMControl *control, double value, double position) { return fabs(value-position) < control->tolerance() ? true : false; }
	/// Helper function to properly toggle the filter PVs.  Takes an AMControl *, casts it to an AMPVControl * then toggles them.
	void toggleControl(AMControl *control) { control->move(1); control->move(0); }

	// Holds the previous state of the filter connectivity.
	bool wasConnected_;

	// Control map to soft limits.
	QMap<AMControl *, QPair<double, double> > softLimits_;
	// Names of microscope positions.
	QPair<QString, QString> microscopeNames_;

	// Control pointers.
	// The current control being pointed to.
	AMControl *current_;

	// The controls used for the control window.
	AMControl *ccdControl_;
	AMControl *microscopeControl_;
	AMControl *fourElControl_;
	AMControl *singleElControl_;
	AMControl *focusControl_;

	// Microscope light PV.
	AMProcessVariable *micLightPV_;

	// Laser power control.
	AMPVControl *laserPower_;

	// The pseudo-motor reset PV.
	AMProcessVariable *resetPseudoMotors_;

	// Filter process variables.
	AMControl *filter250umA_;
	AMControl *filter250umB_;
	AMControl *filter100umA_;
	AMControl *filter100umB_;
	AMControl *filter50umA_;
	AMControl *filter50umB_;
	AMControl *filterShutterUpper_;
	AMControl *filterShutterLower_;

	// A list of all the filters, but not the upper or lower shutters.
	QMap<QString, AMControl *> filterMap_;
};

#endif // VESPERSENDSTATION_H
