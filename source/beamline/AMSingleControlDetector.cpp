#include "AMSingleControlDetector.h"

AMSingleControlDetector::AMSingleControlDetector(const QString& name, AMControl *control, QObject *parent) :
		AMDetectorInfo(name, name, parent), AMDetector(name)
{
	control_ = control;
	connect(control_, SIGNAL(connected(bool)), this, SLOT(onControlConnected(bool)));
	connect(control_, SIGNAL(valueChanged(double)), AMDetector::signalSource(), SIGNAL(readingsChanged()));
}

AMSingleControlDetector::~AMSingleControlDetector(){
	control_ = NULL;
}

const QMetaObject* AMSingleControlDetector::getMetaObject() {
	return metaObject();
}


AMDetectorInfo* AMSingleControlDetector::toInfo() const{
	return new AMDetectorInfo(*this);
}

bool AMSingleControlDetector::setControls(AMDetectorInfo *detectorSettings){
	return false;
}

AMControl* AMSingleControlDetector::control() {
	return control_;
}

bool AMSingleControlDetector::setFromInfo(const AMDetectorInfo *info){
	return false;
}

QString AMSingleControlDetector::description() const{
	return AMDetectorInfo::description();
}

void AMSingleControlDetector::setDescription(const QString &description){
	AMDetectorInfo::setDescription(description);
}

void AMSingleControlDetector::onControlConnected(bool connected){
	setConnected(connected);
}
