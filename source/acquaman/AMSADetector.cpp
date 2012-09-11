#include "AMSADetector.h"

AMSADetector::AMSADetector(const QString &name, const QString &description, QObject *parent)
	: QObject(parent), measurementInfo_(name, description)
{

}

AMSADetector::~AMSADetector() {

}
