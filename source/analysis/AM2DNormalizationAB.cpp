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


#include "AM2DNormalizationAB.h"

AM2DNormalizationAB::~AM2DNormalizationAB(){}

AM2DNormalizationAB::AM2DNormalizationAB(const QString &outputName, QObject *parent)
	: AMStandardAnalysisBlock(outputName, parent)
{
	data_ = 0;
	normalizer_ = 0;
	canAnalyze_ = false;
	dataName_ = "";
	normalizationName_ = "";
	dataUpdateRequired_ = false;
	lowerDirtyIndex_ = AMnDIndex();
	upperDirtyIndex_ = AMnDIndex();
	cachedOutput_ = QVector<double>();
	axes_ << AMAxisInfo("invalid", 0, "No input data") << AMAxisInfo("invalid", 0, "No input data");
	setState(AMDataSource::InvalidFlag);
}

bool AM2DNormalizationAB::areInputDataSourcesAcceptable(const QList<AMDataSource*>& dataSources) const
{
	if(dataSources.isEmpty())
		return true;	// always acceptable, the null input.

	// otherwise we need two input sources, with a rank of 2.
	if(dataSources.count() == 2 && dataSources.at(0)->rank() == 2 && dataSources.at(1)->rank() == 2)
		return true;

	// Otherwise, any number of data sources (greater than 2) that all have a rank of 2.
	if (dataSources.count() > 2){

		for (int i = 0; i < dataSources.size(); i++)
			if (dataSources.at(i)->rank() != 2)
				return false;

		return true;
	}

	return false;
}

void AM2DNormalizationAB::setDataName(const QString &name)
{
	dataName_ = name;
	setModified(true);
	canAnalyze_ = canAnalyze(dataName_, normalizationName_);
	setInputSources();
}

void AM2DNormalizationAB::setNormalizationName(const QString &name)
{
	normalizationName_ = name;
	setModified(true);
	canAnalyze_ = canAnalyze(dataName_, normalizationName_);
	setInputSources();
}

