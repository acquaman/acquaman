#include "XRFFreeRun.h"

#include "util/AMPeriodicTable.h"

XRFFreeRun::XRFFreeRun(QObject *parent) :
	QObject(parent)
{
	xrfTable_ = new XRFPeriodicTable(AMPeriodicTable::table()->elementBySymbol("K")->Kalpha().second.toDouble(), detector_->maximumEnergy(), this);
	xrfTable_->setCurrentElement(xrfTable_->elementBySymbol("Fe"));

	connect(xrfTable_, SIGNAL(addedRegionOfInterest(XRFElement*,QString)), detector_, SLOT(addRegionOfInterest(XRFElement*,QString)));
	connect(xrfTable_, SIGNAL(removedRegionOfInterest(XRFElement*,QString)), detector_, SLOT(removeRegionOfInterest(XRFElement*,QString)));
	// Cleans up anything that might have been missed.
	connect(xrfTable_, SIGNAL(removedAllRegionsOfInterest()), detector_, SLOT(clearRegionsOfInterest()));

	connect(detector_, SIGNAL(roisHaveValues()), this, SLOT(onRoisHaveValues()));
}

void XRFFreeRun::onRoisHaveValues()
{
	disconnect(detector_, SIGNAL(externalRegionsOfInterestChanged()), this, SLOT(onExternalRegionsOfInterestChanged()));
	disconnect(xrfTable_, SIGNAL(removedAllRegionsOfInterest()), detector_, SLOT(clearRegionsOfInterest()));

	XRFElement *current = xrfTable_->currentElement();
	xrfTable_->removeAll();

	// Go through all the regions of interest PVs and if there are any regions set already, pass them on to the rest of the program.
	QString name;

	for (int i = 0; i < detector_->roiList().count(); i++){

		name = detector_->roiList().at(i)->name();

		// If the name is empty then we've reached the end of the road for preset regions of interest.
		if (name.isEmpty()){

			if (current)
				xrfTable_->setCurrentElement(current);
			else
				xrfTable_->setCurrentElement(xrfTable_->elementBySymbol("Fe"));

			connect(detector_, SIGNAL(externalRegionsOfInterestChanged()), this, SLOT(onExternalRegionsOfInterestChanged()));
			connect(xrfTable_, SIGNAL(removedAllRegionsOfInterest()), detector_, SLOT(clearRegionsOfInterest()));

			return;
		}

		addRegionOfInterestToTable(name);
	}
}

void XRFFreeRun::onExternalRegionsOfInterestChanged()
{
	onRoisHaveValues();
}

void XRFFreeRun::addRegionOfInterestToTable(QString name)
{
	QString temp = name.left(name.indexOf(" "));
	XRFElement *el = xrfTable_->elementBySymbol(temp);

	if (el){

		temp = name;

		for (int j = 0; j < el->emissionLines().count(); j++){

			if (temp.compare(el->symbol()+" "+GeneralUtilities::removeGreek(el->emissionLines().at(j).first)) == 0){

				xrfTable_->blockSignals(true);
				xrfTable_->setCurrentElement(el);
				xrfTable_->blockSignals(false);
				xrfTable_->addLineToList(el->emissionLines().at(j).first);
			}
		}
	}
}
