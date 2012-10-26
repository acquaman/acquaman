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


#ifndef VESPERSSPATIALLINESCANCONFIGURATIONVIEW_H
#define VESPERSSPATIALLINESCANCONFIGURATIONVIEW_H

#include "ui/acquaman/AMScanConfigurationView.h"
#include "acquaman/VESPERS/VESPERSSpatialLineScanConfiguration.h"
#include "acquaman/VESPERS/VESPERSSpatialLineDacqScanController.h"

#include <QLineEdit>
#include <QTextEdit>
#include <QButtonGroup>
#include <QDoubleSpinBox>
#include <QToolButton>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>

/// This class builds the view for configuring a spatial line scan for the VESPERS beamline.
class VESPERSSpatialLineScanConfigurationView : public AMScanConfigurationView
{
	Q_OBJECT
public:
	/// Constructor.
	explicit VESPERSSpatialLineScanConfigurationView(VESPERSSpatialLineScanConfiguration *config, QWidget *parent = 0);

	/// Getter for the configuration.
	const AMScanConfiguration* configuration() const { return config_; }

	/// Method that updates the map info label based on the current values of the start, end, and step size.
	void updateMapInfo();

signals:
	/// Sends out a request that the current detector (based on FluorescenceDetectorChoice) to be configured.  Asks the app controller to change to the detector view.  String will be either "Single Element" or "Four Element".
	void configureDetector(const QString &);

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

	/// Helper slot that sets the CCD detector setting in the configuration.
	void onCCDButtonClicked(bool useCCD) { config_->setCCDDetector(useCCD ? 1 : 0); }
	/// Handles passing the name of the CCD file name to the scan configuration when the CCD check box is enabled.
	void onCCDDetectorChanged(int useCCD);
	/// Handles changes in the name of the CCD file name and sets the label that corresponds to it.
	void onCCDFileNameChanged(QString name) { currentCCDFileName_->setText(QString("Current CCD file name:\t%1").arg(name)); }
	/// Handles setting the name of the configuration from the line edit.
	void onScanNameEdited() { config_->setName(scanName_->text()); config_->setUserScanName(scanName_->text());}
	/// Passes on the selection for I0 to the configuration.
	void onI0Clicked(int id) { config_->setIncomingChoice(id); }
	/// Handles changes to the fluorescence detector choice.
	void onFluorescenceDetectorChanged(int id);
	/// Handles changes in the motor selection choice.
	void onMotorChoiceChanged(int id);
	/// Helper slot that handles the setting the estimated time label.
	void onEstimatedTimeChanged();

	/// Emits the configureDetector signal based on the current fluorescence detector choice.
	void onConfigureXRFDetectorClicked();
	/// Emits the configureDetector signal based with 'Roper CCD'.
	void onConfigureRoperDetectorClicked();
	/// Updates roiText_ based on the current state of the ROI list.
	void updateRoiText();
	/// Helper method that returns a list of QPairs where each pair corresponds to the same ROIs.  Used only when using both vortex detectors together.
	QList<QPair<int, int> > findRoiPairs() const;
	/// Handles the context menu.
	void onCustomContextMenuRequested(QPoint pos);

	/// Slot that updates the horizontal step size spin box.
	void updateStep(double val) { step_->setValue(val*1000); }
	/// Slot that updates the I0 buttons.
	void updateI0Buttons(int I0) { I0Group_->button(I0)->setChecked(true); }
	/// Slot that updates the fluorescence detector buttons.
	void updateFluorescenceDetector(int detector) { fluorescenceButtonGroup_->button(detector)->setChecked(true); }
	/// Slot that updates the motor choice buttons.
	void updateMotorChoice(int choice) { motorChoiceButtonGroup_->button(choice)->setChecked(true); }

protected:
	/// Reimplements the show event to update the Regions of Interest text.
	virtual void showEvent(QShowEvent *e) { updateRoiText(); AMScanConfigurationView::showEvent(e); }
	/// Helper method that takes a time in seconds and returns a string of d:h:m:s.
	QString convertTimeToString(double time);
	/// Helper method that updates the x and y step spin boxes if the map is not possible to change.
	void axesAcceptable();

	/// Pointer to the specific scan config the view is modifying.
	VESPERSSpatialLineScanConfiguration *config_;

	/// Pointer to the start point spin box.
	QDoubleSpinBox *start_;
	/// Pointer to the end point spin box.
	QDoubleSpinBox *end_;
	/// Pointer to the step size spin box.
	QDoubleSpinBox *step_;

	/// Pointer to the dwell time per point.
	QDoubleSpinBox *dwellTime_;

	/// Pointer to the label that holds the current map settings.
	QLabel *mapInfo_;

	/// Pointer to the check box for doing XRD maps as well.
	QCheckBox *ccdCheckBox_;
	/// Pointer to the label holding the current file name.
	QLabel *currentCCDFileName_;

	/// Line edit for changing the name of the scan.
	QLineEdit *scanName_;
	/// Label holding the current estimated time for the scan to complete.  Takes into account extra time per point based on experience on the beamline.
	QLabel *estimatedTime_;
	/// Button group for the I0 ion chamber selection.
	QButtonGroup *I0Group_;
	/// Button group for the fluorescence detector selection.
	QButtonGroup *fluorescenceButtonGroup_;
	/// Button group for the motor choice selection.
	QButtonGroup *motorChoiceButtonGroup_;
	/// The text edit that holds all the names of the regions of interest.
	QTextEdit *roiText_;

	/// A label holding text for the the time offset spin box.
	QLabel *timeOffsetLabel_;
	/// A spin box holding the time offset.
	QDoubleSpinBox *timeOffset_;
};

#endif // VESPERSSPATIALLINESCANCONFIGURATIONVIEW_H
