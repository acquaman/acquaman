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


#include "AM3DBinningAB.h"

 AM3DBinningAB::~AM3DBinningAB(){}
AM3DBinningAB::AM3DBinningAB(const QString &outputName, QObject *parent)
	: AMStandardAnalysisBlock(outputName, parent)
{
	sumAxis_ = 2;
	sumRangeMin_ = 0;
	sumRangeMax_ = 0;
	analyzedName_ = "";
	canAnalyze_ = false;
	inputSource_ = 0;
	cacheUpdateRequired_ = true;

	axes_ << AMAxisInfo("invalid", 0, "No input data") << AMAxisInfo("invalid", 0, "No input data");
	setState(AMDataSource::InvalidFlag);
}

// Check if a set of inputs is valid. The empty list (no inputs) must always be valid. For non-empty lists, our specific requirements are...
/* - there must be a single input source or a list of 3D data sources
	- the rank() of that input source must be 3 (two-dimensional)
	*/
bool AM3DBinningAB::areInputDataSourcesAcceptable(const QList<AMDataSource*>& dataSources) const {

	if(dataSources.isEmpty())
		return true;	// always acceptable, the null input.

	// otherwise we need a single input source, with a rank of 2.
	if(dataSources.count() == 1 && dataSources.at(0)->rank() == 3)
		return true;

	// Or we need a list of 2D data sources.
	else if (dataSources.count() > 1){

		for (int i = 0; i < dataSources.count(); i++)
			if (dataSources.at(i)->rank() != 3)
				return false;

		return true;
	}

	return false;
}


/// Set the data source inputs.
void AM3DBinningAB::setInputDataSourcesImplementation(const QList<AMDataSource*>& dataSources) {

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
			break;

		case 1:
			axes_[0] = inputSource_->axisInfoAt(0);
			axes_[1] = inputSource_->axisInfoAt(2);
			break;

		case 2:
			axes_[0] = inputSource_->axisInfoAt(0);
			axes_[1] = inputSource_->axisInfoAt(1);
			break;
		}

		cacheUpdateRequired_ = true;
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

void AM3DBinningAB::setAnalyzedName(const QString &name)
{
	if(analyzedName_ == name)
		return;
	analyzedName_ = name;
	setModified(true);
	canAnalyze_ = canAnalyze(name);
	setInputSource();
}

void AM3DBinningAB::setInputSource()
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
			break;

		case 1:
			axes_[0] = inputSource_->axisInfoAt(0);
			axes_[1] = inputSource_->axisInfoAt(2);
			break;

		case 2:
			axes_[0] = inputSource_->axisInfoAt(0);
			axes_[1] = inputSource_->axisInfoAt(1);
			break;
		}

		cacheUpdateRequired_ = true;
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
		setDescription("Sum");
	}

	reviewState();

	emitSizeChanged();
	emitValuesChanged();
	emitAxisInfoChanged();
	emitInfoChanged();
}

void AM3DBinningAB::reviewState()
{
    if(!canAnalyze_ || inputSource_ == 0 || !inputSource_->isValid()) {
	setState(AMDataSource::InvalidFlag);
	return;
    }

    int s = inputSource_->size(sumAxis_);

    if(sumRangeMin_ >= s || sumRangeMax_ >= s) {
	setState(AMDataSource::InvalidFlag);
    }
    else
	setState(0);
}

