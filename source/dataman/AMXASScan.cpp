#include "AMXASScan.h"


AMXASScan::AMXASScan(const QStringList& detectors, QObject *parent)
	: AMScan(parent),
	detectors_(detectors)
{
	// setup default columns in data structure:
	d_.setXName("eV");

	// create columns for each detector (ex: "tey", "tfy", "urinalWaterTemp", etc.)
	foreach(QString detect, detectors_)
		addDetector(detect);


}

/// Add a new named detector (returns false if detector already exists)
bool AMXASScan::addDetector(const QString& uniqueDetectorName) {
	if(detectors_.contains(uniqueDetectorName))
		return false;
	detectors_ << uniqueDetectorName;
	d_.createColumn(uniqueDetectorName);
	return true;
}






