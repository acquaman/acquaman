#include "SGMMonochromatorInfo.h"
#include <cmath>
SGMMonochromatorInfo::SGMMonochromatorInfo(QObject *parent) :
    QObject(parent)
{
}

QHash<QString, double> SGMMonochromatorInfo::buildValues() const
{
	double sp = 1.69e-06;
	double c1 = 2.45e-05;
	double c2 = -1.59392;
	double s = 509.468;
	double theta = 3.05575;
	double energy = 530;

	int harmonic = 1;
	double undCor = 0;					// Undulator correction?

	double grating = 1.69e-06;			// Are sp and grating the same thing? Also is this the grating angle or some value derived from the grating translation enum?
	double thetaM = 3.05575;			// An angle, grating maybe?
	int m = 1;							// Is this harmonic??
	double d = grating;

	double lambda = 0.000000002;
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

	double monoEncoderTarget = (0.000000001*1239.842*s)/(2*sp*c1*c2*energy*cos(theta/2));
	double undulatorPosition = ((-1)/(0.14295709668))*log(((1)/(36.00511212946))*(((1737.41045746644)/((energy)/(harmonic)))-1))+undCor;
	double exitSlitPosition = -l+((top)/(bottomA-bottomB));

	QHash<QString, double> returnValues;

	returnValues.insert("MonoEncoderTarget", monoEncoderTarget);
	returnValues.insert("UndulatorPosition", undulatorPosition);
	returnValues.insert("ExitSlitPosition", exitSlitPosition);

	return returnValues;
}

#include <QDebug>
void SGMMonochromatorInfo::testValues() const
{
	QHash<QString, double> componentPositions = buildValues();
	foreach(QString key, componentPositions.keys()) {

		qDebug() << "Key: " << key << " Value: " << componentPositions.value(key);
	}
}
