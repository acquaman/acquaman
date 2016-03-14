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


#ifndef AMBASICCONTROLDETECTOREMULATOR_H
#define AMBASICCONTROLDETECTOREMULATOR_H

#include "beamline/AMDetector.h"
#include "beamline/AMControlSet.h"

class AMBasicControlDetectorEmulator : public AMDetector
{
Q_OBJECT
public:
	/// Enum that identifies the different control properties that the emulator can monitor.
	class Control { public: enum Property { Value = 0, Setpoint }; };

	/// Constructor takes a name and description as well as a pointer to the control you wish to acquire
	AMBasicControlDetectorEmulator(const QString &name, const QString &description, AMControl *control, AMControl *statusControl, double statusAcquiringValue, double statusNotAcquiringValue, AMDetectorDefinitions::ReadMethod readMethod,  QObject *parent = 0);
	/// Destructor.
	virtual ~AMBasicControlDetectorEmulator();

	/// Returns 0, because there are no axes for the single point detector
	virtual int size(int axisNumber) const { Q_UNUSED(axisNumber); return 0; }

	/// The basic controls don't require additional power
	virtual bool requiresPower() const { return false; }

	/// Cancelling is not yet implemented for controls
	virtual bool canCancel() const { return false; }
	/// Clearing is not yet implemented for controls
	virtual bool canClear() const { return false; }

	/// Basic controls cannot continuous acquire.
	virtual bool canContinuousAcquire() const { return false; }

	/// Returns the -1, because these controls don't have access to this information (they use the AMDetectorDefinitions::ReadMethod enum)
	virtual double acquisitionTime() const { return -1; }
	/// Returns the acquisition time tolerance.  Returns -1, not sure if that will provide the correct behaviour or not.
	virtual double acquisitionTimeTolerance() const { return -1; }

	/// The cannot be configured in this manner
	virtual bool supportsSynchronizedDwell() const { return false; }
	/// Returns empty string
	virtual QString synchronizedDwellKey() const { return "";}

	/// No triggering source
	virtual bool sharesDetectorTriggerSource() const { return false; }
	/// Returns Null pointer
	virtual AMDetectorTriggerSource* detectorTriggerSource() { return 0; }

	/// Returns RequestRead as the type
	virtual AMDetectorDefinitions::ReadMethod readMethod() const { return readMethod_; }
	/// Returns SingleRead as the type
	virtual AMDetectorDefinitions::ReadMode readMode() const { return AMDetectorDefinitions::SingleRead; }

	/// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are insuffient or (if AM_ENABLE_BOUNDS_CHECKING is defined, any are out of range), or if the data is not ready.
	virtual AMNumber reading(const AMnDIndex& indexes) const;

	/// Returns the control value
	virtual AMNumber singleReading() const;

	/// Fills the given double pointer with our current value
	virtual bool data(double *outputValues) const;

	virtual AMAction3* createSetAcquisitionTimeAction(double seconds) { Q_UNUSED(seconds); return 0; }
	virtual AMAction3* createTriggerAction(AMDetectorDefinitions::ReadMode readMode);

	/// Returns a AM1DProcessVariableDataSource suitable for viewing
	virtual AMDataSource* dataSource() const { return 0; }

	/// Returns the control property that the emulator is monitoring.
	Control::Property controlProperty() const { return controlProperty_; }

signals:
	/// Notifier that the control property that the emulator is monitoring has changed.
	void controlPropertyChanged(Control::Property newProperty);

public slots:
	/// Does nothing and returns false
	virtual bool setAcquisitionTime(double seconds) { Q_UNUSED(seconds); return false; }

	/// The read mode cannot be changed for basic controls
	virtual bool setReadMode(AMDetectorDefinitions::ReadMode readMode) { Q_UNUSED(readMode); return false; }

	/// Controls do not support clearing
	virtual bool clear() { return false; }

	/// Sets the control property that the emulator is monitoring.
	void setControlProperty(Control::Property newProperty);

protected slots:
	/// Determines if the detector is connected to ALL controls and process variables.
	void onControlsConnected(bool connected);
	/// Handles if one or more the controls times out
	void onControlsTimedOut();

	/// Handles changes to the control we're monitoring
	void onControlValueChanged(double value);
	/// Handles changes to the status control (if we have one)
	void onStatusControlValueChanged(double value);

protected:
	bool initializeImplementation();
	bool acquireImplementation(AMDetectorDefinitions::ReadMode readMode);
	bool cleanupImplementation();

	/// Checks all of the scaler settings to make sure we're ready for an acquisition
	void checkReadyForAcquisition();

protected:
	/// Holds the read method we want to use
	AMDetectorDefinitions::ReadMethod readMethod_;

	/// Holds the pointer to the control we're using
	AMControl *control_;
	/// Holds the status control for cases where we're using request read
	AMControl *statusControl_;
	/// The master set of controls
	AMControlSet *allControls_;

	bool waitingForNewData_;
	bool waitingForStatusChange_;

	double statusAcquiringValue_;
	double statusNotAcquiringValue_;

	/// Holds the control property that the emulator is monitoring.
	Control::Property controlProperty_;
};

#endif // AMBASICCONTROLDETECTOREMULATOR_H
