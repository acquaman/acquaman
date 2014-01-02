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


#ifndef VESPERSROPERCCDDETECTORVIEW_H
#define VESPERSROPERCCDDETECTORVIEW_H

#include "ui/beamline/AMOldDetectorView.h"
#include "beamline/VESPERS/VESPERSRoperCCDDetector.h"

#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QDoubleSpinBox>

class VESPERSRoperCCDDetectorView : public AMDetailedOldDetectorView
{
	Q_OBJECT

public:
	/// Default contructor.  Can build a view with a null pointer (ie: not a useful view) if necessary.
 	virtual ~VESPERSRoperCCDDetectorView(){}
	Q_INVOKABLE explicit VESPERSRoperCCDDetectorView(VESPERSRoperCCDDetector *detector = 0, bool configureOnly = false, QWidget *parent = 0);

	/// Returns a pointer to the detector being viewed.
	AMOldDetector *detector() { return detector_; }

protected slots:
	/// Slot used to switch the icon on the currently acquiring indicator.
	void onIsAcquiringChanged(bool isAcquiring) { isAcquiring_->setPixmap(QIcon(isAcquiring == true ? ":/ON.png" : ":/OFF.png").pixmap(25)); }
	/// Slot used to update the state label.
	void onStateChanged(VESPERSRoperCCDDetector::State newState);
	/// Slot used to set the acquire time on the detector.
	void setAcquireTime(double time) { if (time != detector_->acquireTime()) detector_->setAcquireTime(time); }
	/// Overloaded.  Slot used to set the acquire time on the detector.
	void setAcquireTime() { if (acquireTime_->value() != detector_->acquireTime()) detector_->setAcquireTime(acquireTime_->value()); }
	/// Slot used to update the trigger mode combo box.
	void onTriggerModeChanged(VESPERSRoperCCDDetector::TriggerMode mode);
	/// Slot used to set the trigger mode on the detector.
	void setTriggerMode(int newMode);
	/// Slot used to update the image mode combo box.
	void onImageModeChanged(VESPERSRoperCCDDetector::ImageMode mode);
	/// Slot used to set the image mode on the detector.
	void setImageMode(int newMode);
	/// Slot used to set the temperature setpoint.
	void setTemperature() { if (temperatureSetpoint_->value() != detector_->temperature()) detector_->setTemperature(temperatureSetpoint_->value()); }
	/// Slot used to set the temperature feedback.
	void onTemperatureChanged(double temp) { temperatureFeedback_->setText(QString::number(temp, 'g', 3) + QString::fromUtf8(" °C")); }
	/// Slot used to set the autosave setting on the detector.
	void setAutoSave(int autoSave);
	/// Slot used to update the autosave combo box.
	void onAutoSaveChanged(bool autoSave);
	/// Used to set the CCD Path when it changes from the program.
	void ccdPathEdited()
	{
		if (filePathEdit_->text().at(filePathEdit_->text().size()-1) != '\\')
			filePathEdit_->setText(filePathEdit_->text()+"\\");

		detector_->setCCDPath(filePathEdit_->text());
	}
	/// Used to set the CCD File name when it changes from the program.
	void ccdFileEdited() { detector_->setCCDName(fileNameEdit_->text()); }
	/// Used to set the CCD Number when it changes from the program.
	void ccdNumberEdited() { detector_->setCCDNumber(fileNumberEdit_->text().toInt()); }
	/// Used to update the ccdNumber value.
	void ccdNumberUpdate(int val) { fileNumberEdit_->setText(QString::number(val)); }

protected:
	/*! Sets up the view based with the given detector.
	 We are trusting createDetectorView to pass in the correct type of detector, sub classes should trust AMDetector is actually their type. */
	bool setDetector(AMOldDetector *detector, bool configureOnly);

	/// The pointer to the detector.
	VESPERSRoperCCDDetector *detector_;

	/// Label holding the isAcquiring state.
	QLabel *isAcquiring_;
	/// Spin box holding the acquire time.
	QDoubleSpinBox *acquireTime_;
	/// Combo box holding the trigger mode.
	QComboBox *triggerMode_;
	/// Combo box holding the image mode.
	QComboBox *imageMode_;
	/// Label holding the current state.
	QLabel *state_;
	/// Spin box holding the temperature setpoint for the detector.
	QDoubleSpinBox *temperatureSetpoint_;
	/// Label holding the feedback value for the temperature.
	QLabel *temperatureFeedback_;
	/// Combo box holding the autosave options.
	QComboBox *autoSaveComboBox_;

	// CCD setup things.
	/// CCD file path line edit.
	QLineEdit *filePathEdit_;
	/// CCD file name line edit.
	QLineEdit *fileNameEdit_;
	/// CCD file number line edit.
	QLineEdit *fileNumberEdit_;
};

#endif // VESPERSROPERCCDDETECTORVIEW_H
