/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#include "AMScanAxis.h"
#include <math.h>
#include <QDebug>

AMScanAxis::~AMScanAxis(){}

AMScanAxis::AMScanAxis(AMScanAxis::AxisType axisType, AMScanAxisRegion *firstRegion, QObject *parent)
	: AMDbObject(parent)
{
	setAxisType(axisType);

	if (firstRegion->name() == "Unnamed Object")
		firstRegion->setName(name());

	regions_.append(firstRegion);

	connect(firstRegion, SIGNAL(regionStartChanged(AMNumber)), this, SIGNAL(regionsChanged()));
	connect(firstRegion, SIGNAL(regionStepChanged(AMNumber)), this, SIGNAL(regionsChanged()));
	connect(firstRegion, SIGNAL(regionEndChanged(AMNumber)), this, SIGNAL(regionsChanged()));
	connect(firstRegion, SIGNAL(regionTimeChanged(AMNumber)), this, SIGNAL(regionsChanged()));

	axisValid_ = AMScanAxis::InvalidAxis;
	checkAxisValidity();
}

AMScanAxis::AMScanAxis(const AMScanAxis &original)
	: AMDbObject(original)
{
	setAxisType(original.axisType());

	foreach (AMScanAxisRegion *region, original.regions().toList()){
		regions_.append(region->createCopy());

		connect(region, SIGNAL(regionStartChanged(AMNumber)), this, SIGNAL(regionsChanged()));
		connect(region, SIGNAL(regionStepChanged(AMNumber)), this, SIGNAL(regionsChanged()));
		connect(region, SIGNAL(regionEndChanged(AMNumber)), this, SIGNAL(regionsChanged()));
		connect(region, SIGNAL(regionTimeChanged(AMNumber)), this, SIGNAL(regionsChanged()));
	}

	checkAxisValidity();
}

AMScanAxis *AMScanAxis::createCopy() const
{
	AMScanAxis *axis = new AMScanAxis(*this);
	axis->dissociateFromDb(true);
	return axis;
}

AMScanAxis::AxisType AMScanAxis::axisType() const
{
	return axisType_;
}

AMScanAxis::AxisValid AMScanAxis::axisValid() const
{
	return axisValid_;
}

const AMOrderedList<AMScanAxisRegion *> AMScanAxis::regions() const
{
	return regions_;
}

AMScanAxisRegion *AMScanAxis::regionAt(int index) const
{
	return regions_.at(index);
}

int AMScanAxis::regionCount() const
{
	return regions_.count();
}

AMNumber AMScanAxis::axisStart() const
{
	if(axisType_ == AMScanAxis::ContinuousDwellAxis || regionCount() == 0)
		return AMNumber(AMNumber::Null);

	return regions_.at(0)->regionStart();
}

AMNumber AMScanAxis::axisEnd() const
{
	if(axisType_ == AMScanAxis::ContinuousDwellAxis || regionCount() == 0)
		return AMNumber(AMNumber::Null);

	return regions_.at(regionCount()-1)->regionEnd();
}

int AMScanAxis::numberOfPoints() const
{
	int points = 0;

	foreach (AMScanAxisRegion *region, regions_.toList())
		points += region->numberOfPoints();

	return points;
}

double AMScanAxis::timePerAxis() const
{
	double time = 0;

	foreach (AMScanAxisRegion *region, regions_.toList())
		time += region->timePerRegion();

	return time;
}

QString AMScanAxis::toString(const QString &units) const
{
	QString string = "";
	QList<AMScanAxisRegion*> regions = regions_.toList();

	for (int i = 0, count = regions.count(); i < count; i++) {
		AMScanAxisRegion *region = regions.at(i);

		if (region)
			string.append(region->toString(units));
		else
			string.append("--- null region ---");

		if (i != count - 1)
			string.append("\n");
	}

	return string;
}

