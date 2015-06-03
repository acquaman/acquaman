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


#include "AM3DAdditionAB.h"

 AM3DAdditionAB::~AM3DAdditionAB(){}
AM3DAdditionAB::AM3DAdditionAB(const QString &outputName, QObject *parent)
	: AMStandardAnalysisBlock(outputName, parent)
{
	axes_ << AMAxisInfo("invalid", 0, "No input data") << AMAxisInfo("invalid", 0, "No input data") << AMAxisInfo("invalid", 0, "No input data");
	setState(AMDataSource::InvalidFlag);
}

// Check if a set of inputs is valid. The empty list (no inputs) must always be valid. For non-empty lists, our specific requirements are...
/*
- the rank() of the input sources must be 3
*/
bool AM3DAdditionAB::areInputDataSourcesAcceptable(const QList<AMDataSource*>& dataSources) const
{
	if(dataSources.isEmpty())
		return true; // always acceptable, the null input.

	// otherwise input sources have rank of 2.
	for (int i = 0; i < dataSources.count(); i++)
		if (dataSources.at(i)->rank() != 3)
			return false;

	return true;
}

AMNumber AM3DAdditionAB::value(const AMnDIndex &indexes) const
{
	if(indexes.rank() != 3)
		return AMNumber(AMNumber::DimensionError);

	if(!isValid())
		return AMNumber(AMNumber::InvalidError);

#ifdef AM_ENABLE_BOUNDS_CHECKING
	for (int i = 0; i < sources_.size(); i++)
		if (indexes.i() >= sources_.at(i)->size(0) || indexes.j() >= sources_.at(i)->size(1) || indexes.k() >= sources_.at(i)->size(2))
			return AMNumber(AMNumber::OutOfBoundsError);
#endif

	double val = 0;

	for (int i = 0; i < sources_.size(); i++)
		val += (double)sources_.at(i)->value(indexes);

	return val;
}

bool AM3DAdditionAB::values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const
{
	if(indexStart.rank() != 3 || indexEnd.rank() != 3)
		return false;

	if(!isValid())
		return false;

#ifdef AM_ENABLE_BOUNDS_CHECKING
	for (int i = 0; i < sources_.size(); i++)
		if ((unsigned)indexEnd.i() >= (unsigned)axes_.at(0).size || (unsigned)indexEnd.j() >= (unsigned)axes_.at(1).size || (unsigned)indexEnd.k() >= (unsigned)axes_.at(2).size)
			return false;

	if ((unsigned)indexStart.i() > (unsigned)indexEnd.i() || (unsigned)indexStart.j() > (unsigned)indexEnd.j())
		return false;
#endif

	int totalSize = indexStart.totalPointsTo(indexEnd);

	QVector<double> data = QVector<double>(totalSize);
	sources_.at(0)->values(indexStart, indexEnd, data.data());

	// Do the first data source separately to initialize the values.
	for (int i = 0; i < totalSize; i++)
		outputValues[i] = data.at(i);

	// Iterate through the rest of the sources.
	for (int i = 1, count = sources_.size(); i < count; i++){

		sources_.at(i)->values(indexStart, indexEnd, data.data());

		for (int j = 0; j < totalSize; j++)
			outputValues[j] += data.at(j);
	}

	return true;
}

AMNumber AM3DAdditionAB::axisValue(int axisNumber, int index) const
{
	if (!isValid())
		return AMNumber(AMNumber::InvalidError);

	if (axisNumber != 0 && axisNumber != 1 && axisNumber != 2)
		return AMNumber(AMNumber::DimensionError);

	if (index >= axes_.at(axisNumber).size)
		return AMNumber(AMNumber::DimensionError);

	return sources_.at(0)->axisValue(axisNumber, index);
}