void AM3DBinningAB::computeCachedValues() const
{
	AMnDIndex start = AMnDIndex();
	AMnDIndex end = AMnDIndex();

	switch (sumAxis_){

	case 0:
		start = AMnDIndex(sumRangeMin_, 0, 0);
		end = AMnDIndex(sumRangeMax_, inputSource_->size(1)-1, inputSource_->size(2)-1);
		break;

	case 1:
		start = AMnDIndex(0, sumRangeMin_, 0);
		end = AMnDIndex(inputSource_->size(0)-1, sumRangeMax_, inputSource_->size(2)-1);
		break;

	case 2:
		start = AMnDIndex(0, 0, sumRangeMin_);
		end = AMnDIndex(inputSource_->size(0)-1, inputSource_->size(1)-1, sumRangeMax_);
		break;
	}

	int totalPoints = start.totalPointsTo(end);
	int sumRange = sumRangeMax_-sumRangeMin_+1;
	QVector<double> data = QVector<double>(totalPoints);
	inputSource_->values(start, end, data.data());
	cachedData_.fill(-1);

	for (int i = 0; i < totalPoints; i++){

		int insertIndex = int(i/sumRange);

		if (data.at(i) == -1)
			cachedData_[insertIndex] = -1;

		else {
			if ((i%sumRange) == 0)
				cachedData_[insertIndex] = 0;

			cachedData_[insertIndex] += data.at(i);
		}
	}

    cacheUpdateRequired_ = false;
}

bool AM3DBinningAB::canAnalyze(const QString &name) const
{
	// Always can analyze a single 3D data source.
	if (sources_.count() == 1)
		return true;

	if (indexOfInputSource(name) >= 0)
		return true;

	return false;
}

AMNumber AM3DBinningAB::value(const AMnDIndex& indexes) const {
	if(indexes.rank() != 2)
		return AMNumber(AMNumber::DimensionError);

	if(!isValid())
		return AMNumber(AMNumber::InvalidError);

#ifdef AM_ENABLE_BOUNDS_CHECKING
	if((unsigned)indexes.i() >= (unsigned)axes_.at(0).size
			|| (unsigned)indexes.j() >= (unsigned)axes_.at(1).size)
		return AMNumber(AMNumber::OutOfBoundsError);
#endif

    if (cacheUpdateRequired_)
	computeCachedValues();

    return cachedData_.at(indexes.i()*size(1)+indexes.j());
}

bool AM3DBinningAB::values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const
{
	if(indexStart.rank() != 2 || indexEnd.rank() != 2)
		return false;

	if(!isValid())
		return false;

	if (!canAnalyze())
		return false;

#ifdef AM_ENABLE_BOUNDS_CHECKING
	if((unsigned)indexEnd.i() >= (unsigned)axes_.at(0).size || (unsigned)indexStart.i() > (unsigned)indexEnd.i()
			|| (unsigned)indexEnd.j() >= (unsigned)axes_.at(1).size || (unsigned)indexStart.j() > (unsigned)indexEnd.j())
		return false;
#endif

	if (cacheUpdateRequired_)
		computeCachedValues();

	int totalSize = indexStart.totalPointsTo(indexEnd);
	memcpy(outputValues, cachedData_.constData()+indexStart.i()*size(1), totalSize*sizeof(double));

	return true;
}

AMNumber AM3DBinningAB::axisValue(int axisNumber, int index) const {

	if(!isValid())
		return AMNumber(AMNumber::InvalidError);

	if(axisNumber != 0 && axisNumber != 1)
		return AMNumber(AMNumber::DimensionError);

	int actualAxis = -1;

	switch (sumAxis_){

	case 0:
		actualAxis = axisNumber == 0 ? 1 : 2;
		break;

	case 1:
		actualAxis = axisNumber == 0 ? 0 : 2;
		break;

	case 2:
		actualAxis = axisNumber == 0 ? 0 : 1;
		break;
	}

	return inputSource_->axisValue(actualAxis, index);
}

bool AM3DBinningAB::axisValues(int axisNumber, int startIndex, int endIndex, double *outputValues) const
{
	if (!isValid())
		return false;

	if (axisNumber != 0 && axisNumber != 1 && axisNumber != 2)
		return false;

	int actualAxis = -1;

	switch (sumAxis_){

	case 0:
		actualAxis = axisNumber == 0 ? 1 : 2;
		break;

	case 1:
		actualAxis = axisNumber == 0 ? 0 : 2;
		break;

	case 2:
		actualAxis = axisNumber == 0 ? 0 : 1;
		break;
	}

	if (startIndex >= inputSource_->size(actualAxis) || endIndex >= inputSource_->size(actualAxis))
		return false;

    return inputSource_->axisValues(actualAxis, startIndex, endIndex, outputValues);
}

