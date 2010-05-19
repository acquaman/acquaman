#include "AMDetector.h"

AMDetector::AMDetector(QString name, QObject *parent) :
	QObject(parent)
{
	name_ = name;
}

AMSingleControlDetector::AMSingleControlDetector(QString name, AMControl *control, QObject *parent) :
		AMDetector(name, parent)
{
	control_ = control;
}
