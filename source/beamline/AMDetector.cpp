#include "AMDetector.h"


AMSingleControlDetector::AMSingleControlDetector(const QString& name, AMControl *control, QObject *parent) :
		AMDetectorInfo(name, name, parent)
{
	control_ = control;
}

AMSpectralOutputDetector::AMSpectralOutputDetector(const QString& name, AMControl *control, int numSpectrumBins, QString xElementName, QStringList yElementNames, QObject *parent) :
		AMSpectralOutputDetectorInfo(name, name, numSpectrumBins, xElementName, yElementNames, parent)
{
	control_ = control;
}
