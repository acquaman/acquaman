#include "BioXASSSRLMonochromatorEnergyCalibrationScanConfiguration.h"
#include "acquaman/BioXAS/BioXASXASScanActionController.h"
#include "ui/BioXAS/BioXASXASScanConfigurationView.h"

BioXASSSRLMonochromatorEnergyCalibrationScanConfiguration::BioXASSSRLMonochromatorEnergyCalibrationScanConfiguration(QObject *parent) :
    BioXASXASScanConfiguration(parent)
{

}

BioXASSSRLMonochromatorEnergyCalibrationScanConfiguration::BioXASSSRLMonochromatorEnergyCalibrationScanConfiguration(const BioXASSSRLMonochromatorEnergyCalibrationScanConfiguration &original) :
	BioXASXASScanConfiguration(original)
{

}

BioXASSSRLMonochromatorEnergyCalibrationScanConfiguration::~BioXASSSRLMonochromatorEnergyCalibrationScanConfiguration()
{

}

AMScanConfiguration* BioXASSSRLMonochromatorEnergyCalibrationScanConfiguration::createCopy() const
{
	AMScanConfiguration *configuration = new BioXASSSRLMonochromatorEnergyCalibrationScanConfiguration(*this);
	configuration->dissociateFromDb(true);
	return configuration;
}

AMScanController* BioXASSSRLMonochromatorEnergyCalibrationScanConfiguration::createController()
{
	AMScanActionController *controller = new BioXASXASScanActionController(this);
	controller->buildScanController();
	return controller;
}

AMScanConfigurationView* BioXASSSRLMonochromatorEnergyCalibrationScanConfiguration::createView()
{
	return new BioXASXASScanConfigurationView(this);
}
