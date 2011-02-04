#include "AMDetectorInfoList.h"

AMDetectorInfoSet::AMDetectorInfoSet(QObject *parent) :
	QObject(parent)
{
}

//AMDetectorInfoSet::AMDetectorInfoSet(AMDetectorInfoSet *copy, QObject *parent) :
//		QObject(parent)
//{
//
//}

int AMDetectorInfoSet::indexOf(const QString &name){
	for(int x = 0; x < detectors_.count(); x++)
		if(name == detectors_.at(x)->name())
			return x;
	return -1;
}

AMDetectorInfo* AMDetectorInfoSet::detectorByName(const QString &name){
	int index = indexOf(name);
	if(index != -1)
		return detectorAt(index);
	else
		return NULL;
}

bool AMDetectorInfoSet::isDefaultByName(const QString &name){
	int index = indexOf(name);
	if(index != -1)
		return isDefaultAt(index);
	else
		return false;
}

/// Returns false if the AMControl to be added is already in the list; otherwise adds the control and returns true.
bool AMDetectorInfoSet::addDetector(AMDetectorInfo* detector, bool defaultDetector) {
	if(detectors_.contains(detector))
		return false;
	detectors_.append(detector);
	defaultDetectors_.append(defaultDetector);
	return true;
}

/// Returns false if the AMControl to be removed is not present; otherwise removes the control and returns true.
bool AMDetectorInfoSet::removeDetector(AMDetectorInfo* detector) {
	return detectors_.removeOne(detector);
}
