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


#ifndef REIXSXESSCANCONFIGURATIONVIEW_H
#define REIXSXESSCANCONFIGURATIONVIEW_H

#include "ui/acquaman/AMScanConfigurationView.h"
#include "acquaman/REIXS/REIXSXESScanConfiguration.h"
#include "dataman/REIXS/REIXSXESCalibration2.h"

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QTimeEdit>
#include <QRadioButton>
#include <QCheckBox>


class REIXSXESScanConfigurationView : public AMScanConfigurationView
{
	Q_OBJECT
public:
	/// Constructor.  If \c config is 0, we create our own (and delete it, when destroyed).  If a valid \c config is provided, we will operate on that one, but not take ownership of it. It must remain valid as long as this widget exists.
	explicit REIXSXESScanConfigurationView(REIXSXESScanConfiguration* config = 0, QWidget *parent = 0);

	virtual const AMScanConfiguration* configuration() const { return configuration_; }

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
	/// The internal configuration that we modify.
	REIXSXESScanConfiguration* configuration_;
	/// If we've created scanConfiguration_, we can delete it when we are deleted. If it belongs to someone else, don't.
	bool ownsConfiguration_;

	/// UI elements
	QComboBox* gratingSelector_;
	QDoubleSpinBox* centerEVBox_;
	QDoubleSpinBox* defocusDistanceMmBox_;
	QDoubleSpinBox* detectorTiltBox_;

	// removed: QRadioButton *horizontalDetectorButton_, *verticalDetectorButton_;

	QCheckBox* startFromCurrentPositionOption_;
	QCheckBox* doNotClearExistingCountsOption_;

	QDoubleSpinBox* maximumTotalCounts_;
	QTimeEdit* maximumTimeEdit_;

	QComboBox* calibrationSelector_;

	/// A spectrometer calibration object to work with. Used to determine the options for other parameters.
	REIXSXESCalibration2 calibration_;

	/// The current calibrationId that we're working with (initially -1)
	int currentCalibrationId_;


};

#endif // REIXSXESSCANCONFIGURATIONVIEW_H
