/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


class VESPERSCCDDetectorView : public QWidget
{
	Q_OBJECT

public:
	/// Default contructor.  Can build a view with a null pointer (ie: not a useful view) if necessary.
 	virtual ~VESPERSCCDDetectorView();
	Q_INVOKABLE explicit VESPERSCCDDetectorView(VESPERSCCDDetector *detector = 0, QWidget *parent = 0);

protected slots:
	/// Slot that handles when the start button is clicked.  It checks to see if a file with the current name and number exists and, if yes, prompts the user for input.
	void onStartClicked();
	/// Slot used to switch the icon on the currently acquiring indicator.
	void onIsAcquiringChanged();
	/// Slot used to set the acquire time on the detector.
	void setAcquireTime(double time);
	/// Overloaded.  Slot used to set the acquire time on the detector.
	void setAcquireTime();
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
	/// The pointer to the detector.
	VESPERSCCDDetector *detector_;

	/// Label holding the isAcquiring state.
	QLabel *isAcquiring_;
	/// Spin box holding the acquire time.
	QDoubleSpinBox *acquireTime_;
	/// Label holding the current state.
	QLabel *state_;
	/// The QTime for keeping track of the elapsed time.
	QTime elapsedTime_;
	/// The timer used for updating the elapsed time.
	QTimer elapsedTimer_;
	/// The label holding the elapsed time.
	QLabel *elapsedTimeLabel_;
	/// The acquire button.
	QPushButton *startButton_;
	/// The stop button.
	QPushButton *stopButton_;

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
