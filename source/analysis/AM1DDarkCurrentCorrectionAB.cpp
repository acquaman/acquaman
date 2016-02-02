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


#include "AM1DDarkCurrentCorrectionAB.h"
#include <QDebug>

 AM1DDarkCurrentCorrectionAB::~AM1DDarkCurrentCorrectionAB(){}
AM1DDarkCurrentCorrectionAB::AM1DDarkCurrentCorrectionAB(const QString &outputName, QObject *parent)
	: AMStandardAnalysisBlock(outputName, parent)
{
	data_ = 0;
	dwellTime_ = 0;
	darkCurrent_ = 0;
	canAnalyze_ = false;
	dataName_ = "";
	dwellTimeName_ = "";
	axes_ << AMAxisInfo("invalid", 0, "No input data");
	setState(AMDataSource::InvalidFlag);
}

bool AM1DDarkCurrentCorrectionAB::areInputDataSourcesAcceptable(const QList<AMDataSource*>& dataSources) const
{
	if(dataSources.isEmpty())
	return true;	// always acceptable, the null input.

	// otherwise we need two input sources, with a rank of 1.
	if(dataSources.count() == 2 && dataSources.at(0)->rank() == 1 && dataSources.at(1)->rank() == 1)
		return true;

	// Otherwise, any number of data sources (greater than 2) that all have a rank of 1.
	if (dataSources.count() > 2){

		for (int i = 0; i < dataSources.size(); i++)
			if (dataSources.at(i)->rank() != 1)
				return false;

		return true;
	}

	return false;
}

void AM1DDarkCurrentCorrectionAB::setDataName(const QString &name)
{
	dataName_ = name;
	setModified(true);
	canAnalyze_ = canAnalyze(dataName_, dwellTimeName_);
	setInputSources();
}

void AM1DDarkCurrentCorrectionAB::setDwellTimeName(const QString &name)
{
	dwellTimeName_ = name;
	setModified(true);
	canAnalyze_ = canAnalyze(dataName_, dwellTimeName_);
	setInputSources();
}


void AM1DDarkCurrentCorrectionAB::setDarkCurrent(const double newValue)
{
	if (darkCurrent_ != newValue && newValue >= 0) {
		darkCurrent_ = newValue;
		setModified(true);
	}
}

void AM1DDarkCurrentCorrectionAB::setTimeUnitMultiplier(double newMultiplier)
{
	if (timeUnitMultiplier_ != newMultiplier && newMultiplier >= 0) {
		timeUnitMultiplier_ = newMultiplier;
		setModified(true);
	}
}

