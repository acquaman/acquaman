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


#include "AMRegionOfInterest.h"

AMRegionOfInterest::~AMRegionOfInterest(){}

AMRegionOfInterest::AMRegionOfInterest(const QString &name, double energy, const AMRange &range, QObject *parent)
	: AMDbObject(parent)
{
	energy_ = energy;
	boundingRange_ = range;
	value_ = new AMRegionOfInterestAB(name, this);
	value_->setBinningRange(range);
	connect(value_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onValueChanged()));
	setName(name);
	connect(this, SIGNAL(nameChanged(QString)), value_, SLOT(setName(QString)));
}

AMRegionOfInterest::AMRegionOfInterest(const QString &name, double energy, double minimum, double maximum, QObject *parent)
	: AMDbObject(parent)
{
	energy_ = energy;
	boundingRange_ = AMRange(minimum, maximum);
	value_ = new AMRegionOfInterestAB(name, this);
	value_->setBinningRange(boundingRange_);
	connect(value_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onValueChanged()));
	setName(name);
	connect(this, SIGNAL(nameChanged(QString)), value_, SLOT(setName(QString)));
}

AMRegionOfInterest::AMRegionOfInterest(const AMRegionOfInterest &original)
	: AMDbObject(original)
{
	energy_ = original.energy();
	boundingRange_ = original.boundingRange();
	AMRegionOfInterestAB *originalAB = (AMRegionOfInterestAB *)original.valueSource();
	value_ = new AMRegionOfInterestAB(originalAB->name(), this);
	value_->setBinningRange(originalAB->binningRange());
	value_->setInputDataSources(originalAB->inputDataSources());
	connect(value_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onValueChanged()));
}

AMRegionOfInterest *AMRegionOfInterest::createCopy() const
{
	AMRegionOfInterest *region = new AMRegionOfInterest(*this);
	region->dissociateFromDb(true);
	return region;
}

AMRegionOfInterest &AMRegionOfInterest::operator =(const AMRegionOfInterest &other)
{
	if (this != &other){

		AMDbObject::operator =(other);

		energy_ = other.energy();
		boundingRange_ = other.boundingRange();
		disconnect(value_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onValueChanged()));
		value_ = (AMRegionOfInterestAB *)(other.valueSource());
		connect(value_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onValueChanged()));
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
		value_->setBinningRange(range);
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
		value_->setBinningRangeLowerBound(lowerBound);
		emit lowerBoundChanged(boundingRange_.minimum());
		emit boundingRangeChanged(boundingRange_);
		setModified(true);
	}
}

void AMRegionOfInterest::setUpperBound(double upperBound)
{
	if (boundingRange_.maximum() != upperBound){

		boundingRange_.setMaximum(upperBound);
		value_->setBinningRangeUpperBound(upperBound);
		emit upperBoundChanged(boundingRange_.maximum());
		emit boundingRangeChanged(boundingRange_);
		setModified(true);
	}
}

void AMRegionOfInterest::setSpectrumSource(AMDataSource *source)
{
	value_->setInputDataSources(QList<AMDataSource *>() << source);
}

void AMRegionOfInterest::onValueChanged()
{
	emit valueChanged(double(value_->value(AMnDIndex())));
}
