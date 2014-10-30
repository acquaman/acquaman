/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#include "REIXSXESCalibration2.h"

#include "acquaman/REIXS/REIXSXESScanConfiguration.h"

#include <QDebug>

 REIXSXESGratingInfo::~REIXSXESGratingInfo(){}
REIXSXESGratingInfo::REIXSXESGratingInfo(QObject *parent) :
	AMDbObject(parent)
{
	grooveDensity_ = 1200; // l/mm
	radius_ = 10000; //mm
	alpha_ = 89; // deg
	r_ = 350; //mm
	centerPosition_ = QVector3D(20.529, -25.865, 12.892);	// relative to "working coordinates": origin at center of spectromater rotation axle. Oriented with hexapod axes.
	opticalOrigin_ = QVector3D(0,0,0);
	hexapodUVW_ = QVector3D(1,0,0);
	angleCorrection_ = 1.0;
	heightCorrection_ = 0.0;
	endstationTranslation_ = 0.0;
	evRangeMin_ = 100;
	evRangeMax_ = 800;
}

 REIXSXESCalibration2::~REIXSXESCalibration2(){}
REIXSXESCalibration2::REIXSXESCalibration2(QObject *parent) :
    AMDbObject(parent)
{
	// create all default gratings

	// LEG:
	gratings_ << REIXSXESGratingInfo("LEG", 593.02, 5025.6, 86, 350, QVector3D(20.529, -25.865, 12.892), QVector3D(0,0,0), QVector3D(-0.160, -0.22, -0.41), 4.0130868, 0.664364, -0.15, 80, 292);//3.973429915, 0.731106463 for u=-0.175, v=-0.175

	// IMP:
	gratings_ << REIXSXESGratingInfo("Impurity", 892.86, 6699.8, 87, 350, QVector3D(-14.970, -25.723, 13.507), QVector3D(0,0,0), QVector3D(0.068, -0.4, -0.41), 2.946376, 1.094057, -0.15, 83, 784); //(3.02695496, 0.64573899 for U=0.1, v=0.261)  Aug17,2012: old V=-0.22

	// MEG:
	gratings_ << REIXSXESGratingInfo("MEG", 1187.819, 10018, 88, 350, QVector3D(20.374, 2.697, -18.923), QVector3D(0,-0.014,-1), QVector3D(0.2, -0.1, -0.41), 1.737218, 0.632947, -0.15, 82, 2000);//(1.835492434, -0.444231574 for U=0.225)

	// HEG*(HRHEG)
	gratings_ << REIXSXESGratingInfo("HEG*(HRHEG)", 2600, 11461, 88.25, 350, QVector3D(20.278, 26.808, 16.465), QVector3D(0,-0.014,1), QVector3D(0.26,-0.08,-0.41), 1.84104, -1.007515, -0.15, 50, 1000); /// \todo angleCorrection, heightCorrection

	// HRMEG
	gratings_ << REIXSXESGratingInfo("HRMEG", 5400, 10029, 88, 350, QVector3D(-15.220, 26.535, 16.709), QVector3D(0,0,0), QVector3D(0.24,-0.261,-0.41), 1.7981517, -3.57879125, -0.15, 390, 4000);//, 1.521048, -0.453829

	// HRHEG
	gratings_ << REIXSXESGratingInfo("HRHEG (not installed)", 5400, 10029, 88, 350, QVector3D(-15.220, 26.535, 16.709), QVector3D(0,0,0), QVector3D(0.12,-0.261,-0.41), 1.7981517, -3.57879125, -0.15, 390, 4000);//, 1.521048, -0.453829

	// LEG 2nd Order:
	gratings_ << REIXSXESGratingInfo("LEG 2nd Order", 1186.04, 5025.6, 86, 350, QVector3D(20.529, -25.865, 12.892), QVector3D(0,0,0), QVector3D(-0.160, -0.22, -0.41), 4.0130868, 0.664364, -0.15, 80, 292);//3.973429915, 0.731106463 for u=-0.175, v=-0.175

	/*
	// LEG Low E:
	gratings_ << REIXSXESGratingInfo("LEG Low E", 593.02, 5025.6, 86, 350, QVector3D(20.529, -25.865, 12.892), QVector3D(0,0,-0.318), QVector3D(-0.212, -0.22, -0.41), 3.961, 0.345, -0.15, 40, 292);

	// IMP Low E:
	gratings_ << REIXSXESGratingInfo("Impurity Low E", 892.86, 6699.8, 87, 350, QVector3D(-14.970, -25.723, 13.507), QVector3D(0,0,-4.593), QVector3D(-0.684, -0.22, -0.41), 2.2, -3.5, -0.15, 68, 500); //(3.02695496, 0.64573899 for U=0.1, v=0.261)
	*/

	hexapodOrigin_ = QVector3D(2.418, 4.145, -430.478);
	detectorWidth_ = 40.;
}

