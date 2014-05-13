#include "BioXASSideXASScanConfiguration.h"

#include "dataman/AMScanAxisRegion.h"
#include "acquaman/BioXASSide/BioXASSideXASScanActionController.h"
#include "ui/BioXAS/BioXASSideXASScanConfigurationView.h"

BioXASSideXASScanConfiguration::BioXASSideXASScanConfiguration(QObject *parent) :
    AMStepScanConfiguration(parent)
{
    setName("Unnamed Scan");
    setUserScanName("Unnamed Scan");

    AMScanAxisRegion *region = new AMScanAxisRegion();
    AMScanAxis *axis = new AMScanAxis(AMScanAxis::StepAxis, region);
    appendScanAxis(axis);
}

BioXASSideXASScanConfiguration::BioXASSideXASScanConfiguration(const BioXASSideXASScanConfiguration &original)
    : AMStepScanConfiguration(original)
{
    setName(original.name());
    setUserScanName(original.userScanName());
}

BioXASSideXASScanConfiguration::~BioXASSideXASScanConfiguration()
{

}

AMScanConfiguration* BioXASSideXASScanConfiguration::createCopy() const
{
    return new BioXASSideXASScanConfiguration(*this);
}

AMScanController* BioXASSideXASScanConfiguration::createController()
{
    AMScanActionController *controller = new BioXASSideXASScanActionController(this);
    controller->buildScanController();

    return controller;
}

AMScanConfigurationView* BioXASSideXASScanConfiguration::createView()
{
    return new BioXASSideXASScanConfigurationView(this);
}

QString BioXASSideXASScanConfiguration::detailedDescription() const
{
    return "BioXAS Side XAS Scan";
}
