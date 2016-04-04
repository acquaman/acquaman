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
#include "dataman/AMScanAxisEXAFSRegion.h"

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

double AMScanAxis::minimumRegionTime() const
{
	double minTime = 0;

	// Iterate through the regions, identify the smallest region time.

	for (int i = 0, count = regionCount(); i < count; i++) {

		double regionDwell = regionAt(i)->regionTime();

		if (minTime > regionDwell)
			minTime = regionDwell;
	}

	return minTime;
}

double AMScanAxis::maximumRegionTime() const
{
	double maxTime = 0;

	// Iterate through the regions, identify the longest region time.

	for (int i = 0, count = regionCount(); i < count; i++) {

		double regionDwell = 0;

		AMScanAxisRegion *region = regionAt(i);
		AMScanAxisEXAFSRegion *exafsRegion = qobject_cast<AMScanAxisEXAFSRegion*>(region);

		// If the region is an EXAFS region, the dwell time at a particular point could vary.
		// In this case, the best way to make sure the largest possible dwell time is taken
		// into account is to compare against the maximumTime() instead of the regionTime().

		if (exafsRegion)
			regionDwell = double(exafsRegion->maximumTime());
		else
			regionDwell = double(region->regionTime());

		if (maxTime < regionDwell)
			maxTime = regionDwell;
	}

	return maxTime;
}
