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


#ifndef VESPERSENERGYSCANCONFIGURATIONVIEW_H
#define VESPERSENERGYSCANCONFIGURATIONVIEW_H

#include "ui/acquaman/AMScanConfigurationView.h"

#include "ui/acquaman/AMRegionsView.h"
#include "ui/VESPERS/VESPERSScanConfigurationView.h"
#include "acquaman/VESPERS/VESPERSEnergyDacqScanController.h"
#include "acquaman/VESPERS/VESPERSEnergyScanConfiguration.h"

#include <QLineEdit>
#include <QTextEdit>
#include <QButtonGroup>
#include <QDoubleSpinBox>
#include <QToolButton>
#include <QComboBox>
#include <QLabel>

/// This class builds the view for configuring a CCD energy scan for the VESPERS beamline.
class VESPERSEnergyScanConfigurationView : public VESPERSScanConfigurationView
{
	Q_OBJECT

public:
	/// Constructor.  Passes in the configuration that this view will modify.
	VESPERSEnergyScanConfigurationView(VESPERSEnergyScanConfiguration *config, QWidget *parent = 0);
	/// Destructor.
	virtual ~VESPERSEnergyScanConfigurationView();

	/// Getter for the configuration.
	const AMScanConfiguration *configuration() const { return configuration_; }

public slots:

protected slots:
	/// Handles setting the name of the configuration from the line edit.
	void onScanNameEdited();
	/// Handles changes to the ccd detector choice.
	void onCCDDetectorChanged(int id);
	/// Sets the current horizontal and vertical positions and saves them in the configuration.
	void setScanPosition();
	/// Helper slot that sets the time offset for the scan.
	void setTimeOffset(double time) { configuration_->setTimeOffset(time); }
	/// Helper slot that handles the setting the estimated time label.
	void onEstimatedTimeChanged();
	/// Handles making sure "Go To Position" looks appropriate when the motors change.
	void onMotorsUpdated(int id);

	/// Emits the configureDetector signal based with 'Roper CCD' or 'Mar CCD.
	void onConfigureCCDDetectorClicked() { emit configureDetector(ccdDetectorIdToString(int(configuration_->ccdDetector()))); }

protected:
	/// Helper method that checks if the CCD files have the name given by \param name.  Does nothing if everything is okay.  Calls onCCDNameConflict if name conflicts exits.
	void checkCCDFileNames(const QString &name) const;

	/// Pointer to the specific scan config the view is modifying.
	VESPERSEnergyScanConfiguration *configuration_;

	/// Pointer to the CCD help group box.
	QGroupBox *ccdTextBox_;
	/// Label holding the current estimated time for the scan to complete.  Takes into account extra time per point based on experience on the beamline.
	QLabel *estimatedTime_;
};

#endif // VESPERSENERGYSCANCONFIGURATIONVIEW_H
