#include "SXRMBEXAFSScanConfiguration.h"

#include "dataman/AMScanAxisEXAFSRegion.h"
#include "acquaman/SXRMB/SXRMBEXAFSScanActionController.h"
#include "ui/SXRMB/SXRMBEXAFSScanConfigurationView.h"

SXRMBEXAFSScanConfiguration::SXRMBEXAFSScanConfiguration(QObject *parent) :
	AMStepScanConfiguration(parent)
{
	setName("Unnamed Scan");
	setUserScanName("Unnamed Scan");

	edgeEnergy_ = -1;

	AMScanAxisEXAFSRegion *region = new AMScanAxisEXAFSRegion;
	AMScanAxis *axis = new AMScanAxis(AMScanAxis::StepAxis, region);
	appendScanAxis(axis);
}

SXRMBEXAFSScanConfiguration::SXRMBEXAFSScanConfiguration(const SXRMBEXAFSScanConfiguration &original)
	: AMStepScanConfiguration(original)
{
	setName(original.name());
	setUserScanName(original.userScanName());

	edgeEnergy_ = original.edgeEnergy();
}

SXRMBEXAFSScanConfiguration::~SXRMBEXAFSScanConfiguration()
{

}

AMScanConfiguration* SXRMBEXAFSScanConfiguration::createCopy() const
{
	return new SXRMBEXAFSScanConfiguration(*this);
}

AMScanController* SXRMBEXAFSScanConfiguration::createController()
{
	AMScanActionController *controller = new SXRMBEXAFSScanActionController(this);
	controller->buildScanController();

	return controller;
}

AMScanConfigurationView* SXRMBEXAFSScanConfiguration::createView()
{
	return new SXRMBEXAFSScanConfigurationView(this);
}

QString SXRMBEXAFSScanConfiguration::detailedDescription() const
{
	return "SXRMB EXAFS Scan";
}

double SXRMBEXAFSScanConfiguration::edgeEnergy() const
{
	return edgeEnergy_;
}

void SXRMBEXAFSScanConfiguration::setEdgeEnergy(double edgeEnergy){
	if(edgeEnergy_ != edgeEnergy){
		edgeEnergy_ = edgeEnergy;
		emit edgeEnergyChanged(edgeEnergy_);
		setModified(true);
	}
}

void SXRMBEXAFSScanConfiguration::setEdge(QString edgeName)
{
	if (edge_ != edgeName){

		edge_ = edgeName;
		emit edgeChanged(edgeName);
		setModified(true);
	}
}
