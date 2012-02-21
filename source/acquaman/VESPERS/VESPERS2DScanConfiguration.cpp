#include "VESPERS2DScanConfiguration.h"

#include "acquaman/VESPERS/VESPERS2DDacqScanController.h"
#include "ui/VESPERS/VESPERS2DScanConfigurationView.h"

VESPERS2DScanConfiguration::VESPERS2DScanConfiguration(QObject *parent)
	: AM2DScanConfiguration(parent)
{
	setName("2D Map");
	I0_ = Imini;
	fluorescenceDetectorChoice_ = SingleElement;
	usingCCD_ = false;
	roiInfoList_ = AMROIInfoList();
	totalTime_ = 0;
	timeOffset_ = 0.7;
	connect(this, SIGNAL(xStartChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(xStepChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(xEndChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(yStartChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(yStepChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(yEndChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(timeStepChanged(double)), this, SLOT(computeTotalTime()));
}

VESPERS2DScanConfiguration::VESPERS2DScanConfiguration(const VESPERS2DScanConfiguration &original)
	: AM2DScanConfiguration(original.parent())
{
	setName(original.name());
	I0_ = original.incomingChoice();
	fluorescenceDetectorChoice_ = original.fluorescenceDetectorChoice();
	usingCCD_ = original.usingCCD();
	roiInfoList_ = original.roiList();
	totalTime_ = 0;
	timeOffset_ = 0.7;
	connect(this, SIGNAL(xStartChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(xStepChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(xEndChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(yStartChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(yStepChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(yEndChanged(double)), this, SLOT(computeTotalTime()));
	connect(this, SIGNAL(timeStepChanged(double)), this, SLOT(computeTotalTime()));
}

AMScanConfiguration *VESPERS2DScanConfiguration::createCopy() const
{
	return new VESPERS2DScanConfiguration(*this);
}

AMScanController *VESPERS2DScanConfiguration::createController()
{
	return new VESPERS2DDacqScanController(this);
}

AMScanConfigurationView *VESPERS2DScanConfiguration::createView()
{
	return new VESPERS2DScanConfigurationView(this);
}

QString VESPERS2DScanConfiguration::detailedDescription() const
{
	if (usingCCD_)
		return "Spatial x-ray fluorescence 2D map using a CCD for x-ray diffraction.";

	return "Spatial x-ray fluorescence 2D map";
}

QString VESPERS2DScanConfiguration::readRoiList() const
{
	QString prettyRois = "Regions of Interest\n";

	for (int i = 0; i < roiInfoList_.count(); i++)
		prettyRois.append(roiInfoList_.at(i).name() + "\t" + QString::number(roiInfoList_.at(i).low()) + " eV\t" + QString::number(roiInfoList_.at(i).high()) + " eV\n");

	return prettyRois;
}

void VESPERS2DScanConfiguration::computeTotalTime()
{
	double time = 0;

	// Get the number of points.
	time = 	fabs((xEnd()-xStart())/xStep())*fabs((yEnd()-yStart())/yStep());

	// Factor in the time per point.
	time *= timeStep() + timeOffset_;
	totalTime_ = time;
	emit totalTimeChanged(totalTime_);
}
