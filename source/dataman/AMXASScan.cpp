#include "AMXASScan.h"
#include <qdebug.h>

AMXASScan::AMXASScan(const QStringList& detectors, QObject *parent)
	: AMScan(parent)
{
	// setup default columns in data structure:
	d_.setXName("eV");

	// create columns for each detector (ex: "tey", "tfy", "urinalWaterTemp", etc.)
	qDebug() << "READY FOR DETECTORS";
	foreach(QString detect, detectors){
		if(addDetector(detect))
			qDebug() << "Success adding detector " << detect;
		else
			qDebug() << "FAILED adding detector " << detect;
	}


}

/// Add a new named detector (returns false if detector already exists)
bool AMXASScan::addDetector(const QString& uniqueDetectorName) {
	if(detectors_.contains(uniqueDetectorName))
		return false;
	detectors_ << uniqueDetectorName;
	d_.createColumn(uniqueDetectorName);
	return true;
}






