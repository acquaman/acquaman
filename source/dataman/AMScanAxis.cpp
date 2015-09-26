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

	foreach (AMScanAxisRegion *region, regions_.toList())
		string.append(region->toString(units));

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

bool AMScanAxis::adjacent()
{
	bool result = false;
	QList<AMScanAxisRegion*> regions = regions_.toList();

	if (!regions.isEmpty()) {

		// Identify first region.

		AMScanAxisRegion *previousRegion = regions.first();

		if (previousRegion) {
			bool areAdjacent = true;

			// Check that the next region is adjacent to the previous one, for all of the remaining regions.

			for (int regionIndex = 1, regionCount = regions.count(); regionIndex < regionCount && areAdjacent; regionIndex++) {
				AMScanAxisRegion *region = regions.at(regionIndex);

				if (previousRegion && region)
					areAdjacent = areAdjacent && previousRegion->adjacentTo(region);
				else
					areAdjacent = false;

				previousRegion = region;
			}
		}
	}

	return result;
}

bool AMScanAxis::canMerge(AMScanAxis *otherAxis)
{
	bool result = false;

	if (otherAxis)
		result = (axisValid() && otherAxis->axisValid());

	return result;
}

bool AMScanAxis::canSimplify()
{
	return false;
}

bool AMScanAxis::canSimplify(AMScanAxisRegion *region, AMScanAxisRegion *otherRegion)
{
	bool result = (region && region->isValid() && otherRegion && otherRegion->isValid());
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

bool AMScanAxis::merge(AMScanAxis *otherAxis)
{
	bool result = false;

	if (otherAxis) {

	}

	return result;
}

bool AMScanAxis::simplify()
{
	bool result = false;

	return result;
}

void AMScanAxis::setAscending()
{
	foreach (AMScanAxisRegion *region, regions.toList()) {
		if (region)
			region->setAscending();
	}
}

void AMScanAxis::setDescending()
{
	foreach (AMScanAxisRegion *region, regions.toList()) {
		if (region)
			region->setDescending();
	}
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

QList<AMScanAxisRegion*> AMScanAxis::simplify(AMScanAxisRegion *region, AMScanAxisRegion *otherRegion)
{
	QList<AMScanAxisRegion*> result << region << otherRegion;

	if (canSimplify(region, otherRegion)) {
		QList<AMScanAxisRegion*> simplifiedRegions;

		result = simplifiedRegions;
	}

	return result;
}

QList<AMScanAxisRegion*> AMScanAxis::makeAdjacent(AMScanAxisRegion *region, AMScanAxisRegion *otherRegion)
{
	QList<AMScanAxisRegion*> result;

	if (region && region->canMakeAdjacentTo(otherRegion) && otherRegion && otherRegion->canMakeAdjacentTo(region)) {
		bool success = false;

		if (region->regionStep() == otherRegion->regionStep()) {
			if (region->regionTime() == otherRegion->regionTime()) {

				// If the step size and the dwell time are the same for both regions, arbitrarily select
				// which one will move to accomodate the other.

				success = otherRegion->makeAdjacentTo(region);

			} else {

				// If the step size is the same but the dwell time isn't, the one with the shorter dwell time
				// will move to accomodate the one with the longer dwell time.

				if (region->regionTime() < otherRegion->regionTime())
					success = region->makeAdjacentTo(otherRegion);
				else
					success = otherRegion->makeAdjacentTo(region);
			}

		} else {
			if (region->regionTime() == otherRegion->regionTime()) {

				// If the step size is different and the dwell time is the same, the region with the larger
				// step size will move to accomodate the one with the smaller step size.

				if (region->regionStep() < otherRegion->regionStep())
					success = region->makeAdjacentTo(otherRegion);
				else
					success = otherRegion->makeAdjacentTo(region);

			} else {

				// If the step size and the dwell time are different between the regions, a third region is
				// introduced with properties that are a mix of the original two.

				success = false;
			}
		}

		if (success)
			result << region << otherRegion;
	}

	return result;
}

AMScanAxisRegion* AMScanAxis::intersection(AMScanAxisRegion *region, AMScanAxisRegion *otherRegion)
{
	AMScanAxisRegion *result = 0;

	if (region && region->isValid() && otherRegion && otherRegion->isValid() && region->overlapsWith(otherRegion)) {

		if (region->contains(otherRegion)) {

			// If one region is contained completely within the other, the intersection is the
			// contained region.

			result = otherRegion;

		} else if (region->containedBy(otherRegion)) {

			// Same here, but for the opposite region.

			result = region;

		} else {

			// If the two regions overlap and one does not contain the other, the intersection is
			// the area the two regions have in common.

			AMNumber intersectionStart;
			AMNumber intersectionEnd;

			if (region->ascending() && otherRegion->ascending()) {
				intersectionStart = (double(region->regionStart()) > double(otherRegion->regionStart())) ? region->regionStart() : otherRegion->regionStart();
				intersectionEnd = (double(region->regionEnd()) < double(otherRegion->regionEnd())) ? region->regionEnd() : otherRegion->regionEnd();

			} else if (region->ascending() && otherRegion->descending()) {
				intersectionStart = (double(region->regionStart()))
			}
		}
	}
}
