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


#ifndef CLSSIS3820SCALER_H
#define CLSSIS3820SCALER_H

#include "beamline/AMControlSet.h"

class CLSSIS3820ScalerChannel;
class AMBeamlineActionItem;
class AMControl;
class AMDetectorTriggerSource;
class AMDetectorDwellTimeSource;

#include "dataman/info/AMDetectorInfo.h"

/*!
  Builds an abstraction for the SIS 3820 scaler used throughout the CLS.  It takes in a base name of the PV's and builds all the PV's
  and makes the necessary connections.
  */
class CLSSIS3820Scaler : public QObject
{

	Q_OBJECT

public:
	/// Constructor.  Takes the baseName of the PV's as parameters.
	CLSSIS3820Scaler(const QString &baseName, QObject *parent = 0);

	/// Returns whether the scaler is all connected.
	bool isConnected() const;

	/// Returns whether the scaler is currently scanning.
	bool isScanning() const;
	/// Returns whether the scaler is set to be in continuous mode or single shot mode.
	bool isContinuous() const;
	/// Return the current dwell time.  Returns the value in seconds.
	double dwellTime() const;
	/// Returns the number of scans per buffer.
	int scansPerBuffer() const;
	/// Returns the total number of scans the scaler will do.
	int totalScans() const;
	/// Returns a vector of all of the readings for the scaler.
	QVector<int> reading() const;

	/// Returns the individual scaler channel provided by \param index.
	CLSSIS3820ScalerChannel* channelAt(int index);
	/// Returns the list of all the channels.
	AMOrderedList<CLSSIS3820ScalerChannel*> channels();

	/// Returns the trigger source for the scaler.
	AMDetectorTriggerSource* triggerSource();
	/// Returns the dwell time source for the scaler
	AMDetectorDwellTimeSource* dwellTimeSource();
	QString synchronizedDwellKey() const { return synchronizedDwellKey_; }

	/// Creates an action to start the scaler to \param setScanning.
	AMBeamlineActionItem* createStartAction(bool setScanning);
	/// Creates an action to enable continuous mode or enable single shot mode.
	AMBeamlineActionItem* createContinuousEnableAction(bool enableContinuous);
	/// Creates an action to set the dwell time of the scaler to \param dwellTime (in seconds).
	AMBeamlineActionItem* createDwellTimeAction(double dwellTime);
	/// Creates an action that sets the number of scans per buffer to \param scansPerBuffer.
	AMBeamlineActionItem* createScansPerBufferAction(int scansPerBuffer);
	/// Creates an action that sets the total number of scans to \param totalScans.
	AMBeamlineActionItem* createTotalScansAction(int totalScans);

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

protected slots:
	/// Helper slot that handles changes in the scanning status.
	void onScanningToggleChanged();
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
	void onTriggerSourceTriggered(AMDetectorDefinitions::ReadMode readMode);
	bool triggerScalerAcquisition(bool isContinuous);

	void onDwellTimeSourceSetDwellTime(double dwellSeconds);

protected:
	/// List that holds all of the individual scaler channels.
	AMOrderedList<CLSSIS3820ScalerChannel*> scalerChannels_;

	/// Control that handles changing the scanning status.
	AMControl *startToggle_;
	/// Control that handles setting the mode of the scaler.
	AMControl *continuousToggle_;
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
	/// The common dwell time source for this system. Detector implementations can return this as a common means for triggering and comparing shared triggers.
	AMDetectorDwellTimeSource *dwellTimeSource_;
	QString synchronizedDwellKey_;
};

/// This class is an abstraction of an individual channel for the scaler class.
class CLSSIS3820ScalerChannel : public QObject
{

	Q_OBJECT

public:
	/// Constructor.  Takes in a base name and the index of the channel and builds all the necessary PV's.
	CLSSIS3820ScalerChannel(const QString &baseName, int index, QObject *parent = 0);

	/// Returns whether the channel is connected.
	bool isConnected() const;
	/// Returns whether the channel is enabled.
	bool isEnabled() const;
	/// Returns the current value of the channel.
	int reading() const;
	/// Returns the index of this channel.
	int index() const { return index_; }

	/// Creates an action that will enable/disable the channel based on \param setEnabled.
	AMBeamlineActionItem* createEnableAction(bool setEnabled);

	/// Returns the custom settable name
	QString customChannelName() const { return customChannelName_; }

public slots:
	/// Sets whether the channel is enabled or not.
	void setEnabled(bool isEnabled);

	/// Sets the custom channel name, default is an empty string
	void setCustomChannelName(const QString &customChannelName);

signals:
	/// Notifier that the enabled state of the channel has changed.  Passes the new state.
	void enabledChanged(bool isEnabled);
	/// Notifier that the current reading of the channel has changed.  Passes the new reading.
	void readingChanged(int reading);
	/// Notifier that the connectivity of the channel has changed.  Passes the new state.
	void connected(bool isConnected);

protected slots:
	/// Helper slot that emits the enabledChanged() signal.
	void onChannelEnabledChanged();
	/// Helper slot that emits the readingChanged() signal.
	void onChannelReadingChanged(double reading);

protected:
	/// Index of this scaler channel.
	int index_;

	/// Control that handles the enabled flag of the individual channel.
	AMControl *channelEnable_;
	/// Control that handles reading the value of the channel.
	AMControl *channelReading_;
	/// Control set that holds the above two controls.
	AMControlSet *allControls_;

	/// Holds a custom setable name for the channel
	QString customChannelName_;
};

#endif // CLSSIS3820SCALER_H