void AM1DDarkCurrentCorrectionAB::setInputSources()
{
	if (data_){

		disconnect(data_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		disconnect(data_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		disconnect(data_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
		data_ = 0;
	}

	if (dwellTime_){

		disconnect(dwellTime_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		disconnect(dwellTime_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		disconnect(dwellTime_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
		dwellTime_ = 0;
	}

	int dataIndex = indexOfInputSource(dataName_);
	int dwellTimeIndex = indexOfInputSource(dwellTimeName_);

	if (dataIndex >= 0 && dwellTimeIndex >= 0){

		data_ = inputDataSourceAt(dataIndex);
		dwellTime_ = inputDataSourceAt(dwellTimeIndex);
		canAnalyze_ = true;

		axes_[0] = data_->axisInfoAt(0);

		setDescription(QString("Dark Current Corrected %1").arg(data_->name()));

		connect(data_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		connect(data_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		connect(data_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
		connect(dwellTime_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		connect(dwellTime_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		connect(dwellTime_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
	}

	else{

		data_ = 0;
		dwellTime_ = 0;
		canAnalyze_ = false;

		axes_[0] = AMAxisInfo("invalid", 0, "No input data");
		setDescription("Dark Current Corrected 1D Data Source");
	}

	reviewState();

	emitSizeChanged(0);
	emitValuesChanged();
	emitAxisInfoChanged(0);
	emitInfoChanged();
}

bool AM1DDarkCurrentCorrectionAB::canAnalyze(const QString &dataName, const QString &dwellTimeName) const
{
	// Can always analyze two 1D data sources.
	if (sources_.count() == 2)
		return true;

	// The first data source is reserved for the data.
	if (indexOfInputSource(dataName) >= 0 && indexOfInputSource(dwellTimeName))
		return true;

	return false;
}

AMNumber AM1DDarkCurrentCorrectionAB::value(const AMnDIndex &indexes) const
{
	if (indexes.rank() != 1)
		return AMNumber(AMNumber::DimensionError);

	if(!isValid())
		return AMNumber(AMNumber::InvalidError);

#ifdef AM_ENABLE_BOUNDS_CHECKING
		if((unsigned)indexes.i() >= (unsigned)axes_.at(0).size)
			return AMNumber(AMNumber::OutOfBoundsError);
#endif

	// Can't divide by zero.
	if (double(dwellTime_->value(indexes)) == 0)
		return AMNumber(AMNumber::CalculationError);

	// The dwell time must be positive.
	if (double(dwellTime_->value(indexes)) < 0)
		return AMNumber(AMNumber::CalculationError);

	return double(data_->value(indexes)) - darkCurrent_ * double(dwellTime_->value(indexes)) * timeUnitMultiplier_;
}

bool AM1DDarkCurrentCorrectionAB::values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const
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

	int totalSize = indexStart.totalPointsTo(indexEnd);

	QVector<double> data = QVector<double>(totalSize);
	QVector<double> dwellTimes = QVector<double>(totalSize);

	data_->values(indexStart, indexEnd, data.data());
	dwellTime_->values(indexStart, indexEnd, dwellTimes.data());

	for (int i = 0; i < totalSize; i++){

		if (dwellTimes.at(i) == 0 || dwellTimes.at(i) < 0)
			return false;

		outputValues[i] = data.at(i) - darkCurrent_ * dwellTimes.at(i) * timeUnitMultiplier_;
	}

	return true;
}

AMNumber AM1DDarkCurrentCorrectionAB::axisValue(int axisNumber, int index) const
{
	if (!isValid())
		return AMNumber(AMNumber::InvalidError);

	if (axisNumber != 0)
		return AMNumber(AMNumber::DimensionError);

	if (index >= axes_.at(axisNumber).size)
		return AMNumber(AMNumber::DimensionError);

	return data_->axisValue(axisNumber, index);
}

bool AM1DDarkCurrentCorrectionAB::axisValues(int axisNumber, int startIndex, int endIndex, double *outputValues) const
{
	if (!isValid())
		return false;

	if (axisNumber != 0)
		return false;

	if (startIndex >= axes_.at(axisNumber).size || endIndex >= axes_.at(axisNumber).size)
		return false;

	return data_->axisValues(axisNumber, startIndex, endIndex, outputValues);
}

void AM1DDarkCurrentCorrectionAB::onInputSourceValuesChanged(const AMnDIndex& start, const AMnDIndex& end)
{
	emitValuesChanged(start, end);
}

void AM1DDarkCurrentCorrectionAB::onInputSourceSizeChanged()
{
	if(axes_.at(0).size != data_->size(0)){

		axes_[0].size = data_->size(0);
		emitSizeChanged(0);
	}
}

void AM1DDarkCurrentCorrectionAB::onInputSourceStateChanged() {

	// just in case the size has changed while the input source was invalid, and now it's going valid.  Do we need this? probably not, if the input source is well behaved. But it's pretty inexpensive to do it twice... and we know we'll get the size right everytime it goes valid.
	onInputSourceSizeChanged();
	reviewState();
}

void AM1DDarkCurrentCorrectionAB::setInputDataSourcesImplementation(const QList<AMDataSource *> &dataSources)
{
	if (data_){

		disconnect(data_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		disconnect(data_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		disconnect(data_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
		data_ = 0;
	}

	if (dwellTime_){

		disconnect(dwellTime_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		disconnect(dwellTime_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		disconnect(dwellTime_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
		dwellTime_ = 0;
	}

	if (dataSources.isEmpty()){

		data_ = 0;
		dwellTime_ = 0;
		sources_.clear();
		canAnalyze_ = false;

		axes_[0] = AMAxisInfo("invalid", 0, "No input data");
		setDescription("Dark Current Corrected 1D Data Source");
	}

	else if (dataSources.count() == 2){

		data_ = dataSources.at(0);
		dwellTime_ = dataSources.at(1);
		sources_ = dataSources;
		canAnalyze_ = true;

		axes_[0] = data_->axisInfoAt(0);

		setDescription(QString("Dark Current Corrected %1").arg(data_->name()));

		connect(data_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		connect(data_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		connect(data_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
		connect(dwellTime_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		connect(dwellTime_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		connect(dwellTime_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
	}

	else {

		sources_ = dataSources;
		setInputSources();
	}

	reviewState();

	emitSizeChanged(0);
	emitValuesChanged();
	emitAxisInfoChanged(0);
	emitInfoChanged();
}

void AM1DDarkCurrentCorrectionAB::reviewState(){

	if(!canAnalyze_
			|| data_ == 0 || !data_->isValid()
			|| dwellTime_ == 0 || !dwellTime_->isValid()
			|| darkCurrent_ < 0) {
		setState(AMDataSource::InvalidFlag);
		return;
	}
	else
		setState(0);
}

//#include "analysis/AM1DDarkCurrentCorrectionABEditor.h"

QWidget *AM1DDarkCurrentCorrectionAB::createEditorWidget()
{
	return 0;
}
