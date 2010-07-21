#include "AMControlSet.h"

AMControlSet::AMControlSet(QObject *parent) :
	QObject(parent)
{
}

int AMControlSet::indexOf(const QString &name){
	for(int x = 0; x < ctrls_.count(); x++)
		if(name == ctrls_.at(x)->name())
			return x;
	return -1;
}

AMControl* AMControlSet::controlByName(const QString &name){
	int index = indexOf(name);
	if(index != -1)
		return controlAt(index);
	else
		return NULL;
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
QMap<double, double> AMControlOptimization::curve(QList<QVariant> stateParameters, AMRegionsList* contextParameters) {
	QMap<double, double> noMap;
	Q_UNUSED(stateParameters);
	Q_UNUSED(contextParameters);
	return noMap;
}

QMap<QString, QMap<double, double> > AMControlOptimization::collapse(AMRegionsList* contextParameters) {
	QMap<QString, QMap<double, double> > noMap;
	Q_UNUSED(contextParameters);
	return noMap;
}


AMAbstractDetectorSet::AMAbstractDetectorSet(QObject *parent) :
	QObject(parent)
{
}

int AMAbstractDetectorSet::indexOf(const QString &name){
	for(int x = 0; x < detectors_.count(); x++)
		if(name == detectors_.at(x)->name())
			return x;
	return -1;
}

AMAbstractDetector* AMAbstractDetectorSet::detectorByName(const QString &name){
	int index = indexOf(name);
	if(index != -1)
		return detectorAt(index);
	else
		return NULL;
}

bool AMAbstractDetectorSet::isDefaultByName(const QString &name){
	int index = indexOf(name);
	if(index != -1)
		return isDefaultAt(index);
	else
		return false;
}

/// Returns false if the AMControl to be added is already in the list; otherwise adds the control and returns true.
bool AMAbstractDetectorSet::addDetector(AMAbstractDetector* detector, bool defaultDetector) {
	if(detectors_.contains(detector))
		return false;
	detectors_.append(detector);
	defaultDetectors_.append(defaultDetector);
	return true;
}

/// Returns false if the AMControl to be removed is not present; otherwise removes the control and returns true.
bool AMAbstractDetectorSet::removeDetector(AMAbstractDetector* detector) {
	return detectors_.removeOne(detector);
}
