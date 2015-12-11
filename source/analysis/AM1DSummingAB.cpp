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


#include "AM1DSummingAB.h"

#include "util/AMUtility.h"

 AM1DSummingAB::~AM1DSummingAB(){}
AM1DSummingAB::AM1DSummingAB(const QString &outputName, QObject *parent)
	: AMStandardAnalysisBlock(outputName, parent)
{
	cacheUpdateRequired_ = false;
	cachedDataRange_ = AMRange();

	axes_ << AMAxisInfo("invalid", 0, "No input data");
	setState(AMDataSource::InvalidFlag);
}

// Check if a set of inputs is valid. The empty list (no inputs) must always be valid. For non-empty lists, our specific requirements are...
/* - there must be a single input source
- the rank() of that input source must be 1 (one-dimensional)
*/
bool AM1DSummingAB::areInputDataSourcesAcceptable(const QList<AMDataSource*>& dataSources) const
{
	if(dataSources.isEmpty())
		return true; // always acceptable, the null input.

	// otherwise input sources have rank of 1.
	for (int i = 0; i < dataSources.count(); i++)
		if (dataSources.at(i)->rank() != 1)
			return false;

	return true;
}

AMNumber AM1DSummingAB::value(const AMnDIndex &indexes) const
{
	if(indexes.rank() != 1)
		return AMNumber(AMNumber::DimensionError);

	if(!isValid())
		return AMNumber(AMNumber::InvalidError);

#ifdef AM_ENABLE_BOUNDS_CHECKING
	for (int i = 0; i < sources_.size(); i++)
		if (indexes.i() >= sources_.at(i)->size(0))
			return AMNumber(AMNumber::OutOfBoundsError);
#endif

    if (cacheUpdateRequired_)
        computeCachedValues();

    return cachedData_.at(indexes.i());
}

bool AM1DSummingAB::values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const
{
	if(indexStart.rank() != 1 || indexEnd.rank() != 1)
		return false;

	if(!isValid())
		return false;

#ifdef AM_ENABLE_BOUNDS_CHECKING
	for (int i = 0; i < sources_.size(); i++)
		if ((unsigned)indexEnd.i() >= (unsigned)axes_.at(0).size)
			return false;

	if ((unsigned)indexStart.i() > (unsigned)indexEnd.i())
		return false;
#endif

    if (cacheUpdateRequired_)
        computeCachedValues();

    int totalSize = indexStart.totalPointsTo(indexEnd);
    memcpy(outputValues, cachedData_.constData()+indexStart.i(), totalSize*sizeof(double));

	return true;
}

AMNumber AM1DSummingAB::axisValue(int axisNumber, int index) const
{
	if(!isValid())
		return AMNumber(AMNumber::InvalidError);

	if(axisNumber != 0)
		return AMNumber(AMNumber::DimensionError);

#ifdef AM_ENABLE_BOUNDS_CHECKING
	if (index >= sources_.first()->size(0))
		return AMNumber(AMNumber::OutOfBoundsError);
#endif

	return sources_.first()->axisValue(0, index);
}

bool AM1DSummingAB::axisValues(int axisNumber, int startIndex, int endIndex, double *outputValues) const
{
	if (!isValid())
		return false;

	if (axisNumber != 0)
		return false;

	if (startIndex >= axes_.at(axisNumber).size || endIndex >= axes_.at(axisNumber).size)
		return false;

	return sources_.first()->axisValues(axisNumber, startIndex, endIndex, outputValues);
}

// Connected to be called when the values of the input data source change
void AM1DSummingAB::onInputSourceValuesChanged(const AMnDIndex& start, const AMnDIndex& end)
{
    cacheUpdateRequired_ = true;
	emitValuesChanged(start, end);
}

// Connected to be called when the size of the input source changes
void AM1DSummingAB::onInputSourceSizeChanged()
{
    axes_[0] = sources_.at(0)->axisInfoAt(0);
    cacheUpdateRequired_ = true;
    cachedData_ = QVector<double>(size(0));
	emitSizeChanged();
}

// Connected to be called when the state() flags of any input source change
void AM1DSummingAB::onInputSourceStateChanged()
{
	// just in case the size has changed while the input source was invalid, and now it's going valid. Do we need this? probably not, if the input source is well behaved. But it's pretty inexpensive to do it twice... and we know we'll get the size right everytime it goes valid.
	onInputSourceSizeChanged();
	reviewState();
}

// Set the data source inputs.
void AM1DSummingAB::setInputDataSourcesImplementation(const QList<AMDataSource*>& dataSources)
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
		setDescription("-- No input data --");
	}

	// we know that this will only be called with valid input source
	else {

		sources_ = dataSources;

		axes_[0] = sources_.at(0)->axisInfoAt(0);

        cacheUpdateRequired_ = true;
        cachedData_ = QVector<double>(size(0));
		setDescription(QString("Sum of %1 spectra").arg(sources_.size()));

		for (int i = 0; i < sources_.size(); i++){

			connect(sources_.at(i)->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
			connect(sources_.at(i)->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
			connect(sources_.at(i)->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
		}
	}

	reviewState();

    emitSizeChanged();
	emitValuesChanged();
    emitAxisInfoChanged();
	emitInfoChanged();
}

void AM1DSummingAB::reviewState()
{
	// Are there data sources?
	if(sources_.isEmpty()){

		setState(AMDataSource::InvalidFlag);
		return;
	}

	// Are all the data sources the same size?


	for (int i = 1; i < axes_.count(); i++){

		if (axes_[0].size != sources_.at(i)->axisInfoAt(0).size){

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

void AM1DSummingAB::computeCachedValues() const
{
    AMnDIndex start = AMnDIndex(0);
    AMnDIndex end = size()-1;
    int totalSize = start.totalPointsTo(end);

    QVector<double> data = QVector<double>(totalSize);
    sources_.at(0)->values(start, end, data.data());

    // Do the first data source separately to initialize the values.
    cachedData_ = data;

    // Iterate through the rest of the sources.
    for (int i = 1, count = sources_.size(); i < count; i++){

        sources_.at(i)->values(start, end, data.data());

        for (int j = 0; j < totalSize; j++)
            cachedData_[j] += data.at(j);
    }

    cachedDataRange_ = AMUtility::rangeFinder(cachedData_);
    cacheUpdateRequired_ = false;
}
