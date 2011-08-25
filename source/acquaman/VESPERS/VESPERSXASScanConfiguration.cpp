#include "VESPERSXASScanConfiguration.h"
#include "acquaman/VESPERS/VESPERSXASDacqScanController.h"
#include "ui/VESPERS/VESPERSXASScanConfigurationView.h"
#include "beamline/VESPERS/VESPERSBeamline.h"

VESPERSXASScanConfiguration::VESPERSXASScanConfiguration(QObject *parent)
	: AMXASScanConfiguration(parent)
{
	ionChamberNames_.insert(Isplit, "Isplit");
	ionChamberNames_.insert(Iprekb, "Iprekb");
	ionChamberNames_.insert(Imini, "Imini");
	ionChamberNames_.insert(Ipost, "Ipost");

	regions_->setEnergyControl(VESPERSBeamline::vespers()->energyRelative());
	fluorescenceDetectorChoice_ = FourElement;
	It_ = Ipost;
	I0_ = Imini;
}

VESPERSXASScanConfiguration::VESPERSXASScanConfiguration(const VESPERSXASScanConfiguration &original)
	: AMXASScanConfiguration(original.parent())
{
	ionChamberNames_.insert(Isplit, "Isplit");
	ionChamberNames_.insert(Iprekb, "Iprekb");
	ionChamberNames_.insert(Imini, "Imini");
	ionChamberNames_.insert(Ipost, "Ipost");

	regions_->setEnergyControl(VESPERSBeamline::vespers()->energyRelative());
	for (int i = 0; i < original.regionCount(); i++)
		regions_->addRegion(i, original.regionStart(i), original.regionDelta(i), original.regionEnd(i));

	setName(original.name());
	fluorescenceDetectorChoice_ = original.fluorescenceDetectorChoice();
	It_ = original.transmissionChoice();
	I0_ = original.incomingChoice();
}

AMScanConfiguration *VESPERSXASScanConfiguration::createCopy() const
{
	return new VESPERSXASScanConfiguration(*this);
}

AMScanController *VESPERSXASScanConfiguration::createController()
{
	return new VESPERSXASDacqScanController(this);
}

AMScanConfigurationView *VESPERSXASScanConfiguration::createView()
{
	return new VESPERSXASScanConfigurationView(this);
}

QString VESPERSXASScanConfiguration::detailedDescription() const
{
	return QString("VESPERS XAS Scan");
}
