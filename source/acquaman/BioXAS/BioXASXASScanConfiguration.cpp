#include "BioXASXASScanConfiguration.h"

#include "acquaman/BioXAS/BioXASXASScanActionController.h"
#include "beamline/BioXAS/BioXASBeamline.h"
#include "dataman/AMScanAxisEXAFSRegion.h"
#include "ui/BioXAS/BioXASXASScanConfigurationEditor.h"
#include "util/AMEnergyToKSpaceCalculator.h"
#include "util/AMPeriodicTable.h"
#include "util/AMElement.h"
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
		header.append( regionToString(region) );
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
	setupDefaultXANESRegions();
}

void BioXASXASScanConfiguration::removeControl()
{
	AMGenericStepScanConfiguration::removeControl(0);
}

void BioXASXASScanConfiguration::clearRegions()
{
	qDebug() << "Clearing all regions...";

	bool removeOK = true;

	foreach (AMScanAxis *scanAxis, scanAxes()) {
		if (scanAxis)
			removeOK = removeOK && scanAxis->clearRegions();
	}

	if (removeOK)
		qDebug() << "Successfully cleared all regions.";
	else
		qDebug() << "Clearing all regions was NOT successful.";
}

void BioXASXASScanConfiguration::setupDefaultXANESRegions()
{
	qDebug() << "Setting up default XANES regions...";

	// Clear previous regions.

	clearRegions();

	// Create new region and add to the list of regions for the first scan axis.

	if (scanAxisAt(0)) {

		qDebug() << "Adding regions to scan axis.";

		AMScanAxisEXAFSRegion *region = createDefaultXANESRegion(energy());
		scanAxisAt(0)->insertRegion(0, region);

	} else {
		qDebug() << "Scan axis is NOT valid.";
	}

	qDebug() << "Setting up default XANES regions complete.";
}

void BioXASXASScanConfiguration::setupDefaultEXAFSRegions()
{
	qDebug() << "Setting up default EXAFS regions...";

	// Clear previous regions.

	clearRegions();

	// Create new regions and add to the list of regions for the first scan axis.

	if (scanAxisAt(0)) {

		qDebug() << "Adding regions to scan axis...";

		AMScanAxisEXAFSRegion *newRegion;
		double edgeEnergy = energy();

		newRegion = createEXAFSRegion(edgeEnergy, edgeEnergy - 200, 10, edgeEnergy - 30, 1.0);
		scanAxisAt(0)->insertRegion(0, newRegion);

		newRegion = createEXAFSRegion(edgeEnergy, edgeEnergy - 30, 0.5, edgeEnergy + 40, 1.0);
		scanAxisAt(0)->insertRegion(1, newRegion);

		newRegion = createEXAFSRegionInKSpace(edgeEnergy, AMEnergyToKSpaceCalculator::k(edgeEnergy, edgeEnergy + 40), 0.05, 10, 1.0, 10.0);
		scanAxisAt(0)->insertRegion(2, newRegion);

	} else {
		qDebug() << "Scan axis is NOT valid.";
	}

	qDebug() << "Setting up EXAFS regions complete.";
}

void BioXASXASScanConfiguration::setupDefaultEdge()
{
	AMElement *defaultElement = AMPeriodicTable::table()->elementBySymbol("Cu");
	QList<AMAbsorptionEdge> edges = defaultElement->absorptionEdges();

	if (!edges.isEmpty()) {
		AMAbsorptionEdge defaultEdge = edges.first();
		double defaultEnergy = defaultEdge.energy();

		setEdge(edgeToString(defaultEdge));
		setEnergy(defaultEnergy);
	}
}

AMScanAxisEXAFSRegion* BioXASXASScanConfiguration::createDefaultXANESRegion(double edgeEnergy)
{
	return createXANESRegion(edgeEnergy, edgeEnergy - 30, 0.5, edgeEnergy + 40, 1.0);
}

AMScanAxisEXAFSRegion* BioXASXASScanConfiguration::createXANESRegion(double edgeEnergy, double regionStart, double regionStep, double regionEnd, double regionTime) const
{
	AMScanAxisEXAFSRegion *region = new AMScanAxisEXAFSRegion;
	region->setEdgeEnergy(edgeEnergy);
	region->setRegionStart(regionStart);
	region->setRegionStep(regionStep);
	region->setRegionEnd(regionEnd);
	region->setRegionTime(regionTime);

	return region;
}

AMScanAxisEXAFSRegion* BioXASXASScanConfiguration::createEXAFSRegion(double edgeEnergy, double regionStart, double regionStep, double regionEnd, double regionTime) const
{
	AMScanAxisEXAFSRegion *region = new AMScanAxisEXAFSRegion;
	region->setEdgeEnergy(edgeEnergy);
	region->setRegionStart(regionStart);
	region->setRegionStep(regionStep);
	region->setRegionEnd(regionEnd);
	region->setRegionTime(regionTime);

	return region;
}

AMScanAxisEXAFSRegion* BioXASXASScanConfiguration::createEXAFSRegionInKSpace(double edgeEnergy, double regionStart, double regionStep, double regionEnd, double regionTime, double maximumTime) const
{
	AMScanAxisEXAFSRegion *region = new AMScanAxisEXAFSRegion;
	region->setEdgeEnergy(edgeEnergy);
	region->setInKSpace(true);
	region->setRegionStart(regionStart);
	region->setRegionStep(regionStep);
	region->setRegionEnd(regionEnd);
	region->setRegionTime(regionTime);
	region->setMaximumTime(maximumTime);

	return region;
}

QString BioXASXASScanConfiguration::regionToString(AMScanAxisRegion *region) const
{
	QString result;

	if (region) {

		AMScanAxisEXAFSRegion *exafsRegion = qobject_cast<AMScanAxisEXAFSRegion*>(region);
		if (exafsRegion && exafsRegion->inKSpace() && exafsRegion->maximumTime().isValid()) {
			result = QString("Start: %1 eV\tDelta: %2 k\tEnd: %3 k\tStart time: %4 s\tMaximum time (used with variable integration time): %5 s\n")
											  .arg(double(AMEnergyToKSpaceCalculator::energy(energy(), exafsRegion->regionStart())))
											  .arg(double(exafsRegion->regionStep()))
											  .arg(double(exafsRegion->regionEnd()))
											  .arg(double(exafsRegion->regionTime()))
											  .arg(double(exafsRegion->maximumTime()));
		} else {
			result = QString("Start: %1 eV\tDelta: %2 k\tEnd: %3 k\tTime: %4 s\n")
											  .arg(double(AMEnergyToKSpaceCalculator::energy(energy(), exafsRegion->regionStart())))
											  .arg(double(exafsRegion->regionStep()))
											  .arg(double(exafsRegion->regionEnd()))
											  .arg(double(exafsRegion->regionTime()));
		}

	}

	return result;
}

QString BioXASXASScanConfiguration::edgeToString(const AMAbsorptionEdge &edge) const
{
	QString result;

	if (!edge.isNull())
		result = edge.name() + ": " + edge.energyString() + " eV";

	return result;
}

