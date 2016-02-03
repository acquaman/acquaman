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


#ifndef CLSSIS3820SCALER_H
#define CLSSIS3820SCALER_H

#include "beamline/AMControlSet.h"

class CLSSIS3820ScalerChannel;
class CLSSIS3820ScalerAcquisitionMode;
class AMAction3;
class AMControl;
class AMReadOnlyPVControl;
class AMSinglePVControl;
class AMDetectorTriggerSource;
class AMDetectorDwellTimeSource;
class AMCurrentAmplifier;

#include "dataman/info/AMDetectorInfo.h"
#include "util/AMRange.h"
#include "actions3/AMListAction3.h"
#include "source/beamline/AMDetector.h"

#include <QSignalMapper>

#define CLSSIS3820SCALER_NOT_CONNECTED_OR_IS_SCANNING 456000

/*!
  Builds an abstraction for the SIS 3820 scaler used throughout the CLS.  It takes in a base name of the PV's and builds all the PV's
  and makes the necessary connections.
  */
class CLSSIS3820Scaler : public QObject
{

	Q_OBJECT

public:
	/// Enum describing the possible acquisition modes.
	enum AcquisitionMode { SingleShot = 0, Continuous = 1 };
	/// Enum describing the possible scan modes.
	enum ScanMode { NotScanning = 0, Scanning = 1 };

	/// Constructor.  Takes the baseName of the PV's as parameters.
	CLSSIS3820Scaler(const QString &baseName, QObject *parent = 0);
	/// Destructor.
	virtual ~CLSSIS3820Scaler();

	/// Returns whether the scaler is all connected.
	virtual bool isConnected() const;

	/// Returns whether the scaler is currently scanning.
	virtual bool isScanning() const;
	/// Returns whether the scaler is set to be in continuous mode or single shot mode.
	bool isContinuous() const;
	/// Return the current dwell time.  Returns the value in seconds.
	double dwellTime() const;
	/// Returns the dwell time tolerance.  Passes the tolerance from the dwell time control.
	double dwellTimeTolerance() const;
	/// Returns the number of scans per buffer.
	int scansPerBuffer() const;
	/// Returns the total number of scans the scaler will do.
	int totalScans() const;
	/// Returns a vector of all of the readings for the scaler.
	QVector<int> reading() const;
	/// Returns the number of channels that are currently enabled
	int enabledChannelCount() const;

	/// Returns the individual scaler channel provided by \param index.
	CLSSIS3820ScalerChannel* channelAt(int index);
	/// Returns the list of all the channels.
	AMOrderedList<CLSSIS3820ScalerChannel*> channels();

	/// Returns the trigger source for the scaler.
	AMDetectorTriggerSource* triggerSource();
	/// Returns the dwell time source for the scaler
	AMDetectorDwellTimeSource* dwellTimeSource();
	/// Returns the synchronized dwell key.
	QString synchronizedDwellKey() const { return synchronizedDwellKey_; }

	/// Returns the dwell time control.
	AMControl* dwellTimeControl() const { return dwellTime_; }

	/// Creates an action to start the scaler to \param setScanning.
	virtual AMAction3* createStartAction3(bool setScanning);
	/// Creates an action to enable continuous mode or enable single shot mode.
	AMAction3* createContinuousEnableAction3(bool enableContinuous);
	/// Creates an action to set the dwell time of the scaler to \param dwellTime (in seconds).
	AMAction3* createDwellTimeAction3(double dwellTime);
	/// Creates an action that sets the number of scans per buffer to \param scansPerBuffer.
	AMAction3* createScansPerBufferAction3(int scansPerBuffer);
	/// Creates an action that sets the total number of scans to \param totalScans.
	AMAction3* createTotalScansAction3(int totalScans);
	/// Creates an action that waits for the acquisition to finish.  Provide an acceptable time wait so that you don't hang up indefinitely.
	AMAction3* createWaitForDwellFinishedAction(double timeoutTime = 10.0);

	/// Creates and returns a new action that moves the scaler to 'Scaning' mode.
	virtual AMAction3* createMoveToScanningAction();
	/// Creates and returns a new action that moves the scaler to 'NotScanning' mode.
	virtual AMAction3* createMoveToNotScanningAction();

