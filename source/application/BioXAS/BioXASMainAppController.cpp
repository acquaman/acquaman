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

bool BioXASMainAppController::startup()
{
	bool result = false;

	if (BioXASAppController::startup()) {

		// Some first time things.
		AMRun existingRun;

		// We'll use loading a run from the db as a sign of whether this is the first time an application has been run because startupIsFirstTime will return false after the user data folder is created.
		if (!existingRun.loadFromDb(AMDatabase::database("user"), 1)) {

			AMRun firstRun(CLSFacilityID::beamlineName(CLSFacilityID::BioXASMainBeamline), CLSFacilityID::BioXASMainBeamline); //6: BioXAS Main Beamline
			firstRun.storeToDb(AMDatabase::database("user"));
		}

		result = true;
	}

	return result;
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
	BioXASAppController::setupUserInterface();

	mw_->setWindowTitle("Acquaman - BioXAS Main");

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

	// Create persistent view panel:
	////////////////////////////////////

	BioXASMainPersistentView *persistentPanel = new BioXASMainPersistentView();
	persistentPanel->setFixedWidth(400);
	mw_->addRightWidget(persistentPanel);
}

bool BioXASMainAppController::setupDataFolder()
{
	return AMChooseDataFolderDialog::getDataFolder("/AcquamanLocalData/bioxas-m/AcquamanMainData", "/home/bioxas-m/AcquamanMainData", "users", QStringList());
}
