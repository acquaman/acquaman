#include "VESPERSEXAFSScanConfiguration.h"
#include "acquaman/VESPERS/VESPERSEXAFSDacqScanController.h"
#include "ui/VESPERS/VESPERSEXAFSScanConfigurationView.h"
#include "beamline/VESPERS/VESPERSBeamline.h"

VESPERSEXAFSScanConfiguration::VESPERSEXAFSScanConfiguration(QObject *parent)
	: AMEXAFSScanConfiguration(parent)
{
	regions_->setSensibleRange(-30, 40);
	exafsRegions()->setDefaultEdgeEnergy(0); // Everything for XAS related scans on VESPERS is done using relative energy to the edge on a PV level.
	exafsRegions()->setEnergyControl(VESPERSBeamline::vespers()->energyRelative());
	exafsRegions()->setTimeControl(VESPERSBeamline::vespers()->masterDwellTime());
	exafsRegions()->setKControl(VESPERSBeamline::vespers()->kControl());
	exafsRegions()->setDefaultIsRelative(true);
	regions_->setDefaultUnits(" eV");
	regions_->setDefaultTimeUnits(" s");
	setName("XAS Scan");
	fluorescenceDetectorChoice_ = None;
	It_ = Ipost;
	I0_ = Imini;
	edge_ = "";
	energy_ = 0.0;
	useFixedTime_ = false;
	numberOfScans_ = 1;

	roiInfoList_ = AMROIInfoList();

	goToPosition_ = false;
	position_ = qMakePair(0.0, 0.0);
	totalTime_ = 0;
	timeOffset_ = 0.7;
	connect(regions_, SIGNAL(regionsChanged()), this, SLOT(computeTotalTime()));
	connect(regions_, SIGNAL(regionsChanged()), this, SLOT(onEXAFSRegionsChanged()));
	connect(VESPERSBeamline::vespers()->variableIntegrationTime(), SIGNAL(a0Changed(double)), this, SLOT(computeTotalTime()));
	connect(VESPERSBeamline::vespers()->variableIntegrationTime(), SIGNAL(a1Changed(double)), this, SLOT(computeTotalTime()));
	connect(VESPERSBeamline::vespers()->variableIntegrationTime(), SIGNAL(a2Changed(double)), this, SLOT(computeTotalTime()));
}

VESPERSEXAFSScanConfiguration::VESPERSEXAFSScanConfiguration(const VESPERSEXAFSScanConfiguration &original)
	: AMEXAFSScanConfiguration(original)
{
	regions_->setSensibleStart(original.regions()->sensibleStart());
	regions_->setSensibleEnd(original.regions()->sensibleEnd());
	exafsRegions()->setDefaultEdgeEnergy(original.exafsRegions()->defaultEdgeEnergy());
	exafsRegions()->setEnergyControl(VESPERSBeamline::vespers()->energyRelative());
	exafsRegions()->setTimeControl(VESPERSBeamline::vespers()->masterDwellTime());
	exafsRegions()->setKControl(VESPERSBeamline::vespers()->kControl());
	regions_->setDefaultUnits(original.regions()->defaultUnits());
	regions_->setDefaultTimeUnits(original.regions()->defaultTimeUnits());
	exafsRegions()->setDefaultIsRelative(original.exafsRegions()->defaultIsRelative());

	for (int i = 0; i < original.regionCount(); i++){

		// Because I store the values in energy space, I need to ask for them explicitly with no converstion.  Otherwise, the k-space values will be converted twice.
		regions_->addRegion(i, original.regionStartByType(i, AMEXAFSRegion::Energy), original.regionDelta(i), original.regionEndByType(i, AMEXAFSRegion::Energy), original.regionTime(i));
		exafsRegions()->setType(i, original.regionType(i));
		exafsRegions()->setEdgeEnergy(i, original.regionEdgeEnergy(i));
	}

	setName(original.name());
	fluorescenceDetectorChoice_ = original.fluorescenceDetectorChoice();
	It_ = original.transmissionChoice();
	I0_ = original.incomingChoice();
	edge_ = original.edge();
	energy_ = original.energy();
	useFixedTime_ = original.useFixedTime();
	numberOfScans_ = original.numberOfScans();

	goToPosition_ = original.goToPosition();
	position_ = original.position();

	roiInfoList_ = original.roiList();

	timeOffset_ = 0.7;
	totalTime_ = 0;
	computeTotalTime();
	connect(regions_, SIGNAL(regionsChanged()), this, SLOT(computeTotalTime()));
	connect(regions_, SIGNAL(regionsChanged()), this, SLOT(onEXAFSRegionsChanged()));
	connect(VESPERSBeamline::vespers()->variableIntegrationTime(), SIGNAL(a0Changed(double)), this, SLOT(computeTotalTime()));
	connect(VESPERSBeamline::vespers()->variableIntegrationTime(), SIGNAL(a1Changed(double)), this, SLOT(computeTotalTime()));
	connect(VESPERSBeamline::vespers()->variableIntegrationTime(), SIGNAL(a2Changed(double)), this, SLOT(computeTotalTime()));
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
	return exafsRegions()->hasKSpace() ? QString("VESPERS EXAFS Scan") : QString("VESPERS XANES Scan");
}

