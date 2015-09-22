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


#include "AMScanAxisRegion.h"

AMScanAxisRegion::AMScanAxisRegion(const AMNumber &start, const AMNumber &step, const AMNumber &end, const AMNumber &time, QObject *parent)
	: AMDbObject(parent)
{
	regionStart_ = start;
	regionStep_ = step;
	regionEnd_ = end;
	regionTime_ = time;
}

AMScanAxisRegion::AMScanAxisRegion(AMOrderedList<AMScanAxisRegion *> regions, QObject *parent)
	: AMDbObject(parent)
{
	regionStart_ = AMNumber::InvalidError;
	regionStep_ = AMNumber::InvalidError;
	regionEnd_ = AMNumber::InvalidError;
	regionTime_ = AMNumber::InvalidError;

	foreach (AMScanAxisRegion *region, regions.toList())
		merge(region);
}

AMScanAxisRegion::AMScanAxisRegion(const AMScanAxisRegion &original)
	: AMDbObject(original)
{
	regionStart_ = original.regionStart();
	regionStep_ = original.regionStep();
	regionEnd_ = original.regionEnd();
	regionTime_ = original.regionTime();
}

AMScanAxisRegion::~AMScanAxisRegion()
{

}

AMScanAxisRegion *AMScanAxisRegion::createCopy() const
{
	AMScanAxisRegion *region = new AMScanAxisRegion(*this);
	region->dissociateFromDb(true);
	return region;
}

AMNumber AMScanAxisRegion::regionStart() const
{
	return regionStart_;
}

AMNumber AMScanAxisRegion::regionStep() const
{
	return regionStep_;
}

AMNumber AMScanAxisRegion::regionEnd() const
{
	return regionEnd_;
}

AMNumber AMScanAxisRegion::regionTime() const
{
	return regionTime_;
}

int AMScanAxisRegion::numberOfPoints() const
{
	return int((double(regionEnd()) - double(regionStart()))/double(regionStep()) + 1);
}

double AMScanAxisRegion::timePerRegion() const
{
	return numberOfPoints()*double(regionTime());
}

QString AMScanAxisRegion::toString(const QString &units) const
{
	return QString("Start:\t%1 %4\tStep: %2 %4End:\t%3 %4\tTime: %5 s\n")
			.arg(double(regionStart()))
			.arg(double(regionStep()))
			.arg(double(regionEnd()))
			.arg(units)
			.arg(double(regionTime()));
}

bool AMScanAxisRegion::isValid() const
{
	bool result = false;

	if (regionStart_.isValid() && regionStep_.isValid() && regionEnd_.isValid() && regionTime_.isValid())
		result = true;

	return result;
}

bool AMScanAxisRegion::ascending() const
{
	bool result = false;

	if (regionStart_.isValid() && regionEnd_.isValid() && regionStep_.isValid()) {
		if (regionStart_ < regionEnd_ && regionStep_ > 0)
			result = true;
	}

	return result;
}

bool AMScanAxisRegion::descending() const
{
	bool result = false;

	if (regionStart_.isValid() && regionEnd_.isValid() && regionStep_.isValid()) {
		if (regionStart_ > regionEnd_ && regionStep_ < 0)
			result = true;
	}

	return result;
}

double AMScanAxisRegion::size() const
{
	double result = 0;

	if (isValid()) {
		result = fabs(regionEnd_ - regionStart_);
	}

	return result;
}

bool AMScanAxisRegion::canMerge(AMScanAxisRegion *otherRegion) const
{
	bool result = false;

	// We only consider merges if both regions are valid, their times are the same, and they are going the same way.

	bool areValid = (isValid() && otherRegion->isValid());
	bool sameTimes = (regionTime_ == otherRegion->regionTime());
	bool sameDirection = ((ascending() && otherRegion->ascending()) || (descending() && otherRegion->descending()));

	if (areValid && sameTimes && sameDirection) {
		if (overlapsWith(otherRegion) || sharesLimitWith(otherRegion))
			result = true;
	}

	return result;
}

bool AMScanAxisRegion::sharesLimitWith(AMScanAxisRegion *otherRegion) const
{
	bool result = false;

	if (isValid() && otherRegion && otherRegion->isValid()) {

		if (regionStart_ == otherRegion->regionStart())
			result = true;

		else if (regionStart_ == otherRegion->regionEnd())
			result = true;

		else if (regionEnd_ == otherRegion->regionStart())
			result = true;

		else if (regionEnd_ == otherRegion->regionEnd())
			result = true;
	}

	return result;
}

