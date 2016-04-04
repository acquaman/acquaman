#include "SGMLineScanConfiguration.h"

#include "acquaman/SGM/SGMLineScanController.h"
#include "ui/SGM/SGMLineScanConfigurationView.h"
#include "beamline/SGM/SGMBeamline.h"
#include "beamline/SGM/SGMHexapod.h"

SGMLineScanConfiguration::SGMLineScanConfiguration(QObject *parent)
	: AMGenericContinuousScanConfiguration(parent)
{
	setName("SGM Line Scan");
	setUserScanName("SGM Line Scan");
	setAutomaticDirectionAssessment(false);
	setDirection(Increase);

	setControl(0, SGMBeamline::sgm()->hexapod()->xAxisPrimeControl()->toInfo());
	scanAxes_.at(0)->regionAt(0)->setRegionStep(1);
}


SGMLineScanConfiguration::SGMLineScanConfiguration(const SGMLineScanConfiguration &other)
	: AMGenericContinuousScanConfiguration(other)
{

}

SGMLineScanConfiguration::~SGMLineScanConfiguration()
{

}

AMScanConfiguration *SGMLineScanConfiguration::createCopy() const
{
	AMScanConfiguration *configuration = new SGMLineScanConfiguration(*this);
	configuration->dissociateFromDb(true);
	return configuration;
}

AMScanController *SGMLineScanConfiguration::createController()
{
	AMScanActionController *controller = new SGMLineScanController(this);
	controller->buildScanController();
	return controller;
}

AMScanConfigurationView *SGMLineScanConfiguration::createView()
{
	return new SGMLineScanConfigurationView(this, SGMBeamline::sgm()->hexapodControlSet(), AMBeamline::bl()->exposedScientificDetectors());
}

QString SGMLineScanConfiguration::technique() const
{
	return "Line Scan";
}

QString SGMLineScanConfiguration::description() const
{
	return "Continuous Spatial Line Scan";
}

QString SGMLineScanConfiguration::detailedDescription() const
{
	return "Continuous Spatial Line Scan";
}
