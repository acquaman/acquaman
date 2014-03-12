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

bool AMSamplePosition::positionWithinBounds() const{
	if(topLeftPosition().isEmpty() || bottomRightPosition().isEmpty())
		return true;

	if( (position().count() != topLeftPosition().count()) || (position().count() != bottomRightPosition().count()) )
		return false;

	for(int x = 0; x < position().count(); x++){
		double value = position().at(x).value();
		double topLeftValue = topLeftPosition().at(x).value();
		double topLeftTolerance = topLeftPosition().at(x).tolerance();
		double bottomRightValue = bottomRightPosition().at(x).value();
		double bottomRightTolerance = bottomRightPosition().at(x).tolerance();

		if(value > std::max(topLeftValue+topLeftTolerance, bottomRightValue+bottomRightTolerance) )
			return false;
		if(value < std::min(topLeftValue-topLeftTolerance, bottomRightValue-bottomRightTolerance) )
			return false;
	}

	return true;
}

bool AMSamplePosition::overlaps(const AMSamplePosition &other) const{
	if( ((topLeftPosition().isEmpty()) || (bottomRightPosition().isEmpty())) && ((other.topLeftPosition().isEmpty()) || (other.bottomRightPosition().isEmpty())) )
		return false;

	if( (topLeftPosition().isEmpty()) || (bottomRightPosition().isEmpty()) ){
		return other.matchesPosition(position());
	}
	else if( (other.topLeftPosition().isEmpty()) || (other.bottomRightPosition().isEmpty())){
		return matchesPosition(other.position());
	}
	else{
		double ax1, ax2, ay1, ay2, bx1, bx2, by1, by2;
		ax1 = std::min(topLeftPosition().at(0).value(), bottomRightPosition().at(0).value());
		ax2 = std::max(topLeftPosition().at(0).value(), bottomRightPosition().at(0).value());
		ay1 = std::min(topLeftPosition().at(2).value(), bottomRightPosition().at(2).value());
		ay2 = std::max(topLeftPosition().at(2).value(), bottomRightPosition().at(2).value());

		bx1 = std::min(other.topLeftPosition().at(0).value(), other.bottomRightPosition().at(0).value());
		bx2 = std::max(other.topLeftPosition().at(0).value(), other.bottomRightPosition().at(0).value());
		by1 = std::min(other.topLeftPosition().at(2).value(), other.bottomRightPosition().at(2).value());
		by2 = std::max(other.topLeftPosition().at(2).value(), other.bottomRightPosition().at(2).value());

		return (ax1 < bx2) && (ax2 > bx1) && (ay1 < by2) && (ay2 > by1);
	}
}

bool AMSamplePosition::matchesPosition(const AMControlInfoList &other) const{
	if(position() == other)
		return true;

	if(topLeftPosition().isEmpty() || bottomRightPosition().isEmpty())
		return false;

	if(position().count() != other.count() || topLeftPosition().count() != other.count() || bottomRightPosition().count() != other.count())
		return false;

	for(int x = 0; x < position().count(); x++){
		double otherValue = other.at(x).value();
		double topLeftValue = topLeftPosition().at(x).value();
		double topLeftTolerance = topLeftPosition().at(x).tolerance();
		double bottomRightValue = bottomRightPosition().at(x).value();
		double bottomRightTolerance = bottomRightPosition().at(x).tolerance();

		if(otherValue > std::max(topLeftValue+topLeftTolerance, bottomRightValue+bottomRightTolerance) )
			return false;
		if(otherValue < std::min(topLeftValue-topLeftTolerance, bottomRightValue-bottomRightTolerance) )
			return false;
	}

	return true;
}

