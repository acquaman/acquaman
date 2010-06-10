#include "AMAbstractDetector.h"

AMAbstractDetector::AMAbstractDetector(QString name, bool spectralOutput, QObject *parent) :
		QObject(parent)
{
	name_ = name;
	isSpectralOutput_ = spectralOutput;
}

AMAbstractSpectralOutputDetector::AMAbstractSpectralOutputDetector(QString name, int numSpectrumBins, QString xElementName, QStringList yElementNames, QObject *parent) :
		AMAbstractDetector(name, true, parent)
{
	numSpectrumBins_ = numSpectrumBins;
	xElementName_ = xElementName;
	yElementNames_ = yElementNames;
}
