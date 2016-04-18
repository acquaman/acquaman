#include "BioXASXASScanConfiguration.h"

#include <QStringBuilder>

#include "acquaman/BioXAS/BioXASXASScanActionController.h"
#include "beamline/BioXAS/BioXASBeamline.h"
#include "dataman/AMScanAxisEXAFSRegion.h"
#include "ui/BioXAS/BioXASXASScanConfigurationEditor.h"
#include "util/AMEnergyToKSpaceCalculator.h"
#include "util/AMElement.h"
#include "util/AMAbsorptionEdge.h"

BioXASXASScanConfiguration::BioXASXASScanConfiguration(QObject *parent) :
	AMGenericStepScanConfiguration(parent), BioXASScanConfiguration()
{
	setName(description());
	setUserScanName(description());
}

BioXASXASScanConfiguration::BioXASXASScanConfiguration(const BioXASXASScanConfiguration &original) :
	AMGenericStepScanConfiguration(original), BioXASScanConfiguration(original)
{
	setEdge(original.edge());
	setEnergy(original.energy());
}

BioXASXASScanConfiguration::~BioXASXASScanConfiguration()
{

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

bool BioXASXASScanConfiguration::hasXRFDetector() const
{
	bool detectorFound = false;

	for (int i = 0, count = detectorConfigurations_.count(); i < count && !detectorFound; i++) {
		AMDetector *detector = BioXASBeamline::bioXAS()->exposedDetectorByInfo(detectorConfigurations().detectorInfoAt(i));

		if (qobject_cast<AMXRFDetector*>(detector))
			detectorFound = true;
	}

	return detectorFound;
}

void BioXASXASScanConfiguration::clearRegions()
{
	for (int axisIndex = 0, axisCount = scanAxes_.count(); axisIndex < axisCount; axisIndex++) {
		AMScanAxis *scanAxis = scanAxisAt(axisIndex);

		if (scanAxis)
			foreach (AMScanAxisRegion *region, scanAxis->regions().toList())
				removeRegion(axisIndex, region);
	}
}

void BioXASXASScanConfiguration::setupDefaultXANESRegions()
{
	// Clear previous regions.

	clearRegions();

	// Create new region and add to the list of regions for the first scan axis.

	if (scanAxisAt(0)) {
		AMScanAxisEXAFSRegion *region = createDefaultXANESRegion(energy());
		addRegion(0, 0, region);
	}
}

void BioXASXASScanConfiguration::setupDefaultEXAFSRegions()
{
	// Clear previous regions.

	clearRegions();

	// Create new regions and add to the list of regions for the first scan axis.

	if (scanAxisAt(0)) {

		AMScanAxisEXAFSRegion *newRegion;
		double edgeEnergy = energy();

		newRegion = createEXAFSRegion(edgeEnergy, edgeEnergy - 200, 10, edgeEnergy - 30, 1.0);
		addRegion(0, 0, newRegion);

		newRegion = createEXAFSRegion(edgeEnergy, edgeEnergy - 30, 0.5, edgeEnergy + 40, 1.0);
		addRegion(0, 1, newRegion);

		newRegion = createEXAFSRegionInKSpace(edgeEnergy, AMEnergyToKSpaceCalculator::k(edgeEnergy, edgeEnergy + 40), 0.05, 10, 1.0, 10.0);
		addRegion(0, 2, newRegion);
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

QString BioXASXASScanConfiguration::headerText() const
{
	QString header("Configuration of the Scan\n\n");

	header.append("Scanned Edge:\t" + edge() + "\n");

	header.append(regionsOfInterestHeaderString(regionsOfInterest()) % "\n");
	header.append("\n");
	header.append("Regions Scanned\n");

	foreach (AMScanAxisRegion *region, scanAxisAt(0)->regions().toList()){
		header.append(region->toString());
	}

	return header;
}