	/// Creates and returns a new action that moves the scaler to 'Single shot' mode.
	virtual AMAction3* createMoveToSingleShotAction();
	/// Creates and returns a new action that moves the scaler to 'Continuous' mode.
	virtual AMAction3* createMoveToContinuousAction();

	/// Creates a new action that causes this scaler to take a dark current measurement.
	AMAction3* createMeasureDarkCurrentAction(int secondsDwell);

	/// Subclasses of the CLS scaler may require arming, the standard implementation does not
	virtual bool requiresArming() { return false; }

public slots:
	/// Sets the scaler to be scanning or not.
	void setScanning(bool isScanning);
	/// Sets the mode of the scaler to either continuous or single shot.
	void setContinuous(bool isContinuous);
	/// Sets the dwell time for the scaler (in seconds).
	void setDwellTime(double dwellTime);
	/// Sets the number of scans per buffer.
	void setScansPerBuffer(int scansPerBuffer);
	/// Sets the number of total scans.
	void setTotalScans(int totalScans);

	/// Creates the needed actions to perform a dark current correction on all available and able channels, and executes them.
	void measureDarkCurrent(int secondsDwell);

	/// Subclasses of the CLS scaler may require arming, the standard implementation does not
	virtual void arm();

signals:
	/// Notifier that the scanning flag has changed.  Returns the new state.
	void scanningChanged(bool isScanning);
	/// Notifier that the scanning mode has changed.  Returns true if in continuous mode, false if single shot.
	void continuousChanged(bool isContinuous);
	/// Notifier that the dwell time has changed.  Returns the new time.
	void dwellTimeChanged(double dwellTime);
	/// Notifer that the number of scans per buffer have changed.  Returns the new number.
	void scansPerBufferChanged(int scansPerBuffer);
	/// Notifier that the total number of scans have changed.  Returns the new total.
	void totalScansChanged(int totalScans);
	/// Notifier that the scaler readings have changed.
	void readingChanged();
	/// Notifier that the overall state of the scaler is connected or not.
	void connectedChanged(bool isConnected);
	/// Emitted when the scaler channel sr570 sensitivity changes.
	void sensitivityChanged();

	/// Subclasses of the CLS scaler may require arming, the standard implementation does not
	void armed();

protected slots:
	/// Helper slot that handles changes in the scanning status.
	virtual void onScanningToggleChanged();
	/// Helper slot that handles changes to the mode of the scaler.
	void onContinuousToggleChanged();
	/// Helper slot that handles emitting the dwell time changed but in seconds rather than milliseconds.
	void onDwellTimeChanged(double time);
	/// Helper slot that emits the number of scans per buffer.
	void onScanPerBufferChanged(double scansPerBuffer);
	/// Helper slot that emits the number of total scans.
	void onTotalScansChanged(double totalScans);
	/// Helper slot that handles emitting the connectivity of the scaler.
	void onConnectedChanged();

	/// Handles requests for triggering from the AMDetectorTriggerSource
	virtual void onTriggerSourceTriggered(AMDetectorDefinitions::ReadMode readMode);
	void ensureCorrectReadModeForTriggerSource();
	void onModeSwitchSignal();
	bool triggerScalerAcquisition(bool isContinuous);
	void onReadingChanged(double value);
	/// Handles the logic for the removing channels from the waiting list.
	void onChannelReadingChanged(int channelIndex);
	/// Handles checking to see if we are done the acquisition.
	void triggerAcquisitionFinished();

	void onDwellTimeSourceSetDwellTime(double dwellSeconds);

protected:
	/// Method that initializes the trigger source and flag for acquisition.  Gets the code for waiting on channels setup.
	void initializeTriggerSource();
	/// Method that calls set succeeded on the trigger source.  Can be reimplemented for more sophisticated triggers.
	virtual void triggerSourceSucceeded();

	AMDetectorDefinitions::ReadMode readModeFromSettings();

protected:
	/// List that holds all of the individual scaler channels.
	AMOrderedList<CLSSIS3820ScalerChannel*> scalerChannels_;

