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
#include "math.h"

AMScanAxisRegion::AMScanAxisRegion(const AMNumber &start, const AMNumber &step, const AMNumber &end, const AMNumber &time, QObject *parent)
	: AMDbObject(parent)
{
	regionStart_ = start;
	regionStep_ = step;
	regionEnd_ = end;
	regionTime_ = time;
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
	return QString("Start: %1 %4\tStep: %2 %4\tEnd: %3 %4\tTime: %5 s\n")
			.arg(double(regionStart()))
			.arg(double(regionStep()))
			.arg(double(regionEnd()))
			.arg(units)
			.arg(double(regionTime()));
}

bool AMScanAxisRegion::validTime() const
{
	bool result = (regionTime_.isValid()) && (double(regionTime_) > 0);
	return result;
}

bool AMScanAxisRegion::isValid() const
{
	bool result = (regionStart_.isValid() && regionStep_.isValid() && regionEnd_.isValid() && regionTime_.isValid());
	return result;
}

bool AMScanAxisRegion::ascending() const
{
	bool result = false;

	if (regionStart_.isValid() && regionEnd_.isValid() && regionStep_.isValid())
		result = ( (double(regionStart_) < double(regionEnd_)) && (double(regionStep_) > 0) );

	return result;
}

bool AMScanAxisRegion::descending() const
{
	bool result = false;

	if (regionStart_.isValid() && regionEnd_.isValid() && regionStep_.isValid())
		result = ( (double(regionStart_) > double(regionEnd_)) && (double(regionStep_) < 0) );

	return result;
}

bool AMScanAxisRegion::canMerge(AMScanAxisRegion *otherRegion) const
{
	// We only consider merges if both regions are valid (for now).

	bool result = (isValid() && otherRegion->isValid());

	return result;
}

bool AMScanAxisRegion::adjacentTo(AMScanAxisRegion *otherRegion) const
{
	bool result = false;

	if (isValid() && otherRegion && otherRegion->isValid()) {

		if ( (ascending() && otherRegion->ascending()) || (descending() && otherRegion->descending()) ) {

			// If the two regions are going in the same direction, they are adjacent if the start of one
			// matches the end of another.

			result = ( (double(regionStart_) == double(otherRegion->regionEnd())) || (double(regionEnd_) == double(otherRegion->regionStart())) );

		} else if ( (ascending() && otherRegion->descending()) || (descending() && otherRegion->ascending()) ) {

			// If the two regions are going in different directions, they are adjacent if the start of one
			// matches the start of the other or if the end of one matches the end of another.

			result = ( (double(regionStart_) == double(otherRegion->regionStart())) || (double(regionEnd_) == double(otherRegion->regionEnd())) );
		}
	}

	return result;
}

bool AMScanAxisRegion::overlapsWith(AMScanAxisRegion *otherRegion) const
{
	bool result = false;

	if (isValid() && otherRegion && otherRegion->isValid()) {

		if (ascending() && otherRegion->ascending())
			result = ( (double(regionStart_) <= double(otherRegion->regionEnd())) && (double(regionEnd_) >= double(otherRegion->regionStart())) );

		else if (ascending() && otherRegion->descending())
			result = ( (double(regionStart_) <= double(otherRegion->regionStart())) && (double(regionEnd_) >= double(otherRegion->regionEnd())) );

		else if (descending() && otherRegion->ascending())
			result = ( (double(regionStart_) >= double(otherRegion->regionStart())) && (double(regionEnd_) <= double(otherRegion->regionEnd())) );

		else if (descending() && otherRegion->descending())
			result = ( (double(regionStart_) >= double(otherRegion->regionEnd())) && (double(regionEnd_) <= double(otherRegion->regionStart())) );
	}

	return result;
}

bool AMScanAxisRegion::containedBy(AMScanAxisRegion *otherRegion) const
{
	bool result = false;

	if (isValid() && otherRegion && otherRegion->isValid()) {

		if (ascending() && otherRegion->ascending())
			result = ( (double(regionStart_) >= double(otherRegion->regionStart())) && (double(regionEnd_) <= double(otherRegion->regionEnd())) );

		else if (ascending() && otherRegion->descending())
			result = ( (double(regionStart_) >= double(otherRegion->regionEnd())) && (double(regionEnd_) <= double(otherRegion->regionStart())) );

		else if (descending() && otherRegion->ascending())
			result = ( (double(regionStart_) <= double(otherRegion->regionEnd())) && (double(regionEnd_) >= double(otherRegion->regionStart())) );

		else if (descending() && otherRegion->descending())
			result = ( (double(regionStart_) <= double(otherRegion->regionStart())) && (double(regionEnd_) >= double(otherRegion->regionEnd())) );
	}

	return result;
}

