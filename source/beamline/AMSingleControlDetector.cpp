#include "AMSingleControlDetector.h"

AMSingleControlDetector::AMSingleControlDetector(const QString& name, AMControl *control, QObject *parent) :
		AMDetectorInfo(name, name, parent), AMDetector(name)
{
	control_ = control;
	connect(control_, SIGNAL(connected(bool)), this, SLOT(onControlConnected(bool)));
	connect(control_, SIGNAL(valueChanged(double)), AMDetector::signalSource(), SIGNAL(valuesChanged()));
}

AMSingleControlDetector::~AMSingleControlDetector(){
	control_ = NULL;
}

const QMetaObject* AMSingleControlDetector::getMetaObject() {
	return metaObject();
}


AMDetectorInfo AMSingleControlDetector::toInfo(){
	return AMDetectorInfo(*this);
}

bool AMSingleControlDetector::setControls(AMDetectorInfo *detectorSettings){
	return false;
}

AMControl* AMSingleControlDetector::control() {
	return control_;
}

bool AMSingleControlDetector::setFromInfo(const AMDetectorInfo &info){
	return false;
}

void AMSingleControlDetector::onControlConnected(bool connected){
	setConnected(connected);
}
