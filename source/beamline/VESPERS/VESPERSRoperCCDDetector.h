#ifndef VESPERSROPERCCDDETECTOR_H
#define VESPERSROPERCCDDETECTOR_H

#include "beamline/AMDetector.h"
#include "dataman/VESPERS/VESPERSRoperCCDDetectorInfo.h"
#include "beamline/AMControl.h"
#include "actions/AMBeamlineActionItem.h"

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

	/// Returns the description for the detector.
	virtual QString description() const { return VESPERSRoperCCDDetectorInfo::description(); }
	/// Sets the description for the detector.
	virtual void setDescription(const QString &description) { VESPERSRoperCCDDetectorInfo::setDescription(description); }

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
	/// Returns whether autosave is enabled for the detector.
	bool autoSaveEnabled() const { return autoSaveControl_->value() == 1 ? true : false; }
	/// Returns whether the file is being saved.
	bool fileBeingSaved() const { return saveFileControl_->isMoving(); }

	// End of getters that aren't included in the info.
	/////////////////////////////////////////////////////

	// Action getters.  Returns actions that perform a specific action.
	/// Returns a newly created action that sets the acquire time.  Returns 0 if the control is not connected.
	AMBeamlineActionItem *createAcquireTimeAction(double time);
	/// Returns a newly created action that sets the image mode.  Returns 0 if the control is not connected.
	AMBeamlineActionItem *createImageModeAction(VESPERSRoperCCDDetector::ImageMode mode);
	/// Returns a newly created action that sets the trigger mode.  Returns 0 if the control is not connected.
	AMBeamlineActionItem *createTriggerModeAction(VESPERSRoperCCDDetector::TriggerMode mode);
	/// Returns a newly created action that sets the temperature setpoint.  Returns 0 if the control is not connected.
	AMBeamlineActionItem *createTemperatureAction(double temperature);
	/// Returns a newly created action that starts the detector.  Returns 0 if the control is not connected.
	AMBeamlineActionItem *createStartAction();
	/// Returns a newly created action that stops the detector.  Returns 0 if the control is not connected.
	AMBeamlineActionItem *createStopAction();
	/// Creates a newly created action that sets whether the detector should autosave files.  Returns 0 if the control is not connected.
	AMBeamlineActionItem *createAutoSaveAction(bool autoSave);
	/// Creates a newly created action that saves a file.  Returns 0 if the control is not connected.
	AMBeamlineActionItem *createSaveFileAction();

signals:
	/// Notifier that the detector is connected.
	void connected(bool);
	/// Notifier that the temperature has changed.
	void temperatureChanged(double);
	/// Notifier that the temperature setpoint has changed.
	void temperatureSetpointChanged(double);
	/// Notifier that the acquire time has changed.
	void acquireTimeChanged(double);
	/// Notifier that the image mode has changed.
	void imageModeChanged(VESPERSRoperCCDDetector::ImageMode);
	/// Notifier that the trigger mode has changed.
	void triggerModeChanged(VESPERSRoperCCDDetector::TriggerMode);
	/// Notifier that the detector state has changed.
	void stateChanged(VESPERSRoperCCDDetector::State);
	/// Notifier that the status of the detector has changed.
	void isAcquiringChanged(bool);
	/// Notifier that the autosave status of the detector has changed.
	void autoSaveEnabledChanged(bool);
	/// Notifier of the current state for saving a file.
	void saveFileStateChanged(bool);

	/// Notifier that the CCD path has changed.
	void ccdPathChanged(QString);
	/// Notifier that the CCD file name has changed.
	void ccdNameChanged(QString);
	/// Notifier that the CCD number has changed.
	void ccdNumberChanged(int);

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
	/// Sets the state for the autosave status for the detector.
	void setAutoSaveEnabled(bool enable) { autoSaveControl_->move((enable == true) ? 1 : 0); }
	/// Sends the signal to save the current image to the current file path + name + number combo.
	void saveFile() { saveFileControl_->move(1); }

	/// Sets the CCD file path.
	void setCCDPath(QString path) { StringtoAMPV(ccdPath_, path); }
	/// Sets the CCD file name.
	void setCCDName(QString name) { StringtoAMPV(ccdFile_, name); }
	/// Sets the CCD file number.
	void setCCDNumber(int number) { ccdNumber_->setValue(number); }

protected slots:
	/// Helper slot that emits the image mode.
	void onImageModeChanged() { emit imageModeChanged(imageMode()); }
	/// Helper slot that emits the trigger mode.
	void onTriggerModeChanged() { emit triggerModeChanged(triggerMode()); }
	/// Helper slot that emits the state.
	void onStateChanged() { emit stateChanged(state()); }
	/// Helper slot that emits whether the detector is acquiring or not.
	void onIsAcquiringChanged() { emit isAcquiringChanged(isAcquiring()); }
	/// Helper slot that emits whether the detector has autosave enabled or not.
	void onAutoSaveEnabledChanged() { emit autoSaveEnabledChanged(autoSaveEnabled()); }
	/// Helper slot that emits the current status of writing a file.
	void onSaveFileStateChanged() { emit saveFileStateChanged(fileBeingSaved()); }
	/// Helper slot that emits the acquireTime signal and sets the acquire time in the info.
	void onAcquireTimeChanged(double time) { VESPERSRoperCCDDetectorInfo::setAcquireTime(time); emit acquireTimeChanged(time); }
	/// Helper slot that emits the temperature changed signal and sets the temperature in the info.
	void onTemperatureSetpointChanged(double temperature) { VESPERSRoperCCDDetectorInfo::setTemperature(temperature); emit temperatureSetpointChanged(temperature); }

	/// Handles the CCD path update.
	void onCCDPathChanged() { emit ccdPathChanged(AMPVtoString(ccdPath_)); }
	/// Handles the CCD name update.
	void onCCDNameChanged() { emit ccdNameChanged(AMPVtoString(ccdFile_)); }

protected:
	/// Converts the bizarre string output of the pv to a real QString.
	QString AMPVtoString(AMProcessVariable *pv);
	/// Converts the string to the array of integers it needs to be.
	void StringtoAMPV(AMProcessVariable *pv, QString toConvert);

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
	/// Control for setting whether or not the files should be autosaved.
	AMControl *autoSaveControl_;
	/// Control for saving the current file.
	AMControl *saveFileControl_;

	// Various CCD file path PVs.
	/// PV holding the path to the files.
	AMProcessVariable *ccdPath_;
	/// PV holding the name of the file.
	AMProcessVariable *ccdFile_;
	/// PV holding the number of the file.
	AMProcessVariable *ccdNumber_;
};

#endif // VESPERSROPERCCDDETECTOR_H