bool AMScanAxis::ascending() const
{
	bool result = false;
	QList<AMScanAxisRegion*> regions = regions_.toList();

	if (!regions.isEmpty()) {
		bool areAscending = true;

		for (int regionIndex = 0, regionCount = regions.count(); regionIndex < regionCount && areAscending; regionIndex++) {
			AMScanAxisRegion *region = regions.at(regionIndex);

			if (region)
				areAscending = areAscending && region->ascending();
			else
				areAscending = false;
		}

		result = areAscending;
	}

	return result;
}

bool AMScanAxis::descending() const
{
	bool result = false;
	QList<AMScanAxisRegion*> regions = regions_.toList();

	if (!regions.isEmpty()) {
		bool areDescending = true;

		for (int regionIndex = 0, regionCount = regions.count(); regionIndex < regionCount && areDescending; regionIndex++) {
			AMScanAxisRegion *region = regions.at(regionIndex);

			if (region)
				areDescending = areDescending && region->descending();
			else
				areDescending = false;
		}

		result = areDescending;
	}

	return result;
}

bool AMScanAxis::hasIntersectingRegions() const
{
	bool regionsIntersect = false;

	if (regionsValid()) {
		QList<AMScanAxisRegion*> regions = regions_.toList();

		for (int i = 0, count = regions.count(); i < count && !regionsIntersect; i++) {
			AMScanAxisRegion *region = regions.at(i);

			for (int j = 0; j < count && !regionsIntersect; j++) {
				AMScanAxisRegion *anotherRegion = regions.at(j);
				regionsIntersect = (region != anotherRegion && region->intersects(anotherRegion));
			}
		}
	}

	return regionsIntersect;
}

bool AMScanAxis::canMerge(AMScanAxis *otherAxis) const
{
	bool result = (axisValid_ && otherAxis && otherAxis->axisValid());
	return result;
}

bool AMScanAxis::canSimplifyDirection() const
{
	bool result = false;
	QList<AMScanAxisRegion*> regions = regions_.toList();

	if (!regions.isEmpty()) {
		AMScanAxisRegion *previousRegion = 0;
		bool canSimplify = false;

		// Identify the direction of the first valid region.

		previousRegion = regions.at(0);

		if (previousRegion && previousRegion->isValid()) {
			bool regionsValid = true;

			// Iterate through the remaining regions, finding out if they are valid and match the first region's direction.
			// We want to keep iterating as long as there are valid regions and a chance we can simplify a future region's direction.

			for (int regionIndex = 0, regionCount = regions.count(); regionIndex < regionCount && regionsValid && !canSimplify; regionIndex++) {
				AMScanAxisRegion *region = regions.at(regionIndex);

				if (region && region->isValid())
					canSimplify = ((previousRegion->ascending() && region->descending()) || (previousRegion->descending() && region->ascending()));
				else
					regionsValid = false;
			}
		}

		result = canSimplify;
	}

	return result;
}

bool AMScanAxis::canSimplifyIntersectingRegions() const
{
	return (axisValid() && hasIntersectingRegions());
}

bool AMScanAxis::canSimplify() const
{
	return (canSimplifyDirection() || canSimplifyIntersectingRegions());
}

bool AMScanAxis::regionsValid() const
{
	bool result = false;

	QList<AMScanAxisRegion*> regions = regions_.toList();

	if (!regions.isEmpty()) {
		bool areValid = true;

		for (int regionIndex = 0, regionCount = regions.count(); regionIndex < regionCount && areValid; regionIndex++) {
			AMScanAxisRegion *region = regions.at(regionIndex);
			areValid = areValid && (region && region->isValid());
		}

		result = areValid;
	}

	return result;
}

bool AMScanAxis::insertRegion(int index, AMScanAxisRegion *region)
{
	if(axisType_ != AMScanAxis::StepAxis || index < 0 || index > regions_.count())
		return false;

	regions_.insert(index, region);

	if(!sanityCheckRegionsAndAxisType()){

		regions_.remove(index);
		return false;
	}

	emit regionAdded(region);

	connect(region, SIGNAL(regionStartChanged(AMNumber)), this, SIGNAL(regionsChanged()));
	connect(region, SIGNAL(regionStepChanged(AMNumber)), this, SIGNAL(regionsChanged()));
	connect(region, SIGNAL(regionEndChanged(AMNumber)), this, SIGNAL(regionsChanged()));
	connect(region, SIGNAL(regionTimeChanged(AMNumber)), this, SIGNAL(regionsChanged()));

	return true;
}