bool AMSamplePosition::matchesPositionWithinTolerances(const AMControlInfoList &other, const QList<double> tolerances) const{
	if(position() == other)
		return true;

	if( (topLeftPosition().isEmpty() || bottomRightPosition().isEmpty()) && (tolerances.count() != position().count()) )
		return false;
	else if( (topLeftPosition().isEmpty() || bottomRightPosition().isEmpty()) && (tolerances.count() == position().count()) ){
		for(int x = 0; x < position().count(); x++){
			double otherValue = other.at(x).value();
			double topLeftValue = otherValue + tolerances.at(x);
			double bottomRightValue = otherValue - tolerances.at(x);

			if(otherValue > topLeftValue )
				return false;
			if(otherValue < bottomRightValue)
				return false;
		}
		return true;
	}

	if(position().count() != other.count() || topLeftPosition().count() != other.count() || bottomRightPosition().count() != other.count())
		return false;

	for(int x = 0; x < position().count(); x++){
		double otherValue = other.at(x).value();
		double topLeftValue = topLeftPosition().at(x).value();
		//double topLeftTolerance = topLeftPosition().at(x).tolerance();
		double topLeftTolerance = tolerances.at(x);
		double bottomRightValue = bottomRightPosition().at(x).value();
		//double bottomRightTolerance = bottomRightPosition().at(x).tolerance();
		double bottomRightTolerance = tolerances.at(x);

		if(otherValue > std::max(topLeftValue+topLeftTolerance, bottomRightValue+bottomRightTolerance) )
			return false;
		if(otherValue < std::min(topLeftValue-topLeftTolerance, bottomRightValue-bottomRightTolerance) )
			return false;
	}

	return true;
}

double AMSamplePosition::rms3SpaceDistance(const AMControlInfoList &other) const{
	if(matchesPosition(other))
		return 0;

	if( (other.count() != position().count()) || (other.count() < 3) )
		return 9999998;

	if( (topLeftPosition().isEmpty() || bottomRightPosition().isEmpty()) ){
		return sqrt( pow((position().at(0).value()-other.at(0).value()), 2) + pow((position().at(1).value()-other.at(1).value()), 2) + pow((position().at(2).value()-other.at(2).value()), 2) );
	}

	return 9999997;
}

 AMSamplePlate::~AMSamplePlate(){}
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
	if( tolerances.count() == 0 ){
		for(int x = count()-1; x >= 0; x--){
			if( at(x).matchesPosition(position) ){
				return at(x).sampleId();
			}
		}
	}
	else{
		double rmsDistance = 10000;
		double tmpRMS;
		int closestIndex = -1;
		for(int x = count()-1; x >= 0; x--){
			tmpRMS = 0;
			//if( at(x).position().compareWithinTolerance(position, tolerances)){
			if( at(x).matchesPositionWithinTolerances(position, tolerances)){
				/*
				for(int y = 0; y < position.count(); y++)
					tmpRMS += fabs(at(x).position().at(y).value() - position.at(y).value()) * fabs(at(x).position().at(y).value() - position.at(y).value());
				tmpRMS = sqrt(tmpRMS);
				*/
				tmpRMS = at(x).rms3SpaceDistance(position);
				if(tmpRMS < rmsDistance){
					rmsDistance = tmpRMS;
					closestIndex = x;
				}
			}
		}
		if(closestIndex != -1)
			return at(closestIndex).sampleId();
		return -1;
	}
	return -1;
}

bool AMSamplePlate::sampleAtIndexOverlaps(int index) const{
	for(int x = 0; x < count(); x++)
		if( (x != index) && (at(x).overlaps(at(index))))
			return true;

	return false;
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

	//qdebug() << "AMSamplePlate: loading positions in loadFromDb()";

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

	QDateTime beforeTime = QDateTime::currentDateTime();

	if(AMDbObject::loadFromDb(db,id)) {	// use normal loadFromDb(), but...
		if(count() == oldPositionsCount) {	// if the number of sample positions stayed the same, the dataChanged() notifications will be missing, because the new positions were loaded in-place to the old objects.
			for(int i=0; i<oldPositionsCount; i++)
				emit samplePositionChanged(i); // emit our own notification.
		}

		QDateTime afterTime = QDateTime::currentDateTime();
		//qdebug() << "Started at " << beforeTime.toString("hh:mm:ss.zzz") << " Ended at " << afterTime.toString("hh:mm:ss.zzz") << " Difference of " << afterTime.msecsTo(beforeTime);

		return true;
	}

	return false;
}

 AMSamplePosition::~AMSamplePosition(){}