bool AMScanAxisRegion::contains(AMScanAxisRegion *otherRegion) const
{
	bool result = true;

	if (isValid() && otherRegion && otherRegion->isValid()) {

		if (ascending() && otherRegion->ascending())
			result = ( (double(regionStart_) <= double(otherRegion->regionStart())) && (double(regionEnd_) >= double(otherRegion->regionEnd())) );

		else if (ascending() && otherRegion->descending())
			result = ( (double(regionStart_) <= double(otherRegion->regionEnd())) && (double(regionEnd_) >= double(otherRegion->regionStart())) );

		else if (descending() && otherRegion->ascending())
			result = ( (double(regionStart_) >= double(otherRegion->regionEnd())) && (double(regionEnd_) <= double(otherRegion->regionStart())) );

		else if (descending() && otherRegion->descending())
			result = ( (double(regionStart_) >= double(otherRegion->regionStart())) && (double(regionEnd_) <= double(otherRegion->regionEnd())) );
	}

	return result;
}

void AMScanAxisRegion::setRegionStart(const AMNumber &regionStart)
{
	if (double(regionStart_) != double(regionStart)){
		regionStart_ = regionStart;
		emit regionStartChanged(regionStart_);
		setModified(true);
	}
}

void AMScanAxisRegion::setRegionStep(const AMNumber &regionStep)
{
	if (double(regionStep_) != double(regionStep)){
		regionStep_ = regionStep;
		emit regionStepChanged(regionStep_);
		setModified(true);
	}
}

void AMScanAxisRegion::setRegionEnd(const AMNumber &regionEnd)
{
	if (double(regionEnd_) != double(regionEnd)){
		regionEnd_ = regionEnd;
		emit regionEndChanged(regionEnd_);
		setModified(true);
	}
}

void AMScanAxisRegion::setRegionTime(const AMNumber &regionTime)
{
	if (double(regionTime_) != double(regionTime)){
		regionTime_ = regionTime;
		emit regionTimeChanged(regionTime_);
		setModified(true);
	}
}

bool AMScanAxisRegion::merge(AMScanAxisRegion *otherRegion)
{
	bool result = false;

	if (canMerge(otherRegion)) {

		AMNumber newStart = mergeStart(otherRegion);
		AMNumber newStep = mergeStep(otherRegion);
		AMNumber newEnd = mergeEnd(otherRegion);
		AMNumber newTime = mergeTime(otherRegion);

		setRegionStart(newStart);
		setRegionStep(newStep);
		setRegionEnd(newEnd);
		setRegionTime(newTime);

		result = true;
	}

	return result;
}

void AMScanAxisRegion::setAscending()
{
	if (isValid() && descending()) {
		AMNumber newStart = regionEnd_;
		AMNumber newStep = AMNumber(double(regionStep_) * -1);
		AMNumber newEnd = regionStart_;

		setRegionStart(newStart);
		setRegionStep(newStep);
		setRegionEnd(newEnd);
	}
}

void AMScanAxisRegion::setDescending()
{
	if (isValid() && ascending()) {
		AMNumber newStart = regionEnd_;
		AMNumber newStep = AMNumber(double(regionStep_) * -1);
		AMNumber newEnd = regionStart_;

		setRegionStart(newStart);
		setRegionStep(newStep);
		setRegionEnd(newEnd);
	}
}

bool AMScanAxisRegion::mergeAscending(AMScanAxisRegion *otherRegion)
{
	bool result = false;

	if (otherRegion) {

		if (isValid() && otherRegion->isValid()) {

			if (ascending() && otherRegion->ascending())
				result = true;

			else if (ascending() && otherRegion->descending())
				result = (numberOfPoints() >= otherRegion->numberOfPoints());

			else if (descending() && otherRegion->ascending())
				result = (numberOfPoints() <= otherRegion->numberOfPoints());

			else if (descending() && otherRegion->descending())
				result = false;
		}
	}

	return result;
}

bool AMScanAxisRegion::mergeDescending(AMScanAxisRegion *otherRegion)
{
	bool result = false;

	if (otherRegion) {

		if (isValid() && otherRegion->isValid()) {

			if (ascending() && otherRegion->ascending())
				result = false;

			else if (ascending() && otherRegion->descending())
				result = (numberOfPoints() <= otherRegion->numberOfPoints());

			else if (descending() && otherRegion->ascending())
				result = (numberOfPoints() >= otherRegion->numberOfPoints());

			else if (descending() && otherRegion->descending())
				result = true;
		}
	}

	return result;
}

