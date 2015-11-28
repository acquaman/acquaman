#ifndef CLSAMDSSCALER_H
#define CLSAMDSSCALER_H

#include "beamline/AMControlSet.h"

class CLSAMDSScalerChannel;
class AMAction3;
class AMControl;
class AMReadOnlyPVControl;
class AMDetectorTriggerSource;
class AMDetectorDwellTimeSource;
class AMCurrentAmplifier;

class AMDSClientRequest;
class AMDSClientRelativeCountPlusCountDataRequest;

#include "util/AMRange.h"
#include "source/beamline/AMDetector.h"

#define CLSAMDSScaler_NOT_CONNECTED_OR_IS_SCANNING 456000

/*!
  Builds an abstraction for the AMDS Interface to the scaler used throughout the CLS.  It takes in a base name of the PV's and builds all the PV's
  and makes the necessary connections.
  */
class CLSAMDSScaler : public QObject
{
Q_OBJECT

public:
	/// Constructor.  Takes the baseName of the PV's as parameters.
	CLSAMDSScaler(const QString &baseName, const QString &amdsBufferName = QString(), QObject *parent = 0);
	/// Destructor.
	virtual ~CLSAMDSScaler();

	/// Returns whether the scaler is all connected.
	bool isConnected() const;

	/// Returns the buffer name for this detector
	QString amdsBufferName() const;
	/// Configures the server with the given identifier
	void configAMDSServer(const QString &amdsServerIdentifier);
	/// Fetches data if an AMDS Server is set up
	bool retrieveBufferedData(double seconds);
	/// Returns the last continuous data request
	AMDSClientRelativeCountPlusCountDataRequest* lastContinuousDataRequest() const;
	/// Sets the continuous data window for the next AMDSClientDataRequest
	bool setContinuousDataWindow(double continuousDataWindowSeconds);
	/// Returns the underlying polling rate for the AMDS implementation, usually 1ms
	int amdsPollingBaseTimeMilliseconds() const;

	/// Returns whether the scaler is currently scanning.
	bool isStarted() const;
	/// Returns whether the scaler is set to be in continuous mode or single shot mode.
	double dwellMode() const;
	/// Return the current dwell time.  Returns the value in seconds.
	double dwellTime() const;
	/// Returns the dwell time tolerance.  Passes the tolerance from the dwell time control.
	double dwellTimeTolerance() const;
	/// Returns the current acquisition state
	double dwellState() const;

	/// Returns the number of channels that are currently enabled
	int enabledChannelCount() const;

	/// Returns the individual scaler channel provided by \param index.
	CLSAMDSScalerChannel* channelAt(int index) const;
	/// Returns the list of all the channels.
	AMOrderedList<CLSAMDSScalerChannel*> channels() const;

	/// Returns the trigger source for the scaler.
	AMDetectorTriggerSource* triggerSource() const;
	/// Returns the dwell time source for the scaler
	AMDetectorDwellTimeSource* dwellTimeSource() const;

	/// Returns the dwell time control.
	AMControl* dwellTimeControl() const { return dwellTimeControl_; }

	/// Creates an action to start the scaler to \param setScanning.
	AMAction3* createStartAction3(bool setStarted);
	/// Creates an action to enable continuous mode or enable single shot mode.
	AMAction3* createDwellModeAction3(double dwellMode);
	/// Creates an action to set the dwell time of the scaler to \param dwellTime (in seconds).
	AMAction3* createDwellTimeAction3(double dwellTime);

	/// Creates an action that waits for the acquisition to finish.  Provide an acceptable time wait so that you don't hang up indefinitely.
	AMAction3* createWaitForDwellFinishedAction(double timeoutTime = 10.0);

	/// Not implemented right now, returns 0
	/// Creates a new action that causes this scaler to take a dark current measurement.
	AMAction3* createMeasureDarkCurrentAction(int secondsDwell);

	/// Returns a map of vectors (one for each name in the scalerChannelIndexMap) from the AMDSClientDataRequest
	QMap<QString, QVector<qint32> > retrieveScalerData(const QMap<int, QString> &scalerChannelIndexMap, AMDSClientDataRequest *scalerClientDataRequest);
	/// Returns a map of vectors that has been rebased to a different scale based on the baseTimeScale (in milliseconds) passed in
	QMap<QString, QVector<qint32> > rebaseScalerData(const QMap<QString, QVector<qint32> > &scalerData, int baseTimeScale);

public slots:
	/// Sets the scaler to be scanning or not.
	void setStarted(bool start);
	/// Sets the mode of the scaler to either continuous or single shot.
	void setDwellMode(double dwellMode);
	/// Sets the dwell time for the scaler (in seconds).
	void setDwellTime(double dwellTime);

	/// Not implemented right now, does nothing
	/// Creates the needed actions to perform a dark current correction on all available and able channels, and executes them.
	void measureDarkCurrent(int secondsDwell);

signals:
	/// Notifier that the scanning flag has changed.  Returns the new state.
	void startChanged(bool isStarted);
	/// Notifier that the scanning mode has changed.  Returns true if in continuous mode, false if single shot.
	void dwellModeChanged(double dwellMode);
	/// Notifier that the dwell time has changed.  Returns the new time.
	void dwellTimeChanged(double dwellTime);
	/// Notifier that the acquisition state has changed.
	void dwellStateChanged(double dwellState);

