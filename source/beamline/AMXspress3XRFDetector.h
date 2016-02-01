#ifndef AMXSPRESS3XRFDETECTOR_H
#define AMXSPRESS3XRFDETECTOR_H

#include "beamline/AMXRFDetector.h"

#include "beamline/AMDetectorTriggerSource.h"

#include <QTime>
#include <QTimer>

/// Implementation of the XRF detector when using the Xspress3 electronics.  It adds some sophistication to the initialization and acquisition state machines.
/// Subclasses will be responsible for new'ing the controls.
class AMXspress3XRFDetector : public AMXRFDetector
{
	Q_OBJECT

public:
	/// Default constructor.
	AMXspress3XRFDetector(const QString &name, const QString &description, QObject *parent = 0);
	/// Destructor.
	virtual ~AMXspress3XRFDetector();

	/// The Vortex doesn't explicitly require powering on
	virtual bool requiresPower() const { return false; }

	/// Returns the elapsed time from the elapsed time control.
	virtual double elapsedTime() const;

	/// Cancelling is implemented for the Vortex detectors
	virtual bool canCancel() const { return true; }
	/// Clearing is not currently supported for the Vortex detectors
	virtual bool canClear() const { return false; }

	/// Vortex detectors are not currently capable of continuous acquisition
	virtual bool canContinuousAcquire() const { return false; }

	/// The Vortex can be configured to work with synchronized dwell time systems
	virtual bool supportsSynchronizedDwell() const { return false; }
	/// Returns the CLS Synchronized Dwell Time trigger PV string, which acts as the key for the synchronized dwell time lookup system
	virtual QString synchronizedDwellKey() const;

	/// Returns RequestRead as the type
	virtual AMDetectorDefinitions::ReadMethod readMethod() const { return AMDetectorDefinitions::RequestRead; }
	/// Returns SingleRead as the type
	virtual AMDetectorDefinitions::ReadMode readMode() const { return AMDetectorDefinitions::SingleRead; }

	/// Returns false, because the Vortex detectors do not support continuous reads
	virtual bool lastContinuousReading(double *outputValues) const;
	/// The vortex detectors support elapsed time.
	virtual bool supportsElapsedTime() const { return true; }

	/// Returns the current status message.
	QString statusMessage() const { return statusMessage_; }
	/// Returns the number of frames per acquisition.
	int framesPerAcquisition() const { return int(framesPerAcquisitionControl_->value()); }
	/// Returns the current frame index.
	int currentFrame() const { return int(currentFrameCountControl_->value()); }

	/// Returns the threshold (only returns the first element one).
	int threshold() const { return int(thresholdControls_.at(0)->value()); }

	/// Returns whether to not this detector shares a triggering source, such as synchronized dwell time or the main scaler trigger. Default implementation returns false.
	virtual bool sharesDetectorTriggerSource() const { return true; }
	/// Returns the trigger source for this detector. Default implementation returns a NULL pointer.
	virtual AMDetectorTriggerSource* detectorTriggerSource() { return triggerSource_; }

	/// Returns an action that will initialize this detector.
	AMAction3 *createInitializationAction();
	/// Returns an action that sets the frames per acquisition.
	AMAction3 *createFramesPerAcquisitionAction(int number);
	/// Returns an action that disarms the detector.
	AMAction3 *createDisarmAction();

signals:
	/// Notifier that the status message has changed.
	void statusMessageChanged(const QString &);
	/// Notifier that the current frame number has updated.
	void currentFrameCountChanged();
	/// Notifier that the number of frames for the next acquisition has changed.
	void framesPerAcquisitionChanged();
	/// Notifier that the threshold changed.
	void thresholdChanged();

public slots:
	/// The read mode cannot be changed for Amptek detectors
	virtual bool setReadMode(AMDetectorDefinitions::ReadMode readMode);
	/// Our trigger source will need to be provided to us
	void setTriggerSource(AMZebraDetectorTriggerSource *triggerSource);

	/// This disarms the detector.
	void disarm();

	/// Vortex detectors do not support clearing
	virtual bool clear() { return false; }
	/// Sets the number of frames to be used in the next acquisition.
	void setFramesPerAcquisition(int count);

	/// Sets the threshold of the individual elements.
	void setThreshold(int newThreshold);

protected slots:
	/// Handles updating the acquisition state of the detector.
	void updateAcquisitionState();
	/// Handles starting the elapsed time.
	void startElapsedTime();
	/// Handles stopping the elapsed time.
	void stopElapsedTime();
	/// Emits a new elapsed time.
	void onElapsedTimerTimeout();
	/// Handles getting the status message, which is an array of ascii characters and not a string.
	void onStatusMessageChanged();
	/// Handles setting the acquisition succeeded state by waiting on all the spectra.
	void onDataChanged();

	/// Handle triggering with respect to the trigger source.
	virtual void onTriggerSourceTriggered(AMDetectorDefinitions::ReadMode readMode);
	/// This function is called from the Cancelling (acquisition) state for detectors that support cancelling acquisitions. Once the detector has successfully cancelled the acquisition you must call setAcquisitionCancelled()
	virtual bool cancelAcquisitionImplementation();

protected:
	/// Initializes the detector so that you can acquire. Changes the state from Idle or Aborting to Acquire.
	bool initializeImplementation();
	/// Helper function to make all the connections once all the controls have been created.  Call this only once inside the constructor.
	void makeConnections();

	/// The elapsed time.  It has to be a timer since the Quantum electronics don't support elapsed time.
	QTime elapsedTime_;
	/// The elapsed time update timer.
	QTimer elapsedTimeTimer_;
	/// The status message.
	QString statusMessage_;

	/// The status message PV.
	AMReadOnlyPVControl *statusMessageControl_;
	/// The initialization control.
	AMSinglePVControl *initializationControl_;
	/// List of all the channel enable controls.
	QList<AMPVControl *> channelEnableControls_;
	/// The current array counter value for the number of frames.
	AMReadOnlyPVControl *currentFrameCountControl_;
	/// The number of frames control.
	AMPVControl *framesPerAcquisitionControl_;
	/// The erase control.
	AMSinglePVControl *eraseControl_;
	/// The update control.
	AMSinglePVControl *updateControl_;

	/// Thresholds
	QList<AMPVControl *> thresholdControls_;

	/// Flag for whether the system should auto initialize after finishing an acquisition.
	bool autoInitialize_;

	/// Data read flag that helps determine when the detector is finished its acquisition.
	bool dataReady_;
	/// Counter to know how many of the spectra sources have updated their values since acquisition started.
	int dataReadyCounter_;

	/// Flag for holding whether the the trigger source is being used.
	bool isTriggered_;
	/// The common trigger source for this system. Detector implementations can return this as a common means for triggering and comparing shared triggers.
	AMZebraDetectorTriggerSource *triggerSource_;
};

#endif // AMXSPRESS3XRFDETECTOR_H
