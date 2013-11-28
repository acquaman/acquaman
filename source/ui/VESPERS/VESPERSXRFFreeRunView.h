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


#ifndef VESPERSVESPERSXRFFreeRunView_H
#define VESPERSVESPERSXRFFreeRunView_H

#include <QWidget>
#include <QToolButton>
#include <QCheckBox>

#include "ui/VESPERS/XRFDetectorView.h"
#include "acquaman/AMScanController.h"
#include "ui/VESPERS/VESPERSCustomizeRegionsOfInterest.h"
#include "ui/VESPERS/VESPERSXRFScanConfigurationView.h"
#include "acquaman/VESPERS/XRFFreeRun.h"
#include "actions/AMBeamlineScanAction.h"

class VESPERSXRFFreeRunView : public QWidget
{
	Q_OBJECT
public:
	/// Constructor.
	explicit VESPERSXRFFreeRunView(XRFFreeRun *xrfFreeRun, QWidget *parent = 0);

protected slots:
	/// Handles what happens when the start button is clicked.
	void onStartClicked()
	{
		start_->setDisabled(true);
		detector_->setTime(integrationTime_->value());
		//AMBeamlineScanAction *action = new AMBeamlineScanAction(xrfFreeRun_->configuration()->createCopy());
		//action->start();
	}
	/// Handles what happens when the stop button is clicked.
	void onStopClicked();
	/// Changes the disabled state of the start button based on the status of the detector.
	void onStatusChanged(bool status) { start_->setDisabled(status); }
	/// Shows/Hides the more advanced settings in the detector.
	void onAdvancedSettingsChanged(bool advanced);
	/// Handles new values set from the integration time spin box and passes it along to the configuration.
	void onIntegrationTimeUpdate() { detector_->setTime(integrationTime_->value()); }
	/// Handles new values set from the minimum energy spin box and passes it along to the configuration.
	void onMinimumEnergyUpdate() { xrfTable_->setMinimumEnergy(minEnergy_->value()*1000); }
	/// Handles new values set from the maximum energy spin box and passes it along to the configuration.
	void onMaximumEnergyUpdate() { detector_->setMaximumEnergyControl(maxEnergy_->value()*1000); xrfTable_->setMaximumEnergy(maxEnergy_->value()*1000); }
	/// Handles new values for the maximum energy from the detector.
	void onMaximumEnergyChanged(double val) { xrfTable_->setMaximumEnergy(val*1000); }
	/// Handles new values set from the peaking time spin box and passes it along to the configuration.
	void onPeakingTimeUpdate() { detector_->setPeakingTimeControl(peakingTime_->value()); }
	/// Handles resetting everything in the view after the detector reconnects.
	void onConnectionChanged(bool isConnected);

	/// Handles what happens when the show pile up peaks is toggled.
	void onShowSelfPileUpPeaks(bool showPeaks);
	/// Handles what happens when the show combination peaks is toggled.
	void onShowCombinationPileUpPeaks(bool showPeaks);
	/// Handles the changes when the current element changes.
	void onCurrentElementChanged(XRFElement *el) { selfPileUpLabel_->setText("Using: "+el->symbol()); }
	/// Handles getting the combination element for combination pile up peak calculation.
	void getCombinationElement();

protected:

	/// Pointer to the XRFPeriodicTable inside configuration.
	XRFPeriodicTable *xrfTable_;
	/// The detector view.
	XRFDetailedDetectorView *view_;
	/// The pointer to the detector.
	XRFDetector *detector_;
	/// The pointer to the customize view.
	VESPERSCustomizeRegionsOfInterest *customize_;
	/// The pointer to the xrf free run model.
	XRFFreeRun *xrfFreeRun_;

	/// The start button.
	QToolButton *start_;
	/// The integration time spin box.
	QDoubleSpinBox *integrationTime_;
	/// The label for the minimum energy spin box.
	QLabel *minEnergyLabel_;
	/// The minimum energy of the detector.  Value is in keV.
	QDoubleSpinBox *minEnergy_;
	/// The label for the maximum energy spin box.
	QLabel *maxEnergyLabel_;
	/// The maximum energy of the detector.  Value is in keV.
	QDoubleSpinBox *maxEnergy_;
	/// The label for the peaking time spin box.
	QLabel *peakingTimeLabel_;
	/// The peaking time for the detector.  Value is in us.
	QDoubleSpinBox *peakingTime_;

	/// The label that shows what element is being used for self pile up peak visualization.
	QLabel *selfPileUpLabel_;
	/// The check box that enables the the combination pile up peaks.
	QCheckBox *combinationPeakCheckBox_;
	/// The label for showing the combination pile up peak element.
	QLabel *combinationPileUpLabel_;
	/// The button used to change the element for the second element in combination pile up peaks.
	QToolButton *combinationPileUpChoiceButton_;
};

#endif // VESPERSVESPERSXRFFreeRunView_H
