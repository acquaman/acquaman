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


#ifndef REIXSXESSPECTROMETERCONTROLEDITOR_H
#define REIXSXESSPECTROMETERCONTROLEDITOR_H

#include <QGroupBox>
#include "beamline/REIXS/REIXSBeamline.h"

class QLabel;
class QComboBox;
class QDoubleSpinBox;
class QPushButton;

class REIXSXESSpectrometerControlEditor : public QGroupBox
{
	Q_OBJECT

public:
	explicit REIXSXESSpectrometerControlEditor(REIXSSpectrometer* spectrometer, QWidget *parent = 0);


	virtual ~REIXSXESSpectrometerControlEditor();

protected:
	REIXSSpectrometer* spectrometer_;

protected slots:

	// Respond to UI events
	void onMoveButtonClicked();
	void onStopButtonClicked();
	void onGratingComboBoxActivated(int grating);

	// external events from spectrometer
	void populateGratingComboBox();

	// External events, or internal (either the UI, or the spectrometer, has moved, so we need to update the current status)

	void updateCurrentEnergyStatus(double eV);
	void updateCurrentEnergyStatus();
	void updateCurrentGratingStatus();
	void updateMaskPosition();

	// Move failed: provide errorMon messages
	void onSpectrometerMoveSucceeded();
	void onSpectrometerMoveFailed(int reason);

private slots:

	void on_maskComboBox_currentIndexChanged(const QString &arg1);
	void updateEnergyRange();

protected:
	void setupUi();
	void setupConnections();

	QDoubleSpinBox *createDoubleSpinBox(double value, double min, double max, QString suffix = "", double decimals = 2);

protected:
	QComboBox* gratingSelectorBox_;
	QDoubleSpinBox* energyBox_;
	QDoubleSpinBox* defocusOffsetBox_;
	QDoubleSpinBox* tiltOffsetBox_;
	QPushButton* stopButton_;
	QPushButton* moveNowButton_;
	QComboBox* maskComboBox_;

	QLabel *energyRangeLabel_;
	QLabel *maskFeedbackLabel_;
	QLabel *gratingFeedbackLabel_;
	QLabel *energyFeedbackLabel_;
};

#endif // REIXSXESSPECTROMETERCONTROLEDITOR_H
