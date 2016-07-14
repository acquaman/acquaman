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


#include "AM4DBinningAB.h"

#include "AM4DBinningAB.h"

#include "util/AMUtility.h"

 AM4DBinningAB::~AM4DBinningAB(){}
AM4DBinningAB::AM4DBinningAB(const QString &outputName, QObject *parent)
	: AMStandardAnalysisBlock(outputName, parent)
{
	sumAxis_ = 3;
	sumRangeMin_ = 0;
	sumRangeMax_ = 0;
	analyzedName_ = "";
	canAnalyze_ = false;
	cacheUpdateRequired_ = false;
	cachedDataRange_ = AMRange();

	axes_ << AMAxisInfo("invalid", 0, "No input data") << AMAxisInfo("invalid", 0, "No input data") << AMAxisInfo("invalid", 0, "No input data");
	setState(AMDataSource::InvalidFlag);
}

// Check if a set of inputs is valid. The empty list (no inputs) must always be valid. For non-empty lists, our specific requirements are...
/* - there must be a single input source or a list of 4D data sources
	- the rank() of that input source must be 4 (four-dimensional)
	*/
bool AM4DBinningAB::areInputDataSourcesAcceptable(const QList<AMDataSource*>& dataSources) const {

	if(dataSources.isEmpty())
		return true;	// always acceptable, the null input.

	// otherwise we need a single input source, with a rank of 2.
	if(dataSources.count() == 1 && dataSources.at(0)->rank() == 4)
		return true;

	// Or we need a list of 2D data sources.
	else if (dataSources.count() > 1){

		for (int i = 0; i < dataSources.count(); i++)
			if (dataSources.at(i)->rank() != 4)
				return false;

		return true;
	}

	return false;
}


