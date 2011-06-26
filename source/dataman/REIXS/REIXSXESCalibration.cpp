#include "REIXSXESCalibration.h"

REIXSXESCalibration::REIXSXESCalibration(QObject *parent) :
	AMDbObject(parent)
{
	gratingNames_ << "Low Energy" << "Impurity" << "Medium Energy" << "High Energy*" << "Medium Energy (HiRes)";
	gratingGrooveDensities_ << 593.02 << 892.86 << 1187.819 << 2600 << 1800;
	gratingRadii_ << 5025.6 << 6699.8 << 10018 << 11461 << 10029;
	gratingCenterPositionsX_ << 18.946 << -16.552 << 18.517 << 18.499 << -16.998;
	gratingCenterPositionsY_ << -10.917 << -10.899 << 16.915 << 41.810 << 41.405;

	gratingMountTilt_ << 4.07 << 3.07 << 2.07 << 1.82 << 2.07;
	gratingMountRoll_ << 0 << 0 << 0 << 0 << 0;

	hexapodOrigin_ << 0 << 0 << -433.731;

	detectorPos0_ << -2.548 << 498.977 << 25.279;

	slitPos_ << -1.596 << -349.373 << 0.516;
	opticalOrigin_ << 0 << 0 << 0;


	spectrometerRotAngle0_;
	spectrometerRotAngleDrive0_ = -3.7;

	detectorTranslation0_;

	gratingEVRangesMin_;
	gratingEVRangesMax_;
}
