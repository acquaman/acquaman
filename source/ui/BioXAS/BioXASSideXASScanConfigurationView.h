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


#ifndef BIOXASSIDEXASSCANCONFIGURATIONVIEW_H
#define BIOXASSIDEXASSCANCONFIGURATIONVIEW_H

#include "ui/acquaman/AMScanConfigurationView.h"
#include "acquaman/BioXAS/BioXASSideXASScanConfiguration.h"
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
class AMTopFrame;

class BioXASSideXASScanConfigurationView : public AMScanConfigurationView
{

Q_OBJECT

public:
	BioXASSideXASScanConfigurationView(BioXASSideXASScanConfiguration *configuration, QWidget *parent = 0);
	virtual ~BioXASSideXASScanConfigurationView();

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

protected:
	/// The scan configuration being viewed.
	BioXASSideXASScanConfiguration *configuration_;

	AMTopFrame *topFrame_;
	AMEXAFSScanAxisView *regionsView_;
	QPushButton *autoRegionButton_;
	QPushButton *pseudoXAFSButton_;

	QLineEdit *scanName_;

	/// Double spin box for changing the energy.
	QDoubleSpinBox *energy_;
	/// Button used to choose an element to scan over.
	QToolButton *elementChoice_;
	/// The combo box that holds all the lines that can be chosen to scan over.
	QComboBox *lineChoice_;
	/// Label holding the current number of points in a scan.
	QLabel *pointPerScan_;
	/// Label holding the energy space scan range.
	QLabel *scanEnergyRange_;
	/// Check box for using the XRF detector.
	QCheckBox *usingXRFDetectorCheckBox_;
};

#endif // BIOXASXASSCANCONFIGURATIONVIEW_H