void AM3DBinningAB::setSumAxis(int sumAxis)
{
    if(sumAxis < 0 || sumAxis >= 3)
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
		setSumRangeMin(0);
		setSumRangeMax(inputSource_->size(0)-1);
		break;

	case 1:
		axes_[0] = inputSource_->axisInfoAt(0);
		axes_[1] = inputSource_->axisInfoAt(2);
		setSumRangeMin(0);
		setSumRangeMax(inputSource_->size(1)-1);
		break;

	case 2:
		axes_[0] = inputSource_->axisInfoAt(0);
		axes_[1] = inputSource_->axisInfoAt(1);
		setSumRangeMin(0);
		setSumRangeMax(inputSource_->size(2)-1);
		break;
	}

	setDescription(QString("%1 summed (over %2)")
		       .arg(inputSource_->name())
		       .arg(inputSource_->axisInfoAt(sumAxis_).name));
    }

    reviewState();

    emitSizeChanged();
    emitValuesChanged();
    emitAxisInfoChanged();
    emitInfoChanged();
    setModified(true);
}

void AM3DBinningAB::setSumRangeMin(int sumRangeMin)
{
    // no change
    if(sumRangeMin == sumRangeMin_)
	return;

    sumRangeMin_ = sumRangeMin;
    cacheUpdateRequired_ = true;
    reviewState();
    emitValuesChanged();
    setModified(true);
}

void AM3DBinningAB::setSumRangeMax(int sumRangeMax)
{
    if(sumRangeMax == sumRangeMax_)
	return;

    sumRangeMax_ = sumRangeMax;
    reviewState();
    emitValuesChanged();
    setModified(true);
}

// Connected to be called when the values of the input data source change
void AM3DBinningAB::onInputSourceValuesChanged(const AMnDIndex& start, const AMnDIndex& end) {

	if(start.isValid() && end.isValid()) {

		AMnDIndex startIndex = AMnDIndex(2, AMnDIndex::DoInit, 0);
		AMnDIndex endIndex = AMnDIndex(2, AMnDIndex::DoInit, 0);

		switch (sumAxis_){

		case 0:

			startIndex[0] = start.at(1);
			startIndex[1] = start.at(2);
			endIndex[0] = end.at(1);
			endIndex[1] = end.at(2);

			break;

		case 1:

			startIndex[0] = start.at(0);
			startIndex[1] = start.at(2);
			endIndex[0] = end.at(0);
			endIndex[1] = end.at(2);

			break;

		case 2:

			startIndex[0] = start.at(0);
			startIndex[1] = start.at(1);
			endIndex[0] = end.at(0);
			endIndex[1] = end.at(1);

			break;
		}

		cacheUpdateRequired_ = true;
		emitValuesChanged(startIndex, endIndex);
	}

    else{

	cacheUpdateRequired_ = true;
	emitValuesChanged();
    }
}

// Connected to be called when the size of the input source changes
void AM3DBinningAB::onInputSourceSizeChanged()
{
	switch (sumAxis_){

	case 0:

	axes_[0].size = inputSource_->size(1);
	axes_[1].size = inputSource_->size(2);

		break;

	case 1:

	axes_[0].size = inputSource_->size(0);
	axes_[1].size = inputSource_->size(2);

		break;

	case 2:

	axes_[0].size = inputSource_->size(0);
	axes_[1].size = inputSource_->size(1);

		break;
	}

	cacheUpdateRequired_ = true;
	cachedData_ = QVector<double>(size().product());
	emitSizeChanged();
}

void AM3DBinningAB::onInputSourceStateChanged() {

	// just in case the size has changed while the input source was invalid, and now it's going valid.  Do we need this? probably not, if the input source is well behaved. But it's pretty inexpensive to do it twice... and we know we'll get the size right everytime it goes valid.
	onInputSourceSizeChanged();
	reviewState();
}

#include "analysis/AM3DBinningABEditor.h"

QWidget* AM3DBinningAB::createEditorWidget()
{
	return new AM3DBinningABEditor(this);
}

