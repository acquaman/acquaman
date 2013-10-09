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


#ifndef VESPERSEXAFSSCANCONFIGURATIONVIEW_H
#define VESPERSEXAFSSCANCONFIGURATIONVIEW_H

#include "ui/acquaman/AMScanConfigurationView.h"

#include "ui/acquaman/AMRegionsView.h"
#include "ui/acquaman/AMRegionsLineView.h"
#include "ui/VESPERS/VESPERSScanConfigurationView.h"
#include "acquaman/VESPERS/VESPERSEXAFSDacqScanController.h"
#include "acquaman/VESPERS/VESPERSEXAFSScanConfiguration.h"
#include "util/AMElement.h"

#include <QLineEdit>
#include <QTextEdit>
#include <QButtonGroup>
#include <QDoubleSpinBox>
#include <QToolButton>
#include <QComboBox>
#include <QLabel>

/// This class builds the view for configuring an EXAFS scan for the VESPERS beamline.
class VESPERSEXAFSScanConfigurationView : public VESPERSScanConfigurationView
{
	Q_OBJECT

public:
	/// Constructor.
	/// \param config is the EXAFS configuration that the view will modify.
	VESPERSEXAFSScanConfigurationView(VESPERSEXAFSScanConfiguration *config, QWidget *parent = 0);

	/// Getter for the configuration.
	const AMScanConfiguration* configuration() const { return config_; }

public slots:

protected slots:
	/// Handles setting the name of the configuration from the line edit.
	void onScanNameEdited();
	/// Handles changing what are acceptable choices for I0 based on It clicks.  Takes in the id of the new It choice.  Passes choice on to the configuration.
	void onItClicked(int id);
	/// Passes on the selection for I0 to the configuration.
	void onI0Clicked(int id) { config_->setIncomingChoice(id); }
	/// Handles changes to the fluorescence detector choice.
	void onFluorescenceChoiceChanged(int id);
	/// Sets the new energy.
	void setEnergy() { config_->setEnergy(energy_->value()); }
	/// Handles choosing a new element when the element button is clicked.
	void onElementChoiceClicked();
	/// Fills in the combo box with lines that can be scanned.
	void fillLinesComboBox(const AMElement *el);
	/// Handles changes in the combo box index.
	void onLinesComboBoxIndexChanged(int index);
	/// Handles setting the proper information if the edge is changed.
	void onEdgeChanged();
	/// Sets the current horizontal and vertical positions and saves them in the configuration.
	void setScanPosition();
	/// Helper slot that handles the setting the estimated time label.
	void onEstimatedTimeChanged();
	/// Helper slot that sets the time offset for the scan.
	void setTimeOffset(double time) { config_->setTimeOffset(time); }
	/// Slot that sets up the regions for standard XANES scans.
	void onDefaultXANESScanClicked();
	/// Slot that sets up the regions for standard EXAFS scans.
	void onDefaultEXAFSScanClicked();
	/// Handles making sure "Go To Position" looks appropriate when the motors change.
	void onMotorsUpdated(int id);

	/// Emits the configureDetector signal based on the current fluorescence detector choice.
	void onConfigureXRFDetectorClicked() { emit configureDetector(fluorescenceDetectorIdToString(int(config_->fluorescenceDetector()))); }
	/// Updates roiText_ based on the current state of the ROI list.
	void updateRoiText();
	/// Helper slot that sets whether we export spectra in rows or columns.
	void updateExportSpectraInRows(bool exportInColumns) { config_->setExportSpectraInRows(!exportInColumns); }

protected:
	/// Reimplements the show event to update the Regions of Interest text.
	virtual void showEvent(QShowEvent *e) { updateRoiText(); AMScanConfigurationView::showEvent(e); }

	/// Pointer to the specific scan config the view is modifying.
	VESPERSEXAFSScanConfiguration *config_;

	/// This lets you setup regions.
	AMRegionsView *regionsView_;
	/// Visual box that shows the current regions.
	AMRegionsLineView *regionsLineView_;

	/// Double spin box for changing the energy.
	QDoubleSpinBox *energy_;
	/// Button used to choose an element to scan over.
	QToolButton *elementChoice_;
	/// The combo box that holds all the lines that can be chosen to scan over.
	QComboBox *lineChoice_;
	/// Label holding the current estimated time for the scan to complete.  Takes into account extra time per point based on experience on the beamline.
	QLabel *estimatedTime_;
	/// Label holding the current estimated time for the set of scans to complete.
	QLabel *estimatedSetTime_;
	/// Group box holding the roiText information.
	QGroupBox *roiTextBox_;
};

#endif // VESPERSEXAFSSCANCONFIGURATIONVIEW_H
