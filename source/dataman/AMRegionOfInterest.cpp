#include "AMRegionOfInterest.h"

AMRegionOfInterest::AMRegionOfInterest(const QString &name, double energy, const AMRange &range, QObject *parent)
	: AMDbObject(parent)
{
	energy_ = energy;
	boundingRange_ = range;
	setName(name);
}

AMRegionOfInterest::AMRegionOfInterest(const QString &name, double energy, double minimum, double maximum, QObject *parent)
	: AMDbObject(parent)
{
	energy_ = energy;
	boundingRange_ = AMRange(minimum, maximum);
	setName(name);
}

AMRegionOfInterest::AMRegionOfInterest(const AMRegionOfInterest &original)
	: AMDbObject(original)
{
	energy_ = original.energy();
	boundingRange_ = original.boundingRange();
}

AMRegionOfInterest &AMRegionOfInterest::operator =(const AMRegionOfInterest &other)
{
	if (this != &other){

		AMDbObject::operator =(other);

		energy_ = other.energy();
		boundingRange_ = other.boundingRange();
	}

	return *this;
}

void AMRegionOfInterest::setEnergy(double energy)
{
	if (energy_ != energy){

		energy_ = energy;
		emit energyChanged(energy_);
		setModified(true);
	}
}

void AMRegionOfInterest::setBoundingRange(const AMRange &range)
{
	if (boundingRange_ != range){

		boundingRange_ = range;
		emit lowerBoundChanged(boundingRange_.minimum());
		emit upperBoundChanged(boundingRange_.maximum());
		emit boundingRangeChanged(boundingRange_);
		setModified(true);
	}
}

void AMRegionOfInterest::setBoundingRange(double lowerBound, double upperBound)
{
	setBoundingRange(AMRange(lowerBound, upperBound));
}

void AMRegionOfInterest::setLowerBound(double lowerBound)
{
	if (boundingRange_.minimum() != lowerBound){

		boundingRange_.setMinimum(lowerBound);
		emit lowerBoundChanged(boundingRange_.minimum());
		emit boundingRangeChanged(boundingRange_);
		setModified(true);
	}
}

void AMRegionOfInterest::setUpperBound(double upperBound)
{
	if (boundingRange_.maximum() != upperBound){

		boundingRange_.setMaximum(upperBound);
		emit upperBoundChanged(boundingRange_.maximum());
		emit boundingRangeChanged(boundingRange_);
		setModified(true);
	}
}
