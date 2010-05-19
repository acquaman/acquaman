#include "AMControlSet.h"

AMControlSet::AMControlSet(QObject *parent) :
	QObject(parent)
{
}

/// Returns false if the AMControl to be added is already in the list; otherwise adds the control and returns true.
bool AMControlSet::addControl(AMControl* ctrl) {
	if(ctrls_.contains(ctrl))
		return false;
	ctrls_.append(ctrl);
	return true;
}

/// Returns false if the AMControl to be removed is not present; otherwise removes the control and returns true.
bool AMControlSet::removeControl(AMControl* ctrl) {
	return ctrls_.removeOne(ctrl);
}

/// Default implementation returns an empty map. This function is the core of the implementation for subclasses.
//QMap<double, double> AMControlOptimization::curve(QList<QVariant> stateParameters, QList<AMRegion*> contextParameters) {
QMap<double, double> AMControlOptimization::curve(QList<QVariant> stateParameters, AMRegionsList* contextParameters) {
	qDebug() << "I should never be doing this curve call";
	QMap<double, double> noMap;
	Q_UNUSED(stateParameters);
	Q_UNUSED(contextParameters);
	return noMap;
}


AMDetectorSet::AMDetectorSet(QObject *parent) :
	QObject(parent)
{
}

/// Returns false if the AMControl to be added is already in the list; otherwise adds the control and returns true.
bool AMDetectorSet::addDetector(AMDetector* detector) {
	if(detectors_.contains(detector))
		return false;
	detectors_.append(detector);
	return true;
}

/// Returns false if the AMControl to be removed is not present; otherwise removes the control and returns true.
bool AMDetectorSet::removeDetector(AMDetector* detector) {
	return detectors_.removeOne(detector);
}