bool AMScanAxis::appendRegion(AMScanAxisRegion *region)
{
	if(axisType_ != AMScanAxis::StepAxis)
		return false;

	regions_.append(region);

	if(!sanityCheckRegionsAndAxisType()){

		regions_.remove(regions_.count()-1);
		return false;
	}

	emit regionAdded(region);

	connect(region, SIGNAL(regionStartChanged(AMNumber)), this, SIGNAL(regionsChanged()));
	connect(region, SIGNAL(regionStepChanged(AMNumber)), this, SIGNAL(regionsChanged()));
	connect(region, SIGNAL(regionEndChanged(AMNumber)), this, SIGNAL(regionsChanged()));
	connect(region, SIGNAL(regionTimeChanged(AMNumber)), this, SIGNAL(regionsChanged()));

	return true;
}

bool AMScanAxis::overwriteRegion(int index, AMScanAxisRegion *region)
{
	if(index < 0 || index > regions_.count())
		return false;

	AMScanAxisRegion *oldRegion = regions_.at(index);
	regions_[index] = region;

	if(sanityCheckRegionsAndAxisType()){

		regions_[index] = oldRegion;
		return false;
	}

	disconnect(oldRegion, SIGNAL(regionStartChanged(AMNumber)), this, SIGNAL(regionsChanged()));
	disconnect(oldRegion, SIGNAL(regionStepChanged(AMNumber)), this, SIGNAL(regionsChanged()));
	disconnect(oldRegion, SIGNAL(regionEndChanged(AMNumber)), this, SIGNAL(regionsChanged()));
	disconnect(oldRegion, SIGNAL(regionTimeChanged(AMNumber)), this, SIGNAL(regionsChanged()));

	emit regionOverwritten(region);

	connect(region, SIGNAL(regionStartChanged(AMNumber)), this, SIGNAL(regionsChanged()));
	connect(region, SIGNAL(regionStepChanged(AMNumber)), this, SIGNAL(regionsChanged()));
	connect(region, SIGNAL(regionEndChanged(AMNumber)), this, SIGNAL(regionsChanged()));
	connect(region, SIGNAL(regionTimeChanged(AMNumber)), this, SIGNAL(regionsChanged()));

	return true;
}

bool AMScanAxis::removeRegion(AMScanAxisRegion *region)
{
	bool result = false;

	if (region) {

		int index = regions_.toList().indexOf(region);

		if (index >= 0) {
			regions_.remove(index);
			result = true;
		}

		emit regionRemoved(region);

		disconnect(region, SIGNAL(regionStartChanged(AMNumber)), this, SIGNAL(regionsChanged()));
		disconnect(region, SIGNAL(regionStepChanged(AMNumber)), this, SIGNAL(regionsChanged()));
		disconnect(region, SIGNAL(regionEndChanged(AMNumber)), this, SIGNAL(regionsChanged()));
		disconnect(region, SIGNAL(regionTimeChanged(AMNumber)), this, SIGNAL(regionsChanged()));
	}

	return result;
}

bool AMScanAxis::clearRegions()
{
	bool result = false;

	QList<AMScanAxisRegion*> regionsList = regions_.toList();

	if (!regionsList.isEmpty()) {

		// Remove all regions from the list.

		bool removeOK = true;
		foreach (AMScanAxisRegion *region, regionsList) {
			removeOK = removeOK && removeRegion(region);
		}

		result = removeOK;
	}

	return result;
}

void AMScanAxis::setAscending()
{
	foreach (AMScanAxisRegion *region, regions_.toList()) {
		if (region)
			region->setAscending();
	}
}