	/// Control that handles changing the scanning status.
	AMControl *startToggle_;
	/// Control that handles setting the mode of the scaler.
	CLSSIS3820ScalerAcquisitionMode *continuousToggle_;
	/// Controls the dwell time of the scaler.
	AMControl *dwellTime_;
	/// Controls the number of scans per buffer.
	AMControl *scanPerBuffer_;
	/// Controls the number of total scans.
	AMControl *totalScans_;
	/// Control that handles the readings.
	AMControl *reading_;

	/// A control set that holds all of the above controls.
	AMControlSet *allControls_;

	/// Flag that holds whether the scaler has ever been connected to the beamline.
	bool connectedOnce_;

	/// The common trigger source for this system. Detector implementations can return this as a common means for triggering and comparing shared triggers.
	AMDetectorTriggerSource *triggerSource_;
	AMDetectorDefinitions::ReadMode readModeForTriggerSource_;
	bool switchingReadModes_;

	/// The common dwell time source for this system. Detector implementations can return this as a common means for triggering and comparing shared triggers.
	AMDetectorDwellTimeSource *dwellTimeSource_;
	QString synchronizedDwellKey_;

	/// Holds the mapping of the enabled channels during an acquisition.
	QSignalMapper *triggerChannelMapper_;
	/// Flag for knowing if the scaler has been triggered.
	bool triggerSourceTriggered_;
	/// The list of channels we are still waiting to get their monitor before emitting that the scaler acquisition has finished.
	QList<int> waitingChannels_;
};

/// This class is an abstraction of an individual channel for the scaler class.
class CLSSIS3820ScalerChannel : public QObject
{

	Q_OBJECT

public:
	/// Constructor.  Takes in a base name and the index of the channel and builds all the necessary PV's.
	CLSSIS3820ScalerChannel(const QString &baseName, int index, QObject *parent = 0);
	virtual ~CLSSIS3820ScalerChannel();

	/// Returns whether the channel is connected.
	bool isConnected() const;
	/// Returns whether the channel is enabled.
	bool isEnabled() const;
	/// Returns the current value of the channel.
	int reading() const;
	/// Returns the current voltage.  \note Not high voltage.
	double voltage() const;
	/// Returns the index of this channel.
	int index() const { return index_; }

	/// Creates an action that will enable/disable the channel based on \param setEnabled.
	AMAction3* createEnableAction3(bool setEnabled);

	/// Returns the custom settable name
	QString customChannelName() const { return customChannelName_; }

	/// Sets a SR570 to this particular channel.  It is assumed that the SR570 ACTUALLY corresponds to the scalar channel, otherwise, this is meaningless.
	void setCurrentAmplifier(AMCurrentAmplifier *amplifier);
	/// Returns the SR570 that this scaler channel uses for sensitivity changes.  Returns 0 if not set.
	AMCurrentAmplifier *currentAmplifier() const { return currentAmplifier_; }

	/// Returns the AMDetector that this scaler channel uses.
	AMDetector* detector() const { return detector_; }

	/// Returns the minimum voltage range for the ion chamber.
	double minimumVoltage() const { return voltageRange_.minimum(); }
	/// Returns the maximum voltage range for the ion chamber.
	double maximumVoltage() const { return voltageRange_.maximum(); }
	/// Returns the voltage range for the ion chamber.
	const AMRange &voltageRange() const { return voltageRange_; }

	/// Returns whether the voltage is within the linear range of the ion chamber.
	virtual bool withinLinearRange() const { return voltage() <= maximumVoltage() && voltage() >= minimumVoltage(); }
	/// Specific helper function that returns whether the voltage is too low.  Returns false if withinLinearRange is true.
	virtual bool voltageTooLow() const { return !withinLinearRange() && voltage() < minimumVoltage(); }
	/// Specific helper function that returns whether the voltage is too high.  Returns false if withinLinearRange is true.
	virtual bool voltageTooHigh() const { return !withinLinearRange() && voltage() > maximumVoltage(); }

