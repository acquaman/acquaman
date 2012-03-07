/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "REIXSXESCalibration.h"


REIXSXESCalibration::REIXSXESCalibration(QObject *parent) :
	AMDbObject(parent)
{
	gratingNames_ << "LEG" << "Impurity" << "MEG" << "HEG*(HRHEG)" << "HRMEG";
	gratingGrooveDensities_ << 593.02 << 892.86 << 1187.819 << 2600 << 1800;
	gratingRadii_ << 5025.6 << 6699.8 << 10018 << 11461 << 10029;
	gratingCenterPositionsX_ << 20.529 << -14.970 << 20.374 << 20.278 << -15.220;
	gratingCenterPositionsY_ << -25.865 << -25.723 << 2.697 << 26.808 << 26.535;
	gratingCenterPositionsZ_ << 12.892 << 13.507 << -18.923 << 16.465 << 16.709;

	gratingMountTilt_ << 4.0 << 3.0 << 2.0 << 1.75 << 2.0;
	gratingMountRoll_ << 0 << 0 << 0 << 0 << 0;
	// gratingMountTwist_ << 0.41 << 0.41 << 0.41 << 0.41;

	hexapodOrigin_ = QVector3D(2.418, 4.145, -430.478);

	detectorPos0_ = QVector3D(-2.548, 498.977, 25.279);

	slitPos_ = QVector3D(-0.32, -349.95, -6.10);
	opticalOrigin_ = QVector3D(0, 0, 0);


	detectorHeightError_ = 3.03; //mm

	spectrometerTheta0_ = 2.738; //deg
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


	detectorTranslation0_ = 499.609; //mm

	gratingEVRangesMin_ << 100 << 100 << 100 << 100 << 100;
	gratingEVRangesMax_ << 1000 << 1000 << 1000 << 1000 << 1000;
}

AMControlInfoList REIXSXESCalibration::computeSpectrometerPosition(REIXSXESScanConfiguration *scanConfiguration) const
{
	/// \todo Handle orientation of detector (wide/narrow)
	return computeSpectrometerPosition(scanConfiguration->gratingNumber(),
									   scanConfiguration->centerEV(),
									   scanConfiguration->defocusDistanceMm(),
									   scanConfiguration->detectorTiltOffset());

}

AMControlInfoList REIXSXESCalibration::computeSpectrometerPosition(int gratingIndex, double eV, double focusOffsetMm, double tiltOffsetDeg) const
{
	AMControlInfoList rv;

	double hexU = hexapodU(gratingIndex);
	QVector3D hexXYZ = hexapodXYZ(gratingIndex);
	QVector3D hexRST = hexapodRST(gratingIndex);

	QVector3D detPos = detectorPos(eV, gratingIndex);
	double theta = spectrometerTheta(detPos);
	double translation = detectorTranslation(detPos, theta, focusOffsetMm);
	double spectrometerRotation = spectrometerRotationDrive(theta);
	double tilt = tiltStageDrive(eV, gratingIndex, theta, tiltOffsetDeg);

	rv.append(AMControlInfo("spectrometerRotationDrive", spectrometerRotation, 0,0, "mm", 0.1, "Linear drive motor for spectrometer angle"));
	rv.append(AMControlInfo("detectorTranslation", translation, 0,0, "mm", 0.1, "Detector translation"));
	rv.append(AMControlInfo("detectorTiltDrive", tilt, 0.0, 0.0, "mm", 0.1, "Linear drive motor for detector tilt"));
	rv.append(AMControlInfo("hexapodX", hexXYZ.x(), 0, 0, "mm", 0.1, "Hexapod X position"));
	rv.append(AMControlInfo("hexapodY", hexXYZ.y(), 0, 0, "mm", 0.1, "Hexapod Y position"));
	rv.append(AMControlInfo("hexapodZ", hexXYZ.z(), 0, 0, "mm", 0.1, "Hexapod Z position"));
	rv.append(AMControlInfo("hexapodU", hexU, 0,0, "deg", 0.1, "Hexapod Tilt around X axis"));
	rv.append(AMControlInfo("hexapodR", hexRST.x(), 0,0, "mm", 0.1, "Hexapod rotation point R"));
	rv.append(AMControlInfo("hexapodS", hexRST.y(), 0,0, "mm", 0.1, "Hexapod rotation point S"));
	rv.append(AMControlInfo("hexapodT", hexRST.z(), 0,0, "mm", 0.1, "Hexapod rotation point T"));

	return rv;
}

double REIXSXESCalibration::computeEVFromSpectrometerPosition(int gratingIndex, double spectrometerRotationDriveMm, double detectorTranslationMm) const
{
	double theta = spectrometerTheta(spectrometerRotationDriveMm);
	QVector3D pos = detectorPosFromThetaAndTranslation(theta, detectorTranslationMm);
	double dTheta = dThetaFromDetectorPos(pos);

	// qDebug() << "theta:" << theta << "pos" << pos << "dTheta" << dTheta;

	return eVFromDTheta(dTheta, gratingIndex);
}

double REIXSXESCalibration::spectrometerTheta(double spectrometerRotationDrive) const
{
	// lh = liftheight
	double lh = frameH0_ - spectrometerRotationDrive + spectrometerRotDrive0_;

	double l=frameL_;
	double a=frameA_;
	double b=frameB_;
	double a2 = a*a;
	double b2 = b*b;
	double l2 = l*l;
	double lh2 = lh*lh;
	double l3 = l2*l;
	double lh4 = lh2*lh2;
	double lh6 = lh4*lh2;
	double l4 = l2*l2;
	double b4 = b2*b2;
	double a4 = a2*a2;

	// phi = theta_m - d2r(frameATheta_)
	double phi = atan2(

				-1./2.*(a2-b2-l/(4*l2+4*lh2)*(4*a2*l+4*l*lh2+4*l3-4*b2*l+4*sqrt(-2*l2*lh4-l4*lh2-lh2*b4+2*lh4*b2+2*lh4*a2+2*a2*l2*lh2+2*l2*lh2*b2+2*lh2*b2*a2-a4*lh2-lh6))+l2+lh2)/a/lh,

				1./2./(4*l2+4*lh2)*(4*a2*l+4*l*lh2+4*l3-4*b2*l+4*sqrt(-2*l2*lh4-l4*lh2-lh2*b4+2*lh4*b2+2*lh4*a2+2*a2*l2*lh2+2*l2*lh2*b2+2*lh2*b2*a2-a4*lh2-lh6))/a

				);// one nasty inverse function


	double theta_m = phi + d2r(frameATheta_);

	// theta': angle above calibration position = theta - theta_0 [either aligned to working coordianates, or to frame rails. Doesn't matter, since it's a difference between two angles.]
	// theta_m: angle of inclineable rails above horizontal frame rails = theta' + theta_0m
	return theta_m + d2r(spectrometerTheta0_) - d2r(spectrometerTheta0m_);
}
