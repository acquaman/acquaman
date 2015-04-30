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


#include "AM1DDerivativeAB.h"

#include "analysis/AM1DBasicDerivativeABEditor.h"

AM1DDerivativeAB::~AM1DDerivativeAB(){}

AM1DDerivativeAB::AM1DDerivativeAB(const QString &outputName, QObject *parent)
	: AMStandardAnalysisBlock(outputName, parent)
{
	inputSource_ = 0;
	analyzedName_ = "";
	canAnalyze_ = false;

	axes_ << AMAxisInfo("invalid", 0, "No input data");
	setState(AMDataSource::InvalidFlag);
}

bool AM1DDerivativeAB::areInputDataSourcesAcceptable(const QList<AMDataSource*>& dataSources) const
{
	if(dataSources.isEmpty())
		return true;	// always acceptable, the null input.

	// We need a single input source, with a rank of 1.
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

void AM1DDerivativeAB::setAnalyzedName(const QString &name)
{
	analyzedName_ = name;
	setModified(true);
	canAnalyze_ = canAnalyze(name);
	setInputSource();
}

void AM1DDerivativeAB::setInputDataSourcesImplementation(const QList<AMDataSource*>& dataSources)
{
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

		setDescription(QString("Derivative of %1").arg(inputSource_->name()));

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
		setDescription(QString("Derivative of %1").arg(inputSource_->name()));

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

AMNumber AM1DDerivativeAB::value(const AMnDIndex& indexes) const
{
	if(indexes.rank() != 1)
		return AMNumber(AMNumber::DimensionError);

	if(!isValid())
		return AMNumber(AMNumber::InvalidError);

	if (!canAnalyze())
		return AMNumber(AMNumber::InvalidError);

#ifdef AM_ENABLE_BOUNDS_CHECKING
		if((unsigned)indexes.i() >= (unsigned)axes_.at(0).size)
			return AMNumber(AMNumber::OutOfBoundsError);
#endif

	int index = indexes.i();

	// Forward difference.
	if(index == 0){

		if ((double)inputSource_->axisValue(0, 1) == (double)inputSource_->axisValue(0, 0))
			return 0;

		return ((double)inputSource_->value(1)-
				(double)inputSource_->value(0))/
				((double)inputSource_->axisValue(0, 1)-
				 (double)inputSource_->axisValue(0, 0));
	}
	// Backward difference.
	else if(index+1 == axes_.at(0).size){

		if ((double)inputSource_->axisValue(0, index) == (double)inputSource_->axisValue(0, index-1))
				return 0;

		return ((double)inputSource_->value(index)-
				(double)inputSource_->value(index-1))/
				((double)inputSource_->axisValue(0, index)-
				 (double)inputSource_->axisValue(0, index-1));
	}
	// Central difference.
	else {

		if ((double)inputSource_->axisValue(0, index+1) == (double)inputSource_->axisValue(0, index-1))
				return 0;

		return ((double)inputSource_->value(index+1)-
				(double)inputSource_->value(index-1))/
				(2*((double)inputSource_->axisValue(0, index+1)-
				 (double)inputSource_->axisValue(0, index-1)));
	}
}

bool AM1DDerivativeAB::values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const
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

	// Because the indexStart and indexEnd might not correspond to 0 and size-1, we might need to grab more values from inputSource (from one side or the other).
	int totalSize = indexStart.totalPointsTo(indexEnd);
	int offset = indexStart.i();
	// Bools for knowing whether the indices we were given are at either extreme of the array.
	bool veryStart = (offset == 0);
	bool veryEnd = (totalSize == int(axes_.at(0).size));

	// Although substantially more code, I have split up each possibility so that it covers everything properly.  Perhaps later I'll find a code optimization.

	// If we are computing the entire thing then we can get the data without and changes.
	if (veryStart && veryEnd){

		QVector<double> data = QVector<double>(totalSize);
		QVector<double> axis = QVector<double>(totalSize);
		AMAxisInfo axisInfo = inputSource_->axisInfoAt(0);

		inputSource_->values(indexStart, indexEnd, data.data());

		// This is much faster because we can compute all the axis values ourselves rather than ask for them one at a time.
		if (axisInfo.isUniform){

			double axisStart = double(axisInfo.start);
			double axisStep = double(axisInfo.increment);

			for (int i = 0; i < totalSize; i++)
				axis[i] = axisStart + i*axisStep;

			// Because we computed the axis values we are guarenteed that the values won't be bad.
			outputValues[0] = (data.at(1)-data.at(0))/(axis.at(1)-axis.at(0));
			outputValues[totalSize-1] = (data.at(totalSize-1)-data.at(totalSize-2))/(axis.at(totalSize-1)-axis.at(totalSize-2));

			for (int i = 1, count = totalSize-1; i < count; i++)
				outputValues[i] = (data.at(i+1)-data.at(i-1))/(2*(axis.at(i+1)-axis.at(i-1)));
		}

		else {

			// Fill the axis vector.  Should minimize the overhead of making the same function calls and casting the values multiple times.
			for (int i = 0; i < totalSize; i++)
				axis[i] = double(inputSource_->axisValue(0, i));

			// Fill a list of all the indices that will cause division by zero.
			QList<int> badIndices;

			if (axis.at(0) == axis.at(1))
				badIndices.append(0);

			for (int i = 1, count = totalSize-1; i < count; i++)
				if (axis.at(i+1) == axis.at(i-1))
					badIndices.append(i);

			if (axis.at(totalSize-1) == axis.at(totalSize-2))
				badIndices.append(totalSize-1);

			// Compute all the values
			outputValues[0] = (data.at(1)-data.at(0))/(axis.at(1)-axis.at(0));
			outputValues[totalSize-1] = (data.at(totalSize-1)-data.at(totalSize-2))/(axis.at(totalSize-1)-axis.at(totalSize-2));

			for (int i = 1, count = totalSize-1; i < count; i++)
				outputValues[i] = (data.at(i+1)-data.at(i-1))/(2*(axis.at(i+1)-axis.at(i-1)));

			// Fix all the values where division by zero would have occured.  Unfortunately, the default value is currently 0, which is generally important when taking the derivative.
			for (int i = 0, count = badIndices.size(); i < count; i++)
				outputValues[badIndices.at(i)] = 0;
		}
	}

	// If we have the very start and not the very end then we have to get more data from the input source from the end.
	else if (veryStart){

		int dataSize = totalSize+1;
		QVector<double> data = QVector<double>(dataSize);
		QVector<double> axis = QVector<double>(dataSize);
		AMAxisInfo axisInfo = inputSource_->axisInfoAt(0);

		inputSource_->values(indexStart, AMnDIndex(indexEnd.i()+1), data.data());

		// This is much faster because we can compute all the axis values ourselves rather than ask for them one at a time.
		if (axisInfo.isUniform){

			double axisStart = double(axisInfo.start);
			double axisStep = double(axisInfo.increment);

			for (int i = 0; i < dataSize; i++)
				axis[i] = axisStart + i*axisStep;

			// Because we computed the axis values we are guarenteed that the values won't be bad.
			outputValues[0] = (data.at(1)-data.at(0))/(axis.at(1)-axis.at(0));

			// This is safe because data and axis have an extra point more at the end.
			for (int i = 1; i < totalSize; i++)
				outputValues[i] = (data.at(i+1)-data.at(i-1))/(2*(axis.at(i+1)-axis.at(i-1)));
		}

		else {

			// Fill the axis vector.  Should minimize the overhead of making the same function calls and casting the values multiple times.
			for (int i = 0; i < dataSize; i++)
				axis[i] = double(inputSource_->axisValue(0, i+offset));

			// Fill a list of all the indices that will cause division by zero.
			QList<int> badIndices;

			if (axis.at(0) == axis.at(1))
				badIndices.append(0);

			for (int i = 1, count = dataSize-1; i < count; i++)
				if (axis.at(i+1) == axis.at(i-1))
					badIndices.append(i);

			// Compute all the values
			outputValues[0] = (data.at(1)-data.at(0))/(axis.at(1)-axis.at(0));

			for (int i = 1; i < totalSize; i++)
				outputValues[i] = (data.at(i+1)-data.at(i-1))/(2*(axis.at(i+1)-axis.at(i-1)));

			// Fix all the values where division by zero would have occured.  Unfortunately, the default value is currently 0, which is generally important when taking the derivative.
			for (int i = 0, count = badIndices.size(); i < count; i++)
				outputValues[badIndices.at(i)] = 0;
		}
	}

	// If we have the very end and not the very start then we have get more data from the input source from the front.
	else if (veryEnd){

		int dataSize = totalSize+1;
		QVector<double> data = QVector<double>(dataSize);
		QVector<double> axis = QVector<double>(dataSize);
		AMAxisInfo axisInfo = inputSource_->axisInfoAt(0);

		inputSource_->values(AMnDIndex(indexStart.i()-1), indexEnd, data.data());

		// This is much faster because we can compute all the axis values ourselves rather than ask for them one at a time.
		if (axisInfo.isUniform){

			double axisStart = double(axisInfo.start);
			double axisStep = double(axisInfo.increment);

			for (int i = 0; i < dataSize; i++)
				axis[i] = axisStart + (i+offset)*axisStep;

			// Because we computed the axis values we are guarenteed that the values won't be bad.
			// This looks a little weird because data has one extra point at the beginning.
			for (int i = 0, count = totalSize-1; i < count; i++)
				outputValues[i] = (data.at(i+2)-data.at(i))/(2*(axis.at(i+2)-axis.at(i)));

			outputValues[totalSize-1] = (data.at(totalSize-1)-data.at(totalSize-2))/(axis.at(totalSize-1)-axis.at(totalSize-2));
		}

		else {

			// Fill the axis vector.  Should minimize the overhead of making the same function calls and casting the values multiple times.
			for (int i = 0; i < dataSize; i++)
				axis[i] = inputSource_->axisValue(0, i+offset);

			// Fill a list of all the indices that will cause division by zero.
			QList<int> badIndices;

			for (int i = 1, count = dataSize-1; i < count; i++)
				if (axis.at(i+1) == axis.at(i-1))
					badIndices.append(i);

			if (axis.at(dataSize-1) == axis.at(dataSize-2))
				badIndices.append(totalSize-1);

			// Compute all the values
			for (int i = 0, count = totalSize-1; i < count; i++)
				outputValues[i] = (data.at(i+2)-data.at(i))/(2*(axis.at(i+2)-axis.at(i)));

			outputValues[totalSize-1] = (data.at(totalSize)-data.at(totalSize-1))/(axis.at(totalSize)-axis.at(totalSize-1));

			// Fix all the values where division by zero would have occured.  Unfortunately, the default value is currently 0, which is generally important when taking the derivative.
			for (int i = 0, count = badIndices.size(); i < count; i++)
				outputValues[badIndices.at(i)] = 0;
		}
	}

	// If we don't have either then we need to grab an extra point from either side.
	else{

		int dataSize = totalSize+2;
		QVector<double> data = QVector<double>(totalSize);
		QVector<double> axis = QVector<double>(totalSize);
		AMAxisInfo axisInfo = inputSource_->axisInfoAt(0);

		inputSource_->values(AMnDIndex(indexStart.i()-1), AMnDIndex(indexEnd.i()+1), data.data());

		// This is much faster because we can compute all the axis values ourselves rather than ask for them one at a time.
		if (axisInfo.isUniform){

			double axisStart = double(axisInfo.start);
			double axisStep = double(axisInfo.increment);

			for (int i = 0; i < dataSize; i++)
				axis[i] = axisStart + (i+offset)*axisStep;

			// Because we computed the axis values we are guarenteed that the values won't be bad.
			// This is okay because we have an extra point on both ends and the arrays don't line up.
			for (int i = 0; i < totalSize; i++)
				outputValues[i] = (data.at(i+2)-data.at(i))/(2*(axis.at(i+2)-axis.at(i)));
		}

		else {

			// Fill the axis vector.  Should minimize the overhead of making the same function calls and casting the values multiple times.
			for (int i = 0; i < dataSize; i++)
				axis[i] = inputSource_->axisValue(0, i+offset);

			// Fill a list of all the indices that will cause division by zero.
			QList<int> badIndices;

			for (int i = 1; i < dataSize; i++)
				if (axis.at(i+1) == axis.at(i-1))
					badIndices.append(i);

			// Compute all the values
			// This is okay because we have an extra point on both ends and the arrays don't line up.
			for (int i = 0; i < totalSize; i++)
				outputValues[i] = (data.at(i+2)-data.at(i))/(2*(axis.at(i+2)-axis.at(i)));

			// Fix all the values where division by zero would have occured.  Unfortunately, the default value is currently 0, which is generally important when taking the derivative.
			for (int i = 0, count = badIndices.size(); i < count; i++)
				outputValues[badIndices.at(i)] = 0;
		}
	}

	return true;
}

AMNumber AM1DDerivativeAB::axisValue(int axisNumber, int index) const
{
	if(!isValid())
		return AMNumber(AMNumber::InvalidError);

	if(axisNumber != 0)
		return AMNumber(AMNumber::DimensionError);

	return inputSource_->axisValue(0, index);

}

bool AM1DDerivativeAB::axisValues(int axisNumber, int startIndex, int endIndex, AMNumber *outputValues) const
{
	if (!isValid())
		return false;

	if (axisNumber != 0)
		return false;

	if (startIndex >= axes_.at(axisNumber).size || endIndex >= axes_.at(axisNumber).size)
		return false;

	return inputSource_->axisValues(axisNumber, startIndex, endIndex, outputValues);
}

void AM1DDerivativeAB::onInputSourceValuesChanged(const AMnDIndex& start, const AMnDIndex& end)
{
	emitValuesChanged(start, end);
}

void AM1DDerivativeAB::onInputSourceSizeChanged()
{
	axes_[0].size = inputSource_->size(0);
	emitSizeChanged(0);
}

void AM1DDerivativeAB::onInputSourceStateChanged()
{
	// just in case the size has changed while the input source was invalid, and now it's going valid.  Do we need this? probably not, if the input source is well behaved. But it's pretty inexpensive to do it twice... and we know we'll get the size right everytime it goes valid.
	onInputSourceSizeChanged();
	reviewState();
}

void AM1DDerivativeAB::reviewState()
{
	if(!canAnalyze_ || inputSource_ == 0 || !inputSource_->isValid())
		setState(AMDataSource::InvalidFlag);

	else
		setState(0);
}

QWidget *AM1DDerivativeAB::createEditorWidget()
{
	 return new AM1DBasicDerivativeABEditor(this);
}
