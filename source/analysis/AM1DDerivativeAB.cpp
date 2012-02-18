/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "AM1DDerivativeAB.h"

#include "analysis/AM1DBasicDerivativeABEditor.h"

AM1DDerivativeAB::AM1DDerivativeAB(const QString &outputName, QObject *parent) :
	AMStandardAnalysisBlock(outputName, parent)
{
	inputSource_ = 0;
	analyzedName_ = "";
	canAnalyze_ = false;

	axes_ << AMAxisInfo("invalid", 0, "No input data");
	setState(AMDataSource::InvalidFlag);
}

// Check if a set of inputs is valid. The empty list (no inputs) must always be valid. For non-empty lists, our specific requirements are...
/* - there must be a single input source or a list of 1D data sources
 - the rank() of that input source must be 1 (one-dimensional)
 */

bool AM1DDerivativeAB::areInputDataSourcesAcceptable(const QList<AMDataSource*>& dataSources) const {
	if(dataSources.isEmpty())
		return true;	// always acceptable; the null input.

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

// Set the data source inputs.
void AM1DDerivativeAB::setInputDataSourcesImplementation(const QList<AMDataSource*>& dataSources) {

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
		setDescription("Derivative");
	}

	// we know that this will only be called with valid input source
	else if (dataSources.count() == 1){
		inputSource_ = dataSources.at(0);
		sources_ = dataSources;
		canAnalyze_ = true;

		axes_[0] = inputSource_->axisInfoAt(0);

		setDescription(QString("Derivative of %1")
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

void AM1DDerivativeAB::setAnalyzedName(const QString &name)
{
	analyzedName_ = name;
	canAnalyze_ = canAnalyze(name);
	setInputSource();
}

void AM1DDerivativeAB::setInputSource()
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
		setDescription(QString("Derivative of %1")
					   .arg(inputSource_->name()));

		connect(inputSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		connect(inputSource_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		connect(inputSource_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
	}

	else{

		inputSource_ = 0;
		canAnalyze_ = false;
		axes_[0] = AMAxisInfo("invalid", 0, "No input data");
		setDescription("Derivative");
	}

	reviewState();

	emitSizeChanged(0);
	emitValuesChanged();
	emitAxisInfoChanged(0);
	emitInfoChanged();
}

bool AM1DDerivativeAB::canAnalyze(const QString &name) const
{
	// Always can analyze a single 1D data source.
	if (sources_.count() == 1)
		return true;

	if (indexOfInputSource(name) >= 0)
		return true;

	return false;
}

AMNumber AM1DDerivativeAB::value(const AMnDIndex& indexes, bool doBoundsChecking) const{
	if(indexes.rank() != 1)
		return AMNumber(AMNumber::DimensionError);

	if(!isValid())
		return AMNumber(AMNumber::InvalidError);

	if (!canAnalyze())
		return AMNumber(AMNumber::InvalidError);

	if(doBoundsChecking)
		if((unsigned)indexes.i() >= (unsigned)axes_.at(0).size)
			return AMNumber(AMNumber::OutOfBoundsError);

	int index = indexes.i();

	if(index == 0){
		return ((double)inputSource_->value(1, doBoundsChecking)-
				(double)inputSource_->value(0, doBoundsChecking))/
				((double)inputSource_->axisValue(0, 1, doBoundsChecking)-
				 (double)inputSource_->axisValue(0, 0, doBoundsChecking));
	}
	else{
		return ((double)inputSource_->value(index, doBoundsChecking)-(double)inputSource_->value(index-1, doBoundsChecking))/((double)inputSource_->axisValue(0, index, doBoundsChecking)-(double)inputSource_->axisValue(0, index-1,doBoundsChecking));
	}
}

AMNumber AM1DDerivativeAB::axisValue(int axisNumber, int index, bool doBoundsChecking) const{

	if(!isValid())
		return AMNumber(AMNumber::InvalidError);

	if(axisNumber != 0)
		return AMNumber(AMNumber::DimensionError);

	return inputSource_->axisValue(0, index, doBoundsChecking);

}

/// Connected to be called when the values of the input data source change
void AM1DDerivativeAB::onInputSourceValuesChanged(const AMnDIndex& start, const AMnDIndex& end) {
	emitValuesChanged(start, end);
}

/// Connected to be called when the size of the input source changes
void AM1DDerivativeAB::onInputSourceSizeChanged() {
	axes_[0].size = inputSource_->size(0);
	emitSizeChanged(0);
}

/// Connected to be called when the state() flags of any input source change
void AM1DDerivativeAB::onInputSourceStateChanged() {

	reviewState();

	// just in case the size has changed while the input source was invalid, and now it's going valid.  Do we need this? probably not, if the input source is well behaved. But it's pretty inexpensive to do it twice... and we know we'll get the size right everytime it goes valid.
	onInputSourceSizeChanged();
}

void AM1DDerivativeAB::reviewState(){

	if(!canAnalyze_ || inputSource_ == 0 || !inputSource_->isValid()) {
		setState(AMDataSource::InvalidFlag);
		return;
	}
	else
		setState(0);
}

QWidget *AM1DDerivativeAB::createEditorWidget()
{
	 return new AM1DBasicDerivativeABEditor(this);
}

bool AM1DDerivativeAB::loadFromDb(AMDatabase *db, int id) {
	bool success = AMDbObject::loadFromDb(db, id);
	if(success)
		AMDataSource::name_ = AMDbObject::name();	/// \todo This might change the name of a data-source in mid-life, which is technically not allowed.
	return success;
}
