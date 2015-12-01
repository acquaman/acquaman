#include "SGMMapScanConfiguration.h"

#include "acquaman/SGM/SGMMapScanController.h"
#include "ui/SGM/SGMMapScanConfigurationView.h"
#include "beamline/SGM/SGMBeamline.h"
#include "beamline/SGM/SGMHexapod.h"

SGMMapScanConfiguration::SGMMapScanConfiguration(QObject *parent)
	: AMGenericContinuousScanConfiguration(parent)
{
	setName("SGM Map Scan");
	setUserScanName("SGM Map Scan");
	setAutomaticDirectionAssessment(false);
	setDirection(Increase);

	setControl(0, SGMBeamline::sgm()->hexapod()->xAxisPrimeControl()->toInfo());
	scanAxes_.at(0)->regionAt(0)->setRegionStep(1);
	setControl(1, SGMBeamline::sgm()->hexapod()->yAxisPrimeControl()->toInfo());
	scanAxes_.at(1)->regionAt(0)->setRegionStep(0.1);
}


SGMMapScanConfiguration::SGMMapScanConfiguration(const SGMMapScanConfiguration &other)
	: AMGenericContinuousScanConfiguration(other)
{

}

SGMMapScanConfiguration::~SGMMapScanConfiguration()
{

}

AMScanConfiguration *SGMMapScanConfiguration::createCopy() const
{
	AMScanConfiguration *configuration = new SGMMapScanConfiguration(*this);
	configuration->dissociateFromDb(true);
	return configuration;
}

AMScanController *SGMMapScanConfiguration::createController()
{
	AMScanActionController *controller = new SGMMapScanController(this);
	controller->buildScanController();
	return controller;
}

AMScanConfigurationView *SGMMapScanConfiguration::createView()
{
	return new SGMMapScanConfigurationView(this, AMBeamline::bl()->exposedScientificDetectors());
}

QString SGMMapScanConfiguration::technique() const
{
	return "2D Map";
}

QString SGMMapScanConfiguration::description() const
{
	return "Continuous 2D Map";
}

QString SGMMapScanConfiguration::detailedDescription() const
{
	return "Continuous 2D Map";
}
