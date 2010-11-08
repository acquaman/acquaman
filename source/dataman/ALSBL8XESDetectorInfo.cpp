#include "ALSBL8XESDetectorInfo.h"

ALSBL8XESDetectorInfo::ALSBL8XESDetectorInfo(QObject *parent) :
	AMDetectorInfo("xes", "XES Image Detector", parent)
{
	centerEnergy_ = double(0.0);
	grating_ = int(1);
	angle_ = double(0.0);
	posX_ = double(0.0);
	posY_ = double(0.0);
	posA_ = double(0.0);
	slitWidth_ = double(0.0);
	targetCounts_ = int(10000);
	targetExposure_ = double(180);

	pixelsX_ = int(1024);
	pixelsY_ = int(32);
	numGratings_ = int(4);
	anodeWidthX_ = double(40);
	gratingNames_ << "Grating 1" << "Grating 2" << "Grating 3" << "Grating 4";
	slitPositionX_ = double(1138.161);
	slitPositionY_ = double(180.420);
	gratingGrooveDensities_ << 600.0 << 1500.0 << 1000.0 << 1500.0;

	gratingRadii_ << 4999.05 << 5000.2 << 10000.33 << 10000.0;

	gratingPositionsX_ << 801.567 << 801.637 << 801.608 << 801.608;

	gratingPositionsY_ << 102.767 << 102.718 << 102.505 << 102.336;

	zeroOrderRayAngles_ << -10.0815 << -10.2915 << -14.0385 << -14.0385;


	gratingCorrectionsDX_ << 0 << 0 << 0 << 6.55034;

	gratingCorrectionsDY_ << 0 << 0 << 0 << 3.62933;

	gratingCorrectionsDA_ << 0 << 0 << 0 << -0.0100719;

	gratingCorrectionsGrooveDensity_ << 0 << 0 << 0 << 27.1196;

	gratingCorrectionsRadii_ << 0 << 0 << 0 << -65.4955;

	slitCorrectionsDX_ << 0 << 0 << 0 << -1.97679;

	slitCorrectionsDY_ << 0 << 0 << 0 << 2.79737;
}
