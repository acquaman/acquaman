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


#include "AMSamplePlate.h"
#include "util/AMErrorMonitor.h"
#include "util/AMDateTimeUtils.h"

#include "math.h"

AMSamplePlate::AMSamplePlate(QObject *parent) : AMDbObject(parent), AMOrderedList<AMSamplePosition>() {

	setName("New Sample Plate");
	dateTime_ = QDateTime::currentDateTime();

	// Forward internal signals (itemAdded, etc.) from our signalSource() as our own
	connect(signalSource(), SIGNAL(itemAboutToBeAdded(int)), this, SIGNAL(samplePositionAboutToBeAdded(int)));
	connect(signalSource(), SIGNAL(itemAdded(int)), this, SIGNAL(samplePositionAdded(int)));
	connect(signalSource(), SIGNAL(itemAboutToBeRemoved(int)), this, SIGNAL(samplePositionAboutToBeRemoved(int)));
	connect(signalSource(), SIGNAL(itemRemoved(int)), this, SIGNAL(samplePositionRemoved(int)));
	connect(signalSource(), SIGNAL(itemChanged(int)), this, SIGNAL(samplePositionChanged(int)));
}

AMSamplePlate::AMSamplePlate(const AMSamplePlate& other) : AMDbObject(), AMOrderedList<AMSamplePosition>(other) {
	// Forward internal signals (itemAdded, etc.) from our signalSource() as our own
	connect(signalSource(), SIGNAL(itemAboutToBeAdded(int)), this, SIGNAL(samplePositionAboutToBeAdded(int)));
	connect(signalSource(), SIGNAL(itemAdded(int)), this, SIGNAL(samplePositionAdded(int)));
	connect(signalSource(), SIGNAL(itemAboutToBeRemoved(int)), this, SIGNAL(samplePositionAboutToBeRemoved(int)));
	connect(signalSource(), SIGNAL(itemRemoved(int)), this, SIGNAL(samplePositionRemoved(int)));
	connect(signalSource(), SIGNAL(itemChanged(int)), this, SIGNAL(samplePositionChanged(int)));
}

// Using auto-generated assignment operator is fine

int AMSamplePlate::sampleIdAtPosition(const AMControlInfoList &position, const QList<double> tolerances) const{
	if(tolerances.count() == 0){
		for(int x = count()-1; x >= 0; x--)
			if( at(x).position() == position )
				return at(x).sampleId();
	}
	else{
		double rmsDistance = 10000;
		double tmpRMS;
		int closestIndex = -1;
		for(int x = count()-1; x >= 0; x--){
			tmpRMS = 0;
			if( at(x).position().compareWithinTolerance(position, tolerances)){
				for(int y = 0; y < position.count(); y++)
					tmpRMS += fabs(at(x).position().at(y).value() - position.at(y).value()) * fabs(at(x).position().at(y).value() - position.at(y).value());
				tmpRMS = sqrt(tmpRMS);
				if(tmpRMS < rmsDistance){
					rmsDistance = tmpRMS;
					closestIndex = x;
				}
			}
		}
		return at(closestIndex).sampleId();
	}
	return -1;
}



// Export the current positions to the database
AMDbObjectList AMSamplePlate::dbGetPositions() {
	AMDbObjectList rv;

	for(int i=0; i<count(); i++)
		rv << &(values_[i]);

	return rv;
}


// Load the positions for an existing sample plate from the database
void AMSamplePlate::dbLoadPositions(const AMDbObjectList& newPositions) {

	qDebug() << "AMSamplePlate: loading positions in loadFromDb()";

	clear();	// get rid of our existing

	for(int i=0; i<newPositions.count(); i++) {
		AMSamplePosition* newSamplePosition = qobject_cast<AMSamplePosition*>(newPositions.at(i));
		if(newSamplePosition) {
			append(*newSamplePosition);	// note: makes a copy of object pointed to by newControlInfo, and stores in our internal list. Uses default copy constructor in AMControlInfoList
		}
		else {
			AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -1, "Invalid position object found when trying to re-load from the database. Please report this bug to the Acquaman developers."));
		}

		if(newPositions.at(i))
			delete newPositions.at(i);	// we're copying these; don't need to keep these ones around. Our responsibility to delete.
	}
}

bool AMSamplePlate::loadFromDb(AMDatabase *db, int id)
{
	int oldPositionsCount = count();

	if(AMDbObject::loadFromDb(db,id)) {	// use normal loadFromDb(), but...
		if(count() == oldPositionsCount) {	// if the number of sample positions stayed the same, the dataChanged() notifications will be missing, because the new positions were loaded in-place to the old objects.
			for(int i=0; i<oldPositionsCount; i++)
				emit samplePositionChanged(i); // emit our own notification.
		}

		return true;
	}

	return false;
}

