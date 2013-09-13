#include "VESPERS3DScanConfiguration.h"

#include "acquaman/VESPERS/VESPERS3DDacqScanController.h"
#include "ui/VESPERS/VESPERS3DScanConfigurationView.h"

#include <QStringBuilder>

VESPERS3DScanConfiguration::VESPERS3DScanConfiguration(QObject *parent)
	: AM3DScanConfiguration(parent), VESPERSScanConfiguration()
{
	setName("3DMap");
	setUserScanName("3DMap");
	dbObject_->setParent(this);
	setXPriority(1);
	setYPriority(0);
	setZPriority(2);
	setIncomingChoice(VESPERS::Imini);
	setFluorescenceDetector(VESPERS::SingleElement);
	setMotor(VESPERS::Motor(VESPERS::H | VESPERS::V));
	setCCDDetector(VESPERS::Pilatus);
	setCCDFileName("3D Map");
	setRoiInfoList(AMROIInfoList());
	setExportAsAscii(true);
	setExportSpectraSources(true);
	setExportSpectraInRows(true);
	connect(this, SIGNAL(xStartChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(xStepChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(xEndChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(yStartChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(yStepChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(yEndChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(zStartChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(zStepChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(zEndChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(timeStepChanged(double)), this, SLOT(computeTotalTime()));
	connect(dbObject_, SIGNAL(ccdDetectorChanged(int)), this, SLOT(computeTotalTime()));
}

VESPERS3DScanConfiguration::VESPERS3DScanConfiguration(const VESPERS3DScanConfiguration &original)
	: AM3DScanConfiguration(original), VESPERSScanConfiguration(original)
{
	setName(original.name());
	setUserScanName(original.userScanName());
	dbObject_->setParent(this);
	setXPriority(original.xPriority());
	setYPriority(original.yPriority());
	setZPriority(original.zPriority());
	setExportAsAscii(original.exportAsAscii());
	setExportSpectraSources(original.exportSpectraSources());
	setExportSpectraInRows(original.exportSpectraInRows());
	computeTotalTime();
	connect(this, SIGNAL(xStartChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(xStepChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(xEndChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(yStartChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(yStepChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(yEndChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(zStartChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(zStepChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(zEndChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(timeStepChanged(double)), this, SLOT(computeTotalTime()));
	connect(dbObject_, SIGNAL(ccdDetectorChanged(int)), this, SLOT(computeTotalTime()));
}

AMScanConfiguration * VESPERS3DScanConfiguration::createCopy() const
{
	return new  VESPERS3DScanConfiguration(*this);
}

AMScanController * VESPERS3DScanConfiguration::createController()
{
	return new VESPERS3DDacqScanController(this);
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
	header.append(regionOfInterestHeaderString(roiList()) % "\n");
	header.append(motorHeaderString(motor()));
	header.append(ccdDetectorHeaderString(ccdDetector()));

	header.append("\n");
	header.append("Map Dimensions\n");
	header.append("X Axis\n");
	header.append(QString("Start:\t%1 mm\tEnd:\t%2 mm\n").arg(xStart()).arg(xEnd()));
	header.append(QString("Step Size:\t%1 mm\n").arg(xStep()));
	header.append("Y Axis\n");
	header.append(QString("Start:\t%1 mm\tEnd:\t%2 mm\n").arg(yStart()).arg(yEnd()));
	header.append(QString("Step Size:\t%1 mm\n").arg(yStep()));
	header.append("Wire Axis\n");
	header.append(QString("Start:\t%1 mm\tEnd:\t%2 mm\n").arg(zStart()).arg(zEnd()));
	header.append(QString("Step Size:\t%1 mm\n").arg(zStep()));

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
	time = 	fabs((xEnd()-xStart())/xStep()+1)*fabs((yEnd()-yStart())/yStep()+1)*fabs((zEnd()-zStart())/zStep()+1);

	// Factor in the time per point.  There is an extra 6 seconds for Roper CCD images.
	if (ccdDetector() == VESPERS::Roper)
		time *= timeStep() + timeOffset_ + 6.0;
	// There is an extra 3 seconds for the Mar CCD images.
	else if (ccdDetector() == VESPERS::Mar)
		time *= timeStep() + timeOffset_ + 3.0;
	else
		time *= timeStep() + timeOffset_;

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