void AMScanAxis::setDescending()
{
	foreach (AMScanAxisRegion *region, regions_.toList()) {
		if (region)
			region->setDescending();
	}
}

bool AMScanAxis::merge(AMScanAxis *otherAxis)
{
	bool result = false;

	if (otherAxis && otherAxis->axisValid()) {

		// Create a copy of the other axis, and simplify it of any intersecting regions.

		AMScanAxis *newAxis = new AMScanAxis(*otherAxis);
		newAxis->simplify();

		// Simplify this axis of any intersecting regions.

		simplify();

		// Copy the regions from the new axis into this one.

		foreach (AMScanAxisRegion *region, newAxis->regions().toList())
			regions_.append(region);

		// At most, there should be two overlapping regions. Simplify.

		simplify();

		// Merge complete.

		result = true;

	}

	return result;
}

bool AMScanAxis::simplify()
{
	bool simplified = (simplifyIntersectingRegions() && simplifyDirection());
	return simplified;
}

void AMScanAxis::setAxisType(AMScanAxis::AxisType axisType)
{
	if(axisType != AMScanAxis::StepAxis)
		while(regions_.count() > 1)
			regions_.remove(regions_.count()-1);

	axisType_ = axisType;
	checkAxisValidity();
}

bool AMScanAxis::sanityCheckRegionsAndAxisType() const
{
	if(axisType_ != AMScanAxis::StepAxis && regionCount() != 1)
		return false;

	if(axisType_ == AMScanAxis::ContinuousMoveAxis){
		// We don't accepts steps
		if(regions_.at(0)->regionStep().state() != AMNumber::Null)
			return false;
		// We don't accept times
		if(regions_.at(0)->regionTime().state() != AMNumber::Null)
			return false;

		// We need a start
		if(regions_.at(0)->regionStart().state() == AMNumber::Null)
			return false;
		// We need an end
		if(regions_.at(0)->regionEnd().state() == AMNumber::Null)
			return false;
	}

	if(axisType_ == AMScanAxis::ContinuousDwellAxis){
		// We don't accept starts
		if(regions_.at(0)->regionStart().state() != AMNumber::Null)
			return false;
		// We don't accept steps
		if(regions_.at(0)->regionStep().state() != AMNumber::Null)
			return false;
		// We don't accept ends
		if(regions_.at(0)->regionEnd().state() != AMNumber::Null)
			return false;

		// We need a time
		if(regions_.at(0)->regionTime().state() == AMNumber::Null)
			return false;
	}

	if(axisType_ == AMScanAxis::StepAxis){

		for(int i = 0; i < regions_.count(); i++){
			// We need a start for each region
			if(regions_.at(i)->regionStart().state() == AMNumber::Null)
				return false;
			// We need a step for each region
			if(regions_.at(i)->regionStep().state() == AMNumber::Null)
				return false;
			// We need an end for each region
			if(regions_.at(i)->regionEnd().state() == AMNumber::Null)
				return false;
			// We need a time for each region
			if(regions_.at(i)->regionTime().state() == AMNumber::Null)
				return false;
		}
	}

	return true;
}

void AMScanAxis::checkAxisValidity()
{
	if(sanityCheckRegionsAndAxisType())
		axisValid_ = AMScanAxis::ValidAxis;

	else
		axisValid_ = AMScanAxis::InvalidAxis;
}

int AMScanAxis::dbReadAxisType() const
{
	return int(axisType_);
}

void AMScanAxis::dbLoadAxisType(int newAxisType)
{
	setAxisType(AMScanAxis::AxisType(newAxisType));
}

AMDbObjectList AMScanAxis::dbReadRegions() const
{
	AMDbObjectList list;

	foreach (AMScanAxisRegion *region, regions_.toList())
		list << region;

	return list;
}

void AMScanAxis::dbLoadRegions(const AMDbObjectList &newAxisRegions)
{
	for (int i = 0, size = regions_.count(); i < size; i++)
		regions_.at(i)->deleteLater();

	regions_.clear();

	foreach (AMDbObject *object, newAxisRegions){

		AMScanAxisRegion *region = qobject_cast<AMScanAxisRegion *>(object);

		if (region)
			regions_.append(region);
	}
}

