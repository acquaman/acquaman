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


#ifndef VESPERSCCDDETECTORVIEW_H
#define VESPERSCCDDETECTORVIEW_H

#include "ui/beamline/AMOldDetectorView.h"
#include "beamline/VESPERS/VESPERSCCDDetector.h"

#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QDoubleSpinBox>


class VESPERSCCDDetectorView : public AMDetailedOldDetectorView
{
	Q_OBJECT

public:
	/// Default contructor.  Can build a view with a null pointer (ie: not a useful view) if necessary.
	Q_INVOKABLE explicit VESPERSCCDDetectorView(VESPERSCCDDetector *detector = 0, bool configureOnly = false, QWidget *parent = 0);

	/// Returns a pointer to the detector being viewed.
	AMOldDetector *detector() { return detector_; }

protected slots:
	/// Slot that handles when the start button is clicked.  It checks to see if a file with the current name and number exists and, if yes, prompts the user for input.
	void onStartClicked();
	/// Slot used to switch the icon on the currently acquiring indicator.
	void onIsAcquiringChanged(bool isAcquiring);
	/// Slot used to update the state label.
	void onStateChanged(VESPERSCCDDetector::State newState);
	/// Slot used to set the acquire time on the detector.
	void setAcquireTime(double time);
	/// Overloaded.  Slot used to set the acquire time on the detector.
	void setAcquireTime();
	/// Slot used to update the trigger mode combo box.
	void onTriggerModeChanged(VESPERSCCDDetector::TriggerMode mode);
	/// Slot used to set the trigger mode on the detector.
	void setTriggerMode(int newMode);
	/// Slot used to update the image mode combo box.
	void onImageModeChanged(VESPERSCCDDetector::ImageMode mode);
	/// Slot used to set the image mode on the detector.
	void setImageMode(int newMode);
	/// Slot used to set the autosave setting on the detector.
	void setAutoSave(int autoSave);
	/// Slot used to update the autosave combo box.
	void onAutoSaveChanged(bool autoSave);
	/// Used to set the CCD Path when it changes from the program.
	void ccdPathEdited();
	/// Used to set the CCD File name when it changes from the program.
	void ccdFileEdited();
	/// Used to set the CCD Number when it changes from the program.
	void ccdNumberEdited();
	/// Used to update the ccdNumber value.
	void ccdNumberUpdate(int val);
	/// Used to update the elapsed time label.
	void onElapsedTimerTimeout();

protected:
	/*! Sets up the view based with the given detector.
	 We are trusting createDetectorView to pass in the correct type of detector, sub classes should trust AMDetector is actually their type. */
	bool setDetector(AMOldDetector *detector, bool configureOnly);

	/// The pointer to the detector.
	VESPERSCCDDetector *detector_;

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
	/// Combo box holding the autosave options.
	QComboBox *autoSaveComboBox_;
	/// The QTime for keeping track of the elapsed time.
	QTime elapsedTime_;
	/// The timer used for updating the elapsed time.
	QTimer elapsedTimer_;
	/// The label holding the elapsed time.
	QLabel *elapsedTimeLabel_;

	// CCD setup things.
	/// CCD file path line edit.
	QLineEdit *filePathEdit_;
	/// CCD file name line edit.
	QLineEdit *fileNameEdit_;
	/// CCD file number line edit.
	QLineEdit *fileNumberEdit_;

	/// The master layout.
	QVBoxLayout *masterLayout_;
	/// The layout with the other key elements.
	QVBoxLayout *detectorLayout_;
};

#endif // VESPERSCCDDETECTORVIEW_H