	/// Notifier that the scaler readings have changed.
	void readingChanged();
	/// Notifier that the overall state of the scaler is connected or not.
	void connectedChanged(bool isConnected);
	/// Emitted when the scaler channel sr570 sensitivity changes.
	void sensitivityChanged();

	/// Emitted when an AMDSClientDataRequest is received for the buffer matching this scaler
	void amdsDataReady();

protected slots:
	/// Helper slot that handles changes in the scanning status.
	void onStartControlValueChanged(double start);
	/// Helper slot that handles changes to the mode of the scaler.
	void onDwellModeControlValueChanged(double mode);
	/// Helper slot that handles emitting the dwell time changed but in seconds rather than milliseconds.
	void onDwellTimeControlValueChanged(double time);
	/// Helper slot that handles changes to the acquisition state
	void onDwellStateControlValueChanged(double state);

	/// Helper slot that handles emitting the connectivity of the scaler.
	void onConnectedChanged();

	/// Handles requests for triggering from the AMDetectorTriggerSource
	void onTriggerSourceTriggered(AMDetectorDefinitions::ReadMode readMode);

	void onDwellTimeSourceSetDwellTime(double dwellSeconds);

	/// ============= SLOTs to handle AMDSClientAppController signals =========
	/// slot to handle the signal of request data ready
	void onRequestDataReady(AMDSClientRequest* clientRequest);
	/// slot to handle the signal of socketEror
	void onServerError(int errorCode, bool removeServer, const QString &serverIdentifier, const QString &errorMessage);

	void onLastContinuousDataRequestDestroyed();

protected:
	/// List that holds all of the individual scaler channels.
	AMOrderedList<CLSAMDSScalerChannel*> scalerChannels_;

	/// Control that handles changing the scanning status.
	AMControl *startControl_;
	/// Control that handles setting the mode of the scaler.
	AMControl *dwellModeControl_;
	/// Controls the dwell time of the scaler.
	AMControl *dwellTimeControl_;
	/// Control for the acquisition state of the scaler
	AMControl *dwellStateControl_;

	/// A control set that holds all of the above controls.
	AMControlSet *allControls_;

	/// Flag that holds whether the scaler has ever been connected to the beamline.
	bool connectedOnce_;

	/// The common trigger source for this system. Detector implementations can return this as a common means for triggering and comparing shared triggers.
	AMDetectorTriggerSource *triggerSource_;
	bool triggerSourceTriggered_;
	AMDetectorDefinitions::ReadMode readModeForTriggerSource_;

	/// The common dwell time source for this system. Detector implementations can return this as a common means for triggering and comparing shared triggers.
	AMDetectorDwellTimeSource *dwellTimeSource_;
	bool dwellTimeSourceRequested_;


	/// the AMDS Amptek Server identifier
	QString amdsServerIdentifier_;
	/// The AMDS buffer name for this instance
	QString amdsBufferName_;

	/// The data returned from the last acquire(AMDetectorDefinitions::ContinuousMode) call
	AMDSClientRelativeCountPlusCountDataRequest *lastContinuousDataRequest_;
	/// The window for the continuous data request in seconds
	double continuousDataWindowSeconds_;
	/// The underlying polling rate of the AMDS implementation, usually 1ms
	int pollingRateMilliSeconds_;
};

/// This class is an abstraction of an individual channel for the scaler class.
class CLSAMDSScalerChannel : public QObject
{
Q_OBJECT

public:
	/// Constructor.  Takes in a base name and the index of the channel and builds all the necessary PV's.
	CLSAMDSScalerChannel(const QString &baseName, int index, QObject *parent = 0);
	virtual ~CLSAMDSScalerChannel();

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

	/// Return 0 right now, we haven't done the enable/disable portion yet
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
	virtual bool withinLinearRange() const { return voltageRange_.withinRange(voltage()); }
	/// Specific helper function that returns whether the voltage is too low.  Returns false if withinLinearRange is true.
	virtual bool voltageTooLow() const { return !withinLinearRange() && voltage() < minimumVoltage(); }
	/// Specific helper function that returns whether the voltage is too high.  Returns false if withinLinearRange is true.
	virtual bool voltageTooHigh() const { return !withinLinearRange() && voltage() > maximumVoltage(); }

public slots:
	/// Does nothing right now, not done yet
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


protected slots:
	/// Helper slot that emits the enabledChanged() signal.
	void onChannelEnabledChanged();
	/// Helper slot that emits the readingChanged() signal.
	void onChannelReadingChanged(double reading);
	/// Handles figuring out whether the entire scalar channel is connected or not.
	void onConnectedChanged();

protected:
	/// Index of this scaler channel.
	int index_;
	/// Flag to help minimize signal traffic when connecting to controls.
	bool wasConnected_;

	/// The multiplier for this channel, usually the V2F setting
	double channelVoltage_;
	/// The linear voltage range of the detector.
	AMRange voltageRange_;

	/// Control that handles the enabled flag of the individual channel.
	AMControl *channelEnable_;
	/// Control that handles reading the value of the channel.
	AMControl *channelReading_;

	/// Control set that holds the above two controls.
	AMControlSet *allControls_;

	/// Holds a custom setable name for the channel
	QString customChannelName_;

	/// Holds the pointer to the current amplifier (if it has one), which controls the sensitivity of the scalar channel.
	AMCurrentAmplifier *currentAmplifier_;

	/// The channel detector.
	AMDetector *detector_;
};

#endif // CLSAMDSSCALER_H