bool AMScanAxis::canMergeRegions(AMScanAxisRegion *region, AMScanAxisRegion *otherRegion) const
{
	bool result = (region && otherRegion && region->canMerge(otherRegion) && otherRegion->canMerge(region));
	return result;
}

bool AMScanAxis::canMakeRegionsAdjacent(AMScanAxisRegion *region, AMScanAxisRegion *otherRegion) const
{
	bool result = (region && otherRegion && region->canMakeAdjacentTo(otherRegion) && otherRegion->canMakeAdjacentTo(region));
	return result;
}

bool AMScanAxis::canSimplifyIntersection(AMScanAxisRegion *region, AMScanAxisRegion *otherRegion) const
{
	bool result = intersect(region, otherRegion) && (canMergeRegions(region, otherRegion) || canMakeRegionsAdjacent(region, otherRegion));
	return result;
}

bool AMScanAxis::hasSimplifyIntersection(AMScanAxisRegion *region) const
{
	bool result = false;

	if (region) {

		// Identify intersecting regions.

		QList<AMScanAxisRegion*> intersectingRegions = intersections(region);

		if (!intersectingRegions.isEmpty()) {

			// Identify at least one intersection that can be simplified.

			bool simplificationFound = false;

			for (int i = 0, count = intersectingRegions.count(); i < count && !simplificationFound; i++) {
				AMScanAxisRegion *intersectingRegion = intersectingRegions.at(i);

				if (canSimplifyIntersection(region, intersectingRegion))
					simplificationFound = true;
			}

			result = simplificationFound;
		}
	}

	return result;
}

bool AMScanAxis::intersect(AMScanAxisRegion *region, AMScanAxisRegion *otherRegion) const
{
	bool result = (region && otherRegion && region->intersects(otherRegion) && otherRegion->intersects(region));

	if (result)
		qDebug() << "Regions" << regions_.toList().indexOf(region) << "and" << regions_.toList().indexOf(otherRegion) << "intersect.";
	else
		qDebug() << "Regions do not intersect.";

	return result;
}

bool AMScanAxis::mergeRegionsAscending(AMScanAxisRegion *region, AMScanAxisRegion *otherRegion)
{
	bool result = false;

	if (canMergeRegions(region, otherRegion)) {

		if (region->ascending() && otherRegion->ascending())
			result = true;

		else if (region->ascending() && otherRegion->descending())
			result = (region->numberOfPoints() >= otherRegion->numberOfPoints());

		else if (region->descending() && otherRegion->ascending())
			result = (region->numberOfPoints() <= otherRegion->numberOfPoints());

		else if (region->descending() && otherRegion->descending())
			result = false;
	}

	return result;
}

bool AMScanAxis::mergeRegionsDescending(AMScanAxisRegion *region, AMScanAxisRegion *otherRegion)
{
	bool result = false;

	if (canMergeRegions(region, otherRegion)) {

		if (region->ascending() && otherRegion->ascending())
			result = false;

		else if (region->ascending() && otherRegion->descending())
			result = (region->numberOfPoints() <= otherRegion->numberOfPoints());

		else if (region->descending() && otherRegion->ascending())
			result = (region->numberOfPoints() >= otherRegion->numberOfPoints());

		else if (region->descending() && otherRegion->descending())
			result = true;
	}

	return result;
}

bool AMScanAxis::intersectionAscending(AMScanAxisRegion *region, AMScanAxisRegion *otherRegion) const
{
	bool result = false;

	if (intersect(region, otherRegion)) {
		if (region->ascending() && otherRegion->ascending())
			result = true;
		else if (region->ascending() && otherRegion->descending())
			result = (region->numberOfPoints() >= otherRegion->numberOfPoints());
		else if (region->descending() && otherRegion->descending())
			result = (region->numberOfPoints() <= otherRegion->numberOfPoints());
		else if (region->descending() && otherRegion->descending())
			result = false;
	}

	return result;
}

