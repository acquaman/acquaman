#ifndef VESPERSCCDDETECTOR_H
#define VESPERSCCDDETECTOR_H

#include "beamline/AMDetector.h"
#include "dataman/VESPERS/VESPERSCCDDetectorInfo.h"
#include "beamline/AMPVControl.h"
#include "actions/AMBeamlineActionItem.h"
#include "application/VESPERS/VESPERS.h"

class VESPERSCCDDetector : public VESPERSCCDDetectorInfo, public AMDetector
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
	VESPERSCCDDetector(const QString &name, const QString &description, const QString &pvBase, const AMnDIndex &detectorSize, QObject *parent = 0);

	/// Return the meta object.
	const QMetaObject *getMetaObject() { return metaObject(); }

	/// Returns the description of the detector.
	virtual QString description() const { return VESPERSCCDDetectorInfo::description(); }
	/// Sets the description for the detector.
	virtual void setDescription(const QString &description) { VESPERSCCDDetectorInfo::setDescription(description); }

	/// Transforms current settings into a detector info.  Returns a new instance -- caller is responsible for memory.
	virtual AMDetectorInfo *toInfo() const { return new VESPERSCCDDetectorInfo(*this); }
	/// Transforms current settings into a new detector info.
	VESPERSCCDDetectorInfo toMarInfo() const { return VESPERSCCDDetectorInfo(*this); }

	/// Takes a detector info and sets all the settings for the detector.
	virtual bool setFromInfo(const AMDetectorInfo *info);
	/// Takes in a detector info and sets all the settings for the detector.
	void setFromVESPERSInfo(const VESPERSCCDDetectorInfo &info);

	// Getters that aren't included in the info.  These are convenience functions that grab the current value from the control.
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// Returns the temperature.  This returns the value from the control.
	virtual double acquireTime() const { return acquireTimeControl_->value(); }
	/// Returns the current image mode.
	ImageMode imageMode() const;
	/// Returns the trigger mode.
	TriggerMode triggerMode() const;
	/// Returns the state of the detector.
	State state() const;
	/// Returns whether the detector is currently acquiring.
	bool isAcquiring() const { return operationControl_->value() == 1; }
	/// Returns whether autosave is enabled for the detector.
	bool autoSaveEnabled() const { return autoSaveControl_->value() == 1; }
	/// Returns whether the file is being saved.
	bool fileBeingSaved() const { return saveFileControl_->isMoving(); }
	/// Returns the current file path set for the detector.
	QString ccdFilePath() const { return VESPERS::pvToString(ccdPath_); }
	/// Returns the current file name for the detector.
	QString ccdFileName() const { return VESPERS::pvToString(ccdFile_); }
	/// Returns the current number that is used for auto indexing of the file names.
	int ccdFileNumber() const { return int(ccdNumber_->value()); }
	/// Returns the image data.
	QVector<int> imageData() const { return imageData_; }

	// End of getters that aren't included in the info.
	/////////////////////////////////////////////////////

	// Action getters.  Returns actions that perform a specific action.
	/// Returns a newly created action that sets the acquire time.  Returns 0 if the control is not connected.
	AMBeamlineActionItem *createAcquireTimeAction(double time);
	/// Returns a newly created action that sets the image mode.  Returns 0 if the control is not connected.
	AMBeamlineActionItem *createImageModeAction(VESPERSCCDDetector::ImageMode mode);
	/// Returns a newly created action that sets the trigger mode.  Returns 0 if the control is not connected.
	AMBeamlineActionItem *createTriggerModeAction(VESPERSCCDDetector::TriggerMode mode);
	/// Returns a newly created action that starts the detector.  Returns 0 if the control is not connected.
	AMBeamlineActionItem *createStartAction();
	/// Returns a newly created action that stops the detector.  Returns 0 if the control is not connected.
	AMBeamlineActionItem *createStopAction();
	/// Creates a newly created action that sets whether the detector should autosave files.  Returns 0 if the control is not connected.
	AMBeamlineActionItem *createAutoSaveAction(bool autoSave);
	/// Creates a newly created action that saves a file.  Returns 0 if the control is not connected.
	AMBeamlineActionItem *createSaveFileAction();
	/// Creates a newly created action that sets the file path for the detector.  Returns 0 if not connected.
	AMBeamlineActionItem *createFilePathAction(const QString &path);
	/// Creates a newly created action that sets the file name for the detector.  Returns 0 if not connected.
	AMBeamlineActionItem *createFileNameAction(const QString &name);
	/// Creates a newly created action that sets the number for auto-increment.  Returns 0 if not connected.
	AMBeamlineActionItem *createFileNumberAction(int number);

signals:
	/// Notifier that the detector is connected.
	void connected(bool);
	/// Notifier that the acquire time has changed.
	void acquireTimeChanged(double);
	/// Notifier that the image mode has changed.
	void imageModeChanged(VESPERSCCDDetector::ImageMode);
	/// Notifier that the trigger mode has changed.
	void triggerModeChanged(VESPERSCCDDetector::TriggerMode);
	/// Notifier that the detector state has changed.
	void stateChanged(VESPERSCCDDetector::State);
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

	/// Notifier that new image data has successfully been loaded.
	void imageReady();

public slots:
	/// Sets the acquire time for the detector.
	virtual void setAcquireTime(double time)
	{
		VESPERSCCDDetectorInfo::setAcquireTime(time);
		acquireTimeControl_->move(time);
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
	void setAutoSaveEnabled(bool enable) { autoSaveControl_->move(enable ? 1 : 0); }
	/// Sends the signal to save the current image to the current file path + name + number combo.
	void saveFile() { saveFileControl_->move(1); }

	/// Sets the CCD file path.
	void setCCDPath(QString path) { VESPERS::stringToPV(ccdPath_, path); }
	/// Sets the CCD file name.
	void setCCDName(QString name) { VESPERS::stringToPV(ccdFile_, name); }
	/// Sets the CCD file number.
	void setCCDNumber(int number) { ccdNumber_->move(double(number)); }

	/// Loads the file found at \param file.  Needs to be a full path to the file.  If left empty, the function will attempt to use the current path, file name, and number.  Array is filled with 0's if it fails and return false.
	void loadImageFromFile(const QString &filename = QString(""));

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
	void onAcquireTimeChanged(double time) { VESPERSCCDDetectorInfo::setAcquireTime(time); emit acquireTimeChanged(time); }

	/// Handles the CCD path update.
	void onCCDPathChanged() { emit ccdPathChanged(VESPERS::pvToString(ccdPath_)); }
	/// Handles the CCD name update.
	void onCCDNameChanged() { emit ccdNameChanged(VESPERS::pvToString(ccdFile_)); }
	/// Handles propogating the file number signal.
	void onCCDNumberChanged() { emit ccdNumberChanged(int(ccdNumber_->value())); }

protected:
	/// Pure virtual fucntion.  The implementation method used for loading images.  Implementations must emit the imageReady() signal after loading data.
	virtual void loadImageFromFileImplementation(const QString &filename) = 0;

	/// The vector that holds the image data.  For the time being, will only hold one image per detector at a time.
	QVector<int> imageData_;

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
	/// Control holding the number of the file.
	AMControl *ccdNumber_;
};

#endif // VESPERSCCDDETECTOR_H
