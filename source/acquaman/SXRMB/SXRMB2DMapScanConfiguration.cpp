#include "SXRMB2DMapScanConfiguration.h"

#include <math.h>
#include <QStringBuilder>

#include "acquaman/SXRMB/SXRMB2DScanActionController.h"
#include "ui/SXRMB/SXRMB2DMapScanConfigurationView.h"


SXRMB2DMapScanConfiguration::SXRMB2DMapScanConfiguration(QObject *parent)
	: AMStepScanConfiguration(parent), SXRMBScanConfiguration()
{
	timeOffset_ = 0.8;

	setName("2D Map");
	setUserScanName("2D Map");
	setEnergy(3000.0);

	exportAsAscii_ = false;

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

SXRMB2DMapScanConfiguration::SXRMB2DMapScanConfiguration(const SXRMB2DMapScanConfiguration &original)
	: AMStepScanConfiguration(original), SXRMBScanConfiguration(original)
{
	setName(original.name());
	setUserScanName(original.userScanName());

	exportAsAscii_ = original.exportAsAscii();

	connect(scanAxisAt(0)->regionAt(0), SIGNAL(regionStartChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(0)->regionAt(0), SIGNAL(regionStepChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(0)->regionAt(0), SIGNAL(regionEndChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(0)->regionAt(0), SIGNAL(regionTimeChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(1)->regionAt(0), SIGNAL(regionStartChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(1)->regionAt(0), SIGNAL(regionStepChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(1)->regionAt(0), SIGNAL(regionEndChanged(AMNumber)), this, SLOT(computeTotalTime()));
}

AMScanConfiguration *SXRMB2DMapScanConfiguration::createCopy() const
{
	AMScanConfiguration *configuration = new SXRMB2DMapScanConfiguration(*this);
	configuration->dissociateFromDb(true);
	return configuration;
}

AMScanController *SXRMB2DMapScanConfiguration::createController()
{
	AMScanActionController *controller = new SXRMB2DScanActionController(this);
	controller->buildScanController();

	return controller;
}

AMScanConfigurationView *SXRMB2DMapScanConfiguration::createView()
{
	return new SXRMB2DMapScanConfigurationView(this);
}

QString SXRMB2DMapScanConfiguration::technique() const
{
	return "2D Scan";
}

QString SXRMB2DMapScanConfiguration::description() const
{
	return "2D Scan";
}

QString SXRMB2DMapScanConfiguration::detailedDescription() const
{
	return "SXRMB 2D Scan";
}


QString SXRMB2DMapScanConfiguration::headerText() const
{
	QString header("Configuration of the Scan\n\n");

	header.append(regionsOfInterestHeaderString(regionsOfInterest()) % "\n");

	header.append("\n");
	header.append("Map Dimensions\n");
	header.append("X Axis\n");
	header.append(QString("Start:\t%1 mm\tEnd:\t%2 mm\n").arg(double(scanAxisAt(0)->regionAt(0)->regionStart())).arg(double(scanAxisAt(0)->regionAt(0)->regionEnd())));
	header.append(QString("Step Size:\t%1 mm\n").arg(double(scanAxisAt(0)->regionAt(0)->regionStep())));
	header.append("Y Axis\n");
	header.append(QString("Start:\t%1 mm\tEnd:\t%2 mm\n").arg(double(scanAxisAt(1)->regionAt(0)->regionStart())).arg(double(scanAxisAt(1)->regionAt(0)->regionEnd())));
	header.append(QString("Step Size:\t%1 mm\n").arg(double(scanAxisAt(1)->regionAt(0)->regionStep())));
	header.append("\n");
	header.append(QString("Focus position:\t%1 mm\n").arg(y()));

	return header;
}

void SXRMB2DMapScanConfiguration::computeTotalTimeImplementation()
{
	double time = 0;

	// Get the number of points.
	time = scanAxisAt(0)->numberOfPoints() * scanAxisAt(1)->numberOfPoints();

	time *= double(scanAxisAt(0)->regionAt(0)->regionTime()) + timeOffset_;

	totalTime_ = time + 9; // initialization time is about 9s
	setExpectedDuration(totalTime_);
	emit totalTimeChanged(totalTime_);
}

void SXRMB2DMapScanConfiguration::setExportAsAscii(bool exportAsAscii)
{
	if (exportAsAscii_ == exportAsAscii)
		return;

	exportAsAscii_ = exportAsAscii;
}
