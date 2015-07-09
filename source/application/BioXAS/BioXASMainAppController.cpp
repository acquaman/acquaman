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


#include "BioXASMainAppController.h"

#include "acquaman/BioXAS/BioXASMainXASScanConfiguration.h"
#include "beamline/BioXAS/BioXASMainBeamline.h"
#include "ui/BioXAS/BioXASMainPersistentView.h"

BioXASMainAppController::BioXASMainAppController(QObject *parent)
	: BioXASAppController(parent)
{
	// Initialize variables.

	m1MirrorView_ = 0;
	monoConfigView_ = 0;
	m2MirrorView_ = 0;
	scalerView_ = 0;

	configuration_ = 0;
	configurationView_ = 0;
	configurationViewHolder_ = 0;

	commissioningConfiguration_ = 0;
	commissioningConfigurationView_ = 0;
	commissioningConfigurationViewHolder_ = 0;

	monoCalibrationConfiguration_ = 0;
	monoCalibrationConfigurationView_ = 0;
	monoCalibrationConfigurationViewHolder_ = 0;
}

BioXASMainAppController::~BioXASMainAppController()
{

}

void BioXASMainAppController::onRegionOfInterestAdded(AMRegionOfInterest *region)
{
	BioXASAppController::onRegionOfInterestAdded(region);

	configuration_->addRegionOfInterest(region);
}

void BioXASMainAppController::onRegionOfInterestRemoved(AMRegionOfInterest *region)
{
	BioXASAppController::onRegionOfInterestRemoved(region);

	configuration_->removeRegionOfInterest(region);
}

void BioXASMainAppController::onScalerConnected()
{
	CLSSIS3820Scaler *scaler = BioXASMainBeamline::bioXAS()->scaler();

	if (scaler && scaler->isConnected() && !scalerView_) {
		scalerView_ = new BioXASSIS3820ScalerView(scaler, true);
		mw_->addPane(AMMainWindow::buildMainWindowPane("Scaler",":/utilities-system-monitor.png", scalerView_), "Detectors", "Scaler", ":/utilities-system-monitor.png", true);
	}
}

void BioXASMainAppController::registerClasses()
{
	BioXASAppController::registerClasses();
	AMDbObjectSupport::s()->registerClass<BioXASMainXASScanConfiguration>();
}

void BioXASMainAppController::setupExporterOptions()
{
	AMExporterOptionXDIFormat *bioXASDefaultXAS = BioXAS::buildStandardXDIFormatExporterOption("BioXAS XAS (XDI Format)", "", "", true);

	if(bioXASDefaultXAS->id() > 0)
		AMAppControllerSupport::registerClass<BioXASMainXASScanConfiguration, AMExporterXDIFormat, AMExporterOptionXDIFormat>(bioXASDefaultXAS->id());
}

void BioXASMainAppController::initializeBeamline()
{
	BioXASMainBeamline::bioXAS();
}

void BioXASMainAppController::setupUserInterface()
{
	mw_->setWindowTitle("Acquaman - BioXAS Main");

	// Create panes in the main window:
	////////////////////////////////////

	mw_->insertHeading("General", 0);
	mw_->insertHeading("Detectors", 1);
	mw_->insertHeading("Scans", 2);
	mw_->insertHeading("Calibration", 3);

	// Add widgets to main window panes:
	////////////////////////////////////

	m1MirrorView_ = new BioXASM1MirrorView(BioXASMainBeamline::bioXAS()->m1Mirror());
	mw_->addPane(AMMainWindow::buildMainWindowPane("M1 Mirror", ":/system-software-update.png", m1MirrorView_), "General", "M1 Mirror", ":/system-software-update.png");

	monoConfigView_ = new BioXASSSRLMonochromatorConfigurationView(BioXASMainBeamline::bioXAS()->mono());
	mw_->addPane(AMMainWindow::buildMainWindowPane("Monochromator", ":/system-software-update.png", monoConfigView_), "General", "Monochromator", ":/system-software-update.png");

	m2MirrorView_ = new BioXASM2MirrorView(BioXASMainBeamline::bioXAS()->m2Mirror());
	mw_->addPane(AMMainWindow::buildMainWindowPane("M2 Mirror", ":/system-software-update.png", m2MirrorView_), "General", "M2 Mirror", ":/system-software-update.png");

	configuration_ = new BioXASMainXASScanConfiguration();
	configuration_->setEnergy(10000);
	configurationView_ = new BioXASXASScanConfigurationView(configuration_);
	configurationViewHolder_ = new AMScanConfigurationViewHolder3("Configure an XAS Scan", true, true, configurationView_);
	connect(configuration_, SIGNAL(totalTimeChanged(double)), configurationViewHolder_, SLOT(updateOverallScanTime(double)));
	configurationViewHolder_->updateOverallScanTime(configuration_->totalTime());
	mw_->addPane(configurationViewHolder_, "Scans", "XAS Scan", ":/utilities-system-monitor.png");

	commissioningConfiguration_ = new AMGenericStepScanConfiguration;
	commissioningConfigurationView_ = new AMGenericStepScanConfigurationView(commissioningConfiguration_);
	commissioningConfigurationViewHolder_ = new AMScanConfigurationViewHolder3("Commissioning Tool",true, true, commissioningConfigurationView_);
	mw_->addPane(commissioningConfigurationViewHolder_, "Scans", "Commissioning Tool", ":/utilities-system-monitor.png");

//	monoCalibrationConfiguration_ = new BioXASSSRLMonochromatorEnergyCalibrationScanConfiguration();
//	monoCalibrationConfiguration_->setEnergy(10000);
//	monoCalibrationConfigurationView_ = new BioXASXASScanConfigurationView(monoCalibrationConfiguration_);
//	monoCalibrationConfigurationViewHolder_ = new AMScanConfigurationViewHolder3("Calibrate Energy", true, true, monoCalibrationConfigurationView_, ":/utilities-system-monitor.png");
//	mw_->addPane(monoCalibrationConfigurationViewHolder_, "Calibration", "Calibrate Energy", ":/system-search.png");

	// Create persistent view panel:
	////////////////////////////////////

	persistentPanel_ = new BioXASMainPersistentView();
	persistentPanel_->setFixedWidth(400);
	mw_->addRightWidget(persistentPanel_);
}

void BioXASMainAppController::makeConnections()
{
	connect( BioXASMainBeamline::bioXAS()->scaler(), SIGNAL(connectedChanged(bool)), this, SLOT(onScalerConnected()) );
}

void BioXASMainAppController::applyCurrentSettings()
{
	onScalerConnected();
}

#include <QDebug>

void BioXASMainAppController::onCurrentScanActionStartedImplementation(AMScanAction *action)
{
	BioXASAppController::onCurrentScanActionStartedImplementation(action);

	qDebug() << "\nScan started.";

	if (action) {
		AMScanActionInfo *info = qobject_cast<AMScanActionInfo*>(action->info());

		if (info) {
			BioXASSSRLMonochromatorEnergyCalibrationScanConfiguration *config = qobject_cast<BioXASSSRLMonochromatorEnergyCalibrationScanConfiguration*>(info->configuration());

			if (config) {
				qDebug() << "It is a mono calibration scan.";
			}
		}
	}
}
