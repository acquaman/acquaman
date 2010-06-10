#include "AMDetector.h"

/*
AMDetector::AMDetector(QString name, bool spectralOutput, QObject *parent) :
	QObject(parent)
{
	name_ = name;
	isSpectralOutput_ = spectralOutput;
}
*/

AMSingleControlDetector::AMSingleControlDetector(QString name, AMControl *control, QObject *parent) :
		AMDetector(name, false, parent)
{
	control_ = control;
}

AMSpectralOutputDetector::AMSpectralOutputDetector(QString name, AMControl *control, int numSpectrumBins, QString xElementName, QStringList yElementNames, QObject *parent) :
		AMAbstractSpectralOutputDetector(name, numSpectrumBins, xElementName, yElementNames, parent)
{
	control_ = control;
}
