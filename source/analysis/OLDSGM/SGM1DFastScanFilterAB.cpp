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


#include "SGM1DFastScanFilterAB.h"
#include <QDebug>
#include <math.h>

 SGM1DFastScanFilterAB::~SGM1DFastScanFilterAB(){}
SGM1DFastScanFilterAB::SGM1DFastScanFilterAB(const QString &outputName, QObject *parent) :
	AMStandardAnalysisBlock(outputName, parent), inputAxis_("invalid", 0, "No input data")
{
	inputSource_ = 0;

	axes_ << AMAxisInfo("invalid", 0, "No input data");
	cacheCompletelyInvalid_ = true;
	setState(AMDataSource::InvalidFlag);
}

// Check if a set of inputs is valid. The empty list (no inputs) must always be valid. For non-empty lists, our specific requirements are...
/* - there must be a single input source
 - the rank() of that input source must be 1 (one-dimensional)
 */

bool SGM1DFastScanFilterAB::areInputDataSourcesAcceptable(const QList<AMDataSource*>& dataSources) const {
	if(dataSources.isEmpty())
		return true;	// always acceptable, the null input.

	// otherwise we need a single input source, with a rank of 1.
	if(dataSources.count() == 1 && dataSources.at(0)->rank() == 1)
		return true;

	return false;
}

// Set the data source inputs.
void SGM1DFastScanFilterAB::setInputDataSourcesImplementation(const QList<AMDataSource*>& dataSources) {

	// disconnect connections from old source, if it exists.
	if(inputSource_) {
		disconnect(inputSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		disconnect(inputSource_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		disconnect(inputSource_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
	}

	if(dataSources.isEmpty()) {
		inputSource_ = 0;
		sources_.clear();

		inputAxis_ = AMAxisInfo("invalid", 0, "No input data");
		axes_[0] = AMAxisInfo("invalid", 0, "No input data");
		setDescription("-- No input data --");
		invalidateCache();
	}

	// we know that this will only be called with valid input source
	else {
		inputSource_ = dataSources.at(0);
		sources_ = dataSources;
		inputAxis_ = inputSource_->axisInfoAt(0);
		axes_[0] = inputSource_->axisInfoAt(0);


		setDescription(QString("FastScan Filter of %1")
					   .arg(inputSource_->name()));

		connect(inputSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		connect(inputSource_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		connect(inputSource_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));

	}

	reviewState();

	emitSizeChanged(0);
	emitValuesChanged();
	emitAxisInfoChanged(0);
	emitInfoChanged();
}

bool SGM1DFastScanFilterAB::values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const
{
	if(indexStart.rank() != 1 || indexEnd.rank() != 1)
		return false;

	if(!isValid())
		return false;

#ifdef AM_ENABLE_BOUNDS_CHECKING
	if((unsigned)indexEnd.i() >= (unsigned)axes_.at(0).size || (unsigned)indexStart.i() > (unsigned)indexEnd.i())
		return false;
#endif

	int totalSize = indexStart.totalPointsTo(indexEnd);

	if (!cacheCompletelyInvalid_){

		int offset = indexStart.i();

		for (int i = 0; i < totalSize; i++)
			outputValues[i] = double(cachedValues_.at(i+offset));
	}

	else{

		for (int i = 0; i < totalSize; i++)
			outputValues[i] = 27.27;
	}

	return true;
}

AMNumber SGM1DFastScanFilterAB::value(const AMnDIndex& indexes) const{
	if(indexes.rank() != 1)
		return AMNumber(AMNumber::DimensionError);

	if(!isValid())
		return AMNumber(AMNumber::InvalidError);

#ifdef AM_ENABLE_BOUNDS_CHECKING
	if((unsigned)indexes.i() >= (unsigned)axes_.at(0).size)
		return AMNumber(AMNumber::OutOfBoundsError);
#endif

	int index = indexes.i();

	if(!cacheCompletelyInvalid_)
		return cachedValues_.at(index);

	return AMNumber(27.27);
}

/* NTBA - August 29, 2011 (David Chevrier)
   This Q_UNUSED probably needs to be fixed.
   UPDATED (Mark Boots): Removed doBoundsChecking as an argument; you still will want to implement bounds checking #ifdef AM_ENABLE_BOUNDS_CHECKING
  */
AMNumber SGM1DFastScanFilterAB::axisValue(int axisNumber, int index) const{

	if(!isValid() || cacheCompletelyInvalid_)
		return AMNumber(AMNumber::InvalidError);

	if(axisNumber != 0)
		return AMNumber(AMNumber::DimensionError);

	return cachedAxisValues_.at(index);
}

/// Connected to be called when the values of the input data source change
void SGM1DFastScanFilterAB::onInputSourceValuesChanged(const AMnDIndex& start, const AMnDIndex& end) {
	emitValuesChanged(start, end);
}

/// Connected to be called when the size of the input source changes
void SGM1DFastScanFilterAB::onInputSourceSizeChanged() {
	if(inputAxis_.size != inputSource_->size(0)){
		inputAxis_.size = inputSource_->size(0);

		QList<int> ignoreIndices;
		int numLost = 1;
		ignoreIndices << 0;

		int numMoving = 0;
		for(int x = 1; x < inputAxis_.size; x++){
			if( fabs((double)inputSource_->axisValue(0,x)-(double)inputSource_->axisValue(0, x-1)) < 0.001 ){
				numMoving = 0;
				ignoreIndices << x;
				numLost++;
			}
			else if(numMoving >= 3){
				if(numMoving == 3){
					ignoreIndices.pop_back();
					ignoreIndices.pop_back();
					numLost -= 2;
				}
				numMoving++;
			}
			else{
				ignoreIndices << x;
				numLost++;
				numMoving++;
			}
		}

		axes_[0].size = inputAxis_.size-numLost;
		invalidateCache();
		int goodIndex = 0;
		for(int x = 0; x < inputAxis_.size; x++){
			if(!ignoreIndices.contains(x)){
				cachedValues_[goodIndex] = inputSource_->value(x);
				cachedAxisValues_[goodIndex] = inputSource_->axisValue(0, x);
				goodIndex++;
			}
		}
		cacheCompletelyInvalid_ = false;

		emitSizeChanged(0);
	}
}

/// Connected to be called when the state() flags of any input source change
void SGM1DFastScanFilterAB::onInputSourceStateChanged() {

	// just in case the size has changed while the input source was invalid, and now it's going valid.  Do we need this? probably not, if the input source is well behaved. But it's pretty inexpensive to do it twice... and we know we'll get the size right everytime it goes valid.
	onInputSourceSizeChanged();
	reviewState();
}

void SGM1DFastScanFilterAB::reviewState(){
	if(inputSource_ == 0 || !inputSource_->isValid()) {
		setState(AMDataSource::InvalidFlag);
		return;
	}
	else
		setState(0);
}

/// helper function to clear the cachedValues_
void SGM1DFastScanFilterAB::invalidateCache() {
	if(!cacheCompletelyInvalid_ || cachedValues_.size() != axes_.at(0).size) {
		cachedValues_ = QVector<AMNumber>(axes_.at(0).size);	// everything in there is now AMNumber::Null.
		cachedAxisValues_ = QVector<AMNumber>(axes_.at(0).size);
		cacheCompletelyInvalid_ = true;
	}
}