	/// Returns the counts-to-volts slope parameter preference.
	double countsVoltsSlopePreference() const { return countsVoltsSlopePreference_; }
	/// Returns the counts-to-volts conversion slope parameter control.
	AMSinglePVControl* countsVoltsSlopeControl() const { return countsVoltsSlopeControl_; }

public slots:
	/// Sets whether the channel is enabled or not.
	void setEnabled(bool isEnabled);
	/// Sets the custom channel name, default is an empty string
	void setCustomChannelName(const QString &customChannelName);

	/// Sets the minimum voltage for the linear range for the detector.
	void setMinimumVoltage(double min);
	/// Sets the maximum voltage for the linear range for the detector.
	void setMaximumVoltage(double max);
	/// Sets the linear voltage range.
	void setVoltagRange(const AMRange &range);
	/// Overloaded.  Sets the linear voltage range.
	void setVoltagRange(double min, double max);
	/// Sets the counts-to-volts slope parameter preference.
	void setCountsVoltsSlopePreference(double newValue);

	/// Sets an AMDetector to this particular channel. This connection grants us access to the detector's dark current measurement/correction abilities.
	void setDetector(AMDetector *detector);

signals:
	/// Notifier that the enabled state of the channel has changed.  Passes the new state.
	void enabledChanged(bool isEnabled);
	/// Notifier that the current reading of the channel has changed.  Passes the new reading.
	void readingChanged(int reading);
	/// Notifier that the voltage reading of the channel has changed.  Passes the new voltage.  \note This value will be meaningless if the EPICS app isn't configured properly.
	void voltageChanged(double);
	/// Notifier that the connectivity of the channel has changed.  Passes the new state.
	void connected(bool isConnected);
	/// Notifier that a (new) SR570 mapping has been established.
	void currentAmplifierAttached();
	/// Notifier that the custom name of the scaler channel has been changed.  Passes the new name.
	void customNameChanged(const QString &);
	/// Notifier that the voltage range has changed.  Passes the new range.
	void voltageRangeChanged(const AMRange &);
	/// Notifier that the SR570 sensitivity has changed.
	void sensitivityChanged();
	/// Notifier that the detector for this channel has changed.
	void detectorChanged(AMDetector *newDetector);
	/// Notifier that the counts-to-volts slope parameter preference has changed.
	void countsVoltsSlopePreferenceChanged(double newValue);

protected slots:
	/// Helper slot that emits the enabledChanged() signal.
	void onChannelEnabledChanged();
	/// Helper slot that emits the readingChanged() signal.
	void onChannelReadingChanged(double reading);
	/// Handles figuring out whether the entire scalar channel is connected or not.
	void onConnectedChanged();

	/// Updates the counts-to-volts slope parameter control with the preferred value, if a preference has been specified and the control is connected.
	void updateCountsVoltsSlopeControl();

protected:
	/// Index of this scaler channel.
	int index_;
	/// Flag to help minimize signal traffic when connecting to controls.
	bool wasConnected_;

	/// The linear voltage range of the detector.
	AMRange voltageRange_;

	/// Control that handles the enabled flag of the individual channel.
	AMControl *channelEnable_;
	/// Control that handles reading the value of the channel.
	AMControl *channelReading_;
	/// Control that holds the voltage value.  PV name is the "userRate", but that is just a generic term in the EPICS app. \note This is NOT the high voltage.
	AMReadOnlyPVControl *channelVoltage_;
	/// Control set that holds the above two controls.
	AMControlSet *allControls_;

	/// Holds a custom setable name for the channel
	QString customChannelName_;

	/// Holds the pointer to the current amplifier (if it has one), which controls the sensitivity of the scalar channel.
	AMCurrentAmplifier *currentAmplifier_;

	/// The channel detector.
	AMDetector *detector_;

	/// The counts-to-volts slope parameter control.
	AMSinglePVControl *countsVoltsSlopeControl_;
	/// Flag indicating whether a counts-to-volts slope parameter value preference has been specified. Initially false.
	bool haveCountsVoltsSlopePreference_;
	/// The counts-to-volts slope parameter value preference. Initially 1.
	double countsVoltsSlopePreference_;
};

#endif // CLSSIS3820SCALER_H
