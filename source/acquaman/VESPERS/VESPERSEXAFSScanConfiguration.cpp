#include "VESPERSEXAFSScanConfiguration.h"
#include "acquaman/VESPERS/VESPERSEXAFSDacqScanController.h"
#include "ui/VESPERS/VESPERSEXAFSScanConfigurationView.h"
#include "beamline/VESPERS/VESPERSBeamline.h"

VESPERSEXAFSScanConfiguration::VESPERSEXAFSScanConfiguration(QObject *parent)
	: AMEXAFSScanConfiguration(parent)
{
	ionChamberNames_.insert(Isplit, "Isplit");
	ionChamberNames_.insert(Iprekb, "Iprekb");
	ionChamberNames_.insert(Imini, "Imini");
	ionChamberNames_.insert(Ipost, "Ipost");

	regions_->setSensibleRange(-30, 40);
	exafsRegions()->setEnergyControl(VESPERSBeamline::vespers()->energyRelative());
	exafsRegions()->setTimeControl(VESPERSBeamline::vespers()->masterDwellTime());
	exafsRegions()->setKControl(VESPERSBeamline::vespers()->kControl());
	regions_->setDefaultUnits(" eV");
	regions_->setDefaultTimeUnits(" s");
	setName("XAS Scan");
	fluorescenceDetectorChoice_ = None;
	It_ = Ipost;
	I0_ = Imini;
	edge_ = "";
	energy_ = 0.0;

	goToPosition_ = false;
	position_ = qMakePair(0.0, 0.0);
}

VESPERSEXAFSScanConfiguration::VESPERSEXAFSScanConfiguration(const VESPERSEXAFSScanConfiguration &original)
	: AMEXAFSScanConfiguration(original.parent())
{
	ionChamberNames_.insert(Isplit, "Isplit");
	ionChamberNames_.insert(Iprekb, "Iprekb");
	ionChamberNames_.insert(Imini, "Imini");
	ionChamberNames_.insert(Ipost, "Ipost");

	regions_->setSensibleStart(original.regions()->sensibleStart());
	regions_->setSensibleEnd(original.regions()->sensibleEnd());
	exafsRegions()->setEnergyControl(VESPERSBeamline::vespers()->energyRelative());
	exafsRegions()->setTimeControl(VESPERSBeamline::vespers()->masterDwellTime());
	exafsRegions()->setKControl(VESPERSBeamline::vespers()->kControl());
	regions_->setDefaultUnits(original.regions()->defaultUnits());
	regions_->setDefaultTimeUnits(original.regions()->defaultTimeUnits());

	for (int i = 0; i < original.regionCount(); i++)
		regions_->addRegion(i, original.regionStart(i), original.regionDelta(i), original.regionEnd(i), original.regionTime(i));

	setName(original.name());
	fluorescenceDetectorChoice_ = original.fluorescenceDetectorChoice();
	It_ = original.transmissionChoice();
	I0_ = original.incomingChoice();
	edge_ = original.edge();
	energy_ = original.energy();

	goToPosition_ = original.goToPosition();
	position_ = original.position();

	roiInfoList_ = original.roiList();
}

AMScanConfiguration *VESPERSEXAFSScanConfiguration::createCopy() const
{
	return new VESPERSEXAFSScanConfiguration(*this);
}

AMScanController *VESPERSEXAFSScanConfiguration::createController()
{
	return new VESPERSEXAFSDacqScanController(this);
}

AMScanConfigurationView *VESPERSEXAFSScanConfiguration::createView()
{
	return new VESPERSEXAFSScanConfigurationView(this);
}

QString VESPERSEXAFSScanConfiguration::detailedDescription() const
{
	return QString("VESPERS XAS Scan");
}

QString VESPERSEXAFSScanConfiguration::readRoiList() const
{
	QString prettyRois = "Regions of Interest\n";

	for (int i = 0; i < roiInfoList_.count(); i++)
		prettyRois.append(roiInfoList_.at(i).name() + "\t" + QString::number(roiInfoList_.at(i).low()) + " eV\t" + QString::number(roiInfoList_.at(i).high()) + " eV\n");

	return prettyRois;
}
