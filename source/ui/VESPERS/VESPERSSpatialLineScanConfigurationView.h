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
	explicit VESPERSSpatialLineScanConfigurationView(VESPERSSpatialLineScanConfiguration *config, QWidget *parent = 0);
	/// Destructor.
	virtual ~VESPERSSpatialLineScanConfigurationView();

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
	/// Helper slot that manages setting the normal position.
	void onNormalPositionChanged();

	/// Handles passing the name of the CCD file name to the scan configuration when the CCD check box is enabled.
	void onCCDDetectorChanged(int id);
	/// Handles setting the name of the configuration from the line edit.
	void onScanNameEdited();
	/// Passes on the selection for I0 to the configuration.
	void onI0Clicked(int index) { configuration_->setIncomingChoice(index); }
	/// Handles changing what are acceptable choices for I0 based on It clicks.  Takes in the id of the new It choice.  Passes choice on to the configuration.
	void onItClicked(int index);
	/// Handles changes to the fluorescence detector choice.
	void onFluorescenceChoiceChanged(int id);
	/// Handles changes in the motor selection choice.
	void onMotorChanged(int id);
	/// Helper slot that sets the time offset for the scan.
	void setTimeOffset(double time) { configuration_->setTimeOffset(time); }
	/// Helper slot that handles the setting the estimated time label.
	void onEstimatedTimeChanged();
	/// Handles setting the normal position when the "Set Normal" button is pushed.
	void onSetNormalPosition();

	/// Slot that updates the horizontal step size spin box.
	void updateStep(double val) { step_->setValue(val*1000); }
	/// Helper slot that sets whether we export spectra in rows or columns.
	void updateExportSpectraInRows(bool exportInColumns) { configuration_->setExportSpectraInRows(!exportInColumns); }
	/// Helper slot that sets the close fast shutter after scan option.
	void setCloseFastShutter(bool close) { configuration_->setCloseFastShutter(close); }
	/// Helper slot that sets the return to original position after scan option.
	void setReturnToOriginalPosition(bool returnToOriginalPosition) { configuration_->setReturnToOriginalPosition(returnToOriginalPosition); }
	/// Helper slot that sets the flag for whether the scan will cleanup the scaler after it's done.
	void setCleanupScaler(bool cleanupScaler) { configuration_->setCleanupScaler(cleanupScaler); }

protected:
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
	/// Pointer to the normal position used for the scan.
	QDoubleSpinBox *normalPosition_;

	/// The layout of the overall contents.
	QGridLayout *contentsLayout_;
	/// Pointer to the CCD help group box.
	QGroupBox *ccdTextBox_;
	/// Pointer to the label that holds the current map settings.
	QLabel *mapInfo_;
	/// Label holding the current estimated time for the scan to complete.  Takes into account extra time per point based on experience on the beamline.
	QLabel *estimatedTime_;
};

#endif // VESPERSSPATIALLINESCANCONFIGURATIONVIEW_H
