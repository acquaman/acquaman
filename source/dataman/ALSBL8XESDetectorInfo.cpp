#include "ALSBL8XESDetectorInfo.h"

ALSBL8XESDetectorInfo::ALSBL8XESDetectorInfo(QObject *parent) :
	AMDetectorInfo("xes", "XES Image Detector", parent)
{
	centerEnergy_ = (0.0);
	gratingNumber_ = (1);
	detectorAngle_ = (0.0);
	positionX_ = (0.0);
	positionY_ = (0.0);
	positionA_ = (0.0);
	slitWidth_ = (0.0);

	targetCounts_ = (10000);
	targetExposureSeconds_ = (180);

	gratingCount_ = (4);
	anodeWidthMm_ = (40);
	gratingNames_ << "Grating 1" << "Grating 2" << "Grating 3" << "Grating 4";
	slitPositionX_ = (1138.161);
	slitPositionY_ = (180.420);

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

	axes_ << AMAxisInfo("xesImage_x", 1024, "XES Detector eV Axis", "pixels");
	axes_ << AMAxisInfo("xesImage_y", 32, "XES Detector vertical axis", "pixels");
	axes_[0].isUniform = true;
	axes_[0].increment = 1;
	axes_[0].start = 0;
	axes_[1].isUniform = true;
	axes_[1].increment = 1;
	axes_[1].start = 0;

	setUnits("counts");

}