bool AMScanAxisRegion::overlapsWith(AMScanAxisRegion *otherRegion) const
{
	bool result = false;

	if (isValid() && otherRegion && otherRegion->isValid()) {

		if (ascending() && otherRegion->ascending()) {

			if (regionStart_ == otherRegion->regionStart() && regionEnd_ == otherRegion->regionEnd())
				result = true;

			else if (regionStart_ < otherRegion->regionStart() && regionEnd_ > otherRegion->regionStart())
				result = true;

			else if (regionStart_ > otherRegion->regionStart() && regionStart_ < otherRegion->regionEnd())
				result = true;

		} else if (ascending() && otherRegion->descending()) {

			if (regionStart_ == otherRegion->regionEnd() && regionEnd_ == otherRegion->regionStart())
				result = true;

			else if (regionStart_ < otherRegion->regionEnd() && regionEnd_ > otherRegion->regionEnd())
				result = true;

			else if (regionStart_ > otherRegion->regionEnd() && regionStart_ > otherRegion->regionStart())
				result = true;

		} else if (descending() && otherRegion->ascending()) {

			if (regionStart_ == otherRegion->regionEnd() && regionEnd_ == otherRegion->regionStart())
				result = true;

			else if (regionEnd_ < otherRegion->regionStart() && regionStart_ > otherRegion->regionStart())
				result = true;

			else if (regionEnd_ > otherRegion->regionStart() && regionEnd_ < otherRegion->regionEnd())
				result = true;

		} else if (descending() && otherRegion->descending()) {

			if (regionEnd_ == otherRegion->regionEnd() && regionStart_ == otherRegion->regionStart())
				result = true;

			else if (regionEnd_ < otherRegion->regionEnd() && regionStart_ > otherRegion->regionEnd())
				result = true;

			else if (regionEnd_ > otherRegion->regionEnd() && regionEnd_ < otherRegion->regionStart())
				result = true;
		}
	}

	return result;
}

bool AMScanAxisRegion::containedBy(AMScanAxisRegion *otherRegion) const
{
	bool result = false;

	if (isValid() && otherRegion && otherRegion->isValid()) {

		if (ascending() && otherRegion->ascending()) {
			if (regionStart_ >= otherRegion->regionStart() && regionEnd_ <= otherRegion->regionEnd())
				result = true;

		} else if (ascending() && otherRegion->descending()) {
			if (regionStart_ >= otherRegion->regionEnd() && regionEnd_ <= otherRegion->regionStart())
				result = true;

		} else if (descending() && otherRegion->ascending()) {
			if (regionEnd_ >= otherRegion->regionStart() && regionStart_ <= otherRegion->regionEnd())
				result = true;

		} else if (descending() && otherRegion->descending()) {
			if (regionEnd_ >= otherRegion->regionEnd() && regionStart_ <= otherRegion->regionStart())
				result = true;
		}
	}

	return result;
}

bool AMScanAxisRegion::contains(AMScanAxisRegion *otherRegion) const
{
	bool result = true;

	if (isValid() && otherRegion && otherRegion->isValid()) {

		if (ascending() && otherRegion->ascending()) {
			if (regionStart_ <= otherRegion->regionStart() && regionEnd_ >= otherRegion->regionEnd())
				result = true;

		} else if (ascending() && otherRegion->descending()) {
			if (regionStart_ <= otherRegion->regionEnd() && regionEnd_ >= otherRegion->regionStart())
				result = true;

		} else if (descending() && otherRegion->ascending()) {
			if (regionEnd_ <= otherRegion->regionStart() && regionStart_ >= otherRegion->regionEnd())
				result = true;

		} else if (descending() && otherRegion->descending()) {
			if (regionEnd_ <= otherRegion->regionEnd() && regionStart_ >= otherRegion->regionStart())
				result = true;
		}
	}

	return result;
}

void AMScanAxisRegion::setRegionStart(const AMNumber &regionStart)
{
	if (double(regionStart_) != double(regionStart)){

		emit regionStartChanged(regionStart_ = regionStart);
		setModified(true);
	}
}

void AMScanAxisRegion::setRegionStep(const AMNumber &regionStep)
{
	if (double(regionStep_) != double(regionStep)){

		emit regionStepChanged(regionStep_ = regionStep);
		setModified(true);
	}
}

void AMScanAxisRegion::setRegionEnd(const AMNumber &regionEnd)
{
	if (double(regionEnd_) != double(regionEnd)){

		emit regionEndChanged(regionEnd_ = regionEnd);
		setModified(true);
	}
}

void AMScanAxisRegion::setRegionTime(const AMNumber &regionTime)
{
	if (double(regionTime_) != double(regionTime)){

		emit regionTimeChanged(regionTime_ = regionTime);
		setModified(true);
	}
}