AMNumber AMScanAxisRegion::mergeStart(AMScanAxisRegion *otherRegion)
{
	AMNumber result = AMNumber::InvalidError;

	if (otherRegion) {

		if (isValid() && otherRegion->isValid()) {

			if (ascending() && otherRegion->ascending())
				result = (double(regionStart_) <= double(otherRegion->regionStart())) ? regionStart_ : otherRegion->regionStart();

			else if (ascending() && otherRegion->descending() && mergeAscending(otherRegion))
				result = (double(regionStart_) <= double(otherRegion->regionEnd())) ? regionStart_ : otherRegion->regionEnd();

			else if (ascending() && otherRegion->descending() && mergeDescending(otherRegion))
				result = (double(regionEnd_) >= double(otherRegion->regionStart())) ? regionEnd_ : otherRegion->regionStart();

			else if (descending() && otherRegion->ascending() && mergeDescending(otherRegion))
				result = (double(regionStart_) >= double(otherRegion->regionEnd())) ? regionStart_ : otherRegion->regionEnd();

			else if (descending() && otherRegion->ascending() && mergeAscending(otherRegion))
				result = (double(regionEnd_) <= double(otherRegion->regionStart())) ? regionEnd_ : otherRegion->regionStart();

			else if (descending() && otherRegion->descending())
				result = (double(regionStart_) >= double(otherRegion->regionStart())) ? regionStart_ : otherRegion->regionStart();
		}
	}

	return result;
}

AMNumber AMScanAxisRegion::mergeStep(AMScanAxisRegion *otherRegion)
{
	AMNumber result = AMNumber::InvalidError;

	if (otherRegion) {

		if (isValid() && otherRegion->isValid()) {

			if (ascending() && otherRegion->ascending())
				result = (double(regionStep_) <= double(otherRegion->regionStep())) ? regionStep_ : otherRegion->regionStep();

			else if (ascending() && otherRegion->descending() && mergeAscending(otherRegion))
				result = (double(regionStep_) <= fabs(double(otherRegion->regionStep()))) ? regionStep_ : AMNumber(-1 * double(otherRegion->regionStep()));

			else if (ascending() && otherRegion->descending() && mergeDescending(otherRegion))
				result = (double(regionStep_) <= fabs(double(otherRegion->regionStep()))) ? AMNumber(-1 * double(regionStep_)) : otherRegion->regionStep();

			else if (descending() && otherRegion->ascending() && mergeDescending(otherRegion))
				result = (fabs(double(regionStep_)) <= double(otherRegion->regionStep())) ? regionStep_ : AMNumber(-1 * double(otherRegion->regionStep()));

			else if (descending() && otherRegion->ascending() && mergeAscending(otherRegion))
				result = (fabs(double(regionStep_)) <= double(otherRegion->regionStep())) ? AMNumber(-1 * double(regionStep_)) : otherRegion->regionStep();

			else if (descending() && otherRegion->descending())
				result = (fabs(double(regionStep_)) <= fabs(double(otherRegion->regionStep()))) ? regionStep_ : otherRegion->regionStep();
		}
	}

	return result;
}

AMNumber AMScanAxisRegion::mergeEnd(AMScanAxisRegion *otherRegion)
{
	AMNumber result = AMNumber::InvalidError;

	if (otherRegion) {

		if (isValid() && otherRegion->isValid()) {

			if (ascending() && otherRegion->ascending())
				result = (double(regionEnd_) >= double(otherRegion->regionEnd())) ? regionEnd_ : otherRegion->regionEnd();

			else if (ascending() && otherRegion->descending() && mergeAscending(otherRegion))
				result = (double(regionEnd_) >= double(otherRegion->regionStart())) ? regionEnd_ : otherRegion->regionStart();

			else if (ascending() && otherRegion->descending() && mergeDescending(otherRegion))
				result = (double(regionStart_) <= double(otherRegion->regionEnd())) ? regionStart_ : otherRegion->regionEnd();

			else if (descending() && otherRegion->ascending() && mergeDescending(otherRegion))
				result = (double(regionEnd_) <= double(otherRegion->regionStart())) ? regionEnd_ : otherRegion->regionStart();

			else if (descending() && otherRegion->ascending() && mergeAscending(otherRegion))
				result = (double(regionStart_) >= double(otherRegion->regionEnd())) ? regionStart_ : otherRegion->regionEnd();

			else if (descending() && otherRegion->descending())
				result = (double(regionEnd_) <= double(otherRegion->regionEnd())) ? regionEnd_ : otherRegion->regionEnd();
		}
	}

	return result;
}

AMNumber AMScanAxisRegion::mergeTime(AMScanAxisRegion *otherRegion)
{
	AMNumber result = AMNumber::InvalidError;

	if (otherRegion) {

		if (isValid() && otherRegion->isValid()) {
			result = (double(regionTime_) >= double(otherRegion->regionTime())) ? regionTime_ : otherRegion->regionTime();
		}
	}

	return result;
}
