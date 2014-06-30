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


#ifndef VESPERSSPATIALLINESCANCONFIGURATIONVIEW_H
#define VESPERSSPATIALLINESCANCONFIGURATIONVIEW_H

#include "ui/acquaman/AMScanConfigurationView.h"
#include "ui/VESPERS/VESPERSScanConfigurationView.h"
#include "acquaman/VESPERS/VESPERSSpatialLineScanConfiguration.h"

#include <QLineEdit>
#include <QTextEdit>
#include <QButtonGroup>
#include <QDoubleSpinBox>
#include <QToolButton>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>

/// This class builds the view for configuring a spatial line scan for the VESPERS beamline.
class VESPERSSpatialLineScanConfigurationView : public VESPERSScanConfigurationView
{
	Q_OBJECT
public:
	/// Constructor.
	virtual ~VESPERSSpatialLineScanConfigurationView();
	explicit VESPERSSpatialLineScanConfigurationView(VESPERSSpatialLineScanConfiguration *config, QWidget *parent = 0);

	/// Getter for the configuration.
	const AMScanConfiguration* configuration() const { return configuration_; }

	/// Method that updates the map info label based on the current values of the start, end, and step size.
	void updateMapInfo();

protected slots:
	/// Handles setting the start position when the "Use Current" button is pushed.
	void onSetStartPosition();
	/// Handles setting the end position when the "Use Current" button is pushed.
	void onSetEndPosition();
	/// Helper slot that manages setting the x axis start position.
	void onStartChanged();
	/// Helper slot that manages setting the x axis end position.
	void onEndChanged();
	/// Helper slot that manages setting the x axis step size.
	void onStepChanged();
	/// Helper slot that manages setting the time per point.
	void onDwellTimeChanged();
	/// Helper slot that sets the other position.
	void onOtherPositionChanged();

	/// Handles passing the name of the CCD file name to the scan configuration when the CCD check box is enabled.
	void onCCDDetectorChanged(int id);
	/// Handles setting the name of the configuration from the line edit.
	void onScanNameEdited();
	/// Passes on the selection for I0 to the configuration.
	void onI0Clicked(int id) { configuration_->setIncomingChoice(id); }
	/// Handles changing what are acceptable choices for I0 based on It clicks.  Takes in the id of the new It choice.  Passes choice on to the configuration.
	void onItClicked(int id);
	/// Handles changes to the fluorescence detector choice.
	void onFluorescenceDetectorChanged(int id);
	/// Handles changes in the motor selection choice.
	void onMotorChanged(int id);
	/// Updates the button group if the motor is changed from elsewhere in the program.
	void onMotorUpdated(int id);
	/// Helper slot that sets the time offset for the scan.
	void setTimeOffset(double time) { configuration_->setTimeOffset(time); }
	/// Helper slot that handles the setting the estimated time label.
	void onEstimatedTimeChanged();

	/// Emits the configureDetector signal based on the current fluorescence detector choice.
	void onConfigureXRFDetectorClicked() { emit configureDetector(fluorescenceDetectorIdToString(int(configuration_->fluorescenceDetector()))); }
	/// Emits the configureDetector signal based with 'Roper CCD'.
	void onConfigureCCDDetectorClicked() { emit configureDetector(ccdDetectorIdToString(int(configuration_->ccdDetector()))); }
	/// Updates roiText_ based on the current state of the ROI list.
	void updateRoiText();

	/// Slot that updates the horizontal step size spin box.
	void updateStep(double val) { step_->setValue(val*1000); }
	/// Helper slot that sets whether we export spectra in rows or columns.
	void updateExportSpectraInRows(bool exportInColumns) { configuration_->setExportSpectraInRows(!exportInColumns); }

protected:
	/// Reimplements the show event to update the Regions of Interest text.
	virtual void showEvent(QShowEvent *e) { updateRoiText(); AMScanConfigurationView::showEvent(e); }
	/// Helper method that updates the x and y step spin boxes if the map is not possible to change.
	void axesAcceptable();
	/// Helper method that checks if the CCD files have the name given by \param name.  Does nothing if everything is okay.  Calls onCCDNameConflict if name conflicts exits.
	void checkCCDFileNames(const QString &name) const;

	/// Pointer to the specific scan config the view is modifying.
	VESPERSSpatialLineScanConfiguration *configuration_;

	/// Pointer to the start point spin box.
	QDoubleSpinBox *start_;
	/// Pointer to the end point spin box.
	QDoubleSpinBox *end_;
	/// Pointer to the step size spin box.
	QDoubleSpinBox *step_;
	/// Pointer to the other position label.
	QLabel *otherPositionLabel_;
	/// Pointer to the spin box holding the other position.
	QDoubleSpinBox *otherPosition_;
	/// Button holding the pointer to the CCD detector button.  It takes you to either Roper or Mar CCD detector screens.
	QPushButton *configureCCDButton_;

	/// Pointer to the CCD help group box.
	QGroupBox *ccdTextBox_;
	/// Pointer to the label that holds the current map settings.
	QLabel *mapInfo_;
	/// Label holding the current estimated time for the scan to complete.  Takes into account extra time per point based on experience on the beamline.
	QLabel *estimatedTime_;
};

#endif // VESPERSSPATIALLINESCANCONFIGURATIONVIEW_H
