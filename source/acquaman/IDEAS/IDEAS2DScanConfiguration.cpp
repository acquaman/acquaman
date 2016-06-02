#include "IDEAS2DScanConfiguration.h"

#include "acquaman/IDEAS/IDEAS2DScanActionController.h"
#include "ui/IDEAS/IDEAS2DScanConfigurationView.h"

#include <QStringBuilder>

IDEAS2DScanConfiguration::IDEAS2DScanConfiguration(QObject *parent)
	: AMStepScanConfiguration(parent), IDEASScanConfiguration()
{
	setName("Unnamed Scan");
	setUserScanName("Unnamed Scan");

	dbObject_->setParent(this);

	AMScanAxisRegion *region = new AMScanAxisRegion;
	AMScanAxis *axis = new AMScanAxis(AMScanAxis::StepAxis, region);
	appendScanAxis(axis);
	region = new AMScanAxisRegion;
	axis = new AMScanAxis(AMScanAxis::StepAxis, region);
	appendScanAxis(axis);

	connect(scanAxisAt(0)->regionAt(0), SIGNAL(regionStartChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(0)->regionAt(0), SIGNAL(regionStepChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(0)->regionAt(0), SIGNAL(regionEndChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(0)->regionAt(0), SIGNAL(regionTimeChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(1)->regionAt(0), SIGNAL(regionStartChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(1)->regionAt(0), SIGNAL(regionStepChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(1)->regionAt(0), SIGNAL(regionEndChanged(AMNumber)), this, SLOT(computeTotalTime()));
}

IDEAS2DScanConfiguration::IDEAS2DScanConfiguration(const IDEAS2DScanConfiguration &original)
	: AMStepScanConfiguration(original), IDEASScanConfiguration(original)
{
	setName(original.name());
	setUserScanName(original.name());
	dbObject_->setParent(this);

	computeTotalTime();
	connect(scanAxisAt(0)->regionAt(0), SIGNAL(regionStartChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(0)->regionAt(0), SIGNAL(regionStepChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(0)->regionAt(0), SIGNAL(regionEndChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(0)->regionAt(0), SIGNAL(regionTimeChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(1)->regionAt(0), SIGNAL(regionStartChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(1)->regionAt(0), SIGNAL(regionStepChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(1)->regionAt(0), SIGNAL(regionEndChanged(AMNumber)), this, SLOT(computeTotalTime()));
}

IDEAS2DScanConfiguration::~IDEAS2DScanConfiguration()
{

}

AMScanConfiguration* IDEAS2DScanConfiguration::createCopy() const
{
	AMScanConfiguration *configuration = new IDEAS2DScanConfiguration(*this);
	configuration->dissociateFromDb(true);
	return configuration;
}

AMScanController* IDEAS2DScanConfiguration::createController()
{
	IDEAS2DScanActionController *controller = new IDEAS2DScanActionController(this);
	controller->buildScanController();

	return controller;
}

AMScanConfigurationView* IDEAS2DScanConfiguration::createView()
{
	return new IDEAS2DScanConfigurationView(this);
}

QString IDEAS2DScanConfiguration::technique() const
{
	return "2D";
}

QString IDEAS2DScanConfiguration::description() const
{
	return "IDEAS 2D Scan";
}

QString IDEAS2DScanConfiguration::detailedDescription() const
{
	return "Spatial x-ray fluorescence 2D map";
}

QString IDEAS2DScanConfiguration::headerText() const
{
	QString header("Configuration of the Scan\n\n");

	header.append(fluorescenceHeaderString(fluorescenceDetector()));
	header.append(regionsOfInterestHeaderString(regionsOfInterest()) % "\n");

	header.append("\n");
	header.append("Map Dimensions\n");
	header.append("X Axis\n");
	header.append(QString("Start:\t%1 mm\tEnd:\t%2 mm\n").arg(double(scanAxisAt(0)->regionAt(0)->regionStart())).arg(double(scanAxisAt(0)->regionAt(0)->regionEnd())));
	header.append(QString("Step Size:\t%1 mm\n").arg(double(scanAxisAt(0)->regionAt(0)->regionStep())));
	header.append("Y Axis\n");
	header.append(QString("Start:\t%1 mm\tEnd:\t%2 mm\n").arg(double(scanAxisAt(1)->regionAt(0)->regionStart())).arg(double(scanAxisAt(1)->regionAt(0)->regionEnd())));
	header.append(QString("Step Size:\t%1 mm\n").arg(double(scanAxisAt(1)->regionAt(0)->regionStep())));

	return header;
}

void IDEAS2DScanConfiguration::computeTotalTimeImplementation()
{
	double time = scanAxisAt(0)->numberOfPoints()*scanAxisAt(1)->numberOfPoints()*double(scanAxisAt(0)->regionAt(0)->regionTime());

	totalTime_ = time;
	setExpectedDuration(totalTime_);
	emit totalTimeChanged(totalTime_);
}
