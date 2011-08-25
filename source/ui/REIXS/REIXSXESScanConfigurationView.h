/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef REIXSXESSCANCONFIGURATIONVIEW_H
#define REIXSXESSCANCONFIGURATIONVIEW_H

#include "ui/AMScanConfigurationView.h"
#include "acquaman/REIXS/REIXSXESScanConfiguration.h"
#include "dataman/REIXS/REIXSXESCalibration.h"

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QTimeEdit>
#include <QRadioButton>
#include <QCheckBox>


class REIXSXESScanConfigurationView : public AMScanConfigurationView
{
	Q_OBJECT
public:
	explicit REIXSXESScanConfigurationView(QWidget *parent = 0);

	virtual const AMScanConfiguration* configuration() const { return &configuration_; }

signals:

public slots:

protected slots:

	/// called to update the available calibrations;looks in the database for all available
	void onLoadCalibrations();

	/// called whenever the selected calbiration item in the combobox changes
	void onCalibrationIndexChanged(int newIndex);

	/// called when the selected grating changes
	void onSelectedGratingChanged(int newGrating);

	/// Forwards the signal when you adjust the time edit to control the maximum duration of the scan
	void onMaximumTimeEditChanged(const QTime& time);


protected:
	/// The internal configuration that we modify
	REIXSXESScanConfiguration configuration_;

	/// UI elements
	QComboBox* gratingSelector_;
	QDoubleSpinBox* centerEVBox_;
	QDoubleSpinBox* defocusDistanceMmBox_;
	QDoubleSpinBox* detectorTiltBox_;

	QRadioButton *horizontalDetectorButton_, *verticalDetectorButton_;

	QCheckBox* startFromCurrentPositionOption_;

	QDoubleSpinBox* maximumTotalCounts_;
	QTimeEdit* maximumTimeEdit_;

	QComboBox* calibrationSelector_;

	/// A spectrometer calibration object to work with. Used to determine the options for other parameters.
	REIXSXESCalibration calibration_;

	/// The current calibrationId that we're working with (initially -1)
	int currentCalibrationId_;


};

#endif // REIXSXESSCANCONFIGURATIONVIEW_H
