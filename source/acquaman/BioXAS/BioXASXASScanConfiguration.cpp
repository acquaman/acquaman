#include "BioXASXASScanConfiguration.h"

#include "acquaman/BioXAS/BioXASXASScanActionController.h"
#include "dataman/AMScanAxisEXAFSRegion.h"
#include "ui/BioXAS/BioXASXASScanConfigurationEditor.h"
#include "util/AMEnergyToKSpaceCalculator.h"
#include "util/AMAbsorptionEdge.h"

#include <QStringBuilder>

BioXASXASScanConfiguration::BioXASXASScanConfiguration(QObject *parent) :
	AMGenericStepScanConfiguration(parent), BioXASScanConfiguration()
{
	setName(description());
	setUserScanName(description());
}

BioXASXASScanConfiguration::BioXASXASScanConfiguration(const BioXASXASScanConfiguration &original) :
	AMGenericStepScanConfiguration(original), BioXASScanConfiguration()
{

}

BioXASXASScanConfiguration::~BioXASXASScanConfiguration()
{

}

QString BioXASXASScanConfiguration::headerText() const
{
	QString header("Configuration of the Scan\n\n");

	header.append("Scanned Edge:\t" + edge() + "\n");

	header.append(regionsOfInterestHeaderString(regionsOfInterest()) % "\n");
	header.append("\n");
	header.append("Regions Scanned\n");

	foreach (AMScanAxisRegion *region, scanAxisAt(0)->regions().toList()){

		AMScanAxisEXAFSRegion *exafsRegion = qobject_cast<AMScanAxisEXAFSRegion *>(region);

		if (exafsRegion->inKSpace() && (exafsRegion->maximumTime().isValid() || exafsRegion->maximumTime() == exafsRegion->regionTime()))
			header.append(QString("Start: %1 eV\tDelta: %2 k\tEnd: %3 k\tTime: %4 s\n")
						.arg(double(AMEnergyToKSpaceCalculator::energy(energy(), exafsRegion->regionStart())))
						.arg(double(exafsRegion->regionStep()))
						.arg(double(exafsRegion->regionEnd()))
						.arg(double(exafsRegion->regionTime())));

		else if (exafsRegion->inKSpace() && exafsRegion->maximumTime().isValid())
			header.append(QString("Start: %1 eV\tDelta: %2 k\tEnd: %3 k\tStart time: %4 s\tMaximum time (used with variable integration time): %5 s\n")
					  .arg(double(AMEnergyToKSpaceCalculator::energy(energy(), exafsRegion->regionStart())))
					  .arg(double(exafsRegion->regionStep()))
					  .arg(double(exafsRegion->regionEnd()))
					  .arg(double(exafsRegion->regionTime()))
					  .arg(double(exafsRegion->maximumTime())));

		else
			header.append(QString("Start: %1 eV\tDelta: %2 eV\tEnd: %3 eV\tTime: %4 s\n")
					  .arg(double(exafsRegion->regionStart()))
					  .arg(double(exafsRegion->regionStep()))
					  .arg(double(exafsRegion->regionEnd()))
					  .arg(double(exafsRegion->regionTime())));
	}

	return header;
}

AMScanConfiguration* BioXASXASScanConfiguration::createCopy() const
{
	AMScanConfiguration *configuration = new BioXASXASScanConfiguration(*this);
	configuration->dissociateFromDb(true);
	return configuration;
}

AMScanController* BioXASXASScanConfiguration::createController()
{
	AMScanActionController *controller = new BioXASXASScanActionController(this);
	controller->buildScanController();

	return controller;
}

AMScanConfigurationView* BioXASXASScanConfiguration::createView()
{
	return new BioXASXASScanConfigurationEditor(this);
}

void BioXASXASScanConfiguration::setControl(AMControlInfo newInfo)
{
	AMGenericStepScanConfiguration::setControl(0, newInfo);
}

void BioXASXASScanConfiguration::removeControl()
{
	AMGenericStepScanConfiguration::removeControl(0);
}

void BioXASXASScanConfiguration::onRegionAdded(AMScanAxisRegion *region)
{
	AMScanAxisEXAFSRegion *exafsRegion = qobject_cast<AMScanAxisEXAFSRegion *>(region);

	if (exafsRegion){

		connect(exafsRegion, SIGNAL(regionStartChanged(AMNumber)), this, SLOT(computeTotalTime()));
		connect(exafsRegion, SIGNAL(regionStepChanged(AMNumber)), this, SLOT(computeTotalTime()));
		connect(exafsRegion, SIGNAL(regionEndChanged(AMNumber)), this, SLOT(computeTotalTime()));
		connect(exafsRegion, SIGNAL(regionTimeChanged(AMNumber)), this, SLOT(computeTotalTime()));
		connect(exafsRegion, SIGNAL(maximumTimeChanged(AMNumber)), this, SLOT(computeTotalTime()));
		connect(exafsRegion, SIGNAL(equationChanged(AMVariableIntegrationTime::Equation)), this, SLOT(computeTotalTime()));
		computeTotalTime();
	}
}

void BioXASXASScanConfiguration::onRegionRemoved(AMScanAxisRegion *region)
{
	region->disconnect(this);
	computeTotalTime();
}
