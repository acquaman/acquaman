#include "VESPERSXRFScanConfiguration.h"
#include "acquaman/VESPERS/VESPERSXRFScanController.h"
#include "util/AMPeriodicTable.h"

VESPERSXRFScanConfiguration::VESPERSXRFScanConfiguration(XRFDetector *detector, QObject *parent)
	: AMScanConfiguration(parent)
{
	detector_ = detector;
	xrfDetectorInfo_ = detector_->toXRFInfo();

	xrfTable_ = new XRFPeriodicTable(AMPeriodicTable::table()->elementBySymbol("K")->Kalpha().second.toDouble(), detector_->maximumEnergy(), this);

	connect(xrfTable_, SIGNAL(addedRegionOfInterest(XRFElement*,QString)), detector_, SLOT(addRegionOfInterest(XRFElement*,QString)));
	connect(xrfTable_, SIGNAL(removedRegionOfInterest(XRFElement*,QString)), detector_, SLOT(removeRegionOfInterest(XRFElement*,QString)));
	// Cleans up anything that might have been missed.
	connect(xrfTable_, SIGNAL(removedAllRegionsOfInterest()), detector_, SLOT(clearRegionsOfInterest()));

	connect(detector_, SIGNAL(roisHaveValues(bool)), this, SLOT(onRoisHaveValues(bool)));
	connect(detector_, SIGNAL(externalRegionOfInterestChanged()), this, SLOT(onExternalRegionOfInterestChanged()));
}

VESPERSXRFScanConfiguration::VESPERSXRFScanConfiguration(QObject *parent)
	: AMScanConfiguration(parent)
{
}

AMScanConfiguration *VESPERSXRFScanConfiguration::createCopy() const
{
	return new VESPERSXRFScanConfiguration(*this);
}

AMScanController *VESPERSXRFScanConfiguration::createController()
{
	return new VESPERSXRFScanController(this);
}

QString VESPERSXRFScanConfiguration::detailedDescription() const
{
	if (!xrfDetectorInfo_.name().isEmpty())
		return QString("XRF Free Run Scan\nDetector: %1\nReal time: %2 s").arg(xrfDetectorInfo_.name()).arg(xrfDetectorInfo_.integrationTime());

	return QString();
}

void VESPERSXRFScanConfiguration::onRoisHaveValues(bool hasValues)
{
	if (hasValues){

		// Go through all the regions of interest PVs and if there are any regions set already, pass them on to the rest of the program.
		QString name;

		for (int i = 0; i < detector_->roiList().count(); i++){

			name = detector_->roiList().at(i)->name();

			// If the name is empty then we've reached the end of the road for preset regions of interest.
			if (name.isEmpty()){

				xrfTable_->setCurrentElement(xrfTable_->elementBySymbol("Fe"));
				return;
			}

			addRegionOfInterestToTable(name);
		}
	}
}

void VESPERSXRFScanConfiguration::onExternalRegionOfInterestChanged()
{
	// Disconnect the detector from the table for a moment.
	disconnect(xrfTable_, SIGNAL(addedRegionOfInterest(XRFElement*,QString)), detector_, SLOT(addRegionOfInterest(XRFElement*,QString)));
	disconnect(xrfTable_, SIGNAL(removedRegionOfInterest(XRFElement*,QString)), detector_, SLOT(removeRegionOfInterest(XRFElement*,QString)));
	disconnect(xrfTable_, SIGNAL(removedAllRegionsOfInterest()), detector_, SLOT(clearRegionsOfInterest()));

	XRFElement *current = xrfTable_->currentElement();
	xrfTable_->removeAll();

	for (int i = 0; i < detector_->roiInfoList()->count(); i++)
		addRegionOfInterestToTable(detector_->roiInfoList()->at(i).name());

	xrfTable_->setCurrentElement(current);

	// Reconnect the detector to the table for a moment.
	connect(xrfTable_, SIGNAL(addedRegionOfInterest(XRFElement*,QString)), detector_, SLOT(addRegionOfInterest(XRFElement*,QString)));
	connect(xrfTable_, SIGNAL(removedRegionOfInterest(XRFElement*,QString)), detector_, SLOT(removeRegionOfInterest(XRFElement*,QString)));
	connect(xrfTable_, SIGNAL(removedAllRegionsOfInterest()), detector_, SLOT(clearRegionsOfInterest()));
}

void VESPERSXRFScanConfiguration::addRegionOfInterestToTable(QString name)
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
