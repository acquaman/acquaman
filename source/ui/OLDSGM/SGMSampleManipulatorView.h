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


#ifndef SGMSAMPLEMANIPULATORVIEW_H
#define SGMSAMPLEMANIPULATORVIEW_H

#include <QGroupBox>
#include <QIcon>

class QCheckBox;
class QComboBox;
class QPushButton;
class QSlider;
class QToolButton;
class QButtonGroup;
class QGridLayout;

class AMAction3;
class SGMMAXvMotor;
class AMExtendedControlEditor;

class SGMSampleManipulatorView : public QGroupBox
{
Q_OBJECT
public:
 	virtual ~SGMSampleManipulatorView();
	SGMSampleManipulatorView(bool bigButtons = false, QWidget *parent = 0);

protected slots:
	void onMUpButtonPressed();
	void onMUpButtonReleased();
	void onMDownButtonPressed();
	void onMDownButtonReleased();
	void onMInboardButtonPressed();
	void onMInboardButtonReleased();
	void onMOutboardButtonPressed();
	void onMOutboardButtonReleased();
	void onMUpstreamButtonPressed();
	void onMUpstreamButtonReleased();
	void onMDownstreamButtonPressed();
	void onMDownstreamButtonReleased();
	void onMCWButtonPressed();
	void onMCWButtonReleased();
	void onMCCWButtonPressed();
	void onMCCWButtonReleased();

	void onStopAllButtonClicked();
	void onJogButtonChecked(bool checked);
	void onJogSettingComboBoxChanged(int index);

	void onTransferPositionButtonClicked();
	void onMeasurePositionButtonClicked();

	void onIlluminatorSliderValueMoved(int newValue);
	void onIlluminatorFeedbackChanged(double newValue);
	void onIlluminatorPreset(int presetIndex);


protected:
	QPushButton *mUpButton_;
	QPushButton *mDownButton_;
	QPushButton *mInboardButton_;
	QPushButton *mOutboardButton_;
	QPushButton *mUpstreamButton_;
	QPushButton *mDownstreamButton_;
	QPushButton *mCWButton_;
	QPushButton *mCCWButton_;
	QCheckBox *jogBox_;
	QComboBox *jogSettingComboBox_;

	QPushButton *stopAllButton_;
	QPushButton *transferPositionButton_;
	AMAction3 *transferPositionActions_;
	QPushButton *measurePositionButton_;
	AMAction3 *measurementPositionActions_;

	SGMMAXvMotor *mVerticalCtrl_;
	SGMMAXvMotor *mHorizontalCtrl_;
	SGMMAXvMotor *mInPlaneCtrl_;
	SGMMAXvMotor *mRotationCtrl_;

	AMExtendedControlEditor *mVerticalNC_;
	AMExtendedControlEditor *mHorizontalNC_;
	AMExtendedControlEditor *mInPlaneNC_;
	AMExtendedControlEditor *mRotationNC_;

	QSlider *illuminatorSlider_;
	QButtonGroup *illuminatorPresets_;
	QToolButton *illuminatorOff_;
	QToolButton *illuminatorDim_;
	QToolButton *illuminatorMid_;
	QToolButton *illuminatorOn_;

	QIcon upIcon_, downIcon_, inboardIcon_, outboardIcon_, upstreamIcon_, downstreamIcon_, cwIcon_, ccwIcon_;

	QGridLayout *gl_;

	double lastHVValue_;

	bool isJogging_;
	double jogStep_;
};

#endif // SGMSAMPLEMANIPULATORVIEW_H
