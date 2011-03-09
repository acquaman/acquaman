#include "AMSpectralOutputDetector.h"

AMSpectralOutputDetector::AMSpectralOutputDetector(const QString& name, AMControl *control, int numSpectrumBins, QString xElementName, QStringList yElementNames, QObject *parent) :
		AMSpectralOutputDetectorInfo(name, name, numSpectrumBins, xElementName, yElementNames, parent), AMDetector(name)
{
	control_ = control;
	connect(control_, SIGNAL(connected(bool)), AMDetector::signalSource(), SIGNAL(ready(bool)));
}

AMSpectralOutputDetector::~AMSpectralOutputDetector() {
	control_ = NULL;
}

const QMetaObject* AMSpectralOutputDetector::getMetaObject() {
	return metaObject();
}

AMDetectorInfo AMSpectralOutputDetector::toInfo(){

}

bool AMSpectralOutputDetector::setControls(AMSpectralOutputDetectorInfo *detectorSettings){
	return false;
}

bool AMSpectralOutputDetector::setFromInfo(const AMDetectorInfo &info){
	return false;
}