bool AM3DAdditionAB::axisValues(int axisNumber, int startIndex, int endIndex, AMNumber *outputValues) const
{
	if (!isValid())
		return false;

	if (axisNumber != 0 && axisNumber != 1 && axisNumber != 2)
		return false;

	if (startIndex >= axes_.at(axisNumber).size || endIndex >= axes_.at(axisNumber).size)
		return false;

	return sources_.at(0)->axisValues(axisNumber, startIndex, endIndex, outputValues);
}

// Connected to be called when the values of the input data source change
void AM3DAdditionAB::onInputSourceValuesChanged(const AMnDIndex& start, const AMnDIndex& end)
{
	emitValuesChanged(start, end);
}

// Connected to be called when the size of the input source changes
void AM3DAdditionAB::onInputSourceSizeChanged()
{
	if(axes_.at(0).size != sources_.at(0)->size(0)){

		axes_[0].size = sources_.at(0)->size(0);
		emitSizeChanged(0);
	}

	if(axes_.at(1).size != sources_.at(0)->size(1)){

		axes_[1].size = sources_.at(0)->size(1);
		emitSizeChanged(1);
	}

	if(axes_.at(2).size != sources_.at(0)->size(2)){

		axes_[2].size = sources_.at(0)->size(2);
		emitSizeChanged(2);
	}
}

// Connected to be called when the state() flags of any input source change
void AM3DAdditionAB::onInputSourceStateChanged()
{
	// just in case the size has changed while the input source was invalid, and now it's going valid. Do we need this? probably not, if the input source is well behaved. But it's pretty inexpensive to do it twice... and we know we'll get the size right everytime it goes valid.
	onInputSourceSizeChanged();
	reviewState();
}

// Set the data source inputs.
void AM3DAdditionAB::setInputDataSourcesImplementation(const QList<AMDataSource*>& dataSources)
{
	// disconnect connections from old sources, if they exist.
	if(!sources_.isEmpty()) {

		for (int i = 0; i < sources_.size(); i++){

			disconnect(sources_.at(i)->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
			disconnect(sources_.at(i)->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
			disconnect(sources_.at(i)->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
		}
	}

	if(dataSources.isEmpty()) {

		sources_.clear();
		axes_[0] = AMAxisInfo("invalid", 0, "No input data");
		axes_[1] = AMAxisInfo("invalid", 0, "No input data");
		axes_[2] = AMAxisInfo("invalid", 0, "No input data");

		setDescription("-- No input data --");
	}

	// we know that this will only be called with valid input source
	else {

		sources_ = dataSources;

		axes_[0] = sources_.at(0)->axisInfoAt(0);
		axes_[1] = sources_.at(0)->axisInfoAt(1);
		axes_[2] = sources_.at(0)->axisInfoAt(2);

		setDescription(QString("Sum of spectra from %1 maps").arg(sources_.size()));

		for (int i = 0; i < sources_.size(); i++){

			connect(sources_.at(i)->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
			connect(sources_.at(i)->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
			connect(sources_.at(i)->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
		}
	}

	reviewState();

	emitSizeChanged(0);
	emitSizeChanged(1);
	emitSizeChanged(2);
	emitValuesChanged();
	emitAxisInfoChanged(0);
	emitAxisInfoChanged(1);
	emitAxisInfoChanged(2);
	emitInfoChanged();
}

void AM3DAdditionAB::reviewState()
{
	// Are there data sources?
	if(sources_.isEmpty()){

		setState(AMDataSource::InvalidFlag);
		return;
	}

	// Are all the data sources the same size?
	for (int i = 1; i < sources_.count(); i++){

		if (axes_.at(0).size != sources_.at(i)->axisInfoAt(0).size || axes_.at(1).size != sources_.at(i)->axisInfoAt(1).size || axes_.at(2).size != sources_.at(i)->axisInfoAt(2).size){

			setState(AMDataSource::InvalidFlag);
			return;
		}
	}

	// Validity check on all data sources.
	bool valid = true;

	for (int i = 0; i < sources_.size(); i++)
		valid = valid && sources_.at(i)->isValid();

	if (valid)
		setState(0);
	else
		setState(AMDataSource::InvalidFlag);
}