QString VESPERSEXAFSScanConfiguration::readRoiList() const
{
	QString prettyRois = "Regions of Interest\n";

	for (int i = 0; i < roiInfoList_.count(); i++)
		prettyRois.append(roiInfoList_.at(i).name() + "\t" + QString::number(roiInfoList_.at(i).low()) + " eV\t" + QString::number(roiInfoList_.at(i).high()) + " eV\n");

	return prettyRois;
}

void VESPERSEXAFSScanConfiguration::onEXAFSRegionsChanged()
{
	if (exafsRegions()->hasKSpace()){

		CLSVariableIntegrationTime *timeApp = VESPERSBeamline::vespers()->variableIntegrationTime();
		bool needToCompute = false;
		int lastVal = exafsRegions()->count()-1;

		if (exafsRegions()->count() > 1 && (exafsRegions()->time(lastVal-1) != timeApp->defautTime())){

			timeApp->setDefaultTime(exafsRegions()->time(lastVal-1));
			needToCompute = true;
		}

		if (exafsRegions()->startByType(lastVal, AMEXAFSRegion::kSpace) != timeApp->lowValue()){

			timeApp->setLowValue(exafsRegions()->startByType(lastVal, AMEXAFSRegion::kSpace));
			needToCompute = true;
		}

		if (exafsRegions()->endByType(lastVal, AMEXAFSRegion::kSpace) != timeApp->highValue()){

			timeApp->setHighValue(exafsRegions()->endByType(lastVal, AMEXAFSRegion::kSpace));
			needToCompute = true;
		}

		if (exafsRegions()->time(lastVal) != timeApp->maximumTime()){

			timeApp->setMaximumTime(exafsRegions()->time(lastVal));
			needToCompute = true;
		}

		if (needToCompute)
			timeApp->compute();
	}
}

void VESPERSEXAFSScanConfiguration::computeTotalTime()
{
	double time = 0;

	if (exafsRegions()->hasKSpace() && !useFixedTime_){

		for (int i = 0; i < regions_->count(); i++){

			if (exafsRegions()->type(i) == AMEXAFSRegion::kSpace)
				time += VESPERSBeamline::vespers()->variableIntegrationTime()->totalTime(regions_->delta(i)) + ((regions_->end(i) - regions_->start(i))/regions_->delta(i))*timeOffset_;

			else
				time += ((regions_->end(i) - regions_->start(i))/regions_->delta(i))*(regions_->time(i) + timeOffset_); // Seems to take about 0.7 seconds for extra beamline stuff to happen.
		}
	}

	else{

		for (int i = 0; i < regions_->count(); i++)
			time += ((regions_->end(i) - regions_->start(i))/regions_->delta(i))*(regions_->time(i) + timeOffset_); // Seems to take about 0.7 seconds for extra beamline stuff to happen.
	}

	totalTime_ = time + 9; // There is a 9 second miscellaneous startup delay.
	emit totalTimeChanged(totalTime_);
}
