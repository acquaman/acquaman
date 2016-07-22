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


#include "AM1DIntegralAB.h"

#include "analysis/AM1DBasicIntegralABEditor.h"

 AM1DIntegralAB::~AM1DIntegralAB(){}
AM1DIntegralAB::AM1DIntegralAB(const QString &outputName, QObject *parent)
	: AMStandardAnalysisBlock(outputName, parent)
{
	inputSource_ = 0;
	analyzedName_ = "";
	canAnalyze_ = false;

	cacheCompletelyInvalid_ = false;

	axes_ << AMAxisInfo("invalid", 0, "No input data");
	setState(AMDataSource::InvalidFlag);
}

// Check if a set of inputs is valid. The empty list (no inputs) must always be valid. For non-empty lists, our specific requirements are...
/* - there must be a single input source or a list of 1D data sources
 - the rank() of that input source must be 1 (one-dimensional)
 */

bool AM1DIntegralAB::areInputDataSourcesAcceptable(const QList<AMDataSource*>& dataSources) const {
	if(dataSources.isEmpty())
		return true;	// always acceptable, the null input.

	// otherwise we need a single input source, with a rank of 1.
	if(dataSources.count() == 1 && dataSources.at(0)->rank() == 1)
		return true;

	// Or we need a list of 1D data sources.
	else if (dataSources.count() > 1){

		for (int i = 0; i < dataSources.count(); i++)
			if (dataSources.at(i)->rank() != 1)
				return false;

		return true;
	}

	return false;
}

void AM1DIntegralAB::setAnalyzedName(const QString &name)
{
	analyzedName_ = name;
	setModified(true);
	canAnalyze_ = canAnalyze(name);
	setInputSource();
}

// Set the data source inputs.
void AM1DIntegralAB::setInputDataSourcesImplementation(const QList<AMDataSource*>& dataSources) {

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
		setDescription("Integral");
	}

	// we know that this will only be called with valid input source
	else if (dataSources.count() == 1){
		inputSource_ = dataSources.at(0);
		sources_ = dataSources;
		canAnalyze_ = true;

		axes_[0] = inputSource_->axisInfoAt(0);

		setDescription(QString("Integral of %1")
					   .arg(inputSource_->name()));

		connect(inputSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		connect(inputSource_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		connect(inputSource_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));

	}

	else {

		sources_ = dataSources;
		setInputSource();
	}

	reviewState();

	emitSizeChanged(0);
	emitValuesChanged();
	emitAxisInfoChanged(0);
	emitInfoChanged();
}

