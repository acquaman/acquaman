#include "REIXSXESCalibration.h"


REIXSXESCalibration::REIXSXESCalibration(QObject *parent) :
	AMDbObject(parent)
{
	gratingNames_ << "Low Energy" << "Impurity" << "Medium Energy" << "High Energy*" << "Medium Energy (HiRes)";
	gratingGrooveDensities_ << 593.02 << 892.86 << 1187.819 << 2600 << 1800;
	gratingRadii_ << 5025.6 << 6699.8 << 10018 << 11461 << 10029;
	gratingCenterPositionsX_ << 18.946 << -16.552 << 18.517 << 18.499 << -16.998;
	gratingCenterPositionsY_ << -10.917 << -10.899 << 16.915 << 41.810 << 41.405;
	gratingCenterPositionsZ_ << 9.958 << 10.637 << -22.173 << 12.6 << 12.915;

	gratingMountTilt_ << 4.07 << 3.07 << 2.07 << 1.82 << 2.07;
	gratingMountRoll_ << 0 << 0 << 0 << 0 << 0;

	hexapodOrigin_ = QVector3D(0, 0, -433.731);

	detectorPos0_ = QVector3D(-2.548, 498.977, 25.279);

	slitPos_ = QVector3D(-1.596, -349.373, 0.516);
	opticalOrigin_ = QVector3D(0, 0, 0);


	detectorHeightError_ = 3.03; //mm

	spectrometerTheta0_ = 3.242; //deg
	spectrometerTheta0m_ = 3.07; //deg

	frameA_ = 2850.66;//mm
	frameATheta_ = 3.205021;//deg
	frameB_ = 940;//mm
	frameL_ = 2849.70;//mm
	frameH0_ = 950.2;//mm


	tiltA_ = 142.394; //mm
	tiltB_ = 109.814; //mm
	tiltHomePos_ = 70.91; //mm


	spectrometerRotDrive0_ = -3.7; //mm


	detectorTranslation0_ = 499.617; //mm

	gratingEVRangesMin_ << 100 << 100 << 100 << 100 << 100;
	gratingEVRangesMax_ << 1000 << 1000 << 1000 << 1000 << 1000;
}
