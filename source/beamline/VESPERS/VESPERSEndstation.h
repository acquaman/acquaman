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


#ifndef VESPERSENDSTATION_H
#define VESPERSENDSTATION_H

#include <QObject>
#include <QMap>

#include "beamline/AMPVControl.h"
#include "util/AMRange.h"

class VESPERSEndstation : public QObject
{
	Q_OBJECT

public:
	/// The constructor.  Builds and encapsulates many of the controls used for the endstation.
	virtual ~VESPERSEndstation();
	explicit VESPERSEndstation(QObject *parent = 0);

	/// Returns the state of the endstation XIA shutter.
	bool shutterState() const { return (int)filterShutterLower_->value() == 1; }
	/// Returns a bool on the state of the power of the microscope.  True for powered, false for un-powered.
	bool laserPowered() const { return (int)laserPower_->value() == 1; }
	/// Returns whether the microscope is in the home position.
	bool microscopeInSafePosition(double value) const;
	/// Overloaded function.  Uses current value.
	bool microscopeInSafePosition() const;
	/// Returns whether the CCD is in the home position.
	bool ccdInSafePosition(double value) const;
	/// Overloaded function.  Uses current value.
	bool ccdInSafePosition() const;
	/// Enables/Disables the microscope PV by blocking signals.
	void lightEnabled(bool on) { micLightPV_->blockSignals(!on); }
	/// Returns the current control being focused on.
	AMControl *current() const { return current_; }
	/// Returns a control based on the name.  Returns 0 if invalid.
	AMControl *control(QString name) const;
	/// Returns the soft limits for the given control (if any) as a QPair<double, double>.
	AMRange getLimits(AMControl *control) const { return softLimits_.value(control); }
	/// Returns the microscope setpoint names as a QPair<QString, QString>.
	QPair<QString, QString> microscopeNames() const { return microscopeNames_; }

	/// Returns the current distance of the single element vortex from the sample.
	double distanceToSingleElementVortex() const { return singleElControl_->value(); }
	/// Returns the current distance of the four element vortex from the sample.
	double distanceToFourElementVortex() const { return fourElControl_->value(); }
	/// Returns the current distancce of the CCD to the sample.
	double distanceToCCD() const { return ccdControl_->value(); }
	/// Returns the current thickness of the filters in um.
	int filterThickness() const { return filterThickness_; }
	/// Returns the current value of the laser sensor.
	double laserPosition() const { return laserPositionControl_->value(); }
	/// Returns whether the laser sensor is reading values or not.
	bool laserPositionValid() const { return laserPositionStatusControl_->value() == 0.0; }
	/// Returns whether the helium buffer is attached to the CCD.
	bool heliumBufferAttached() const { return heliumBufferAttached_; }
	/// Returns whether the CCD is at 90 degrees or not.
	bool ccdAt90Degrees() const { return ccdAt90Degrees_; }
	/// Returns the safe position for the CCD when the buffer is attached.
	double ccdSafePositionwHeliumBuffer() const { return upperCcdSoftLimitwHeliumBuffer_; }

	/// Returns the shutter control.
	AMControl *shutterControl() const { return filterShutterLower_; }

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
	/// The focus normal feeback motor position changed notification.
	void focusNormalFbkChanged(double);
	/// The focus y feedback motor position changed notification.
	void focusYFbkChanged(double);
	/// The microscope feedback motor position changed notification.
	void microscopeFbkChanged(double);
	/// The single element motor position changed notification.
	void singleElFbkChanged(double);
	/// The four element motor position changed notification.
	void fourElFbkChanged(double);
	/// Notifier that the laser position has changed.
	void laserPositionChanged(double);
	/// Notifier that the laser position is valid or not.
	void laserPositionValidityChanged(bool);

public slots:
	/// Sets the current control that should be focused on.  It will find the correct control based on the name.  Control is set to 0 if invalid name is given.
	void setCurrent(QString name);
	/// Toggles the current state of the laser power.
	void toggleLaserPower();
	/// Loads the config file and then sets up the soft limits.
	bool loadConfiguration();
	/// Changes the filters placed in the beamline based on \code index.  The valid numbers are 0 - 16, 0 um to 800 um in 50 um intervals.
	void setFilterThickness(int index);
	/// Opens/Closes the shutter.
	void setShutterState(bool state);
	/// Convenience slot.  Opens the endstation shutter.
	void openShutter() { setShutterState(true); }
	/// Convenience slot.  Closes the endstation shutter.
	void closeShutter() { setShutterState(false); }
	/// Set the value for the microscope.  Must be between 0 and 100.
	void setLightIntensity(int intensity) { if (micLightPV_->getInt() != intensity) micLightPV_->setValue(intensity); }
	/// Set the flag on whether the helium buffer is attached.
	void setHeliumBufferFlag(bool attached);
	/// Set the flag on whether or not the CCD is at 90 degrees or not.
	void setCCDAt90Degrees(bool at90Degrees);

protected slots:
	/// Helper slot that emits the right signal based on the current state of filterLower.
	void onShutterChanged(double val) { emit shutterChanged((int)val == 1); }
	/// Determines if the filters are all connected.  They shouldn't be accessed until they are all connected.
	void onFiltersConnected();
	/// Helper slot that emits a signal with the current index related to the thickness of the filters.
	void onFiltersChanged();
	/// Handles updating the control window if the configuration changes.
	void updateControl(AMControl *control);
	/// Checks the value of the laser sensor status and emits the signal based on the value.
	void onLaserPositionValidityChanged(double value);

protected:
	/// Returns whether the \code control \code value is within tolerance of \code position.
	bool controlWithinTolerance(AMControl *control, double value, double position) const { return fabs(value-position) < control->tolerance(); }

	// Holds the previous state of the filter connectivity.
	bool wasConnected_;

	// Control map to soft limits.
	QMap<AMControl *, AMRange> softLimits_;
	// Names of microscope positions.
	QPair<QString, QString> microscopeNames_;
	/// Flag holding whether or not the helium buffer is attached to the CCD or not.
	bool heliumBufferAttached_;
	/// Upper soft limit when using the helium buffer.
	double upperCcdSoftLimitwHeliumBuffer_;
	/// Flag holding whether or not the CCD is at 90 degrees or not.
	bool ccdAt90Degrees_;

	// Control pointers.
	// The current control being pointed to.
	AMControl *current_;

	// The controls used for the control window.
	AMControl *ccdControl_;
	AMControl *microscopeControl_;
	AMControl *fourElControl_;
	AMControl *singleElControl_;
	AMControl *laserPositionControl_;
	AMControl *laserPositionStatusControl_;

	// Microscope light PV.
	AMProcessVariable *micLightPV_;

	// Laser power control.
	AMPVControl *laserPower_;

	// Filter process variables.
	AMControl *filter250umA_;
	AMControl *filter250umB_;
	AMControl *filter100umA_;
	AMControl *filter100umB_;
	AMControl *filter50umA_;
	AMControl *filter50umB_;
	AMControl *filterShutterUpper_;
	AMControl *filterShutterLower_;

	/// Returns the thickness off the filters in um.
	int filterThickness_;

	// A list of all the filters, but not the upper or lower shutters.
	QMap<QString, AMControl *> filterMap_;
};

#endif // VESPERSENDSTATION_H
