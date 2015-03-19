#include "BioXASMainXASScanConfiguration.h"

#include "dataman/AMScanAxisEXAFSRegion.h"
#include "acquaman/BioXAS/BioXASMainXASScanActionController.h"
#include "ui/BioXAS/BioXASMainXASScanConfigurationView.h"

BioXASMainXASScanConfiguration::BioXASMainXASScanConfiguration(QObject *parent) : AMStepScanConfiguration(parent)
{
    setName("Unnamed Scan");
    setUserScanName("Unnamed Scan");

    edgeEnergy_ = -1;

    AMScanAxisEXAFSRegion *region = new AMScanAxisEXAFSRegion;
    AMScanAxis *axis = new AMScanAxis(AMScanAxis::StepAxis, region);
    appendScanAxis(axis);
}

BioXASMainXASScanConfiguration::BioXASMainXASScanConfiguration(const BioXASMainXASScanConfiguration &original)
    : AMStepScanConfiguration(original)
{
    setName(original.name());
    setUserScanName(original.name());

    edgeEnergy_ = original.edgeEnergy();
}

BioXASMainXASScanConfiguration::~BioXASMainXASScanConfiguration()
{

}

AMScanConfiguration* BioXASMainXASScanConfiguration::createCopy() const
{
    AMScanConfiguration *configuration = new BioXASMainXASScanConfiguration(*this);
    configuration->dissociateFromDb(true);
    return configuration;
}

AMScanController* BioXASMainXASScanConfiguration::createController()
{
    AMScanActionController *controller = new BioXASMainXASScanActionController(this);
    controller->buildScanController();

    return controller;
}

AMScanConfigurationView* BioXASMainXASScanConfiguration::createView()
{
    return new BioXASMainXASScanConfigurationView(this);
}

QString BioXASMainXASScanConfiguration::detailedDescription() const
{
    return "BioXAS Side XAS Scan";
}

void BioXASMainXASScanConfiguration::setEdgeEnergy(double edgeEnergy){
    if(edgeEnergy_ != edgeEnergy){
        edgeEnergy_ = edgeEnergy;
        emit edgeEnergyChanged(edgeEnergy_);
        setModified(true);
    }
}

void BioXASMainXASScanConfiguration::setEdge(QString edgeName)
{
    if (edge_ != edgeName){
        edge_ = edgeName;
        emit edgeChanged(edgeName);
        setModified(true);
    }
}
