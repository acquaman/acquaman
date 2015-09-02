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


#include "AM1DRunningAverageFilterAB.h"

 AM1DRunningAverageFilterAB::~AM1DRunningAverageFilterAB(){}
AM1DRunningAverageFilterAB::AM1DRunningAverageFilterAB(int filterSize, const QString &outputName, QObject *parent) :
	AMStandardAnalysisBlock(outputName, parent)
{
	inputSource_ = 0;
	filterSize_ = filterSize;

	axes_ << AMAxisInfo("invalid", 0, "No input data");
	setState(AMDataSource::InvalidFlag);
}

// Check if a set of inputs is valid. The empty list (no inputs) must always be valid. For non-empty lists, our specific requirements are...
/* - there must be a single input source
 - the rank() of that input source must be 1 (one-dimensional)
 */

bool AM1DRunningAverageFilterAB::areInputDataSourcesAcceptable(const QList<AMDataSource*>& dataSources) const {
	if(dataSources.isEmpty())
		return true;	// always acceptable, the null input.

	// otherwise we need a single input source, with a rank of 1.
	if(dataSources.count() == 1 && dataSources.at(0)->rank() == 1)
		return true;

	return false;
}

AMNumber AM1DRunningAverageFilterAB::value(const AMnDIndex& indexes) const{
	if(indexes.rank() != 1)
		return AMNumber(AMNumber::DimensionError);

	if(!isValid())
		return AMNumber(AMNumber::InvalidError);

#ifdef AM_ENABLE_BOUNDS_CHECKING
		if((unsigned)indexes.i() >= (unsigned)axes_.at(0).size)
			return AMNumber(AMNumber::OutOfBoundsError);
#endif

	int index = indexes.i();


	double runningAverage = 0;
	int numAvgPoints = 1;

	runningAverage += (double)inputSource_->value(index);

	for(int x = 1; x <= (filterSize_-1)/2; x++){

		if( (index-x) >= 0 ){

			runningAverage += (double)inputSource_->value(index-x);
			numAvgPoints++;
		}

		if( (index+x) < axes_.at(0).size ){

			runningAverage += (double)inputSource_->value(index+x);
			numAvgPoints++;
		}
	}

	return runningAverage/((double)numAvgPoints);
}

bool AM1DRunningAverageFilterAB::values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const
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

	QVector<double> data = QVector<double>(totalSize);
	inputSource_->values(indexStart, indexEnd, data.data());

	int numberOfPoints = (filterSize_-1)/2;
	double average = 0;
	int tempNumberOfPoints = 1;

	// If the filter size is bigger then the total size of the data then we will just check every point along the way.
	if (numberOfPoints > totalSize){

		for (int i = 0; i < totalSize; i++){

			average = data.at(i);
			tempNumberOfPoints = 1;

			for (int j = 1; j < numberOfPoints; j++){

				if ((i-j) >= 0){

					average += data.at(i-j);
					tempNumberOfPoints++;
				}

				if ((i+j) < totalSize){

					average += data.at(i+j);
					tempNumberOfPoints++;
				}
			}

			outputValues[i] = average/double(tempNumberOfPoints);
		}
	}

	// Otherwise, we can optimize the middle indices of the without having to check if we will be accessing data outside of the array range.
	else {

		// Compute the beginning values.
		for (int i = 0; i < numberOfPoints; i++){

			average = data.at(i);
			tempNumberOfPoints = 1;

			// Only need the substraction check at the beginning.
			for (int j = 1; j < numberOfPoints; j++){

				if ((i-j) >= 0){

					average += data.at(i-j);
					tempNumberOfPoints++;
				}

				average += data.at(i+j);
				tempNumberOfPoints++;
			}

			outputValues[i] = average/double(tempNumberOfPoints);
		}

		// No need for conditionals in the middle of the data.
		double averagePoints = double(numberOfPoints);

		for (int i = numberOfPoints, count = totalSize-numberOfPoints; i < count; i++){

			average = data.at(i);

			for (int j = 1; j < numberOfPoints; j++)
				average += data.at(i-j) + data.at(i+j);

			outputValues[i] = average/averagePoints;
		}

		// Compute the last values.
		for (int i = totalSize-numberOfPoints; i < totalSize; i++){

			average = data.at(i);
			tempNumberOfPoints = 1;

			// Only need the addition check at the end.
			for (int j = 1; j < numberOfPoints; j++){

				average += data.at(i-j);
				tempNumberOfPoints++;

				if ((i+j) < totalSize){

					average += data.at(i+j);
					tempNumberOfPoints++;
				}
			}

			outputValues[i] = average/double(tempNumberOfPoints);
		}
	}

	return true;
}

AMNumber AM1DRunningAverageFilterAB::axisValue(int axisNumber, int index) const{

	if(!isValid())
		return AMNumber(AMNumber::InvalidError);

	if(axisNumber != 0)
		return AMNumber(AMNumber::DimensionError);

	return inputSource_->axisValue(0, index);

}

bool AM1DRunningAverageFilterAB::axisValues(int axisNumber, int startIndex, int endIndex, double *outputValues) const
{
	if (!isValid())
		return false;

	if (axisNumber != 0)
		return false;

	if (startIndex >= axes_.at(axisNumber).size || endIndex >= axes_.at(axisNumber).size)
		return false;

	return inputSource_->axisValues(axisNumber, startIndex, endIndex, outputValues);
}

/// Connected to be called when the values of the input data source change
void AM1DRunningAverageFilterAB::onInputSourceValuesChanged(const AMnDIndex& start, const AMnDIndex& end) {
	emitValuesChanged(start, end);
}

/// Connected to be called when the size of the input source changes
void AM1DRunningAverageFilterAB::onInputSourceSizeChanged() {
	axes_[0].size = inputSource_->size(0);
	emitSizeChanged(0);
}

/// Connected to be called when the state() flags of any input source change
void AM1DRunningAverageFilterAB::onInputSourceStateChanged() {

	// just in case the size has changed while the input source was invalid, and now it's going valid.  Do we need this? probably not, if the input source is well behaved. But it's pretty inexpensive to do it twice... and we know we'll get the size right everytime it goes valid.
	onInputSourceSizeChanged();
	reviewState();
}

// Set the data source inputs.
void AM1DRunningAverageFilterAB::setInputDataSourcesImplementation(const QList<AMDataSource*>& dataSources) {

	// disconnect connections from old source, if it exists.
	if(inputSource_) {
		disconnect(inputSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		disconnect(inputSource_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		disconnect(inputSource_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
	}

	if(dataSources.isEmpty()) {
		inputSource_ = 0;
		sources_.clear();

		axes_[0] = AMAxisInfo("invalid", 0, "No input data");
		setDescription("-- No input data --");
	}

	// we know that this will only be called with valid input source
	else {
		inputSource_ = dataSources.at(0);
		sources_ = dataSources;

		axes_[0] = inputSource_->axisInfoAt(0);

		setDescription(QString("Running Average of %1")
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

void AM1DRunningAverageFilterAB::reviewState(){
	if(inputSource_ == 0 || !inputSource_->isValid()) {
		setState(AMDataSource::InvalidFlag);
		return;
	}
	else
		setState(0);
}
