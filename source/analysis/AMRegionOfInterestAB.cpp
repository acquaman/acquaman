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


#include "AMRegionOfInterestAB.h"

#include "util/AMUtility.h"
#include "util/AMErrorMonitor.h"

AMRegionOfInterestAB::AMRegionOfInterestAB(const QString &outputName, QObject *parent)
	: AMStandardAnalysisBlock(outputName, parent)
{
	spectrum_ = 0;
	binningRange_ = AMRange();
	cacheUpdateRequired_ = false;
	cachedDataRange_ = AMRange();

	setState(AMDataSource::InvalidFlag);
}

AMRegionOfInterestAB::~AMRegionOfInterestAB(){}

bool AMRegionOfInterestAB::areInputDataSourcesAcceptable(const QList<AMDataSource*>& dataSources) const
{
	if(dataSources.isEmpty())
		return true; // always acceptable, the null input.

	// otherwise there is one data source
	if (dataSources.size() == 1)
		return dataSources.first()->rank() >= 1;

	return false;
}

AMNumber AMRegionOfInterestAB::value(const AMnDIndex &indexes) const
{
	if(indexes.rank() != rank())
		return AMNumber(AMNumber::DimensionError);

	if(!isValid())
		return AMNumber(AMNumber::InvalidError);

#ifdef AM_ENABLE_BOUNDS_CHECKING
	for (int i = 0, size = axes_.size(); i < size; i++)
		if (indexes.at(i) >= axes_.at(i).size)
			return AMNumber(AMNumber::OutOfBoundsError);
#endif

	if (!binningRange_.isValid())
		return AMNumber(AMNumber::InvalidError);

	if (cacheUpdateRequired_) {
		computeCachedValues();
	}

	int index = 0;

	switch(rank()){

	case 0:
		break;


	case 1:
		index = indexes.i();
		break;

	case 2:
		index = indexes.i()*size(1) + indexes.j();
		break;

	case 3:
		index = indexes.i()*size(1)*size(2) + indexes.j()*size(2) + indexes.k();
		break;
	}

	return cachedData_.at(index);
}

bool AMRegionOfInterestAB::values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const
{
	if(indexStart.rank() != rank() || indexEnd.rank() != indexStart.rank())
		return false;

	if(!isValid())
		return false;

#ifdef AM_ENABLE_BOUNDS_CHECKING
	for (int i = 0, size = axes_.size(); i < size; i++)
		if (indexEnd.at(i) >= axes_.at(i).size || (unsigned)indexStart.at(i) > (unsigned)indexEnd.at(i))
			return false;
#endif

	if (!binningRange_.isValid())
		return false;

	if (cacheUpdateRequired_)
		computeCachedValues();

	int totalSize = indexStart.totalPointsTo(indexEnd);
	memcpy(outputValues, cachedData_.constData()+indexStart.flatIndexInArrayOfSize(size()), totalSize*sizeof(double));

	return true;
}

void AMRegionOfInterestAB::setBinningRange(const AMRange &newRange)
{
	binningRange_ = newRange;
	cacheUpdateRequired_ = true;
	dirtyIndices_.clear();
	emitValuesChanged();
}

void AMRegionOfInterestAB::setBinningRangeLowerBound(double lowerBound)
{
	binningRange_.setMinimum(lowerBound);
	cacheUpdateRequired_ = true;
	dirtyIndices_.clear();
	emitValuesChanged();
}

void AMRegionOfInterestAB::setBinningRangeUpperBound(double upperBound)
{
	binningRange_.setMaximum(upperBound);
	cacheUpdateRequired_ = true;
	dirtyIndices_.clear();
	emitValuesChanged();
}

AMNumber AMRegionOfInterestAB::axisValue(int axisNumber, int index) const
{
	if(!isValid())
		return AMNumber(AMNumber::InvalidError);

	if(axisNumber < 0 || axisNumber >= rank())
		return AMNumber(AMNumber::DimensionError);

#ifdef AM_ENABLE_BOUNDS_CHECKING
	if (index < 0 || index >= spectrum_->size(rank()))
		return AMNumber(AMNumber::OutOfBoundsError);
#endif

	if (rank() == 0)
		return AMNumber(AMNumber::Null);

	return spectrum_->axisValue(axisNumber, index);
}

bool AMRegionOfInterestAB::axisValues(int axisNumber, int startIndex, int endIndex, double *outputValues) const
{
	if (!isValid())
		return false;

	if (axisNumber < 0 || axisNumber >= rank())
		return false;

#ifdef AM_ENABLE_BOUNDS_CHECKING
	if (startIndex < 0 || startIndex >= spectrum_->size(rank()) || endIndex < 0 || endIndex >= spectrum_->size(rank()))
		return false;
#endif

	if (rank() == 0)
		return false;

	return spectrum_->axisValues(axisNumber, startIndex, endIndex, outputValues);
}

