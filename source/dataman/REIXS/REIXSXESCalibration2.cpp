#include "REIXSXESCalibration2.h"

#include "acquaman/REIXS/REIXSXESScanConfiguration.h"

#include <QDebug>

REIXSXESGratingInfo::REIXSXESGratingInfo(QObject *parent) :
	AMDbObject(parent)
{
	grooveDensity_ = 1200; // l/mm
	radius_ = 10000; //mm
	alpha_ = 89; // deg
	r_ = 350; //mm
	centerPosition_ = QVector3D(20.529, -25.865, 12.892);	// relative to "working coordinates": origin at center of spectromater rotation axle; oriented with hexapod axes.
	opticalOrigin_ = QVector3D(0,0,0);
	hexapodUVW_ = QVector3D(1,0,0);
	angleCorrection_ = 1.0;
	heightCorrection_ = 0.0;
	endstationTranslation_ = 0.0;
	evRangeMin_ = 100;
	evRangeMax_ = 800;
}

REIXSXESCalibration2::REIXSXESCalibration2(QObject *parent) :
    AMDbObject(parent)
{
	// create all default gratings

	// LEG:
	gratings_ << REIXSXESGratingInfo("LEG", 593.02, 5025.6, 86, 350, QVector3D(20.529, -25.865, 12.892), QVector3D(0,0,0), QVector3D(-0.175, -0.175, -0.41), 4.05666014, 0.349, -0.15, 50, 1000);
	// IMP:
	gratings_ << REIXSXESGratingInfo("Impurity", 892.86, 6699.8, 87, 350, QVector3D(-14.970, -25.723, 13.507), QVector3D(0,0,0), QVector3D(0.1, -0.261, -0.41), 3.02695496, 0.64573899, -0.15, 50, 1000); //2.947598786, 1.608997311
	// MEG:
	gratings_ << REIXSXESGratingInfo("MEG", 1187.819, 10018, 88, 350, QVector3D(20.374, 2.697, -18.923), QVector3D(0,-0.014,-1), QVector3D(0.225, -0.1, -0.41), 1.813308264, -0.019207904, -0.15, 50, 1000);
	// HEG*(HRHEG)
	gratings_ << REIXSXESGratingInfo("HEG*(HRHEG)", 2600, 11461, 88.25, 350, QVector3D(20.278, 26.808, 16.465), QVector3D(0,0,0), QVector3D(0.5,-0.261,-0.41), 2.8, 0.35, -0.15, 50, 1000); /// \todo angleCorrection, heightCorrection
	// HRMEG
	gratings_ << REIXSXESGratingInfo("HRMEG", 1800, 10029, 88, 350, QVector3D(-15.220, 26.535, 16.709), QVector3D(0,0,0), QVector3D(0.12,-0.261,-0.41), 2.8, 0.35, -0.15, 50, 1000); /// \todo angleCorrection, heightCorrection


	hexapodOrigin_ = QVector3D(2.418, 4.145, -430.478);
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

		delete dbo;
	}
}

AMControlInfoList REIXSXESCalibration2::computeSpectrometerPosition(REIXSXESScanConfiguration *scanConfiguration) const
{
	return computeSpectrometerPosition(scanConfiguration->gratingNumber(),
									   scanConfiguration->centerEV(),
									   scanConfiguration->defocusDistanceMm(),
									   scanConfiguration->detectorTiltOffset());
}

AMControlInfoList REIXSXESCalibration2::computeSpectrometerPosition(int gratingIndex, double eV, double focusOffsetMm, double tiltOffsetDeg) const {

	AMControlInfoList rv;

	QVector3D xyz = hexapodXYZ(gratingIndex);
	QVector3D uvw = hexapodUVW(gratingIndex);
	QVector3D rst = hexapodRST(gratingIndex);

	double _beta = beta(gratingIndex, eV);
	double _rPrime = rPrime(gratingIndex, _beta) + focusOffsetMm;
//	double _spectrometerAngle = spectrometerAngle(gratingIndex, _beta, _rPrime);
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
//	qDebug() << "   spectrometerAngle:" << _spectrometerAngle;
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
