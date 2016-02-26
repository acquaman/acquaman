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


#ifndef IDEASXASSCANCONFIGURATIONVIEW_H
#define IDEASXASSCANCONFIGURATIONVIEW_H

#include "ui/acquaman/AMScanConfigurationView.h"

#include "acquaman/IDEAS/IDEASXASScanConfiguration.h"

#include "util/AMElement.h"


#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QLabel>
#include <QToolButton>

class AMScanController;
class AMEXAFSScanAxisView;

class IDEASXASScanConfigurationView : public AMScanConfigurationView
{

Q_OBJECT

public:
	IDEASXASScanConfigurationView(IDEASXASScanConfiguration *configuration, QWidget *parent = 0);
	virtual ~IDEASXASScanConfigurationView();

	virtual const AMScanConfiguration* configuration() const;

public slots:
	/// Slot that sets up the regions for standard XANES scans.
	void setupDefaultXANESScanRegions();
	/// Slot that sets up the regions for standard EXAFS scans.
	void setupDefaultEXAFSScanRegions();

protected slots:
	/// Handles setting the name of the configuration from the line edit.
	void onScanNameEdited();
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
	/// Helper slot that handles the setting the estimated time label.
	void onEstimatedTimeChanged();
	/// Handles updates of displayed detector ROIs
	void onROIChange();
	/// Slot that updates reference checkbox when transmission is checkbox changes
	void onIsTransScanChecked(bool checked);
	/// Slot that updates the fluorescence detector buttons.
	void updateFluorescenceDetectorComboBox(IDEAS::FluorescenceDetectors detector);
	/// Slot that updates the fluorescence detector comboBox items when the Ge13Elemet connects.
	void updateFluorescenceDetectorComboBoxGe13Element(bool connected);
	/// Handles changes to the fluorescence detector choice.
	void onFluorescenceChoiceChanged(int id);
	/// Handles changes in peaking time from detector
	void onPeakingTimeChanged(double value);

protected:
	/// Creates a combo box for the fluorescenceDetector enum.  Returns a pointer to the widget.
	QComboBox *createFluorescenceComboBox();

	IDEASXASScanConfiguration *configuration_;

	AMEXAFSScanAxisView *regionsView_;
	QPushButton *autoRegionButton_;
	QPushButton *pseudoXAFSButton_;

	QCheckBox *isTransScanCheckBox_;
	QCheckBox *useRefCheckBox_;

	QLineEdit *scanName_;

	/// Label for peaking value sent from the detector
	QLabel *peakingSetting_;


	/// Double spin box for changing the energy.
	QDoubleSpinBox *energy_;
	/// Button used to choose an element to scan over.
	QToolButton *elementChoice_;
	/// The combo box that holds all the lines that can be chosen to scan over.
	QComboBox *lineChoice_;
	/// Label holding the current estimated time for the scan to complete.  Takes into account extra time per point based on experience on the beamline.
	QLabel *estimatedTime_;
	/// Label holding the current number of points in a scan.
	QLabel *pointPerScan_;
	/// Label holding the energy space scan range.
	QLabel *scanEnergyRange_;
	/// Label holding the currently selected ROIs.
	QLabel *ROIsLabel_;
	/// Combo box for choosing the fluorescence detector.
	QComboBox *fluorescenceDetectorComboBox_;
};

#endif // IDEASXASSCANCONFIGURATIONVIEW_H