AMDbObjectList REIXSXESCalibration2::dbReadGratings()
{
	AMDbObjectList rv;
	for(int i=0,cc=gratings_.count(); i<cc; ++i)
		rv << &(gratings_[i]);

	return rv;
}

void REIXSXESCalibration2::dbLoadGratings(const AMDbObjectList& newGratings)
{
	// get rid of existing gratings
	gratings_.clear();

	foreach(AMDbObject* dbo, newGratings) {
		REIXSXESGratingInfo* g = qobject_cast<REIXSXESGratingInfo*>(dbo);
		if(g)
			gratings_ << *g;

		dbo->deleteLater();
	}
}

AMControlInfoList REIXSXESCalibration2::computeSpectrometerPosition(int gratingIndex, double eV, double focusOffsetMm, double tiltOffsetDeg) const {

	AMControlInfoList rv;

	QVector3D xyz = hexapodXYZ(gratingIndex);
	QVector3D uvw = hexapodUVW(gratingIndex);
	QVector3D rst = hexapodRST(gratingIndex);

	double _beta = beta(gratingIndex, eV);
	double _rPrime = rPrime(gratingIndex, _beta) + focusOffsetMm;
	/*
	double _spectrometerAngle = spectrometerAngle(gratingIndex, _beta, _rPrime);
	*/
	double _liftHeight = liftHeight(spectrometerAngle(gratingIndex, _beta, _rPrime));

	double _tiltStage = tiltStage(_beta, tiltOffsetDeg);

	rv.append(AMControlInfo("spectrometerRotationDrive", _liftHeight, 0,0, "mm", 0.1, "Linear drive motor for spectrometer angle"));
	rv.append(AMControlInfo("detectorTranslation", _rPrime, 0,0, "mm", 0.1, "Detector translation"));
	rv.append(AMControlInfo("detectorTiltDrive", _tiltStage, 0.0, 0.0, "mm", 0.1, "Linear drive motor for detector tilt"));
	rv.append(AMControlInfo("hexapodX", xyz.x(), 0, 0, "mm", 0.1, "Hexapod X position"));
	rv.append(AMControlInfo("hexapodY", xyz.y(), 0, 0, "mm", 0.1, "Hexapod Y position"));
	rv.append(AMControlInfo("hexapodZ", xyz.z(), 0, 0, "mm", 0.1, "Hexapod Z position"));
	rv.append(AMControlInfo("hexapodU", uvw.x(), 0,0, "deg", 0.1, "Hexapod Tilt around X axis"));
	rv.append(AMControlInfo("hexapodV", uvw.y(), 0,0, "deg", 0.1, "Hexapod Tilt around Y axis"));
	rv.append(AMControlInfo("hexapodW", uvw.z(), 0,0, "deg", 0.1, "Hexapod Tilt around Z axis"));
	rv.append(AMControlInfo("hexapodR", rst.x(), 0,0, "mm", 0.1, "Hexapod rotation point R"));
	rv.append(AMControlInfo("hexapodS", rst.y(), 0,0, "mm", 0.1, "Hexapod rotation point S"));
	rv.append(AMControlInfo("hexapodT", rst.z(), 0,0, "mm", 0.1, "Hexapod rotation point T"));
	rv.append(AMControlInfo("endstationTranslation", gratings_.at(gratingIndex).endstationTranslation(), 0,0, "mm", 0.1, "Endstation Translation"));

	qDebug() << "Spectrometer Geometry calculations for: " << gratings_.at(gratingIndex).name() << eV << "eV," << focusOffsetMm << "mm defocus," << tiltOffsetDeg << "deg tilt offset:";
	qDebug() << "   beta:" << _beta;
	/*
	qDebug() << "   spectrometerAngle:" << _spectrometerAngle;
	*/
	qDebug() << "   lift (spectrometer rotation):" << _liftHeight;
	qDebug() << "   rPrime (including defocus):" << _rPrime;
	qDebug() << "   tilt drive (including offset):" << _tiltStage;
	qDebug() << "   Hexapod XYZ:" << xyz;
	qDebug() << "   Hexapod UVW:" << uvw;
	qDebug() << "   Hexapod RST:" << rst;

	return rv;
}

double REIXSXESCalibration2::computeEVFromSpectrometerPosition(int gratingIndex, double liftHeight, double rPrime, int order) const
{
	double beta = betaFromPosition(gratingIndex, spectrometerAngle(liftHeight), rPrime);
	return eV(gratingIndex, beta, order);
}
