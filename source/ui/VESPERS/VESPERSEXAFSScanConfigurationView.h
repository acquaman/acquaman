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


#ifndef VESPERSEXAFSSCANCONFIGURATIONVIEW_H
#define VESPERSEXAFSSCANCONFIGURATIONVIEW_H

#include "ui/acquaman/AMScanConfigurationView.h"

#include "ui/VESPERS/VESPERSScanConfigurationView.h"
#include "acquaman/VESPERS/VESPERSEXAFSScanConfiguration.h"
#include "util/AMElement.h"
#include "ui/dataman/AMEXAFSScanAxisView.h"

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
	/// Destructor.
	virtual ~VESPERSEXAFSScanConfigurationView();

	/// Getter for the configuration.
	const AMScanConfiguration* configuration() const { return configuration_; }

public slots:
	/// Slot that sets up the regions for standard XANES scans.
	void setupDefaultXANESScanRegions();
	/// Slot that sets up the regions for standard EXAFS scans.
	void setupDefaultEXAFSScanRegions();

protected slots:
	/// Handles setting the name of the configuration from the line edit.
	void onScanNameEdited();
	/// Handles changing what are acceptable choices for I0 based on It clicks.  Takes in the id of the new It choice.  Passes choice on to the configuration.
	void onItClicked(int index);
	/// Passes on the selection for I0 to the configuration.
	void onI0Clicked(int index) { configuration_->setIncomingChoice(index); }
	/// Handles changes to the fluorescence detector choice.
	void onFluorescenceChoiceChanged(int id);
	/// Sets the new energy.
	void setEnergy();
	/// Handles choosing a new element when the element button is clicked.
	void onElementChoiceClicked();
	/// Fills in the combo box with lines that can be scanned.
	void fillLinesComboBox(AMElement *el);
	/// Handles changes in the combo box index.
	void onLinesComboBoxIndexChanged(int index);
	/// Handles setting the proper information if the edge is changed.
	void onEdgeChanged();
	/// Sets the current horizontal and vertical positions and saves them in the configuration.
	void setScanPosition();
	/// Helper slot that handles the setting the estimated time label.
	void onEstimatedTimeChanged();
	/// Helper slot that sets the time offset for the scan.
	void setTimeOffset(double time) { configuration_->setTimeOffset(time); }
	/// Handles making sure "Go To Position" looks appropriate when the motors change.
	void onMotorsUpdated(int id);

	/// Helper slot that sets whether we export spectra in rows or columns.
	void updateExportSpectraInRows(bool exportInColumns) { configuration_->setExportSpectraInRows(!exportInColumns); }
	/// Helper slot that sets the close fast shutter after scan option.
	void setCloseFastShutter(bool close) { configuration_->setCloseFastShutter(close); }
	/// Helper slot that sets the return to original position after scan option.
	void setReturnToOriginalPosition(bool returnToOriginalPosition) { configuration_->setReturnToOriginalPosition(returnToOriginalPosition); }
	/// Helper slot that sets the flag for whether the scan will cleanup the scaler after it's done.
	void setCleanupScaler(bool cleanupScaler) { configuration_->setCleanupScaler(cleanupScaler); }

protected:
	/// Pointer to the specific scan config the view is modifying.
	VESPERSEXAFSScanConfiguration *configuration_;

	/// View for setting up the regions.
	AMEXAFSScanAxisView *regionsView_;
	/// Double spin box for changing the energy.
	QDoubleSpinBox *energy_;
	/// Button used to choose an element to scan over.
	QToolButton *elementChoice_;
	/// The combo box that holds all the lines that can be chosen to scan over.
	QComboBox *lineChoice_;
	/// Label holding the current estimated time for the scan to complete.  Takes into account extra time per point based on experience on the beamline.
	QLabel *estimatedTime_;
};

#endif // VESPERSEXAFSSCANCONFIGURATIONVIEW_H
