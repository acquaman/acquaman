#ifndef REIXSXESSCANCONFIGURATIONVIEW_H
#define REIXSXESSCANCONFIGURATIONVIEW_H

#include "ui/AMScanConfigurationView.h"
#include "acquaman/REIXS/REIXSXESScanConfiguration.h"
#include "dataman/REIXS/REIXSXESCalibration.h"

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QTimeEdit>

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