void AM1DIntegralAB::setInputSource()
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

		axes_[0] = inputSource_->axisInfoAt(0);
		setDescription(QString("Integral of %1")
					   .arg(inputSource_->name()));

		connect(inputSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		connect(inputSource_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		connect(inputSource_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
	}

	else{

		inputSource_ = 0;
		canAnalyze_ = false;
		axes_[0] = AMAxisInfo("invalid", 0, "No input data");
		setDescription("Integral");
	}

	invalidateCache();
	reviewState();

	emitSizeChanged(0);
	emitValuesChanged();
	emitAxisInfoChanged(0);
	emitInfoChanged();
}

bool AM1DIntegralAB::canAnalyze(const QString &name) const
{
	// Always can analyze a single 1D data source.
	if (sources_.count() == 1)
		return true;

	if (indexOfInputSource(name) >= 0)
		return true;

	return false;
}

AMNumber AM1DIntegralAB::value(const AMnDIndex& indexes) const{
	if(indexes.rank() != 1)
		return AMNumber(AMNumber::DimensionError);

	if(!isValid())
		return AMNumber(AMNumber::InvalidError);

	if (!canAnalyze())
		return AMNumber(AMNumber::InvalidError);

	if((unsigned)indexes.i() >= (unsigned)axes_.at(0).size)
		return AMNumber(AMNumber::OutOfBoundsError);

	AMNumber rv = cachedValues_.at(indexes.i());
	// if we haven't calculated this sum yet, the cached value will be invalid. Sum and store.
	if(!rv.isValid()) {

		double newVal = 0.0;
		int index = indexes.i();

		// Implementing Int[f(x)] ~ 1/2 * SUM {(x[i+1] - x[i])*(f(x[i+1]) + f(x[i])) }
		if ((unsigned)index == ((unsigned)axes_.at(0).size - 1))
			newVal = 0.5*(double(inputSource_->axisValue(0, index))-double(inputSource_->axisValue(0, index-1)))*(double(inputSource_->value(index)) + double(inputSource_->value(index-1)));

		else
			newVal = 0.5*(double(inputSource_->axisValue(0, index+1))-double(inputSource_->axisValue(0, index)))*(double(inputSource_->value(index+1)) + double(inputSource_->value(index)));

		if (index != 0)
			newVal += double(value(AMnDIndex(index-1)));

		cachedValues_[index] = newVal;
		cacheCompletelyInvalid_ = false;

		return newVal;
	}
	// otherwise return the value we have.
	else
		return rv;
}

bool AM1DIntegralAB::values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const
{
	if(indexStart.rank() != 1 || indexEnd.rank() != 1)
		return false;

	if(!isValid())
		return false;

	if (!canAnalyze())
		return false;

	if((unsigned)indexEnd.i() >= (unsigned)axes_.at(0).size || (unsigned)indexStart.i() > (unsigned)indexEnd.i())
		return false;

	int totalSize = indexStart.totalPointsTo(indexEnd);
	int offset = indexStart.i();

	QVector<double> data = QVector<double>(totalSize);
	QVector<double> axis = QVector<double>(totalSize);

	AMAxisInfo axisInfo = inputSource_->axisInfoAt(0);

	inputSource_->values(indexStart, indexEnd, data.data());

	// This is much faster because we can compute all the axis values ourselves rather than ask for them one at a time.
	if (axisInfo.isUniform){

		double axisStart = double(axisInfo.start);
		double axisStep = double(axisInfo.increment);

		for (int i = 0; i < totalSize; i++)
			axis[i] = axisStart + (i+offset)*axisStep;
	}

	else {

		// Fill the axis vector.  Should minimize the overhead of making the same function calls and casting the values multiple times.
		for (int i = 0; i < totalSize; i++)
			axis[i] = inputSource_->axisValue(0, i+offset);
	}

	// Implementing Int[f(x)] ~ 1/2 * SUM {(x[i+1] - x[i])*(f(x[i+1]) + f(x[i])) }
	if (offset == 0)
		outputValues[0] = 0.5*((axis.at(1)-axis.at(0))*data.at(1)+data.at(0));

	// If we are not at the very start, then we need to call the value(-1) which could be very expensive if the cached value is not valid.
	else
		outputValues[0] = 0.5*((axis.at(1)-axis.at(0))*data.at(1)+data.at(0)) + double(value(AMnDIndex(offset-1)));

	for (int i = 1, count = totalSize-1; i < count; i++)
		outputValues[i] = 0.5*((axis.at(i+1)-axis.at(i))*data.at(i+1)+data.at(i)) + outputValues[i-1];

	outputValues[totalSize-1] = 0.5*((axis.at(totalSize-1)-axis.at(totalSize-2))*data.at(totalSize-1)+data.at(totalSize-2)) + outputValues[totalSize-2];

	for (int i = 0; i < totalSize; i++)
		cachedValues_[i+offset] = AMNumber(outputValues[i]);

	cacheCompletelyInvalid_ = false;

	return true;
}

AMNumber AM1DIntegralAB::axisValue(int axisNumber, int index) const{

	if(!isValid())
		return AMNumber(AMNumber::InvalidError);

	if(axisNumber != 0)
		return AMNumber(AMNumber::DimensionError);

	return inputSource_->axisValue(0, index);

}

bool AM1DIntegralAB::axisValues(int axisNumber, int startIndex, int endIndex, double *outputValues) const
{
	if (!isValid())
		return false;

	if (axisNumber != 0)
		return false;

	if (startIndex >= axes_.at(axisNumber).size || endIndex >= axes_.at(axisNumber).size)
		return false;

	return inputSource_->axisValues(axisNumber, startIndex, endIndex, outputValues);
}

// Connected to be called when the values of the input data source change
void AM1DIntegralAB::onInputSourceValuesChanged(const AMnDIndex& start, const AMnDIndex& end) {

	if(start.isValid() && end.isValid()) {

		int startIndex = start.i();
		int endIndex = end.i();

		for(int i=startIndex; i<=endIndex; i++)
			cachedValues_[i] = AMNumber();	// invalidate the changed region

		emitValuesChanged(startIndex, endIndex);
	}
	else {

		invalidateCache();
		emitValuesChanged();
	}
}

/// Connected to be called when the size of the input source changes
void AM1DIntegralAB::onInputSourceSizeChanged() {

	if (axes_.at(0).size != inputSource_->size(0)){
		axes_[0].size = inputSource_->size(0);
		cachedValues_.resize(axes_.at(0).size);
		emitSizeChanged(0);
	}
}

/// Connected to be called when the state() flags of any input source change
void AM1DIntegralAB::onInputSourceStateChanged() {

	// just in case the size has changed while the input source was invalid, and now it's going valid.  Do we need this? probably not, if the input source is well behaved. But it's pretty inexpensive to do it twice... and we know we'll get the size right everytime it goes valid.
	onInputSourceSizeChanged();
	reviewState();
}

void AM1DIntegralAB::reviewState(){

	if(!canAnalyze_ || inputSource_ == 0 || !inputSource_->isValid()) {
		setState(AMDataSource::InvalidFlag);
		return;
	}
	else
		setState(0);
}

QWidget *AM1DIntegralAB::createEditorWidget()
{
	 return new AM1DBasicIntegralABEditor(this);
}
