#include "SXRMBEXAFSScanConfiguration.h"

#include "dataman/AMScanAxisEXAFSRegion.h"
#include "acquaman/SXRMB/SXRMBEXAFSScanActionController.h"
#include "ui/SXRMB/SXRMBEXAFSScanConfigurationView.h"
#include "util/AMEnergyToKSpaceCalculator.h"

#include <QStringBuilder>

SXRMBEXAFSScanConfiguration::SXRMBEXAFSScanConfiguration(QObject *parent) :
	AMStepScanConfiguration(parent), SXRMBScanConfiguration()
{
	timeOffset_ = 1.333;

    setName("EXAFS Scan");
    setUserScanName("EXAFS Scan");

	edgeEnergy_ = -1;
	edge_ = "";

	microprobeSampleStageX_ = 0.0;
	microprobeSampleStageZ_ = 0.0;
    setEnableBrukerDetector(false);

	AMScanAxisEXAFSRegion *region = new AMScanAxisEXAFSRegion;
	AMScanAxis *axis = new AMScanAxis(AMScanAxis::StepAxis, region);
	appendScanAxis(axis);

	connect(axis, SIGNAL(regionAdded(AMScanAxisRegion*)), this, SLOT(onRegionAdded(AMScanAxisRegion*)));
	connect(axis, SIGNAL(regionRemoved(AMScanAxisRegion*)), this, SLOT(onRegionRemoved(AMScanAxisRegion*)));
	connect(region, SIGNAL(regionStartChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(region, SIGNAL(regionStepChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(region, SIGNAL(regionEndChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(region, SIGNAL(regionTimeChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(region, SIGNAL(maximumTimeChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(region, SIGNAL(equationChanged(AMVariableIntegrationTime::Equation)), this, SLOT(computeTotalTime()));
}

SXRMBEXAFSScanConfiguration::SXRMBEXAFSScanConfiguration(const SXRMBEXAFSScanConfiguration &original)
	: AMStepScanConfiguration(original), SXRMBScanConfiguration(original)
{
	setName(original.name());
	setUserScanName(original.userScanName());

	edgeEnergy_ = original.edgeEnergy();
	edge_ = original.edge();

	microprobeSampleStageX_ = original.microprobeSampleStageX();
	microprobeSampleStageZ_ = original.microprobeSampleStageZ();
    setEnableBrukerDetector(original.enableBrukerDetector());

	computeTotalTime();

	connect(scanAxisAt(0), SIGNAL(regionAdded(AMScanAxisRegion*)), this, SLOT(onRegionAdded(AMScanAxisRegion*)));
	connect(scanAxisAt(0), SIGNAL(regionRemoved(AMScanAxisRegion*)), this, SLOT(onRegionRemoved(AMScanAxisRegion*)));

	foreach (AMScanAxisRegion *region, scanAxisAt(0)->regions().toList()){

		AMScanAxisEXAFSRegion *exafsRegion = qobject_cast<AMScanAxisEXAFSRegion *>(region);
		connect(exafsRegion, SIGNAL(regionStartChanged(AMNumber)), this, SLOT(computeTotalTime()));
		connect(exafsRegion, SIGNAL(regionStepChanged(AMNumber)), this, SLOT(computeTotalTime()));
		connect(exafsRegion, SIGNAL(regionEndChanged(AMNumber)), this, SLOT(computeTotalTime()));
		connect(exafsRegion, SIGNAL(regionTimeChanged(AMNumber)), this, SLOT(computeTotalTime()));
		connect(exafsRegion, SIGNAL(maximumTimeChanged(AMNumber)), this, SLOT(computeTotalTime()));
		connect(exafsRegion, SIGNAL(equationChanged(AMVariableIntegrationTime::Equation)), this, SLOT(computeTotalTime()));
	}
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

void SXRMBEXAFSScanConfiguration::setMicroprobeSampleStageX(double microprobeSampleStageX)
{
	if(microprobeSampleStageX_ != microprobeSampleStageX){
		microprobeSampleStageX_ = microprobeSampleStageX;
		emit microprobeSampleStageXChanged(microprobeSampleStageX_);
		setModified(true);
	}
}

void SXRMBEXAFSScanConfiguration::setMicroprobeSampleStageZ(double microprobeSampleStageZ)
{
	if(microprobeSampleStageZ_ != microprobeSampleStageZ){
		microprobeSampleStageZ_ = microprobeSampleStageZ;
		emit microprobeSampleStageZChanged(microprobeSampleStageZ_);
		setModified(true);
	}
}

void SXRMBEXAFSScanConfiguration::setEnableBrukerDetector(bool enableDetector)
{
    if (enableBrukerDetector_ == enableDetector)
        return;

    enableBrukerDetector_ = enableDetector;
}

QString SXRMBEXAFSScanConfiguration::headerText() const
{
	QString header("Configuration of the Scan\n\n");

	header.append("Scanned Edge:\t" + edge() + "\n");
	header.append(QString("Focus position:\t%1 mm\n").arg(normalPosition()));

	header.append(regionsOfInterestHeaderString(regionsOfInterest()) % "\n");
	header.append("\n");
	header.append("Regions Scanned\n");

	foreach (AMScanAxisRegion *region, scanAxisAt(0)->regions().toList()){

		AMScanAxisEXAFSRegion *exafsRegion = qobject_cast<AMScanAxisEXAFSRegion *>(region);

		if (exafsRegion->inKSpace() && (exafsRegion->maximumTime().isValid() || exafsRegion->maximumTime() == exafsRegion->regionTime()))
			header.append(QString("Start: %1 eV\tDelta: %2 k\tEnd: %3 k\tTime: %4 s\n")
						  .arg(double(AMEnergyToKSpaceCalculator::energy(edgeEnergy_, exafsRegion->regionStart())))
						  .arg(double(exafsRegion->regionStep()))
						  .arg(double(exafsRegion->regionEnd()))
						  .arg(double(exafsRegion->regionTime())));

		else if (exafsRegion->inKSpace() && exafsRegion->maximumTime().isValid())
			header.append(QString("Start: %1 eV\tDelta: %2 k\tEnd: %3 k\tStart time: %4 s\tMaximum time (used with variable integration time): %5 s\n")
						  .arg(double(AMEnergyToKSpaceCalculator::energy(edgeEnergy_, exafsRegion->regionStart())))
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

double SXRMBEXAFSScanConfiguration::microprobeSampleStageX() const
{
	return microprobeSampleStageX_;
}

double SXRMBEXAFSScanConfiguration::microprobeSampleStageZ() const
{
	return microprobeSampleStageZ_;
}

void SXRMBEXAFSScanConfiguration::computeTotalTimeImplementation()
{
	double time = 0;

	// Some region stuff.
	foreach (AMScanAxisRegion *region, scanAxisAt(0)->regions().toList()){

		AMScanAxisEXAFSRegion *exafsRegion = qobject_cast<AMScanAxisEXAFSRegion *>(region);
		int numberOfPoints = int((double(exafsRegion->regionEnd()) - double(exafsRegion->regionStart()))/double(exafsRegion->regionStep()) + 1);

		if (exafsRegion->inKSpace() && exafsRegion->maximumTime().isValid()){

			QVector<double> regionTimes = QVector<double>(numberOfPoints);
			AMVariableIntegrationTime calculator(exafsRegion->equation(), exafsRegion->regionTime(), exafsRegion->maximumTime(), exafsRegion->regionStart(), exafsRegion->regionStep(), exafsRegion->regionEnd(), exafsRegion->a2());
			calculator.variableTime(regionTimes.data());

			for (int i = 0; i < numberOfPoints; i++)
				time += regionTimes.at(i);
		}

		else
			time += (double(exafsRegion->regionTime())+timeOffset_)*numberOfPoints;
	}

	totalTime_ = time; // There is a 9 second miscellaneous startup delay.
	setExpectedDuration(totalTime_);
	emit totalTimeChanged(totalTime_);
}

void SXRMBEXAFSScanConfiguration::onRegionAdded(AMScanAxisRegion *region)
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

void SXRMBEXAFSScanConfiguration::onRegionRemoved(AMScanAxisRegion *region)
{
	region->disconnect(this);
	computeTotalTime();
}