void AM2DNormalizationAB::setInputSources()
{
	if (data_){

		disconnect(data_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		disconnect(data_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		disconnect(data_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
		data_ = 0;
	}

	if (normalizer_){

		disconnect(normalizer_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		disconnect(normalizer_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		disconnect(normalizer_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
		normalizer_ = 0;
	}

	int dataIndex = indexOfInputSource(dataName_);
	int normalizationIndex = indexOfInputSource(normalizationName_);

	if (dataIndex >= 0 && normalizationIndex >= 0){

		data_ = inputDataSourceAt(dataIndex);
		normalizer_ = inputDataSourceAt(normalizationIndex);
		canAnalyze_ = true;

		axes_[0] = data_->axisInfoAt(0);
		axes_[1] = data_->axisInfoAt(1);

		setDescription(QString("Normalized %1 map").arg(data_->name()));

		connect(data_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		connect(data_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		connect(data_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
		connect(normalizer_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		connect(normalizer_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		connect(normalizer_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
	}

	else{

		data_ = 0;
		normalizer_ = 0;
		canAnalyze_ = false;

		axes_[0] = AMAxisInfo("invalid", 0, "No input data");
		axes_[1] = AMAxisInfo("invalid", 0, "No input data");
		setDescription("Normalized 2D Data Source");
	}

	reviewState();

	emitSizeChanged(0);
	emitSizeChanged(1);
	emitValuesChanged();
	emitAxisInfoChanged(0);
	emitAxisInfoChanged(1);
	emitInfoChanged();

	dataUpdateRequired_ = true;
	lowerDirtyIndex_ = AMnDIndex(axes_.size(), AMnDIndex::DoInit, 0);
	upperDirtyIndex_ = size()-1;
	cachedOutput_ = QVector<double>(size().product());
}

bool AM2DNormalizationAB::canAnalyze(const QString &dataName, const QString &normalizationName) const
{
	// Can always analyze two 2D data sources.
	if (sources_.count() == 2)
		return true;

	// The first data source is reserved for the data.
	if (indexOfInputSource(dataName) >= 0 && indexOfInputSource(normalizationName))
		return true;

	return false;
}

AMNumber AM2DNormalizationAB::value(const AMnDIndex &indexes) const
{
	if (indexes.rank() != 2)
		return AMNumber(AMNumber::DimensionError);

	if(!isValid())
		return AMNumber(AMNumber::InvalidError);

#ifdef AM_ENABLE_BOUNDS_CHECKING
	if((unsigned)indexes.i() >= (unsigned)axes_.at(0).size
			&& (unsigned)indexes.j() >= (unsigned)axes_.at(1).size)
		return AMNumber(AMNumber::OutOfBoundsError);
#endif

	if (dataUpdateRequired_)
		computeCachedValues();

	return cachedOutput_.at(indexes.i()+size(0)*indexes.j());
}

bool AM2DNormalizationAB::values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const
{
	if (indexStart.rank() != 2 || indexEnd.rank() != 2)
		return false;

	if(!isValid())
		return false;

#ifdef AM_ENABLE_BOUNDS_CHECKING
	if((unsigned)indexEnd.i() >= (unsigned)axes_.at(0).size || (unsigned)indexStart.i() > (unsigned)indexEnd.i()
			|| (unsigned)indexEnd.j() >= (unsigned)axes_.at(1).size || (unsigned)indexStart.j() > (unsigned)indexEnd.j())
		return false;
#endif

	if (dataUpdateRequired_)
		computeCachedValues();

	int totalSize = indexStart.totalPointsTo(indexEnd);

	if (totalSize == size().product())
		memcpy(outputValues, cachedOutput_.constData(), totalSize*sizeof(double));

	else {

		int rowLength = indexStart.distanceTo(0, indexEnd);
		int xOffset = indexStart.i();
		int yOffset = indexStart.j();
		int xSize = size(0);
		const double *cachedOutputDoublePointer = cachedOutput_.constData();

		for (int j = 0, jSize = indexStart.distanceTo(1, indexEnd); j < jSize; j++)
			memcpy(outputValues+rowLength*j, cachedOutputDoublePointer+(xOffset+xSize*(j+yOffset)), rowLength*sizeof(double));
	}

	return true;
}

void AM2DNormalizationAB::computeCachedValues() const
{
	int totalSize = lowerDirtyIndex_.totalPointsTo(upperDirtyIndex_);
	QVector<double> data = QVector<double>(totalSize);
	QVector<double> normalizer = QVector<double>(totalSize);

	data_->values(lowerDirtyIndex_, upperDirtyIndex_, data.data());
	normalizer_->values(lowerDirtyIndex_, upperDirtyIndex_, normalizer.data());

	int xOffset = lowerDirtyIndex_.i();
	int yOffset = lowerDirtyIndex_.j();
	int xSize = size(0);

	for (int j = 0, jSize = lowerDirtyIndex_.distanceTo(1, upperDirtyIndex_); j < jSize; j++){

		for (int i = 0, iSize = lowerDirtyIndex_.distanceTo(0, upperDirtyIndex_); i < iSize; i++){

			int index = i+xOffset + (j+yOffset)*xSize;
			int dataIndex = i + j*iSize;

			if (normalizer.at(dataIndex) == 0)
				cachedOutput_[index] = 0;

			else if (normalizer.at(dataIndex) < 0 || data.at(dataIndex) == -1)
				cachedOutput_[index] = -1;

			else
				cachedOutput_[index] = data.at(dataIndex)/normalizer.at(dataIndex);
		}
	}

	dataUpdateRequired_ = false;
	lowerDirtyIndex_ = AMnDIndex();
	upperDirtyIndex_ = AMnDIndex();
}

AMNumber AM2DNormalizationAB::axisValue(int axisNumber, int index) const
{
	if (!isValid())
		return AMNumber(AMNumber::InvalidError);

	if (axisNumber != 0 && axisNumber != 1)
		return AMNumber(AMNumber::DimensionError);

	if (index >= axes_.at(axisNumber).size)
		return AMNumber(AMNumber::DimensionError);

	return data_->axisValue(axisNumber, index);
}

bool AM2DNormalizationAB::axisValues(int axisNumber, int startIndex, int endIndex, AMNumber *outputValues) const
{
	if (!isValid())
		return false;

	if (axisNumber != 0 && axisNumber != 1)
		return false;

	if (startIndex >= axes_.at(axisNumber).size || endIndex >= axes_.at(axisNumber).size)
		return false;

	return data_->axisValues(axisNumber, startIndex, endIndex, outputValues);
}

void AM2DNormalizationAB::onInputSourceValuesChanged(const AMnDIndex& start, const AMnDIndex& end)
{
	if (!dataUpdateRequired_)
		dataUpdateRequired_ = true;

	lowerDirtyIndex_ = lowerDirtyIndex_.globalMinimum(start);
	upperDirtyIndex_ = upperDirtyIndex_.globalMaximum(end);

	emitValuesChanged(start, end);
}

void AM2DNormalizationAB::onInputSourceSizeChanged()
{
	if(axes_.at(0).size != data_->size(0)){

		axes_[0].size = data_->size(0);
		emitSizeChanged(0);
	}

	if(axes_.at(1).size != data_->size(1)){

		axes_[1].size = data_->size(1);
		emitSizeChanged(1);
	}

	dataUpdateRequired_ = true;
	lowerDirtyIndex_ = AMnDIndex(axes_.size(), AMnDIndex::DoInit, 0);
	upperDirtyIndex_ = size()-1;
	cachedOutput_ = QVector<double>(size().product());
}

void AM2DNormalizationAB::onInputSourceStateChanged() {

	// just in case the size has changed while the input source was invalid, and now it's going valid.  Do we need this? probably not, if the input source is well behaved. But it's pretty inexpensive to do it twice... and we know we'll get the size right everytime it goes valid.
	onInputSourceSizeChanged();
	reviewState();
}

void AM2DNormalizationAB::setInputDataSourcesImplementation(const QList<AMDataSource *> &dataSources)
{
	if (data_){

		disconnect(data_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		disconnect(data_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		disconnect(data_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
		data_ = 0;
	}

	if (normalizer_){

		disconnect(normalizer_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		disconnect(normalizer_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		disconnect(normalizer_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
		normalizer_ = 0;
	}

	if (dataSources.isEmpty()){

		data_ = 0;
		normalizer_ = 0;
		sources_.clear();
		canAnalyze_ = false;

		axes_[0] = AMAxisInfo("invalid", 0, "No input data");
		axes_[1] = AMAxisInfo("invalid", 0, "No input data");
		setDescription("Normalized 2D Data Source");

		dataUpdateRequired_ = false;
		lowerDirtyIndex_ = AMnDIndex();
		upperDirtyIndex_ = AMnDIndex();
		cachedOutput_ = QVector<double>();
	}

	else if (dataSources.count() == 2){

		data_ = dataSources.at(0);
		normalizer_ = dataSources.at(1);
		sources_ = dataSources;
		canAnalyze_ = true;

		axes_[0] = data_->axisInfoAt(0);
		axes_[1] = data_->axisInfoAt(1);

		setDescription(QString("Normalized %1 map").arg(data_->name()));

		connect(data_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		connect(data_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		connect(data_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
		connect(normalizer_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		connect(normalizer_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		connect(normalizer_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));

		dataUpdateRequired_ = true;
		lowerDirtyIndex_ = AMnDIndex(axes_.size(), AMnDIndex::DoInit, 0);
		upperDirtyIndex_ = size()-1;
		cachedOutput_ = QVector<double>(size().product());
	}

	else {

		sources_ = dataSources;
		setInputSources();
	}

	reviewState();

	emitSizeChanged(0);
	emitSizeChanged(1);
	emitValuesChanged();
	emitAxisInfoChanged(0);
	emitAxisInfoChanged(1);
	emitInfoChanged();
}

void AM2DNormalizationAB::reviewState(){

	if(!canAnalyze_
			|| data_ == 0 || !data_->isValid()
			|| normalizer_ == 0 || !normalizer_->isValid()) {
		setState(AMDataSource::InvalidFlag);
		return;
	}
	else
		setState(0);
}

#include "analysis/AM2DNormalizationABEditor.h"

QWidget *AM2DNormalizationAB::createEditorWidget()
{
	return new AM2DNormalizationABEditor(this);
}
