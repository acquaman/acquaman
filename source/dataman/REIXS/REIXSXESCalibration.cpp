#include "REIXSXESCalibration.h"

REIXSXESCalibration::REIXSXESCalibration(QObject *parent) :
	AMDbObject(parent)
{
	gratingNames_ << "LEG" << "Impurity" << "MEG" << "HEG*" << "HR MEG";
	gratingGrooveDensities_ << 593.02 << 892.86 << 1187.819 << 2600 << 1800;
	gratingRadii_;
	gratingCenterPositions_;
	gratingMountTilt_;
	gratingMountRoll_;

	hexapodOrigin_;

	detectorPos0_;

	slitPos_;
	opticalOrigin_;


	spectrometerRotAngle0_;
	spectrometerRotAngleDrive0_;

	detectorTranslation0_;

	gratingEVRangesMin_;
	gratingEVRangesMax_;
}
