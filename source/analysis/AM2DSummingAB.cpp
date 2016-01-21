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


#include "AM2DSummingAB.h"

#include "util/AMUtility.h"

/// Constructor. \c outputName is the name() for the output data source.
 AM2DSummingAB::~AM2DSummingAB(){}
AM2DSummingAB::AM2DSummingAB(const QString& outputName, QObject* parent)
	: AMStandardAnalysisBlock(outputName, parent) {

	inputSource_ = 0;
	sumAxis_ = 0;
	sumRangeMin_ = 0;
	sumRangeMax_ = 0;
	analyzedName_ = "";
	canAnalyze_ = false;

	// cached information
	cacheUpdateRequired_ = false;
	dirtyIndices_.clear();
	cachedDataRange_ = AMRange();

	axes_ << AMAxisInfo("invalid", 0, "No input data");
	setState(AMDataSource::InvalidFlag);
}

// Check if a set of inputs is valid. The empty list (no inputs) must always be valid. For non-empty lists, our specific requirements are...
/* - there must be a single input source or a list of 2D data sources
	- the rank() of that input source must be 2 (two-dimensional)
	*/
bool AM2DSummingAB::areInputDataSourcesAcceptable(const QList<AMDataSource*>& dataSources) const {

	if(dataSources.isEmpty())
		return true;	// always acceptable, the null input.

	// otherwise we need a single input source, with a rank of 2.
	if(dataSources.count() == 1 && dataSources.at(0)->rank() == 2)
		return true;

	// Or we need a list of 2D data sources.
	else if (dataSources.count() > 1){

		for (int i = 0; i < dataSources.count(); i++)
			if (dataSources.at(i)->rank() != 2)
				return false;

		return true;
	}

	return false;
}


/// Set the data source inputs.
void AM2DSummingAB::setInputDataSourcesImplementation(const QList<AMDataSource*>& dataSources)
{

	AMDataSource *currentInputDataSource = 0;

	QString defaultDescriptionForEmptyDataSource;
	if (dataSources.isEmpty()) {
		sources_.clear();
		defaultDescriptionForEmptyDataSource = "-- No input data --";
	} else {
		// we know that this will only be called with valid input source
		sources_ = dataSources;
		defaultDescriptionForEmptyDataSource = "Sum";

		if (dataSources.count() == 1) {
			currentInputDataSource = dataSources.at(0);
		} else {
			int index = indexOfInputSource(analyzedName_);
			if (index >= 0){
				currentInputDataSource = inputDataSourceAt(index);
			}
		}
	}

	updateInputSource(currentInputDataSource);
}

void AM2DSummingAB::setAnalyzedName(const QString &name)
{
	if (analyzedName_ == name)
		return;

	analyzedName_ = name;
	setModified(true);

	AMDataSource *dataSource = 0;
	int index = indexOfInputSource(analyzedName_);
	if (index >= 0){
		dataSource = inputDataSourceAt(index);
	}

	updateInputSource(dataSource);
}

