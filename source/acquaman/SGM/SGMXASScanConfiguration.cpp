#include "SGMXASScanConfiguration.h"

#include "acquaman/SGM/SGMXASScanController.h"
#include "ui/SGM/SGMXASScanConfigurationView.h"
#include "beamline/SGM/SGMBeamline.h"
#include "beamline/SGM/energy/SGMEnergyControlSet.h"

SGMXASScanConfiguration::SGMXASScanConfiguration(QObject *parent)
	: AMGenericContinuousScanConfiguration(parent)
{
	setName("SGM XAS");
	setUserScanName("SGM XAS");
	setAutomaticDirectionAssessment(true);
	setDirection(Increase);

	setControl(0, SGMBeamline::sgm()->energyControlSet()->energy()->toInfo());
	scanAxes_.at(0)->regionAt(0)->setRegionStep(1);
}

SGMXASScanConfiguration::SGMXASScanConfiguration(const SGMXASScanConfiguration &other)
	: AMGenericContinuousScanConfiguration(other)
{

}

SGMXASScanConfiguration::~SGMXASScanConfiguration()
{

}

AMScanConfiguration *SGMXASScanConfiguration::createCopy() const
{
	AMScanConfiguration *configuration = new SGMXASScanConfiguration(*this);
	configuration->dissociateFromDb(true);
	return configuration;
}

AMScanController *SGMXASScanConfiguration::createController()
{
	AMScanActionController *controller = new SGMXASScanController(this);
	controller->buildScanController();
	return controller;
}

AMScanConfigurationView *SGMXASScanConfiguration::createView()
{
	return new SGMXASScanConfigurationView(this, AMBeamline::bl()->exposedScientificDetectors());
}

QString SGMXASScanConfiguration::technique() const
{
	return "XAS";
}

QString SGMXASScanConfiguration::description() const
{
	return "XAS Continuous Scan";
}

QString SGMXASScanConfiguration::detailedDescription() const
{
	return "XAS Continuous Scan";
}
