#include "AMXASScan.h"
#include <qdebug.h>

AMXASScan::AMXASScan(const QList<AMDetectorInfo*> &detectors, QObject *parent)
	: AMScan(parent)
{
	// setup default columns in data structure:
	d_->setXName("eV");

	// create columns for each detector (ex: "tey", "tfy", "urinalWaterTemp", etc.)

	foreach(AMDetectorInfo *detect, detectors) {
		addDetector(detect);
	}


}

/// Add a new named detector (returns false if detector already exists)
bool AMXASScan::addDetector(const AMDetectorInfo *uniqueDetector) {

#warning("Mark or David, come back and clean this up.  What if the number of binNames is not the same as binCount?")

	if(detectorNames_.contains(uniqueDetector->name()))
		return false;
	detectors_ << uniqueDetector;
	detectorNames_ << uniqueDetector->name();
//	qDebug() << "Adding column for " << uniqueDetector->name();
	if(!uniqueDetector->isSpectralOutput())
		d_->createColumn(uniqueDetector->name());
	else{
		AMSpectralOutputDetector *specDtctr = (AMSpectralOutputDetector*)uniqueDetector;
//		qDebug() << "CREATING " << specDtctr->numSpectrumBins() << " SUBTREE FOR PGT";
		AMDataTree *tmpTree;

		/// \todo What is the difference between these two?
		if(specDtctr->axisName().isEmpty())
			tmpTree = new AMDataTree(specDtctr->binCount(), "x", false);
		else
			tmpTree = new AMDataTree(specDtctr->binCount(), "pgtEV", true);

		for(int x = 0; x < specDtctr->binNames().count(); x++)
			tmpTree->createColumn(specDtctr->binNames().at(x));
		d_->createSubtreeColumn(specDtctr->name(), tmpTree);
//		qDebug() << "Subx is " << tmpTree->xName();
//		for(int x = 0; x < tmpTree->yColumnNames().count(); x++)
//			qDebug() << " Suby" << x << " is " << tmpTree->yColumnNames().at(x);
	}
	return true;
}


#include "dataman/SGM2004FileLoader.h"
#include "dataman/ALSBL8XASFileLoader.h"

bool AMXASScan::loadDataImplementation() {

	SGM2004FileLoader sgmLoader(this);

	if(fileFormat() == sgmLoader.formatTag()) {
		if(sgmLoader.loadFromFile(filePath(), false, false)) {
			return true;
		}
		else {
			AMErrorMon::report(AMErrorReport(this, AMErrorReport::Serious, -1, QString("Could not load raw XAS scan data from '%1'").arg(filePath())));
			return false;
		}
	}


	ALSBL8XASFileLoader alsLoader(this);

	if(fileFormat() == alsLoader.formatTag()) {
		if(alsLoader.loadFromFile(filePath(), false, false)) {
			return true;
		}
		else {
			AMErrorMon::report(AMErrorReport(this, AMErrorReport::Serious, -1, QString("Could not load raw XAS scan data from '%1'").arg(filePath())));
			return false;
		}
	}



	AMErrorMon::report(AMErrorReport(this, AMErrorReport::Serious, -1, QString("Could not load raw XAS scan data. The '%1' file format isn't supported.").arg(fileFormat())));
	return false;
}