void AM2DSummingAB::updateInputSource(AMDataSource *dataSource, const QString emptyDataSourceDescription)
{
	// disconnect connections from old source, if it exists.
	if(inputSource_) {
		disconnect(inputSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		disconnect(inputSource_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		disconnect(inputSource_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
		inputSource_ = 0;
	}

	// update inputSource_
	if (dataSource) {
		inputSource_ = dataSource;
		canAnalyze_ = true;

		int otherAxis = (sumAxis_ == 0) ? 1 : 0;
		axes_[0] = inputSource_->axisInfoAt(otherAxis);
		setDescription(QString("%1 Summed (over %2)")
					   .arg(inputSource_->name())
					   .arg(inputSource_->axisInfoAt(sumAxis_).name));

		cacheUpdateRequired_ = true;
		dirtyIndices_.clear();
		cachedData_ = QVector<double>(size().product());

		connect(inputSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		connect(inputSource_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		connect(inputSource_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));

	} else {
		inputSource_ = 0;
		canAnalyze_ = false;

		axes_[0] = AMAxisInfo("invalid", 0, "No input data");
		setDescription(emptyDataSourceDescription);
	}

	reviewState();

	emitSizeChanged();
	emitValuesChanged();
	emitAxisInfoChanged();
	emitInfoChanged();
}

void AM2DSummingAB::reviewState()
{
	if(!canAnalyze_ || inputSource_ == 0 || !inputSource_->isValid()) {
		setState(AMDataSource::InvalidFlag);
		return;
	}

	int s = inputSource_->size(sumAxis_);

	if(sumRangeMin_ >= s || sumRangeMax_ >= s || sumRangeMin_ > sumRangeMax_) {
		setState(AMDataSource::InvalidFlag);
	}
	else
		setState(0);
}

void AM2DSummingAB::computeCachedValues() const
{
	if (inputSource_ == 0) {
		return;
	}

	AMnDIndex start = AMnDIndex();
	AMnDIndex end = AMnDIndex();

	if (dirtyIndices_.isEmpty()){
		start = AMnDIndex(inputSource_->rank(), AMnDIndex::DoInit);
		end = inputSource_->size()-1;

	} else {
		start = dirtyIndices_.first();
		end = dirtyIndices_.last();
	}

	start[sumAxis_] = sumRangeMin_;
	end[sumAxis_] = sumRangeMax_;
	AMnDIndex flatIndexStart = start;
	flatIndexStart.setRank(rank());

	int totalPoints = start.totalPointsTo(end);
	int flatStartIndex = flatIndexStart.flatIndexInArrayOfSize(size());
	int sumRange = sumRangeMax_-sumRangeMin_+1;
	QVector<double> data = QVector<double>(totalPoints);
	inputSource_->values(start, end, data.data());
	cachedData_.fill(-1);

	for (int i = 0; i < totalPoints; i++){

		int insertIndex = int((flatStartIndex+i)/sumRange);

		if (data.at(i) == -1) {
			cachedData_[insertIndex] = -1;

		} else {
			if ((i%sumRange) == 0)
				cachedData_[insertIndex] = 0;

			cachedData_[insertIndex] += data.at(i);
		}
	}

	if (dirtyIndices_.isEmpty()) {
		cachedDataRange_ = AMUtility::rangeFinder(cachedData_, -1);

	} else {
		AMRange cachedRange = AMUtility::rangeFinder(cachedData_.mid(flatStartIndex, totalPoints), -1);

		if (cachedDataRange_.minimum() > cachedRange.minimum())
			cachedDataRange_.setMinimum(cachedRange.minimum());

		if (cachedDataRange_.maximum() < cachedRange.maximum())
			cachedDataRange_.setMaximum(cachedRange.maximum());
	}

	cacheUpdateRequired_ = false;
	dirtyIndices_.clear();
}

AMNumber AM2DSummingAB::value(const AMnDIndex& indexes) const {
	if(indexes.rank() != 1)
		return AMNumber(AMNumber::DimensionError);

	if(!isValid())
		return AMNumber(AMNumber::InvalidError);

#ifdef AM_ENABLE_BOUNDS_CHECKING
	if((unsigned)indexes.i() >= (unsigned)axes_.at(0).size)
		return AMNumber(AMNumber::OutOfBoundsError);
#endif

	if (cacheUpdateRequired_)
	    computeCachedValues();

	return cachedData_.at(indexes.i());
}

bool AM2DSummingAB::values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const
{
	if(indexStart.rank() != 1 || indexEnd.rank() != 1)
		return false;

	if(!isValid())
		return false;

	if (!canAnalyze())
		return false;

#ifdef AM_ENABLE_BOUNDS_CHECKING
	if((unsigned)indexEnd.i() >= (unsigned)axes_.at(0).size || (unsigned)indexStart.i() > (unsigned)indexEnd.i())
		return false;
#endif

	if (sumRangeMin_ > sumRangeMax_)
		return false;

	if (cacheUpdateRequired_)
		computeCachedValues();

	int totalSize = indexStart.totalPointsTo(indexEnd);
	memcpy(outputValues, cachedData_.constData()+indexStart.i(), totalSize*sizeof(double));

	return true;
}

AMNumber AM2DSummingAB::axisValue(int axisNumber, int index) const {

	if(!isValid() || inputSource_ == 0)
		return AMNumber(AMNumber::InvalidError);

	if(axisNumber != 0 && axisNumber != 1)
		return AMNumber(AMNumber::DimensionError);

	int otherAxis = (sumAxis_ == 0) ? 1 : 0;

	return inputSource_->axisValue(otherAxis, index);
}

bool AM2DSummingAB::axisValues(int axisNumber, int startIndex, int endIndex, double *outputValues) const
{
	if (!isValid() || inputSource_ == 0)
		return false;

	if (axisNumber != 0 && axisNumber != 1)
		return false;

	int otherAxis = (sumAxis_ == 0) ? 1 : 0;

	if (startIndex >= inputSource_->size(otherAxis) || endIndex >= inputSource_->size(otherAxis))
		return false;

	return inputSource_->axisValues(otherAxis, startIndex, endIndex, outputValues);
}

void AM2DSummingAB::setSumAxis(int sumAxis)
{
	if(sumAxis < 0 || sumAxis >= 2)
		return;

	if(sumAxis == sumAxis_)
		return;	// no change

	sumAxis_ = sumAxis;
	setModified(true);
	int otherAxis = (sumAxis_ == 0) ? 1 : 0;
	setSumRangeMin(0);

	// if we have a data source, set our output axisInfo to match the input source's other axis. This also changes our size.
	if(inputSource_) {
		setSumRangeMax(inputSource_->size(otherAxis)-1);

		axes_[0] = inputSource_->axisInfoAt(otherAxis);
		setDescription(QString("%1 summed (over %2)")
					   .arg(inputSource_->name())
					   .arg(inputSource_->axisInfoAt(sumAxis_).name));
	} else {
		setSumRangeMax(0);
	}

	cacheUpdateRequired_ = true;
	dirtyIndices_.clear();

	reviewState();
	emitSizeChanged();
	emitValuesChanged();
	emitAxisInfoChanged();
	emitInfoChanged();
}

void AM2DSummingAB::setSumRangeMin(int sumRangeMin)
{
	// no change
	if(sumRangeMin == sumRangeMin_)
		return;

	sumRangeMin_ = sumRangeMin;
	setModified(true);

	cacheUpdateRequired_ = true;
	dirtyIndices_.clear();

	reviewState();
	emitValuesChanged();
}

void AM2DSummingAB::setSumRangeMax(int sumRangeMax)
{
	if(sumRangeMax == sumRangeMax_)
		return;

	sumRangeMax_ = sumRangeMax;
	setModified(true);

	cacheUpdateRequired_ = true;
	dirtyIndices_.clear();

	reviewState();
	emitValuesChanged();
}

// Connected to be called when the values of the input data source change
void AM2DSummingAB::onInputSourceValuesChanged(const AMnDIndex& start, const AMnDIndex& end)
{
	int otherAxis = (sumAxis_ == 0) ? 1 : 0;
	int startIndex = start.at(otherAxis);
	int endIndex = end.at(otherAxis);

	cacheUpdateRequired_ = true;

//	if (startIndex == endIndex)
//		dirtyIndices_ << start;

	emitValuesChanged(startIndex, endIndex);
}

/// Connected to be called when the size of the input source changes
void AM2DSummingAB::onInputSourceSizeChanged() {

	int otherAxis = (sumAxis_ == 0) ? 1 : 0;
	axes_[0].size = inputSource_->size(otherAxis);

	cacheUpdateRequired_ = true;
	dirtyIndices_.clear();
	cachedData_ = QVector<double>(size().product());

	emitSizeChanged();
}

/// Connected to be called when the state() flags of any input source change
void AM2DSummingAB::onInputSourceStateChanged() {

	// just in case the size has changed while the input source was invalid, and now it's going valid.  Do we need this? probably not, if the input source is well behaved. But it's pretty inexpensive to do it twice... and we know we'll get the size right everytime it goes valid.
	onInputSourceSizeChanged();

	reviewState();
}

#include "analysis/AM2DSummingABEditor.h"
QWidget* AM2DSummingAB::createEditorWidget() {
	return new AM2DSummingABEditor(this);
}