bool AMScanAxis::intersectionDescending(AMScanAxisRegion *region, AMScanAxisRegion *otherRegion) const
{
	bool result = false;

	if (intersect(region, otherRegion)) {
		if (region->ascending() && otherRegion->ascending())
			result = false;
		else if (region->ascending() && otherRegion->descending())
			result = (region->numberOfPoints() <= otherRegion->numberOfPoints());
		else if (region->descending() && otherRegion->ascending())
			result = (region->numberOfPoints() >= otherRegion->numberOfPoints());
		else if (region->descending() && otherRegion->descending())
			result = true;
	}

	return result;
}

bool AMScanAxis::simplifyDirection()
{
	bool result = false;

	bool valid = (axisValid_ && regionsValid());
	bool canSimplify = canSimplifyDirection();

	if (valid && !canSimplify) {
		result = true;

	} else if (valid && canSimplify) {
		int ascendingPoints = 0;
		int descendingPoints = 0;

		QList<AMScanAxisRegion*> regions = regions_.toList();

		// Iterate through the regions and identify the number of points for ascending regions and the number of points for descending regions.

		for (int regionIndex = 0, regionCount = regions.count(); regionIndex < regionCount; regionIndex++) {
			AMScanAxisRegion *region = regions.at(regionIndex);

			if (region->ascending() && !region->descending())
				ascendingPoints += region->numberOfPoints();
			else if (!region->ascending() && region->descending())
				descendingPoints += region->numberOfPoints();
		}

		// The axis direction should be the one with the most points.

		if (ascendingPoints >= descendingPoints)
			setAscending();
		else
			setDescending();

		result = true;
	}

	return result;
}

bool AMScanAxis::simplifyIntersectingRegions()
{
	bool result = false;

	while (canSimplifyIntersectingRegions()) {

		// Identify a region with intersections that can be simplified.

		AMScanAxisRegion *toSimplify = 0;

		for (int i = 0, count = regions_.count(); i < count && !toSimplify; i++) {
			AMScanAxisRegion *region = regions_.at(i);

			if (region && hasSimplifyIntersection(region))
				toSimplify = region;
		}

		// Apply simplifications to that region.

		if (toSimplify)
			simplifyIntersections(toSimplify);
	}

	return result;
}

bool AMScanAxis::simplifyIntersections(AMScanAxisRegion *region)
{
	bool result = true;	// if there are no intersections, we should succeed.

	QList<AMScanAxisRegion*> regions = regions_.toList();
	qDebug() << "Simplifying intersections for region" << regions.indexOf(region);

	QList<AMScanAxisRegion*> intersectingRegions = intersections(region);

	if (!intersectingRegions.isEmpty()) {

		for (int i = 0, count = intersectingRegions.count(); i < count; i++) {

			// Identify the intersecting region.

			AMScanAxisRegion *intersectingRegion = intersectingRegions.first();

			// Figure out if a simplification can be applied.

			if (canSimplifyIntersection(region, intersectingRegion)) {

				qDebug() << "Can simplify intersection.";

				// Remove region and the intersecting region from the list of scan axis regions.

				regions_.remove(regions.indexOf(region));
				regions_.remove(regions.indexOf(intersectingRegion));

				// Apply intersection simplification.

				QList<AMScanAxisRegion*> results = simplifyIntersection(region, intersectingRegion);

				// Add results to the list of scan axis regions.

				foreach (AMScanAxisRegion *result, results)
					regions_.append(result);

				result &= true;

			} else {
				result = false;
			}
		}
	}

	return result;
}

QList<AMScanAxisRegion*> AMScanAxis::intersections(AMScanAxisRegion *region) const
{
	QList<AMScanAxisRegion*> results;
	QList<AMScanAxisRegion*> regions = regions_.toList();

	qDebug() << "Intersections for region" << regions.indexOf(region);

	if (!regions.isEmpty() && region) {

		for (int i = 0, count = regions.count(); i < count; i++) {
			AMScanAxisRegion *otherRegion = regions.at(i);

			if (region != otherRegion && intersect(region, otherRegion)) {
				results.append(otherRegion);
				qDebug() << otherRegion->toString();
			}
		}
	}

	return results;
}

