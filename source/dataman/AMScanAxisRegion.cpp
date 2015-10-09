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

bool AMScanAxisRegion::within(const AMNumber &point) const
{
	bool result = false;

	if (isValid() && point.isValid()) {
		if (ascending())
			result = ((double(regionStart_) <= double(point)) && (double(regionEnd_) >= double(point)));
		else if (descending())
			result = ((double(regionEnd_) <= double(point)) && (double(regionStart_) >= double(point)));
	}

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

bool AMScanAxisRegion::sameStart(const AMScanAxisRegion *region, const AMScanAxisRegion *otherRegion)
{
	bool result = (region && otherRegion && (double(region->regionStart()) == double(otherRegion->regionStart())));
	return result;
}

bool AMScanAxisRegion::sameStep(const AMScanAxisRegion *region, const AMScanAxisRegion *otherRegion)
{
	bool result = (region && otherRegion && (double(region->regionStep()) == double(otherRegion->regionStep())));
	return result;
}

bool AMScanAxisRegion::sameStepSize(const AMScanAxisRegion *region, const AMScanAxisRegion *otherRegion)
{
	bool result = (region && otherRegion && (fabs(double(region->regionStep())) == fabs(double(otherRegion->regionStep()))));
	return result;
}

bool AMScanAxisRegion::sameEnd(const AMScanAxisRegion *region, const AMScanAxisRegion *otherRegion)
{
	bool result = (region && otherRegion && (double(region->regionEnd()) == double(otherRegion->regionEnd())));
	return result;
}

bool AMScanAxisRegion::sameTime(const AMScanAxisRegion *region, const AMScanAxisRegion *otherRegion)
{
	bool result = (region && otherRegion && (double(region->regionTime()) == double(otherRegion->regionTime())));
	return result;
}

bool AMScanAxisRegion::sameDirection(const AMScanAxisRegion *region, const AMScanAxisRegion *otherRegion)
{
	bool result = (region && otherRegion && ((region->ascending() && otherRegion->ascending()) || (region->descending() && otherRegion->descending())));
	return result;
}

bool AMScanAxisRegion::identical(const AMScanAxisRegion *region, const AMScanAxisRegion *otherRegion)
{
	bool result = sameStart(region, otherRegion) && sameStep(region, otherRegion) && sameEnd(region, otherRegion) && sameTime(region, otherRegion);
	return result;
}

bool AMScanAxisRegion::contains(const AMScanAxisRegion *region, const AMNumber &point)
{
	bool result = false;

	if (region && region->isValid()) {
		if (region->ascending())
			result = ( (double(region->regionStart()) <= double(point)) && (double(region->regionEnd()) >= double(point)) );
		else if (region->descending())
			result = ( (double(region->regionStart()) >= double(point)) && (double(region->regionEnd()) <= double(point)) );
	}

	return result;
}

bool AMScanAxisRegion::contains(const AMScanAxisRegion *region, const AMScanAxisRegion *otherRegion)
{
	bool result = false;

	if (region && region->isValid() && otherRegion && otherRegion->isValid())
		result = (contains(region, otherRegion->regionStart()) && contains(region, otherRegion->regionEnd()));

	return result;
}

bool AMScanAxisRegion::adjacent(const AMScanAxisRegion *region, const AMScanAxisRegion *otherRegion)
{
	bool result = false;

	if (region && region->isValid() && otherRegion && otherRegion->isValid()) {

		if ( (region->ascending() && otherRegion->ascending()) || (region->descending() && otherRegion->descending()) ) {

			// If the two regions are going in the same direction, they are adjacent if the start of one
			// matches the end of another.

			result = ( (double(region->regionStart()) == double(otherRegion->regionEnd())) || (double(region->regionEnd()) == double(otherRegion->regionStart())) );

		} else if ( (region->ascending() && otherRegion->descending()) || (region->descending() && otherRegion->ascending()) ) {

			// If the two regions are going in different directions, they are adjacent if the start of one
			// matches the start of the other or if the end of one matches the end of another.

			result = ( (double(region->regionStart()) == double(otherRegion->regionStart())) || (double(region->regionEnd()) == double(otherRegion->regionEnd())) );
		}
	}

	return result;
}

bool AMScanAxisRegion::intersect(const AMScanAxisRegion *region, const AMScanAxisRegion *otherRegion)
{
	bool result = false;

	if (region && region->isValid() && otherRegion && otherRegion->isValid()) {

		if (region->ascending() && otherRegion->ascending())
			result = ( (double(region->regionStart()) <= double(otherRegion->regionEnd())) && (double(region->regionEnd()) >= double(otherRegion->regionStart())) );

		else if (region->ascending() && otherRegion->descending())
			result = ( (double(region->regionStart()) <= double(otherRegion->regionStart())) && (double(region->regionEnd()) >= double(otherRegion->regionEnd())) );

		else if (region->descending() && otherRegion->ascending())
			result = ( (double(region->regionStart()) >= double(otherRegion->regionStart())) && (double(region->regionEnd()) <= double(otherRegion->regionEnd())) );

		else if (region->descending() && otherRegion->descending())
			result = ( (double(region->regionStart()) >= double(otherRegion->regionEnd())) && (double(region->regionEnd()) <= double(otherRegion->regionStart())) );
	}

	return result;
}

AMScanAxisRegion* AMScanAxisRegion::intersection(const AMScanAxisRegion *region, const AMScanAxisRegion *otherRegion)
{
	AMScanAxisRegion *result = 0;

	if (intersect(region, otherRegion)) {

		// If the two regions intersect, the intersection is the area the two regions have in common.

		AMNumber start = region->intersectionStart(otherRegion);
		AMNumber end = region->intersectionEnd(otherRegion);

		result = new AMScanAxisRegion(start, AMNumber::InvalidError, end, AMNumber::InvalidError);
	}

	return result;
}

bool AMScanAxisRegion::canMerge(const AMScanAxisRegion *region, const AMScanAxisRegion *otherRegion)
{
	// We consider merges only if both regions are valid, for now.

	bool result = ( region && region->isValid() && otherRegion && otherRegion->isValid() && (sameStepSize(region, otherRegion) || sameTime(region, otherRegion)) );
	return result;
}

bool AMScanAxisRegion::canMakeAdjacent(const AMScanAxisRegion *region, const AMScanAxisRegion *otherRegion)
{
	// We will consider moving only if both regions are valid, for now.

	bool result = (region && region->isValid() && otherRegion && otherRegion->isValid());
	return result;
}

bool AMScanAxisRegion::canDivide(const AMScanAxisRegion *region, const AMNumber &point)
{
	bool result = (region && region->isValid() && point.isValid() && contains(region, point));
	return result;
}

bool AMScanAxisRegion::canSubtract(const AMScanAxisRegion *region, const AMScanAxisRegion *otherRegion)
{
	// We will consider subtractions involving valid regions only, for now.

	bool result = ( region && region->isValid() && otherRegion && otherRegion->isValid() && canDivide(region, otherRegion->regionStart()) && canDivide(region, otherRegion->regionEnd()));
	return result;
}

QList<AMScanAxisRegion*> AMScanAxisRegion::divide(const AMScanAxisRegion *region, const AMNumber &dividePoint)
{
	QList<AMScanAxisRegion*> results;

	if (canDivide(region, dividePoint)) {
		results << new AMScanAxisRegion(region->regionStart(), region->regionStep(), dividePoint, region->regionTime());
		results << new AMScanAxisRegion(dividePoint, region->regionStep(), region->regionEnd(), region->regionTime());
	}

	return results;
}

QList<AMScanAxisRegion*> AMScanAxisRegion::subtract(const AMScanAxisRegion *region, const AMScanAxisRegion *otherRegion)
{
	QList<AMScanAxisRegion*> results;

	if (canSubtract(region, otherRegion)) {

		// Keep track of the regions that we DONT want, so we can handle deleting them later.

		QList<AMScanAxisRegion*> toDelete;

		// Subtracting another region is (almost) the same as dividing at that region's
		// start and end points.

		QList<AMScanAxisRegion*> firstResult = divide(region, otherRegion->regionStart());
		QList<AMScanAxisRegion*> secondResult = divide(region, otherRegion->regionEnd());

		// The final results are the regions that are not identical to otherRegion.

		foreach (AMScanAxisRegion *result, firstResult) {
			if (!AMScanAxisRegion::identical(result, otherRegion))
				results << result;
			else
				toDelete << result;
		}

		foreach (AMScanAxisRegion *result, secondResult) {
			if (!AMScanAxisRegion::identical(result, otherRegion))
				results << result;
			else
				toDelete << result;
		}

		// Delete the undesired results.

		foreach (AMScanAxisRegion *result, toDelete) {
			if (result)
				result->deleteLater();
		}
	}

	return results;
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

bool AMScanAxisRegion::merge(AMScanAxisRegion *otherRegion)
{
	bool result = false;

	if (canMerge(this, otherRegion)) {
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

bool AMScanAxisRegion::makeAdjacentTo(AMScanAxisRegion *otherRegion)
{
	bool result = false;

	if (canMakeAdjacent(this, otherRegion)) {
		AMNumber newStart = makeAdjacentStart(otherRegion);
		AMNumber newEnd = makeAdjacentEnd(otherRegion);

		setRegionStart(newStart);
		setRegionEnd(newEnd);

		result = true;
	}

	return result;
}

AMNumber AMScanAxisRegion::mergeStart(AMScanAxisRegion *otherRegion) const
{
	AMNumber result = AMNumber::InvalidError;

	if (canMerge(this, otherRegion)) {

		if (ascending() && otherRegion->ascending())
			result = (double(regionStart()) <= double(otherRegion->regionStart())) ? regionStart() : otherRegion->regionStart();

		else if (ascending() && otherRegion->descending())
			result = (double(regionStart()) <= double(otherRegion->regionEnd())) ? regionStart() : otherRegion->regionEnd();

		else if (descending() && otherRegion->ascending())
			result = (double(regionStart()) >= double(otherRegion->regionEnd())) ? regionStart() : otherRegion->regionEnd();

		else if (descending() && otherRegion->descending())
			result = (double(regionStart()) >= double(otherRegion->regionStart())) ? regionStart() : otherRegion->regionStart();
	}

	return result;
}

AMNumber AMScanAxisRegion::mergeStep(AMScanAxisRegion *otherRegion) const
{
	AMNumber result = AMNumber::InvalidError;

	if (canMerge(this, otherRegion)) {

		if (ascending() && otherRegion->ascending())
			result = (double(regionStep()) <= double(otherRegion->regionStep())) ? regionStep() : otherRegion->regionStep();

		else if (ascending() && otherRegion->descending())
			result = (double(regionStep()) <= fabs(double(otherRegion->regionStep()))) ? regionStep() : AMNumber(-1 * double(otherRegion->regionStep()));

		else if (descending() && otherRegion->ascending())
			result = (fabs(double(regionStep())) <= double(otherRegion->regionStep())) ? regionStep() : AMNumber(-1 * double(otherRegion->regionStep()));

		else if (descending() && otherRegion->descending())
			result = (fabs(double(regionStep())) <= fabs(double(otherRegion->regionStep()))) ? regionStep() : otherRegion->regionStep();
	}

	return result;
}

AMNumber AMScanAxisRegion::mergeEnd(AMScanAxisRegion *otherRegion) const
{
	AMNumber result = AMNumber::InvalidError;

	if (canMerge(this, otherRegion)) {

		if (ascending() && otherRegion->ascending())
			result = (double(regionEnd()) >= double(otherRegion->regionEnd())) ? regionEnd() : otherRegion->regionEnd();

		else if (ascending() && otherRegion->descending())
			result = (double(regionEnd()) >= double(otherRegion->regionStart())) ? regionEnd() : otherRegion->regionStart();

		else if (descending() && otherRegion->ascending())
			result = (double(regionEnd()) <= double(otherRegion->regionStart())) ? regionEnd() : otherRegion->regionStart();

		else if (descending() && otherRegion->descending())
			result = (double(regionEnd()) <= double(otherRegion->regionEnd())) ? regionEnd() : otherRegion->regionEnd();
	}

	return result;
}

AMNumber AMScanAxisRegion::mergeTime(AMScanAxisRegion *otherRegion) const
{
	AMNumber result = AMNumber::InvalidError;

	if (canMerge(this, otherRegion))
		result = (double(regionTime()) >= double(otherRegion->regionTime())) ? regionTime() : otherRegion->regionTime();

	return result;
}

AMNumber AMScanAxisRegion::intersectionStart(const AMScanAxisRegion *otherRegion) const
{
	AMNumber result = AMNumber::InvalidError;

	if (intersect(this, otherRegion)) {

		if (ascending() && otherRegion->ascending())
			result = (double(regionStart()) >= double(otherRegion->regionStart())) ? regionStart() : otherRegion->regionStart();

		else if (ascending() && otherRegion->descending())
			result = (double(regionStart()) >= double(otherRegion->regionEnd())) ? regionStart() : otherRegion->regionEnd();

		else if (descending() && otherRegion->ascending())
			result = (double(regionStart()) <= double(otherRegion->regionEnd())) ? regionStart() : otherRegion->regionEnd();

		else if (descending() && otherRegion->descending())
			result = (double(regionStart()) <= double(otherRegion->regionStart())) ? regionStart() : otherRegion->regionStart();
	}

	return result;
}

AMNumber AMScanAxisRegion::intersectionEnd(const AMScanAxisRegion *otherRegion) const
{
	AMNumber result = AMNumber::InvalidError;

	if (intersect(this, otherRegion)) {

		if (ascending() && otherRegion->ascending())
			result = (double(regionEnd()) <= double(otherRegion->regionEnd())) ? regionEnd() : otherRegion->regionEnd();

		else if (ascending() && otherRegion->descending())
			result = (double(regionEnd()) <= double(otherRegion->regionStart())) ? regionEnd() : otherRegion->regionStart();

		else if (descending() && otherRegion->ascending())
			result = (double(regionEnd()) >= double(otherRegion->regionStart())) ? regionEnd() : otherRegion->regionStart();

		else if (descending() && otherRegion->descending())
			result = (double(regionEnd()) >= double(otherRegion->regionEnd())) ? regionEnd() : otherRegion->regionEnd();
	}

	return result;
}

AMNumber AMScanAxisRegion::makeAdjacentStart(AMScanAxisRegion *otherRegion) const
{
	AMNumber result = AMNumber::InvalidError;

	if (canMakeAdjacent(this, otherRegion)) {

		if (ascending() && otherRegion->ascending())
			result = (double(regionStart()) < double(otherRegion->regionStart())) ? regionStart() : otherRegion->regionEnd();

		else if (ascending() && otherRegion->descending())
			result = (double(regionStart()) < double(otherRegion->regionEnd())) ? regionStart() : otherRegion->regionStart();

		else if (descending() && otherRegion->ascending())
			result = (double(regionStart()) > double(otherRegion->regionEnd())) ? regionStart() : otherRegion->regionStart();

		else if (descending() && otherRegion->descending())
			result = (double(regionStart()) > double(otherRegion->regionStart())) ? regionStart() : otherRegion->regionEnd();
	}

	return result;
}

AMNumber AMScanAxisRegion::makeAdjacentEnd(AMScanAxisRegion *otherRegion) const
{
	AMNumber result = AMNumber::InvalidError;

	if (canMakeAdjacent(this, otherRegion)) {

		if (ascending() && otherRegion->ascending())
			result = (double(regionEnd()) > double(otherRegion->regionEnd())) ? regionEnd() : otherRegion->regionStart();

		else if (ascending() && otherRegion->descending())
			result = (double(regionEnd()) > double(otherRegion->regionStart())) ? regionEnd() : otherRegion->regionEnd();

		else if (descending() && otherRegion->ascending())
			result = (double(regionEnd()) < double(otherRegion->regionStart())) ? regionEnd() : otherRegion->regionEnd();

		else if (descending() && otherRegion->descending())
			result = (double(regionEnd()) < double(otherRegion->regionEnd())) ? regionEnd() : otherRegion->regionStart();
	}

	return result;
}
