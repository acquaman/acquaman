#include "BioXASXASScanConfiguration.h"

#include "acquaman/BioXAS/BioXASXASScanActionController.h"
#include "dataman/AMScanAxisEXAFSRegion.h"
#include "ui/BioXAS/BioXASXASScanConfigurationView.h"
#include "util/AMEnergyToKSpaceCalculator.h"

#include <QStringBuilder>

BioXASXASScanConfiguration::BioXASXASScanConfiguration(QObject *parent) :
	AMStepScanConfiguration(parent), BioXASScanConfiguration()
{
	setName("XAS Scan");
	setUserScanName("XAS Scan");

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

BioXASXASScanConfiguration::BioXASXASScanConfiguration(const BioXASXASScanConfiguration &original) :
	AMStepScanConfiguration(original), BioXASScanConfiguration()
{
	setName(original.name());
	setUserScanName(original.name());

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

AMScanConfigurationView* BioXASXASScanConfiguration::createView()
{
	return new BioXASXASScanConfigurationView(this);
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

void BioXASXASScanConfiguration::computeTotalTimeImplementation()
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

	totalTime_ = time;
	setExpectedDuration(totalTime_);
	emit totalTimeChanged(totalTime_);
	emit configurationChanged();
}