// Set the data source inputs.
void AM4DBinningAB::setInputDataSourcesImplementation(const QList<AMDataSource*>& dataSources) {

	// disconnect connections from old source, if it exists.
	if(inputSource_) {
		disconnect(inputSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		disconnect(inputSource_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		disconnect(inputSource_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
		inputSource_ = 0;
	}

	if(dataSources.isEmpty()) {
		inputSource_ = 0;
		sources_.clear();
		canAnalyze_ = false;

		axes_[0] = AMAxisInfo("invalid", 0, "No input data");
		axes_[1] = AMAxisInfo("invalid", 0, "No input data");
		axes_[2] = AMAxisInfo("invalid", 0, "No input data");
		setDescription("-- No input data --");
	}

	// we know that this will only be called with valid input source
	else if (dataSources.count() == 1) {

		inputSource_ = dataSources.at(0);
		sources_ = dataSources;
		canAnalyze_ = true;

		switch (sumAxis_){

		case 0:
			axes_[0] = inputSource_->axisInfoAt(1);
			axes_[1] = inputSource_->axisInfoAt(2);
			axes_[2] = inputSource_->axisInfoAt(3);
			break;

		case 1:
			axes_[0] = inputSource_->axisInfoAt(0);
			axes_[1] = inputSource_->axisInfoAt(2);
			axes_[2] = inputSource_->axisInfoAt(3);
			break;

		case 2:
			axes_[0] = inputSource_->axisInfoAt(0);
			axes_[1] = inputSource_->axisInfoAt(1);
			axes_[2] = inputSource_->axisInfoAt(3);
			break;

		case 3:
			axes_[0] = inputSource_->axisInfoAt(0);
			axes_[1] = inputSource_->axisInfoAt(1);
			axes_[2] = inputSource_->axisInfoAt(2);
		}

		cacheUpdateRequired_ = true;
		dirtyIndices_.clear();
		cachedData_ = QVector<double>(size().product());

		setDescription(QString("%1 Summed (over %2)")
						 .arg(inputSource_->name())
						 .arg(inputSource_->axisInfoAt(sumAxis_).name));

		connect(inputSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		connect(inputSource_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		connect(inputSource_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));

	}

	else {

		sources_ = dataSources;
		setInputSource();
	}

	reviewState();

	emitSizeChanged();
	emitValuesChanged();
	emitAxisInfoChanged();
	emitInfoChanged();
}

void AM4DBinningAB::setAnalyzedName(const QString &name)
{
	if(analyzedName_ == name)
		return;
	analyzedName_ = name;
	setModified(true);
	canAnalyze_ = canAnalyze(name);
	setInputSource();
}

void AM4DBinningAB::setInputSource()
{
	// disconnect connections from old source, if it exists.
	if(inputSource_) {
		disconnect(inputSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		disconnect(inputSource_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		disconnect(inputSource_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
		inputSource_ = 0;
	}

	int index = indexOfInputSource(analyzedName_);

	if (index >= 0){

		inputSource_ = inputDataSourceAt(index);
		canAnalyze_ = true;

		switch (sumAxis_){

		case 0:
			axes_[0] = inputSource_->axisInfoAt(1);
			axes_[1] = inputSource_->axisInfoAt(2);
			axes_[2] = inputSource_->axisInfoAt(3);
			break;

		case 1:
			axes_[0] = inputSource_->axisInfoAt(0);
			axes_[1] = inputSource_->axisInfoAt(2);
			axes_[2] = inputSource_->axisInfoAt(3);
			break;

		case 2:
			axes_[0] = inputSource_->axisInfoAt(0);
			axes_[1] = inputSource_->axisInfoAt(1);
			axes_[2] = inputSource_->axisInfoAt(3);
			break;

		case 3:
			axes_[0] = inputSource_->axisInfoAt(0);
			axes_[1] = inputSource_->axisInfoAt(1);
			axes_[2] = inputSource_->axisInfoAt(2);
		}

		cacheUpdateRequired_ = true;
		dirtyIndices_.clear();
		cachedData_ = QVector<double>(size().product());

		setDescription(QString("%1 Summed (over %2)")
					   .arg(inputSource_->name())
					   .arg(inputSource_->axisInfoAt(sumAxis_).name));

		connect(inputSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		connect(inputSource_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		connect(inputSource_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
	}

	else{

		inputSource_ = 0;
		canAnalyze_ = false;
		axes_[0] = AMAxisInfo("invalid", 0, "No input data");
		axes_[1] = AMAxisInfo("invalid", 0, "No input data");
		axes_[2] = AMAxisInfo("invalid", 0, "No input data");
		setDescription("Sum");
	}

	reviewState();

	emitSizeChanged();
	emitValuesChanged();
	emitAxisInfoChanged();
	emitInfoChanged();
}

void AM4DBinningAB::reviewState()
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

void AM4DBinningAB::computeCachedValues() const
{
	AMnDIndex start = AMnDIndex();
	AMnDIndex end = AMnDIndex();

	if (dirtyIndices_.isEmpty()){

		start = AMnDIndex(inputSource_->rank(), AMnDIndex::DoInit);
		end = inputSource_->size()-1;
		cachedData_.fill(-1);
	}

	else {

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

	for (int i = 0; i < totalPoints; i++){

		int insertIndex = flatStartIndex + int(i/sumRange);

		if (data.at(i) == -1)
			cachedData_[insertIndex] = -1;

		else {
			if ((i%sumRange) == 0)
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

bool AM4DBinningAB::canAnalyze(const QString &name) const
{
	// Always can analyze a single 3D data source.
	if (sources_.count() == 1)
		return true;

	if (indexOfInputSource(name) >= 0)
		return true;

	return false;
}

AMNumber AM4DBinningAB::value(const AMnDIndex& indexes) const {
	if(indexes.rank() != 3)
		return AMNumber(AMNumber::DimensionError);

	if(!isValid())
		return AMNumber(AMNumber::InvalidError);

	if((unsigned)indexes.i() >= (unsigned)axes_.at(0).size
			|| (unsigned)indexes.j() >= (unsigned)axes_.at(1).size
			|| (unsigned)indexes.k() >= (unsigned)axes_.at(2).size)
		return AMNumber(AMNumber::OutOfBoundsError);

	if (cacheUpdateRequired_)
	    computeCachedValues();

	return cachedData_.at(indexes.i()*size(1)*size(2)+indexes.j()*size(2)+indexes.k());
}

bool AM4DBinningAB::values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const
{
	if(indexStart.rank() != 3 || indexEnd.rank() != 3)
		return false;

	if(!isValid())
		return false;

	if (!canAnalyze())
		return false;

	if((unsigned)indexEnd.i() >= (unsigned)axes_.at(0).size || (unsigned)indexStart.i() > (unsigned)indexEnd.i()
			|| (unsigned)indexEnd.j() >= (unsigned)axes_.at(1).size || (unsigned)indexStart.j() > (unsigned)indexEnd.j()
			|| (unsigned)indexEnd.k() >= (unsigned)axes_.at(2).size || (unsigned)indexStart.k() > (unsigned)indexEnd.k())
		return false;

	if (cacheUpdateRequired_)
		computeCachedValues();

	int totalSize = indexStart.totalPointsTo(indexEnd);
	memcpy(outputValues, cachedData_.constData()+indexStart.flatIndexInArrayOfSize(size()), totalSize*sizeof(double));

	return true;
}

AMNumber AM4DBinningAB::axisValue(int axisNumber, int index) const {

	if(!isValid())
		return AMNumber(AMNumber::InvalidError);

	if(axisNumber != 0 && axisNumber != 1 && axisNumber != 2)
		return AMNumber(AMNumber::DimensionError);

	int actualAxis = -1;

	switch (sumAxis_){

	case 0:
		actualAxis = axisNumber + 1;
		break;

	case 1:
		actualAxis = axisNumber == 0 ? axisNumber : axisNumber + 1;
		break;

	case 2:
		actualAxis = axisNumber == 3 ? axisNumber + 1 : axisNumber;
		break;

	case 3:
		actualAxis = axisNumber;
		break;
	}

	return inputSource_->axisValue(actualAxis, index);
}

bool AM4DBinningAB::axisValues(int axisNumber, int startIndex, int endIndex, double *outputValues) const
{
	if (!isValid())
		return false;

	if (axisNumber != 0 && axisNumber != 1 && axisNumber != 2)
		return false;

	int actualAxis = -1;

	switch (sumAxis_){

	case 0:
		actualAxis = axisNumber + 1;
		break;

	case 1:
		actualAxis = axisNumber == 0 ? axisNumber : axisNumber + 1;
		break;

	case 2:
		actualAxis = axisNumber == 3 ? axisNumber + 1 : axisNumber;
		break;

	case 3:
		actualAxis = axisNumber;
		break;
	}

	if (startIndex >= inputSource_->size(actualAxis) || endIndex >= inputSource_->size(actualAxis))
		return false;

	return inputSource_->axisValues(actualAxis, startIndex, endIndex, outputValues);
}

void AM4DBinningAB::setSumAxis(int sumAxis)
{
	if(sumAxis < 0 || sumAxis >= 4)
		return;

	if(sumAxis == sumAxis_)
		return;	// no change

	sumAxis_ = sumAxis;

	// if we have a data source, set our output axisInfo to match the input source's other axis. This also changes our size.
	if(inputSource_) {

		switch (sumAxis_){

		case 0:
			axes_[0] = inputSource_->axisInfoAt(1);
			axes_[1] = inputSource_->axisInfoAt(2);
			axes_[2] = inputSource_->axisInfoAt(3);
			setSumRangeMin(0);
			setSumRangeMax(inputSource_->size(0)-1);
			break;

		case 1:
			axes_[0] = inputSource_->axisInfoAt(0);
			axes_[1] = inputSource_->axisInfoAt(2);
			axes_[2] = inputSource_->axisInfoAt(3);
			setSumRangeMin(0);
			setSumRangeMax(inputSource_->size(1)-1);
			break;

		case 2:
			axes_[0] = inputSource_->axisInfoAt(0);
			axes_[1] = inputSource_->axisInfoAt(1);
			axes_[2] = inputSource_->axisInfoAt(3);
			setSumRangeMin(0);
			setSumRangeMax(inputSource_->size(2)-1);
			break;

		case 3:
			axes_[0] = inputSource_->axisInfoAt(0);
			axes_[1] = inputSource_->axisInfoAt(1);
			axes_[2] = inputSource_->axisInfoAt(2);
			setSumRangeMin(0);
			setSumRangeMax(inputSource_->size(3)-1);
			break;
		}

		setDescription(QString("%1 summed (over %2)")
					   .arg(inputSource_->name())
					   .arg(inputSource_->axisInfoAt(sumAxis_).name));
	}

	reviewState();

	cacheUpdateRequired_ = true;
	dirtyIndices_.clear();
	setModified(true);

	emitSizeChanged();
	emitValuesChanged();
	emitAxisInfoChanged();
	emitInfoChanged();
}

void AM4DBinningAB::setSumRangeMin(int sumRangeMin)
{
	// no change
	if(sumRangeMin == sumRangeMin_)
		return;

	sumRangeMin_ = sumRangeMin;
	cacheUpdateRequired_ = true;
	dirtyIndices_.clear();
	reviewState();
	setModified(true);
	emitValuesChanged();
}

void AM4DBinningAB::setSumRangeMax(int sumRangeMax)
{
	if(sumRangeMax == sumRangeMax_)
		return;

	sumRangeMax_ = sumRangeMax;
	cacheUpdateRequired_ = true;
	dirtyIndices_.clear();
	reviewState();
	setModified(true);
	emitValuesChanged();
}

// Connected to be called when the values of the input data source change
void AM4DBinningAB::onInputSourceValuesChanged(const AMnDIndex& start, const AMnDIndex& end)
{
	AMnDIndex startIndex = AMnDIndex(3, AMnDIndex::DoInit, 0);
	AMnDIndex endIndex = AMnDIndex(3, AMnDIndex::DoInit, 0);

	switch (sumAxis_){

	case 0:

		startIndex[0] = start.at(1);
		startIndex[1] = start.at(2);
		startIndex[2] = start.at(3);
		endIndex[0] = end.at(1);
		endIndex[1] = end.at(2);
		endIndex[2] = end.at(3);

		break;

	case 1:

		startIndex[0] = start.at(0);
		startIndex[1] = start.at(2);
		startIndex[2] = start.at(3);
		endIndex[0] = end.at(0);
		endIndex[1] = end.at(2);
		endIndex[2] = end.at(3);

		break;

	case 2:

		startIndex[0] = start.at(0);
		startIndex[1] = start.at(1);
		startIndex[2] = start.at(2);
		endIndex[0] = end.at(0);
		endIndex[1] = end.at(1);
		endIndex[2] = end.at(2);

		break;
	}

	cacheUpdateRequired_ = true;

	if (startIndex == endIndex)
		dirtyIndices_ << start;

	emitValuesChanged(startIndex, endIndex);
}

// Connected to be called when the size of the input source changes
void AM4DBinningAB::onInputSourceSizeChanged()
{
	switch (sumAxis_){

	case 0:

		axes_[0].size = inputSource_->size(1);
		axes_[1].size = inputSource_->size(2);
		axes_[2].size = inputSource_->size(3);

		break;

	case 1:

		axes_[0].size = inputSource_->size(0);
		axes_[1].size = inputSource_->size(2);
		axes_[2].size = inputSource_->size(3);

		break;

	case 2:

		axes_[0].size = inputSource_->size(0);
		axes_[1].size = inputSource_->size(1);
		axes_[2].size = inputSource_->size(3);

		break;

	case 3:

		axes_[0].size = inputSource_->axisInfoAt(0).size;
		axes_[1].size = inputSource_->axisInfoAt(1).size;
		axes_[2].size = inputSource_->axisInfoAt(2).size;

		break;
	}

	cacheUpdateRequired_ = true;
	dirtyIndices_.clear();
	cachedData_ = QVector<double>(size().product());
	emitSizeChanged();
}

// Connected to be called when the state() flags of any input source change
void AM4DBinningAB::onInputSourceStateChanged() {

	// just in case the size has changed while the input source was invalid, and now it's going valid.  Do we need this? probably not, if the input source is well behaved. But it's pretty inexpensive to do it twice... and we know we'll get the size right everytime it goes valid.
	onInputSourceSizeChanged();
	reviewState();
}

#include "analysis/AM4DBinningABEditor.h"

QWidget* AM4DBinningAB::createEditorWidget()
{
	return new AM4DBinningABEditor(this);
}

