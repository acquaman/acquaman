/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "XRFFreeRun.h"

#include "util/AMPeriodicTable.h"

XRFFreeRun::XRFFreeRun(XRFDetector *detector, QObject *parent)
	: QObject(parent)
{
	detector_ = detector;
	config_ = new VESPERSXRFScanConfiguration(detector_->toXRFInfo());
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
	/// \note To fix my current problem with ROIs updating when they shouldn't, I might just need to disconnect ALL the signals to the detector.  Worth a try anyway.
	disconnect(xrfTable_, SIGNAL(addedRegionOfInterest(XRFElement*,QString)), detector_, SLOT(addRegionOfInterest(XRFElement*,QString)));
	disconnect(xrfTable_, SIGNAL(removedRegionOfInterest(XRFElement*,QString)), detector_, SLOT(removeRegionOfInterest(XRFElement*,QString)));
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

			connect(xrfTable_, SIGNAL(addedRegionOfInterest(XRFElement*,QString)), detector_, SLOT(addRegionOfInterest(XRFElement*,QString)));
			connect(xrfTable_, SIGNAL(removedRegionOfInterest(XRFElement*,QString)), detector_, SLOT(removeRegionOfInterest(XRFElement*,QString)));
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
				detector_->addRegionOfInterest(el, el->emissionLines().at(j).first, false);
			}
		}
	}
}
