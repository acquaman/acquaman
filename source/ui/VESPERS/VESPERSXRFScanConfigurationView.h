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


#ifndef VESPERSXRFSCANCONFIGURATIONVIEW_H
#define VESPERSXRFSCANCONFIGURATIONVIEW_H

#include "ui/acquaman/AMScanConfigurationView.h"
#include "acquaman/VESPERS/VESPERSXRFScanConfiguration.h"

#include <QDoubleSpinBox>
#include <QLabel>

class VESPERSXRFScanConfigurationView : public AMScanConfigurationView
{
	Q_OBJECT
public:
	/// Default constructor.
	VESPERSXRFScanConfigurationView(VESPERSXRFScanConfiguration *scanConfig, QWidget *parent = 0);
	/// Destructor.
	virtual ~VESPERSXRFScanConfigurationView();

	/// Returns a pointer to the current configuration.
	const AMScanConfiguration *configuration() const { return configuration_; }

protected slots:
	/// Shows/Hides the more advanced settings in the detector.
	void onAdvancedSettingsChanged(bool advanced);
	/// Handles new values set from the integration time spin box and passes it along to the configuration.
	void onIntegrationTimeUpdate() { configuration_->setIntegrationTime(integrationTime_->value()); }
	/// Handles new values set from the maximum energy spin box and passes it along to the configuration.
	void onMaximumEnergyUpdate() { configuration_->setMaximumEnergy(maxEnergy_->value()*1000); }
	/// Handles new values set from the peaking time spin box and passes it along to the configuration.
	void onPeakingTimeUpdate() { configuration_->setPeakingTime(peakingTime_->value()); }

protected:

	/// The current configuration.
	VESPERSXRFScanConfiguration *configuration_;

	/// The integration time spin box.
	QDoubleSpinBox *integrationTime_;
	/// The label for the maximum energy spin box.
	QLabel *maxEnergyLabel_;
	/// The maximum energy of the detector.  Value is in keV.
	QDoubleSpinBox *maxEnergy_;
	/// The label for the peaking time spin box.
	QLabel *peakingTimeLabel_;
	/// The peaking time for the detector.  Value is in us.
	QDoubleSpinBox *peakingTime_;
};

#endif // VESPERSXRFSCANCONFIGURATIONVIEW_H
