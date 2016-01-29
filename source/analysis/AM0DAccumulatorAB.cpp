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


#include "AM0DAccumulatorAB.h"

AM0DAccumulatorAB::AM0DAccumulatorAB(const QString &outputName, QObject *parent) : AMStandardAnalysisBlock(outputName, parent)
{
	axes_ << AMAxisInfo("invalid", 0, "No input data");
	setState(AMDataSource::InvalidFlag);

	dataStored_ = QList<double>();
	dataMax_ = 50;
}

AM0DAccumulatorAB::~AM0DAccumulatorAB()
{

}

QList<double> AM0DAccumulatorAB::dataStored() const
{
	return dataStored_;
}

int AM0DAccumulatorAB::dataStoredCount() const
{
	return dataStored_.size();
}

int AM0DAccumulatorAB::dataStoredCountMax() const
{
	return dataMax_;
}

bool AM0DAccumulatorAB::areInputDataSourcesAcceptable(const QList<AMDataSource *> &dataSources) const
{
	// null input is acceptable.
	if (dataSources.isEmpty())
		return true;

	// we expect to receive maximum one input data source.
	if (dataSources.size() > 1)
		return false;

	// the data source must have rank zero.
	if (dataSources.at(0)->rank() != 0)
		return false;

	return true;
}

AMNumber AM0DAccumulatorAB::value(const AMnDIndex &indexes) const
{
	if (indexes.rank() != 1)
		return AMNumber(AMNumber::DimensionError);

	if (!isValid())
		return AMNumber(AMNumber::InvalidError);

	int index = indexes.i();

	if (index < 0 || index >= dataStored_.size())
		return AMNumber(AMNumber::OutOfBoundsError);

	return dataStored_.at(index);
}

bool AM0DAccumulatorAB::values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const
{
	if (!isValid())
		return false;

	if (indexStart.rank() != 1 || indexEnd.rank() != 1)
		return false;

	if (indexStart.i() < 0 || indexStart.i() >= dataStored_.size())
		return false;

	if (indexEnd.i() < 0 || indexEnd.i() >= dataStored_.size())
		return false;

	if (indexStart.i() > indexEnd.i())
		return false;

	int totalPoints = indexStart.totalPointsTo(indexEnd);

	for (int i = 0; i < totalPoints; i++) {
		outputValues[i] = dataStored_.at(indexStart.i() + i);
	}

	return true;
}

AMNumber AM0DAccumulatorAB::axisValue(int axisNumber, int index) const
{
	if (!isValid())
		return AMNumber(AMNumber::InvalidError);

	if (axisNumber != 0)
		return AMNumber(AMNumber::DimensionError);

	if (index < 0 || index >= dataStored_.size())
		return AMNumber(AMNumber::OutOfBoundsError);

	return index;
}

bool AM0DAccumulatorAB::axisValues(int axisNumber, int startIndex, int endIndex, double *outputValues) const
{
	if (!isValid())
		return false;

	if (axisNumber != 0)
		return false;

	if (startIndex < 0 || startIndex >= dataStored_.size())
		return false;

	if (endIndex < 0 || endIndex >= dataStored_.size())
		return false;

	for (int i = 0, size = endIndex-startIndex+1; i < size; i++)
        outputValues[i] = double(i+startIndex);

	return true;
}

void AM0DAccumulatorAB::setDataStoredCountMax(int newMax)
{
	if (dataMax_ != newMax && newMax > 0) {

		if (newMax < dataStored_.size()) {
			dataStored_ = dataStored_.mid(dataStored_.size() - 1 - newMax, newMax);
		}

		dataMax_ = newMax;
		emit dataStoredMaxChanged(dataMax_);
	}
}

void AM0DAccumulatorAB::onInputSourceValuesChanged(const AMnDIndex &start, const AMnDIndex &end)
{
	Q_UNUSED(start)
	Q_UNUSED(end)

	// update the values stored for this source. We assume the source is 0D, so the latest value is always recovered with an empty AMnDIndex.
	double newValue = sources_.at(0)->value(AMnDIndex());

	// if we are greater than the data stored max, remove the first value from the list.
	if (dataStored_.size() > dataMax_) {
		dataStored_.removeFirst();
	}

	dataStored_.append(newValue);

	// update the axes info for this data source to reflect the (possibly) new number of points to plot.
	axes_[0] = AMAxisInfo(sources_.at(0)->name(), dataStored_.size());

	emitSizeChanged(0);
	emitValuesChanged(AMnDIndex(0), AMnDIndex(dataStored_.size() - 1));
	emitAxisInfoChanged(0);
}

void AM0DAccumulatorAB::onInputSourceStateChanged()
{
	reviewState();
}

void AM0DAccumulatorAB::setInputDataSourcesImplementation(const QList<AMDataSource *> &dataSources)
{
	// disconnect connections from old sources, if they exist.
	if (!sources_.isEmpty()) {
		disconnect( sources_.at(0)->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)) );
		disconnect( sources_.at(0)->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourcesStateChanged()) );
	}

	// set to invalid if new dataSources is empty.
	if (dataSources.isEmpty()) {
		sources_.clear();
		axes_[0] = AMAxisInfo("invalid", 0, "No input data");
		setDescription("-- No input data --");

	// if data source is valid, set sources_, axis info, description, and connections.
	} else {

		sources_ = dataSources;
		axes_[0] = sources_.at(0)->axisInfoAt(0);
		setDescription(QString("Value updates for %1").arg(sources_.at(0)->name()));

		connect( sources_.at(0)->signalSource(), SIGNAL(valuesChanged(AMnDIndex, AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex, AMnDIndex)) );
		connect( sources_.at(0)->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()) );
	}

	reviewState();

	emitSizeChanged(0);
	emitValuesChanged();
	emitAxisInfoChanged(0);
	emitInfoChanged();
}

void AM0DAccumulatorAB::reviewState()
{
	// are there data sources?
	if (sources_.isEmpty()) {
		setState(AMDataSource::InvalidFlag);
		return;
	}

	// is there one data source?
	if (sources_.size() != 1) {
		setState(AMDataSource::InvalidFlag);
		return;
	}

	// does the data source have rank 0?
	if (sources_.at(0)->rank() != 0) {
		setState(AMDataSource::InvalidFlag);
		return;
	}

	// check that data source is valid.
	if (!sources_.at(0)->isValid()) {
		setState(AMDataSource::InvalidFlag);
		return;
	}

	setState(0);
}
