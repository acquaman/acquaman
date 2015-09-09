#include "SGMMonochromatorInfo.h"
#include <cmath>
SGMMonochromatorInfo::SGMMonochromatorInfo(QObject *parent) :
    QObject(parent)
{
}

QHash<QString, double> SGMMonochromatorInfo::buildValues() const
{
    // Which of these are actually inputs: energy (obv.), grating(?)

    double sp = 1.69e-06;               // Does this change?
    double c1 = 2.45e-05;               // Does this change?
    double c2 = -1.59392;               // Does this change?
    double s = 509.468;                 // Does this change?
    double theta = 3.05575;             // Is this always the same as thetaM? Does it change?
	double energy = 530;

	int harmonic = 1;
	double undCor = 0;					// Undulator correction?

	double grating = 1.69e-06;			// Are sp and grating the same thing? Also is this the grating angle or some value derived from the grating translation enum?
    double thetaM = 3.05575;			// An angle, presumably. Grating angle maybe? Is the value always the same as theta?
    int m = 1;							// Is this always the same as the harmonic??
	double d = grating;

    double lambda = 0.000000002;        // Is this always the same? Does it represent anything, or is it just a mathematical construct.
	double mlambda_2d =  m * lambda / (2 * d);
	double cosThetaM_2 = cos(thetaM/2);
	double thetaI = asin(mlambda_2d/cosThetaM_2) + (thetaM / 2);
	double thetaD = -thetaM + thetaI;

	/* Where do these come from, will they ever change? */
	double k = 70480;
	double i = 1500;
	double l = 4546;

	double bottomA = (cos(thetaD + thetaM) + cos(thetaD)) / k;
	double bottomB = ((cos(thetaD + thetaM)) * (cos(thetaD + thetaM)) / i);
    double top = (cos(thetaD)) * (cos(thetaD));

    double monoEncoderTarget = (0.000000001*1239.842*s)/(2*sp*c1*c2*energy*cos(theta/2));   // What is the mono encoder target moving?
	double undulatorPosition = ((-1)/(0.14295709668))*log(((1)/(36.00511212946))*(((1737.41045746644)/((energy)/(harmonic)))-1))+undCor;
	double exitSlitPosition = -l+((top)/(bottomA-bottomB));

	QHash<QString, double> returnValues;

	returnValues.insert("MonoEncoderTarget", monoEncoderTarget);
	returnValues.insert("UndulatorPosition", undulatorPosition);
	returnValues.insert("ExitSlitPosition", exitSlitPosition);

    return returnValues;
}

bool nearEnough(double v1, double v2) {

    return (qAbs(v1 - v2) < 0.0001);
}

#include <QDebug>
void SGMMonochromatorInfo::testValue(const QString& name, double requiredEnergy, SGMGratingTranslation::GratingSelection gratingSelection, double expectedAngleEncoderTarget, double expectedExitSlitPosition, double expectedUndulatorPosition) const
{
    SGMGratingTranslation gratingTranslation(gratingSelection);
    double angleEncoderTarget_ = gratingAngleEncoderTargetFromEnergy(gratingTranslation, requiredEnergy);
    double undulatorPosition_ = undulatorPosition(requiredEnergy, FirstHarmonic, 0);
    double exitSlitPosition_ = exitSlitPosition(gratingTranslation, requiredEnergy);

    if(!nearEnough(angleEncoderTarget_, expectedAngleEncoderTarget)) {
        qDebug() << name << " angle encoder test failed";
        qDebug() << "\tExpected: " << expectedAngleEncoderTarget << " Actual: " << angleEncoderTarget_;
    }

    if(!nearEnough(undulatorPosition_, expectedUndulatorPosition)) {
        qDebug() << name << " undulator position test failed";
        qDebug() << "\tExpected: " << expectedUndulatorPosition << " Actual: " << undulatorPosition_;
    }

    if(!nearEnough(exitSlitPosition_, expectedExitSlitPosition)) {
        qDebug() << name << " exit slit position test failed";
        qDebug() << "\tExpected: " << expectedExitSlitPosition << " Actual: " << exitSlitPosition_;
    }
}

void SGMMonochromatorInfo::testValues() const
{
    // Carbon K Edge:
    testValue("Carbon K Edge Start", 270.0, SGMGratingTranslation::LowGrating, -397720, 286.63, -14991);
    testValue("Carbon K Edge Middle", 295.0, SGMGratingTranslation::LowGrating, -377497, 200.46, -140470);
    testValue("Carbon K Edge End", 320.0, SGMGratingTranslation::LowGrating, -348005, 100.54, -133061);

    // Nitrogen K Edge:
    testValue("Nitrogen K Edge Start", 380.0, SGMGratingTranslation::LowGrating, -293057, 1.37942, -115321);
    testValue("Nitrogen K Edge Middle", 410.0, SGMGratingTranslation::LowGrating, -268341, 3.88, -105051);
    testValue("Nitrogen K Edge End", 440.0, SGMGratingTranslation::LowGrating, -253095, 15.5151, -99461);

    // Oxygen K Edge:
    testValue("Oxygen K Edge Start", 530.0, SGMGratingTranslation::LowGrating, -210116, 166.586, -77335);
    testValue("Oxygen K Edge Middle", 545.0, SGMGratingTranslation::LowGrating, -204333, 198.591, -73988);
    testValue("Oxygen K Edge End", 560.0, SGMGratingTranslation::LowGrating, -198860, 232.356, -70692);

}

double SGMMonochromatorInfo::energyFromGrating(SGMGratingTranslation gratingTranslation, double gratingAngleEncoderTarget)
{
    return 1e-9 * 1239.842 / ((2 * gratingTranslation.spacing() * gratingTranslation.c1() * gratingTranslation.c2() * gratingAngleEncoderTarget) / gratingTranslation.radiusCurvatureOffset() * cos(gratingTranslation.thetaM() / 2));
}

double SGMMonochromatorInfo::gratingAngleEncoderTargetFromEnergy(SGMGratingTranslation gratingTranslation, double energy)
{
    return 1e-9 * 1239.842 / ((2 * gratingTranslation.spacing() * gratingTranslation.c1() * gratingTranslation.c2() * energy) / gratingTranslation.radiusCurvatureOffset() * cos(gratingTranslation.thetaM() / 2));
}

double SGMMonochromatorInfo::undulatorPosition(double energy, UndulatorHarmonic undulatorHarmonic, double undulatorOffset)
{
    return (-1/0.14295709668) * log( (1/36.00511212946)*((1737.41045746644/(energy/int(undulatorHarmonic))) -1)) + undulatorOffset;
}

double SGMMonochromatorInfo::exitSlitPosition(SGMGratingTranslation gratingTranslation, double energy)
{
    double xOffset = 4600; // Need to research this value. Seems to have been altered over time.
    double lambda = (1239.842 / energy) * 1.0e-9;
    double thetaD = -gratingTranslation.thetaM() + asin((lambda) / (2* int(gratingTranslation.gratingSelection()))*(1/(cos(gratingTranslation.thetaM() / 2)))) + gratingTranslation.thetaM() / 2;

    return -xOffset + (pow(cos(thetaD),2)) / (((cos(thetaD + gratingTranslation.thetaM()) + cos(thetaD)) / 70480) - (pow(cos(thetaD + gratingTranslation.thetaM()),2))/1500);
}


