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


#ifndef VESPERSROPERCCDDETECTOR_H
#define VESPERSROPERCCDDETECTOR_H

//<<<<<<< HEAD
//#include "beamline/AMOldDetector.h"
//#include "dataman/VESPERS/VESPERSRoperCCDDetectorInfo.h"
//#include "beamline/AMPVControl.h"
//#include "actions/AMBeamlineActionItem.h"
//=======
#include "beamline/VESPERS/VESPERSCCDDetector.h"

class VESPERSRoperQRunnableImageLoader;
//>>>>>>> master

/*!
  This class encapsulates the Roper CCD used on VESPERS.  Although currently not available, it will have the current image set available for viewing.
  What it will offer at the moment is the ability to control the important aspects of the detector, such as the temperature, accumulationn time,
  starting, stopping, etc.
  */
//<<<<<<< HEAD
//class VESPERSRoperCCDDetector : public VESPERSRoperCCDDetectorInfo, public AMOldDetector
//=======
class VESPERSRoperCCDDetector : public VESPERSCCDDetector
//>>>>>>> master
{
	Q_OBJECT

public:
	/// Constructor.
	VESPERSRoperCCDDetector(const QString &name, const QString &description, QObject *parent = 0);
/*
<<<<<<< HEAD
	/// Return the meta object.
	const QMetaObject *getMetaObject() { return metaObject(); }

	/// Returns the description for the detector.
	virtual QString description() const { return VESPERSRoperCCDDetectorInfo::description(); }
	/// Sets the description for the detector.
	virtual void setDescription(const QString &description) { VESPERSRoperCCDDetectorInfo::setDescription(description); }

	/// Transforms current settings into a detector info.  Returns a new instance -- caller is reponsible for memory.
	virtual AMOldDetectorInfo *toInfo() const { return new VESPERSRoperCCDDetectorInfo(*this); }
	/// Transforms current settings into a new detector info.
	VESPERSRoperCCDDetectorInfo toRoperInfo() const { return VESPERSRoperCCDDetectorInfo(*this); }

	/// Takes a detector info and sets all the settings for the detector.
	virtual bool setFromInfo(const AMOldDetectorInfo *info);
	/// Takes in a detector info and sets all the settings for the detector.
	void setFromRoperInfo(const VESPERSRoperCCDDetectorInfo &info);

	// Getters that aren't included in the info.  These are convenience functions that grab the current value from the control.
	//////////////////////////////////////////////////

	/// Returns the temperature.  This returns the value from the control.
	virtual double acquireTime() const { return acquireTimeControl_->value(); }
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
	/// Returns the current file path set for the detector.
	QString ccdFilePath() const { return AMPVtoString(ccdPath_); }
	/// Returns the current file name for the detector.
	QString ccdFileName() const { return AMPVtoString(ccdFile_); }
	/// Returns the current number that is used for auto indexing of the file names.
	int ccdFileNumber() const { return ccdNumber_->getInt(); }

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
=======
*/
protected slots:
	/// Sets the new image data after receiving the signal from the QRunnable.
	void onImageDataChanged(const QVector<int> &data);
//>>>>>>> master

protected:
	/// The implementation method used for loading images.  Implementations must emit the imageReady() signal after loading data.
	virtual void loadImageFromFileImplementation(const QString &filename);
};

#include <QRunnable>
#include <QMutex>

/// This is a speicific QRunnable class that reads in the file of the CCD images.  This allows the task to be run in its own thread using QThreadPool.
class VESPERSRoperQRunnableImageLoader : public QObject, public QRunnable
{
	Q_OBJECT

public:
	/// Constructor.  Takes in the file path of the image to be loaded as well as the necessary size for the data.
	VESPERSRoperQRunnableImageLoader(const QString &fileName, int size, QObject *parent = 0);

	/// The method that does the work.  Needs to use proper thread safety.
	virtual void run();

signals:
	/// Signal that passes the data that was loaded.
	void imageData(const QVector<int> &data);

protected:
	/// The file name with the full path.
	QString fileName_;
	/// The size of the data that should be expected.
	int size_;
	/// The mutex used for locking the run method.
	QMutex locker_;
};

#endif // VESPERSROPERCCDDETECTOR_H
