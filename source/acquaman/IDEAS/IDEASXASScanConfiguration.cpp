#include "IDEASXASScanConfiguration.h"

#include "beamline/AMBeamline.h"
#include "acquaman/IDEAS/IDEASXASScanActionController.h"
#include "ui/IDEAS/IDEASXASScanConfigurationView.h"

IDEASXASScanConfiguration::IDEASXASScanConfiguration(QObject *parent) :
	AMXASScanConfiguration(parent)
{
	AMXASRegionsList *castToXASRegionsList = qobject_cast<AMXASRegionsList*>(regions_);
	if(castToXASRegionsList)
		castToXASRegionsList->setEnergyControl(AMBeamline::bl()->exposedControlByName("Energy"));
	regions_->setDefaultTimeControl(AMBeamline::bl()->exposedControlByName("masterDwell"));

	regions_->setSensibleRange(2000, 12000);
	regions_->setDefaultUnits(" eV");
	regions_->setDefaultTimeUnits(" s");
}

IDEASXASScanConfiguration::IDEASXASScanConfiguration(const IDEASXASScanConfiguration &original) :
	AMXASScanConfiguration(original)
{
	AMXASRegionsList *castToXASRegionsList = qobject_cast<AMXASRegionsList*>(regions_);
	if(castToXASRegionsList)
		castToXASRegionsList->setEnergyControl(AMBeamline::bl()->exposedControlByName("Energy"));
	regions_->setDefaultTimeControl(AMBeamline::bl()->exposedControlByName("masterDwell"));

	regions_->setSensibleStart(original.regions()->sensibleStart());
	regions_->setSensibleEnd(original.regions()->sensibleEnd());
	regions_->setDefaultUnits(original.regions()->defaultUnits());
	regions_->setDefaultTimeUnits(original.regions()->defaultTimeUnits());

	for(int x = 0; x < original.regionCount(); x++)
		regions_->addRegion(x, original.regionStart(x), original.regionDelta(x), original.regionEnd(x), original.regionTime(x));
}

IDEASXASScanConfiguration::~IDEASXASScanConfiguration()
{

}

AMScanConfiguration* IDEASXASScanConfiguration::createCopy() const{
	return new IDEASXASScanConfiguration(*this);
}

AMScanController* IDEASXASScanConfiguration::createController()
{
	IDEASXASScanActionController *controller = new IDEASXASScanActionController(this);
	controller->buildScanController();

	return controller;
}

AMScanConfigurationView* IDEASXASScanConfiguration::createView(){
	return new IDEASXASScanConfigurationView(this);
}

QString IDEASXASScanConfiguration::detailedDescription() const{
	return QString("XAS Scan from %1 to %2").arg(regionStart(0)).arg(regionEnd(regionCount()-1));
}
