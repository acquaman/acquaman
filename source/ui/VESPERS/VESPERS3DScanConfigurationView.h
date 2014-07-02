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


#ifndef VESPERS3DSCANCONFIGURATIONVIEW_H
#define VESPERS3DSCANCONFIGURATIONVIEW_H

#include "ui/VESPERS/VESPERSScanConfigurationView.h"
#include "acquaman/VESPERS/VESPERS3DScanConfiguration.h"

#include <QLineEdit>
#include <QTextEdit>
#include <QButtonGroup>
#include <QDoubleSpinBox>
#include <QToolButton>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>

/// This class builds the view for configuring a 2D map scan for the VESPERS beamline.
class VESPERS3DScanConfigurationView : public VESPERSScanConfigurationView
{
	Q_OBJECT

public:
	/// Constructor.  \param config is the 3D connfiguration that the view will modify.
	virtual ~VESPERS3DScanConfigurationView();
	VESPERS3DScanConfigurationView(VESPERS3DScanConfiguration *config, QWidget *parent = 0);

	/// Getter for the configuration.
	const AMScanConfiguration *configuration() const { return configuration_; }

	/// Method that updates the map info label based on the current values of the start, end, and step size.
	void updateMapInfo();

protected slots:
	/// Sets the x-axis start position.
	void setXAxisStart(const AMNumber &value);
	/// Sets the y-axis start position.
	void setYAxisStart(const AMNumber &value);
	/// Sets the wire axis start position.
	void setWireAxisStart(const AMNumber &value);
	/// Sets the x-axis step position.
	void setXAxisStep(const AMNumber &value);
	/// Sets the y-axis step position.
	void setYAxisStep(const AMNumber &value);
	/// Sets the wire axis step position.
	void setWireAxisStep(const AMNumber &value);
	/// Sets the x-axis end position.
	void setXAxisEnd(const AMNumber &value);
	/// Sets the y-axis start position.
	void setYAxisEnd(const AMNumber &value);
	/// Sets the wire axis end position.
	void setWireAxisEnd(const AMNumber &value);
	/// Sets the dwell time.
	void setDwellTime(const AMNumber &value);

	/// Handles setting the start position when the "Use Current" button is pushed.
	void onSetStartPosition();
	/// Handles setting the end position when the "Use Current" button is pushed.
	void onSetEndPosition();
	/// Handles setting the start position of the wire when the "Use Current" button is pushed.
	void onSetWireStartPosition();
	/// Handles setting the end position of the wire when the "Use Current" button is pushed.
	void onSetWireEndPosition();
	/// Helper slot that manages setting the x axis start position.
	void onXStartChanged();
	/// Helper slot that manages setting the x axis end position.
	void onXEndChanged();
	/// Helper slot that manages setting the x axis step size.
	void onXStepChanged();
	/// Helper slot that manages setting the y axis start position.
	void onYStartChanged();
	/// Helper slot that manages setting the y axis end position.
	void onYEndChanged();
	/// Helper slot that manages setting the y axis step size.
	void onYStepChanged();
	/// Helper slot that manages setting the wire axis start position.
	void onWireStartChanged();
	/// Helper slot that manages setting the wire axis end position.
	void onWireEndChanged();
	/// Helper slot that manages setting the wire axis step size.
	void onWireStepChanged();
	/// Helper slot that manages setting the time per point.
	void onDwellTimeChanged();

	/// Handles passing the name of the CCD file name to the scan configuration when the CCD check box is enabled.
	void onCCDDetectorChanged(int id);
	/// Handles setting the name of the configuration from the line edit.
	void onScanNameEdited();
	/// Passes on the selection for I0 to the configuration.
	void onI0Clicked(int id) { configuration_->setIncomingChoice(id); }
	/// Handles changes to the fluorescence detector choice.
	void onFluorescenceChoiceChanged(int id);
	/// Handles changes in the motor selection choice.
	void onMotorChanged(int id);
	/// Helper slot that sets the time offset for the scan.
	void setTimeOffset(double time) { configuration_->setTimeOffset(time); }
	/// Helper slot that handles the setting the estimated time label.
	void onEstimatedTimeChanged();

	/// Helper slot that passes the signal on to the base method.
	void onConfigureXRFDetectorClicked() { emit configureDetector(fluorescenceDetectorIdToString(int(configuration_->fluorescenceDetector()))); }
	/// Emits the configureDetector signal based with 'Roper CCD'.
	void onConfigureCCDDetectorClicked() { emit configureDetector(ccdDetectorIdToString(int(configuration_->ccdDetector()))); }
	/// Updates roiText_ based on the current state of the ROI list.
	void updateRoiText();

	/// Slot that updates the horizontal step size spin box.
	void updateXStep(double val) { hStep_->setValue(val*1000); }
	/// Slot that updates the vertical step size spin box.
	void updateYStep(double val) { vStep_->setValue(val*1000); }
	/// Slot that updates the wire step size spin box.
	void updateZStep(double val) { wireStep_->setValue(val*1000); }

	/// Helper slot that sets whether we use SMAK or Ascii for the auto exporter.
	void updateAutoExporter(int useAscii) { configuration_->setExportAsAscii(useAscii == 0); }
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
	VESPERS3DScanConfiguration *configuration_;

	/// Pointer to the horizontal start point.
	QDoubleSpinBox *hStart_;
	/// Pointer to the horizontal end point.
	QDoubleSpinBox *hEnd_;
	/// Pointer to the vertical start point.
	QDoubleSpinBox *vStart_;
	/// Pointer to the vertical end point.
	QDoubleSpinBox *vEnd_;
	/// Pointer to the horizontal step size.
	QDoubleSpinBox *hStep_;
	/// Pointer to the vertical step size.
	QDoubleSpinBox *vStep_;
	/// Pointer to the wire start point.
	QDoubleSpinBox *wireStart_;
	/// Pointer to the wire end point.
	QDoubleSpinBox *wireEnd_;
	/// Pointer to the wire step size.
	QDoubleSpinBox *wireStep_;

	/// Pointer to the CCD help group box.
	QGroupBox *ccdTextBox_;
	/// Pointer to the label that holds the current map settings.
	QLabel *mapInfo_;
	/// Pointer to the button that gets the CCD detector screen to switch.
	QPushButton *configureCCDButton_;
	/// Label holding the current estimated time for the scan to complete.  Takes into account extra time per point based on experience on the beamline.
	QLabel *estimatedTime_;
};

#endif // VESPERS3DSCANCONFIGURATIONVIEW_H
