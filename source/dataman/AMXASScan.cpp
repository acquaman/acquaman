#include "AMXASScan.h"
#include <qdebug.h>

AMXASScan::AMXASScan(const QStringList& detectors, QObject *parent)
	: AMScan(parent)
{
	// setup default columns in data structure:
	d_->setXName("eV");

	// create columns for each detector (ex: "tey", "tfy", "urinalWaterTemp", etc.)

	foreach(QString detect, detectors) {
		addDetector(detect);
	}


}

/// Add a new named detector (returns false if detector already exists)
bool AMXASScan::addDetector(const QString& uniqueDetectorName) {
	if(detectors_.contains(uniqueDetectorName))
		return false;
	detectors_ << uniqueDetectorName;
	qDebug() << "Adding column for " << uniqueDetectorName;
	if(uniqueDetectorName != "pgt")
		d_->createColumn(uniqueDetectorName);
	else{
		qDebug() << "CREATING 1024 SUBTREE FOR PGT";
		AMDataTree *tmpTree = new AMDataTree(1024, "pgtEV", true);
		tmpTree->createColumn("pgtCounts");
		d_->createSubtreeColumn(uniqueDetectorName, tmpTree);
		qDebug() << "Subx is " << tmpTree->xName() << " Suby0 is " << tmpTree->yColumnNames().at(0);
	}
	return true;
}






