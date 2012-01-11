#ifndef VESPERSROPERCCDDETECTOR_H
#define VESPERSROPERCCDDETECTOR_H

#include "beamline/AMDetector.h"
#include "dataman/VESPERS/VESPERSRoperCCDDetectorInfo.h"
#include "beamline/AMControl.h"

/*!
  This class encapsulates the Roper CCD used on VESPERS.  Although currently not available, it will have the current image set available for viewing.
  What it will offer at the moment is the ability to control the important aspects of the detector, such as the temperature, accumulationn time,
  starting, stopping, etc.
  */
class VESPERSRoperCCDDetector : public VESPERSRoperCCDDetectorInfo, public AMDetector
{
	Q_OBJECT

public:
	/// Enum that handles the image mode.  Either normal, continuous, or focus.
	enum ImageMode { Normal = 0, Continuous, Focus };
	/// Enum that handles the trigger mode. Possibilities are Free run, External sync, Bulb trigger, single trigger.
	enum TriggerMode { FreeRun = 0, ExtSync, BulbTrigger, SingleTrigger };
	/// Enum that handles the detector state.
	enum State { Idle = 0, Acquire, Readout, Correct, Saving, Aborting, Error, Waiting };

	/// Constructor.
	VESPERSRoperCCDDetector(const QString &name, const QString &description, QObject *parent = 0);

	/// Return the meta object.
	const QMetaObject *getMetaObject() { return metaObject(); }

	/// Returns the description for the dector.
	virtual QString description() const { return VESPERSRoperCCDDetectorInfo::description(); }

	/// Transforms current settings into a detector info.  Returns a new instance -- caller is reponsible for memory.
	virtual AMDetectorInfo *toInfo() const { return new VESPERSRoperCCDDetectorInfo(*this); }
	/// Transforms current settings into a new detector info.
	VESPERSRoperCCDDetectorInfo toRoperInfo() const { return VESPERSRoperCCDDetectorInfo(*this); }

	/// Takes a detector info and sets all the settings for the detector.
	virtual bool setFromInfo(const AMDetectorInfo *info);
	/// Takes in a detector info and sets all the settings for the detector.
	void setFromRoperInfo(const VESPERSRoperCCDDetectorInfo &info);

	// Getters that aren't included in the info.  These are convenience functions that grab the current value from the control.
	//////////////////////////////////////////////////

	/// Returns the remaining time that the detector will spend acquiring.
	double timeRemaining() const { return timeRemainingControl_->value(); }
	/// Returns the temperature.  This returns the value from the control.
	virtual double acquireTime() const { return temperatureControl_->value(); }
	/// Returns the current image mode.
	ImageMode imageMode() const;
	/// Returns the trigger mode.
	TriggerMode triggerMode() const;
	/// Returns the state of the detector.
	State state() const;
	/// Returns the temperature.  Thiss returns the value from the control.
	virtual double temperature() const { return temperatureControl_->value(); }
	/// Returns whether the detector is currently acquiring.
	bool isAcquiring() const { return operationControl_->value() == 1 ? true : false; }

	// End of getters that aren't included in the info.
	/////////////////////////////////////////////////////

signals:
	/// Notifier that the temperature has changed.
	void temperatureChanged(double);
	/// Notifier that the acquire time has changed.
	void acquireTimeChanged(double);
	/// Notifier that the image mode has changed.
	void imageModeChanged(VESPERSRoperCCDDetector::ImageMode);
	/// Notifier that the trigger mode has changed.
	void triggerModeChanged(VESPERSRoperCCDDetector::TriggerMode);
	/// Notifier that the detector state has changed.
	void stateChanged(VESPERSRoperCCDDetector::State);
	/// Notifier that the time remaining has changed.
	void timeRemainingChanged(double);
	/// Notifier that the status of the detector has changed.
	void isAcquiringChanged(bool);

public slots:
	/// Sets the acquire time for the detector.
	virtual void setAcquireTime(double time)
	{
		VESPERSRoperCCDDetectorInfo::setAcquireTime(time);
		acquireTimeControl_->move(time);
	}

	/// Sets the temperature for the detector.
	virtual void setTemperature(double temperature)
	{
		VESPERSRoperCCDDetectorInfo::setTemperature(temperature);
		temperatureControl_->move(temperature);
	}
	/// Sets the image mode for the detector.
	void setImageMode(ImageMode mode) { imageModeControl_->move((int)mode); }
	/// Sets the trigger mode for the detector.
	void setTriggerMode(TriggerMode mode) { triggerModeControl_->move((int)mode); }
	/// Starts the detector.
	void start() { operationControl_->move(1); }
	/// Stops the detector.
	void stop() { operationControl_->move(0); }

protected slots:
	/// Helper slot that emits the image mode.
	void onImageModeChanged() { emit imageModeChanged(imageMode()); }
	/// Helper slot that emits the trigger mode.
	void onTriggerModeChanged() { emit triggerModeChanged(triggerMode()); }
	/// Helper slot that emits the state.
	void onStateChanged() { emit stateChanged(state()); }
	/// Helper slot that emits whether the detector is acquiring or not.
	void onIsAcquiringChanged() { emit isAcquiringChanged(isAcquiring()); }

protected:
	/// Control for the temperature.
	AMControl *temperatureControl_;
	/// Control for the image mode.
	AMControl *imageModeControl_;
	/// Control for the trigger mode control.
	AMControl *triggerModeControl_;
	/// Control for operation of the detector.  Status of acquiring, starting, stopping.
	AMControl *operationControl_;
	/// Control for the detector state.
	AMControl *stateControl_;
	/// Control for the exposure time.
	AMControl *acquireTimeControl_;
	/// Control for the time remaining.
	AMControl *timeRemainingControl_;
};

#endif // VESPERSROPERCCDDETECTOR_H