QList<AMScanAxisRegion*> AMScanAxis::simplifyIntersection(AMScanAxisRegion *region, AMScanAxisRegion *otherRegion)
{
	QList<AMScanAxisRegion*> result;

	if (canSimplifyIntersection(region, otherRegion)) {

		bool sameStepSize = region->sameStepSize(otherRegion);
		bool sameTime = region->sameTime(otherRegion);
		bool canMerge = canMergeRegions(region, otherRegion);
		bool canMakeAdjacent = canMakeRegionsAdjacent(region, otherRegion);

		if (sameStepSize) {
			if (sameTime) {

				if (canMakeAdjacent) {
					// If the step size and the dwell time are the same for both regions, arbitrarily select
					// which one will move to accomodate the other.

					otherRegion->makeAdjacentTo(region);
					result << region << otherRegion;

				} else if (canMerge) {
					// If the two regions can't become adjacent, it's because one is nested inside the other.
					// The regions can be merged, in this instance.

					otherRegion->merge(region);
					result << otherRegion;

				}

			} else {

				if (canMakeAdjacent) {
					// If the step size is the same but the dwell time isn't, the one with the shorter dwell time
					// will move to accomodate the one with the longer dwell time.

					if (double(region->regionTime()) < double(otherRegion->regionTime()))
						region->makeAdjacentTo(otherRegion);
					else
						otherRegion->makeAdjacentTo(region);

					result << region << otherRegion;

				} else if (canMerge) {
					// If the two regions can't become adjacent, it's because one is nested inside the other.
					// The regions can be merged, in this instance.

					otherRegion->merge(region);
					result << otherRegion;
				}
			}

		} else {

			if (sameTime) {

				if (canMakeAdjacent) {
					// If the step size is different and the dwell time is the same, the region with the larger
					// step size will move to accomodate the one with the smaller step size.

					if (double(region->regionStep()) < double(otherRegion->regionStep()))
						otherRegion->makeAdjacentTo(region);
					else
						region->makeAdjacentTo(otherRegion);

					result << region << otherRegion;

				} else if (canMerge) {
					// If the two regions can't become adjacent, it's because one is nested inside the other.
					// The regions can be merged, in this instance.

					otherRegion->merge(region);
					result << otherRegion;
				}

			} else {

				if (canMakeAdjacent) {
					// If the step size and the dwell time are different, a third region is generated to
					// act as a bridge between the two and the two shift to become adjacent to it.

					AMScanAxisRegion *regionIntersection = region->intersection(otherRegion);
					regionIntersection->setParent(this);

					if (regionIntersection) {

						// Set the intersection's step.

						AMNumber step = AMNumber::InvalidError;

						if (double(region->regionStep()) < double(otherRegion->regionStep()))
							step = region->regionStep();
						else
							step = otherRegion->regionStep();

						regionIntersection->setRegionStep(step);

						// Set the intersection's time.

						AMNumber time = AMNumber::InvalidError;

						if (double(region->regionTime()) > double(otherRegion->regionTime()))
							time = region->regionTime();
						else
							time = otherRegion->regionTime();

						regionIntersection->setRegionTime(time);

						// Shift the two original regions to make room for the third.

						region->makeAdjacentTo(regionIntersection);
						otherRegion->makeAdjacentTo(regionIntersection);

						result << region << regionIntersection << otherRegion;
					}

				} else if (!canMerge) {
					// If the two regions can't become adjacent or be merged, it's because one is nested inside
					// the other, but they have different step and time. The containing region is broken up into
					// fragments that flank the contained region.

					if (region->contains(otherRegion))
						result << region->subtract(otherRegion) << otherRegion;
					else if (region->containedBy(otherRegion))
						result << otherRegion->subtract(otherRegion) << region;
				}
			}
		}
	}

	return result;
}
