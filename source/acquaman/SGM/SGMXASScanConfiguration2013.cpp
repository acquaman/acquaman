#include "SGMXASScanConfiguration2013.h"

#include "beamline/AMBeamline.h"
#include "beamline/SGM/SGMBeamline.h"

 SGMXASScanConfiguration2013::~SGMXASScanConfiguration2013(){}
SGMXASScanConfiguration2013::SGMXASScanConfiguration2013(QObject *parent) :
	AMXASScanConfiguration(parent), SGMScanConfiguration2013()
{
	AMXASRegionsList *castToXASRegionsList = qobject_cast<AMXASRegionsList*>(regions_);
	if(castToXASRegionsList)
		castToXASRegionsList->setEnergyControl(AMBeamline::bl()->exposedControlByName("energy"));
	regions_->setDefaultTimeControl(AMBeamline::bl()->exposedControlByName("masterDwell"));

	regions_->setSensibleRange(200, 2000);
	regions_->setDefaultUnits(" eV");
	regions_->setDefaultTimeUnits(" s");
	connect(SGMBeamline::sgm()->exitSlitGap(), SIGNAL(valueChanged(double)), this, SLOT(checkIfMatchesBeamline()));
	connect(SGMBeamline::sgm()->grating(), SIGNAL(valueChanged(double)), this, SLOT(checkIfMatchesBeamline()));
	connect(SGMBeamline::sgm()->harmonic(), SIGNAL(valueChanged(double)), this, SLOT(checkIfMatchesBeamline()));
	connect(SGMBeamline::sgm()->undulatorTracking(), SIGNAL(valueChanged(double)), this, SLOT(checkIfMatchesBeamline()));
	connect(SGMBeamline::sgm()->monoTracking(), SIGNAL(valueChanged(double)), this, SLOT(checkIfMatchesBeamline()));
	connect(SGMBeamline::sgm()->exitSlitTracking(), SIGNAL(valueChanged(double)), this,SLOT(checkIfMatchesBeamline()));
	connect(this, SIGNAL(configurationChanged()), this, SLOT(checkIfMatchesBeamline()));

	//detectorConfigurations_ = AMBeamline::bl()->exposedDetectors()->toInfoSet();
}

SGMXASScanConfiguration2013::SGMXASScanConfiguration2013(const SGMXASScanConfiguration2013 &original) :
	AMXASScanConfiguration(original), SGMScanConfiguration2013(original)
{
	AMXASRegionsList *castToXASRegionsList = qobject_cast<AMXASRegionsList*>(regions_);
	if(castToXASRegionsList)
		castToXASRegionsList->setEnergyControl(AMBeamline::bl()->exposedControlByName("energy"));
	regions_->setDefaultTimeControl(AMBeamline::bl()->exposedControlByName("masterDwell"));

	regions_->setSensibleStart(original.regions()->sensibleStart());
	regions_->setSensibleEnd(original.regions()->sensibleEnd());
	regions_->setDefaultUnits(original.regions()->defaultUnits());
	regions_->setDefaultTimeUnits(original.regions()->defaultTimeUnits());

	for(int x = 0; x < original.regionCount(); x++)
		regions_->addRegion(x, original.regionStart(x), original.regionDelta(x), original.regionEnd(x), original.regionTime(x));

	detectorConfigurations_ = original.detectorConfigurations();
	setTrackingGroup(original.trackingGroup());
	setFluxResolutionGroup(original.fluxResolutionGroup());
	connect(SGMBeamline::sgm()->exitSlitGap(), SIGNAL(valueChanged(double)), this, SLOT(checkIfMatchesBeamline()));
	connect(SGMBeamline::sgm()->grating(), SIGNAL(valueChanged(double)), this, SLOT(checkIfMatchesBeamline()));
	connect(SGMBeamline::sgm()->harmonic(), SIGNAL(valueChanged(double)), this, SLOT(checkIfMatchesBeamline()));
	connect(SGMBeamline::sgm()->undulatorTracking(), SIGNAL(valueChanged(double)), this, SLOT(checkIfMatchesBeamline()));
	connect(SGMBeamline::sgm()->monoTracking(), SIGNAL(valueChanged(double)), this, SLOT(checkIfMatchesBeamline()));
	connect(SGMBeamline::sgm()->exitSlitTracking(), SIGNAL(valueChanged(double)), this,SLOT(checkIfMatchesBeamline()));
	connect(this, SIGNAL(configurationChanged()), this, SLOT(checkIfMatchesBeamline()));
}

AMScanConfiguration* SGMXASScanConfiguration2013::createCopy() const{
	return new SGMXASScanConfiguration2013(*this);
}

#include "SGMXASScanActionController.h"

AMScanController* SGMXASScanConfiguration2013::createController()
{
	SGMXASScanActionController *controller = new SGMXASScanActionController(this);
	controller->buildScanController();

	return controller;
}

#include "ui/SGM/SGMXASScanConfiguration2013View.h"

AMScanConfigurationView* SGMXASScanConfiguration2013::createView(){
	return new SGMXASScanConfiguration2013View(this);
}

QString SGMXASScanConfiguration2013::detailedDescription() const{
	return QString("XAS Scan from %1 to %2\nExit Slit: %3\nGrating: %4\nHarmonic: %5").arg(regionStart(0)).arg(regionEnd(regionCount()-1)).arg(exitSlitGap(), 0, 'f', 1).arg(SGMBeamlineInfo::sgmInfo()->sgmGratingDescription(SGMBeamlineInfo::sgmGrating(grating()))).arg(SGMBeamlineInfo::sgmInfo()->sgmHarmonicDescription(SGMBeamlineInfo::sgmHarmonic(harmonic())));
}

void SGMXASScanConfiguration2013::checkIfMatchesBeamline()
{
	bool currentMatchStatus;
	if(SGMBeamline::sgm()->isConnected())
	{
		currentMatchStatus =
			(qFuzzyCompare(dbObject()->exitSlitGap() + 1.0e-200, SGMBeamline::sgm()->exitSlitGap()->value() + 1.0e-200) &&
			 qFuzzyCompare(dbObject()->grating() + 1.0e-200, SGMBeamline::sgm()->grating()->value()  + 1.0e-200) &&
			 qFuzzyCompare(dbObject()->harmonic()  + 1.0e-200, SGMBeamline::sgm()->harmonic()->value()  + 1.0e-200) &&
			 qFuzzyCompare(dbObject()->undulatorTracking()  + 1.0e-200, SGMBeamline::sgm()->undulatorTracking()->value()  + 1.0e-200) &&
			 qFuzzyCompare(dbObject()->monoTracking()  + 1.0e-200, SGMBeamline::sgm()->monoTracking()->value()  + 1.0e-200) &&
			 qFuzzyCompare(dbObject()->exitSlitTracking()  + 1.0e-200, SGMBeamline::sgm()->exitSlitTracking()->value()  + 1.0e-200));
	}
	else
	{
		currentMatchStatus = false;
	}

	if(currentMatchStatus != this->matchesCurrentBeamline_)
	{
		matchesCurrentBeamline_ = currentMatchStatus;
		emit this->matchingBeamlineStatusChanged(currentMatchStatus);
	}
}
