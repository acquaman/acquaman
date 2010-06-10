#include "AMXASScan.h"
#include <qdebug.h>

AMXASScan::AMXASScan(const QList<AMAbstractDetector*> &detectors, QObject *parent)
	: AMScan(parent)
{
	// setup default columns in data structure:
	d_->setXName("eV");

	// create columns for each detector (ex: "tey", "tfy", "urinalWaterTemp", etc.)

	foreach(AMAbstractDetector *detect, detectors) {
		addDetector(detect);
	}


}

/// Add a new named detector (returns false if detector already exists)
bool AMXASScan::addDetector(const AMAbstractDetector *uniqueDetector) {
	if(detectorNames_.contains(uniqueDetector->name()))
		return false;
	detectors_ << uniqueDetector;
	detectorNames_ << uniqueDetector->name();
	qDebug() << "Adding column for " << uniqueDetector->name();
	if(!uniqueDetector->isSpectralOutput())
		d_->createColumn(uniqueDetector->name());
	else{
		AMSpectralOutputDetector *specDtctr = (AMSpectralOutputDetector*)uniqueDetector;
		qDebug() << "CREATING " << specDtctr->numSpectrumBins() << " SUBTREE FOR PGT";
		AMDataTree *tmpTree;
		if(specDtctr->xElementName().isEmpty())
			tmpTree = new AMDataTree(specDtctr->numSpectrumBins(), "x", false);
		else
			tmpTree = new AMDataTree(specDtctr->numSpectrumBins(), "pgtEV", true);
		for(int x = 0; x < specDtctr->yElementNames().count(); x++)
			tmpTree->createColumn(specDtctr->yElementNames().at(x));
		d_->createSubtreeColumn(specDtctr->name(), tmpTree);
		qDebug() << "Subx is " << tmpTree->xName();
		for(int x = 0; x < tmpTree->yColumnNames().count(); x++)
			qDebug() << " Suby" << x << " is " << tmpTree->yColumnNames().at(x);
	}
	return true;
}