bool AMScanAxisRegion::merge(AMScanAxisRegion *otherRegion)
{
	bool result = false;

	if (canMerge(otherRegion)) {
		setRegionStart( mergeStart(otherRegion) );
		setRegionStep( mergeStep(otherRegion) );
		setRegionEnd( mergeEnd(otherRegion) );
		setRegionTime( mergeTime(otherRegion) );

		result = true;
	}

	return result;
}

bool AMScanAxisRegion::mergeAscending(AMScanAxisRegion *otherRegion)
{
	bool result = false;

	if (mergeStep(otherRegion) > 0)
		result = true;

	return result;
}

bool AMScanAxisRegion::mergeDescending(AMScanAxisRegion *otherRegion)
{
	bool result = false;

	if (mergeStep(otherRegion) < 0)
		return true;

	return result;
}

AMNumber AMScanAxisRegion::mergeStart(AMScanAxisRegion *otherRegion)
{
	AMNumber result = AMNumber::InvalidError;

	if (isValid() && otherRegion && otherRegion->isValid()) {

		if (ascending() && otherRegion->ascending()) {

			if (regionStart_ <= otherRegion->regionStart())
				result = regionStart_;
			else
				result = otherRegion->regionStart();

		} else if (ascending() && otherRegion->descending() && mergeAscending(otherRegion)) {

			if (regionStart_ <= otherRegion->regionEnd())
				result = regionStart_;
			else
				result = otherRegion->regionEnd();

		} else if (ascending() && otherRegion->descending() && mergeDescending(otherRegion)) {

			if (regionEnd_ > otherRegion->regionStart())
				result = regionEnd_;
			else
				result = otherRegion->regionStart();

		} else if (descending() && otherRegion->ascending() && mergeDescending(otherRegion)) {

			if (regionStart_ <= otherRegion->regionEnd())
				result = regionStart_;
			else
				result = otherRegion->regionEnd();

		} else if (descending() && otherRegion->ascending() && mergeAscending(otherRegion)) {

			if (regionEnd_ < otherRegion->regionStart())
				result = regionEnd_;
			else
				result = otherRegion->regionStart();

		} else if (descending() && otherRegion->descending()) {

			if (regionStart_ <= otherRegion->regionStart())
				result = regionStart_;
			else
				result = otherRegion->regionStart();
		}
	}

	return result;
}

AMNumber AMScanAxisRegion::mergeStep(AMScanAxisRegion *otherRegion)
{
	AMNumber result = AMNumber::InvalidError;

	if (isValid() && otherRegion && otherRegion->isValid()) {
		if (numberOfPoints() >= otherRegion->numberOfPoints())
			result = regionStep_;
		else
			result = otherRegion->regionStep();
	}

	return result;
}

AMNumber AMScanAxisRegion::mergeEnd(AMScanAxisRegion *otherRegion)
{
	AMNumber result = AMNumber::InvalidError;

	if (isValid() && otherRegion && otherRegion->isValid()) {

		if (ascending() && otherRegion->ascending()) {

			if (regionEnd_ < otherRegion->regionEnd())
				result = otherRegion->regionEnd();
			else
				result = regionEnd_;

		} else if (ascending() && otherRegion->descending() && mergeAscending(otherRegion)) {

			if (regionEnd_ < otherRegion->regionStart())
				result = otherRegion->regionStart();
			else
				result = regionEnd_;

		} else if (ascending() && otherRegion->descending() && mergeDescending(otherRegion)) {

			if (regionStart_ < otherRegion->regionEnd())
				result = regionStart_;
			else
				result = otherRegion->regionEnd();

		} else if (descending() && otherRegion->ascending() && mergeDescending(otherRegion)) {

			if (regionEnd_ > otherRegion->regionStart())
				result = otherRegion->regionStart();
			else
				result = regionEnd_;

		} else if (descending() && otherRegion->ascending() && mergeAscending(otherRegion)) {

			if (regionStart_ < otherRegion->regionEnd())
				result = otherRegion->regionEnd();
			else
				result = regionStart_;

		} else if (descending() && otherRegion->descending()) {

			if (regionEnd_ > otherRegion->regionEnd())
				result = otherRegion->regionEnd();
			else
				result = regionEnd_;
		}
	}

	return result;
}

double AMScanAxisRegion::mergeTime(AMScanAxisRegion *otherRegion)
{
	double result = 0;

	if (isValid() && otherRegion->isValid()) {
		if (regionTime_ >= otherRegion->regionTime())
			result = regionTime_;
		else
			result = otherRegion->regionTime();
	}

	return result;
}