void AMRegionOfInterestAB::onInputSourceValuesChanged(const AMnDIndex& start, const AMnDIndex& end)
{
	// This doesn't need to be really thorough like other more general AB's because regions of interest only ever look at the last axis for data.
	AMnDIndex newStart = start;
	AMnDIndex newEnd = end;
	newStart.setRank(rank());
	newEnd.setRank(rank());
	cacheUpdateRequired_ = true;

//	if (newStart == newEnd) {
//		dirtyIndices_ << start;
//	}

	emitValuesChanged(newStart, newEnd);
}

void AMRegionOfInterestAB::onInputSourceSizeChanged()
{
	for (int i = 0, size = axes_.size(); i < size; i++)
		axes_[i].size = spectrum_->size(i);

	cacheUpdateRequired_ = true;
	cachedData_ = QVector<double>(size().product());
	emitSizeChanged();
}

void AMRegionOfInterestAB::onInputSourceStateChanged()
{
	// just in case the size has changed while the input source was invalid, and now it's going valid. Do we need this? probably not, if the input source is well behaved. But it's pretty inexpensive to do it twice... and we know we'll get the size right everytime it goes valid.
	onInputSourceSizeChanged();
	reviewState();
}

void AMRegionOfInterestAB::setInputDataSourcesImplementation(const QList<AMDataSource*>& dataSources)
{
	// disconnect connections from old sources, if they exist.
	if(spectrum_ != 0) {

		disconnect(spectrum_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		disconnect(spectrum_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		disconnect(spectrum_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));

		spectrum_ = 0;
	}

	if(dataSources.isEmpty()) {

		sources_.clear();
		setDescription("-- No input data --");
		axes_.clear();
	}

	// we know that this will only be called with valid input source
	else {

		spectrum_ = dataSources.first();
		sources_ = dataSources;

		setDescription(name());
		axes_.clear();

		for (int i = 0, size = spectrum_->rank()-1; i < size; i++)
			axes_.append(spectrum_->axisInfoAt(i));

		cacheUpdateRequired_ = true;
		cachedData_ = QVector<double>(size().product());

		connect(spectrum_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		connect(spectrum_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		connect(spectrum_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
	}

	reviewState();

	emitSizeChanged();
	emitValuesChanged();
	emitAxisInfoChanged();
	emitInfoChanged();
}


void AMRegionOfInterestAB::reviewState()
{
	if (spectrum_ == 0 || !spectrum_->isValid() || !binningRange_.isValid()){

		setState(AMDataSource::InvalidFlag);
	}
	else
		setState(0);
}
#include <QDebug>
void AMRegionOfInterestAB::computeCachedValues() const
{
	// Need to turn the range into index positions.
	AMAxisInfo axisInfoOfInterest = spectrum_->axisInfoAt(spectrum_->rank()-1);

	int minimum = int((binningRange_.minimum() - double(axisInfoOfInterest.start))/double(axisInfoOfInterest.increment));
	int maximum = int((binningRange_.maximum() - double(axisInfoOfInterest.start))/double(axisInfoOfInterest.increment));
	int axisLength = maximum - minimum + 1;
	AMnDIndex start = AMnDIndex(spectrum_->rank(), AMnDIndex::DoInit);
	AMnDIndex end = spectrum_->size()-1;

	if (dirtyIndices_.isEmpty()){

		start = AMnDIndex(spectrum_->rank(), AMnDIndex::DoInit);
		end = spectrum_->size()-1;
	}

	else{

		start = dirtyIndices_.first();
		end = dirtyIndices_.last();
	}

	start[rank()] = minimum;
	end[rank()] = maximum;
	AMnDIndex flatIndexStart = start;
	flatIndexStart.setRank(rank());

	int totalPoints = start.totalPointsTo(end);
	int flatStartIndex = flatIndexStart.flatIndexInArrayOfSize(size());
	QVector<double> data = QVector<double>(totalPoints);
	spectrum_->values(start, end, data.data());

	cachedData_.fill(-1);

	for (int i = 0; i < totalPoints; i++){

		int insertIndex = int((flatStartIndex+i)/axisLength);

		if (data.at(i) == -1)
			cachedData_[insertIndex] = -1;

		else {
			if ((i%axisLength) == 0)
				cachedData_[insertIndex] = 0;

			cachedData_[insertIndex] += data.at(i);
		}
	}

	if (dirtyIndices_.isEmpty())
		cachedDataRange_ = AMUtility::rangeFinder(cachedData_, -1);

	else{
		AMRange cachedRange = AMUtility::rangeFinder(cachedData_.mid(flatStartIndex, totalPoints), -1);

		if (cachedDataRange_.minimum() > cachedRange.minimum())
			cachedDataRange_.setMinimum(cachedRange.minimum());

		if (cachedDataRange_.maximum() < cachedRange.maximum())
			cachedDataRange_.setMaximum(cachedRange.maximum());
	}

	cacheUpdateRequired_ = false;
	dirtyIndices_.clear();
}
