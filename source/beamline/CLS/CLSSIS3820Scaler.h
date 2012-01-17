#ifndef CLSSIS3820SCALER_H
#define CLSSIS3820SCALER_H

#include "beamline/AMControlSet.h"

class CLSSIS3820ScalerChannel;
class AMBeamlineActionItem;

/*!
  Builds an abstraction for the SIS 3820 scalar used throughout the CLS.  It takes in a base name of the PV's and builds all the PV's
  and makes the necessary connections.
  */
class CLSSIS3820Scaler : public QObject
{

	Q_OBJECT

public:
	/// Constructor.  Takes the baseName of the PV's as parameters.
	CLSSIS3820Scaler(const QString &baseName, QObject *parent = 0);

	/// Returns whether the scalar is all connected.
	bool isConnected() const;

	/// Returns whether the scalar is currently scanning.
	bool isScanning() const;
	/// Returns whether the scalar is set to be in continuous mode or single shot mode.
	bool isContinuous() const;
	/// Return the current dwell time.  Returns the value in seconds.
	double dwellTime() const;
	/// Returns the number of scans per buffer.
	int scansPerBuffer() const;
	/// Returns the total number of scans the scalar will do.
	int totalScans() const;
	/// Returns a vector of all of the readings for the scalar.
	QVector<int> reading() const;

	/// Returns the individual scalar channel provided by \param index.
	CLSSIS3820ScalerChannel* channelAt(int index);
	/// Returns the list of all the channels.
	AMOrderedList<CLSSIS3820ScalerChannel*> channels();

	/// Creates an action to start the scalar to \param setScanning.
	AMBeamlineActionItem* createStartAction(bool setScanning);
	/// Creates an action to enable continuous mode or enable single shot mode.
	AMBeamlineActionItem* createContinuousEnableAction(bool enableContinuous);
	/// Creates an action to set the dwell time of the scalar to \param dwellTime.
	AMBeamlineActionItem* createDwellTimeAction(double dwellTime);
	/// Creates an action that sets the number of scans per buffer to \param scansPerBuffer.
	AMBeamlineActionItem* createScansPerBufferAction(int scansPerBuffer);
	/// Creates an action that sets the total number of scans to \param totalScans.
	AMBeamlineActionItem* createTotalScansAction(int totalScans);

public slots:
	/// Sets the scalar to be scanning or not.
	void setScanning(bool isScanning);
	/// Sets the mode of the scalar to either continuous or single shot.
	void setContinuous(bool isContinuous);
	/// Sets the dwell time for the scalar.
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
	/// Notifier that the scalar readings have changed.
	void readingChanged();
	/// Notifier that the overall state of the scalar is connected or not.
	void connectedChanged(bool isConnected);

protected slots:
	/// Helper slot that handles changes in the scanning status.
	void onScanningToggleChanged();
	/// Helper slot that handles changes to the mode of the scalar.
	void onContinuousToggleChanged();
	/// Helper slot that emits the number of scans per buffer.
	void onScanPerBufferChanged(double scansPerBuffer);
	/// Helper slot that emits the number of total scans.
	void onTotalScansChanged(double totalScans);
	/// Helper slot that handles emitting the connectivity of the scalar.
	void onConnectedChanged();

protected:
	/// List that holds all of the individual scalar channels.
	AMOrderedList<CLSSIS3820ScalerChannel*> scalerChannels_;

	/// Control that handles changing the scanning status.
	AMControl *startToggle_;
	/// Control that handles setting the mode of the scalar.
	AMControl *continuousToggle_;
	/// Controls the dwell time of the scalar.
	AMControl *dwellTime_;
	/// Controls the number of scans per buffer.
	AMControl *scanPerBuffer_;
	/// Controls the number of total scans.
	AMControl *totalScans_;
	/// Control that handles the readings.
	AMControl *reading_;

	/// A control set that holds all of the above controls.
	AMControlSet *allControls_;

	/// Flag that holds whether the scalar has ever been connected to the beamline.
	bool connectedOnce_;
};

/// This class is an abstraction of an individual channel for the scalar class.
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

	/// Creates an action that will enable/disable the channel based on \param setEnabled.
	AMBeamlineActionItem* createEnableAction(bool setEnabled);

public slots:
	/// Sets whether the channel is enabled or not.
	void setEnabled(bool isEnabled);

signals:
	/// Notifier that the enabled state of the channel has changed.  Passes the new state.
	void enabledChanged(bool isEnabled);
	/// Notifier that the current reading of the channel has changed.  Passes the new reading.
	void readingChanged(int reading);
	/// Notifier that the current reading changed but passed as a string.
	void readingChanged(const QString &reading);
	/// Notifier that the connectivity of the channel has changed.  Passes the new state.
	void connected(bool isConnected);

protected slots:
	/// Helper slot that emits the enabledChanged() signal.
	void onChannelEnabledChanged();
	/// Helper slot that emits the readingChanged() signal.
	void onChannelReadingChanged(double reading);

protected:
	/// Control that handles the enabled flag of the individual channel.
	AMControl *channelEnable_;
	/// Control that handles reading the value of the channel.
	AMControl *channelReading_;
	/// Control set that holds the above two controls.
	AMControlSet *allControls_;
};

#endif // CLSSIS3820SCALER_H
