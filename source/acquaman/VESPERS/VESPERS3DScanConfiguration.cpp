#include "VESPERS3DScanConfiguration.h"

#include <QStringBuilder>
#include <cmath>

#include "acquaman/VESPERS/VESPERS3DDacqScanController.h"
#include "ui/VESPERS/VESPERS3DScanConfigurationView.h"

VESPERS3DScanConfiguration::~VESPERS3DScanConfiguration(){}

VESPERS3DScanConfiguration::VESPERS3DScanConfiguration(QObject *parent)
	: AMStepScanConfiguration(parent), VESPERSScanConfiguration()
{
	setName("3DMap");
	setUserScanName("3DMap");
	dbObject_->setParent(this);
	setIncomingChoice(VESPERS::Imini);
	setFluorescenceDetector(VESPERS::SingleElement);
	setMotor(VESPERS::Motors(VESPERS::H | VESPERS::V));
	setCCDDetector(VESPERS::Pilatus);
	setCCDFileName("3D Map");
	setExportAsAscii(true);
	setExportSpectraSources(true);
	setExportSpectraInRows(true);

	AMScanAxisRegion *region = new AMScanAxisRegion;
	AMScanAxis *axis = new AMScanAxis(AMScanAxis::StepAxis, region);
	appendScanAxis(axis);
	region = new AMScanAxisRegion;
	axis = new AMScanAxis(AMScanAxis::StepAxis, region);
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
	connect(scanAxisAt(2)->regionAt(0), SIGNAL(regionStartChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(2)->regionAt(0), SIGNAL(regionStepChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(2)->regionAt(0), SIGNAL(regionEndChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(dbObject_, SIGNAL(ccdDetectorChanged(int)), this, SLOT(computeTotalTime()));
}

VESPERS3DScanConfiguration::VESPERS3DScanConfiguration(const VESPERS3DScanConfiguration &original)
	: AMStepScanConfiguration(original), VESPERSScanConfiguration(original)
{
	setName(original.name());
	setUserScanName(original.userScanName());
	dbObject_->setParent(this);
	setExportAsAscii(original.exportAsAscii());
	setExportSpectraSources(original.exportSpectraSources());
	setExportSpectraInRows(original.exportSpectraInRows());
	computeTotalTime();
	connect(scanAxisAt(0)->regionAt(0), SIGNAL(regionStartChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(0)->regionAt(0), SIGNAL(regionStepChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(0)->regionAt(0), SIGNAL(regionEndChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(0)->regionAt(0), SIGNAL(regionTimeChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(1)->regionAt(0), SIGNAL(regionStartChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(1)->regionAt(0), SIGNAL(regionStepChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(1)->regionAt(0), SIGNAL(regionEndChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(2)->regionAt(0), SIGNAL(regionStartChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(2)->regionAt(0), SIGNAL(regionStepChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(scanAxisAt(2)->regionAt(0), SIGNAL(regionEndChanged(AMNumber)), this, SLOT(computeTotalTime()));
	connect(dbObject_, SIGNAL(ccdDetectorChanged(int)), this, SLOT(computeTotalTime()));
}

AMScanConfiguration * VESPERS3DScanConfiguration::createCopy() const
{
	return new  VESPERS3DScanConfiguration(*this);
}

AMScanController * VESPERS3DScanConfiguration::createController()
{
//	return new VESPERS3DDacqScanController(this);
	return 0; //NULL
}

AMScanConfigurationView * VESPERS3DScanConfiguration::createView()
{
	return new  VESPERS3DScanConfigurationView(this);
}

QString  VESPERS3DScanConfiguration::detailedDescription() const
{
	return "Spatial x-ray fluorescence 3D map using a CCD for x-ray diffraction.";
}

QString  VESPERS3DScanConfiguration::headerText() const
{
	QString header("Configuration of the Scan\n\n");

	header.append(fluorescenceHeaderString(fluorescenceDetector()));
	header.append(incomingChoiceHeaderString(incomingChoice()));
	header.append(regionsOfInterestHeaderString(regionsOfInterest()) % "\n");
	header.append(motorHeaderString(motor()));
	header.append(ccdDetectorHeaderString(ccdDetector()));

	header.append("\n");
	header.append("Map Dimensions\n");
	header.append("X Axis\n");
	header.append(QString("Start:\t%1 mm\tEnd:\t%2 mm\n").arg(double(scanAxisAt(0)->regionAt(0)->regionStart())).arg(double(scanAxisAt(0)->regionAt(0)->regionEnd())));
	header.append(QString("Step Size:\t%1 mm\n").arg(double(scanAxisAt(0)->regionAt(0)->regionStep())));
	header.append("Y Axis\n");
	header.append(QString("Start:\t%1 mm\tEnd:\t%2 mm\n").arg(double(scanAxisAt(1)->regionAt(0)->regionStart())).arg(double(scanAxisAt(1)->regionAt(0)->regionEnd())));
	header.append(QString("Step Size:\t%1 mm\n").arg(double(scanAxisAt(1)->regionAt(0)->regionStep())));
	header.append("Wire Axis\n");
	header.append(QString("Start:\t%1 mm\tEnd:\t%2 mm\n").arg(double(scanAxisAt(2)->regionAt(0)->regionStart())).arg(double(scanAxisAt(2)->regionAt(0)->regionEnd())));
	header.append(QString("Step Size:\t%1 mm\n").arg(double(scanAxisAt(2)->regionAt(0)->regionStep())));

	if (normalPosition() != 888888.88){

		header.append("\n");
		header.append(QString("Focus position:\t%1 mm\n").arg(normalPosition()));
	}

	return header;
}

QString  VESPERS3DScanConfiguration::xAxisName() const
{
	return "Horizontal (H)";
}

QString  VESPERS3DScanConfiguration::xAxisUnits() const
{
	return "mm";
}

QString  VESPERS3DScanConfiguration::yAxisName() const
{
	return "Vertical (V)";
}

QString  VESPERS3DScanConfiguration::yAxisUnits() const
{
	return "mm";
}

QString  VESPERS3DScanConfiguration::zAxisName() const
{
	return "Wire";
}

QString  VESPERS3DScanConfiguration::zAxisUnits() const
{
	return "mm";
}

void  VESPERS3DScanConfiguration::computeTotalTimeImplementation()
{
	double time = 0;

	// Get the number of points.
	time = 	fabs((double(scanAxisAt(0)->regionAt(0)->regionEnd())-double(scanAxisAt(0)->regionAt(0)->regionStart()))/double(scanAxisAt(0)->regionAt(0)->regionStep())+1)
			*fabs((double(scanAxisAt(1)->regionAt(0)->regionEnd())-double(scanAxisAt(1)->regionAt(0)->regionStart()))/double(scanAxisAt(1)->regionAt(0)->regionStep())+1)
			*fabs((double(scanAxisAt(2)->regionAt(0)->regionEnd())-double(scanAxisAt(2)->regionAt(0)->regionStart()))/double(scanAxisAt(2)->regionAt(0)->regionStep())+1);

	// Factor in the time per point.  There is an extra 6 seconds for CCD images for the Roper and Mar.
	if (ccdDetector() == VESPERS::Roper)
		time *= double(scanAxisAt(0)->regionAt(0)->regionTime()) + timeOffset_ + 6.0;
	else if (ccdDetector() == VESPERS::Mar)
		time *= double(scanAxisAt(0)->regionAt(0)->regionTime()) + timeOffset_ + 3.0;
	else
		time *= double(scanAxisAt(0)->regionAt(0)->regionTime()) + timeOffset_;

	totalTime_ = time + 9;
	setExpectedDuration(totalTime_);
	emit totalTimeChanged(totalTime_);
}

void  VESPERS3DScanConfiguration::setExportAsAscii(bool exportAsAscii)
{
	if (exportAsAscii_ == exportAsAscii)
		return;

	exportAsAscii_ = exportAsAscii;
}

void  VESPERS3DScanConfiguration::setExportSpectraSources(bool exportSpectra)
{
	if (exportSpectraSources_ == exportSpectra)
		return;

	exportSpectraSources_ = exportSpectra;
}

void VESPERS3DScanConfiguration::setExportSpectraInRows(bool exportInRows)
{
	if (exportSpectraInRows_ == exportInRows)
		return;

	exportSpectraInRows_ = exportInRows;
}
