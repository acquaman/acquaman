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


#include "AMOrderReductionAB.h"

#include "util/AMUtility.h"

 AMOrderReductionAB::~AMOrderReductionAB(){}
AMOrderReductionAB::AMOrderReductionAB(const QString &outputName, QObject *parent)
	: AMStandardAnalysisBlock(outputName, parent)
{
	source_ = 0;
	selectedName_ = "";
	reducedAxis_ = -1;
	cacheUpdateRequired_ = false;
	cachedDataRange_ = AMRange();

	axes_.clear();
	setState(AMDataSource::InvalidFlag);
}

bool AMOrderReductionAB::areInputDataSourcesAcceptable(const QList<AMDataSource *> &dataSources) const
{
	if (dataSources.isEmpty())
		return true;	// always acceptable, the null input.

	// otherwise we need one input source that is NOT rank 0.
	if (dataSources.count() == 1 && dataSources.at(0)->rank() != 0)
		return true;

	// Otherwise, any number of data sources where at least one does not have rank 0.
	if (dataSources.count() > 1){

		for (int i = 0, size = dataSources.size(); i < size; i++)
			if (dataSources.at(i)->rank() != 0)
				return true;
	}

	return false;
}

void AMOrderReductionAB::setSelectedName(const QString &name)
{
	selectedName_ = name;
	setModified(true);
	setInputSources();
}

void AMOrderReductionAB::setReducedAxis(int axis)
{
	reducedAxis_ = axis;
	setModified(true);
	updateAxes();
}

void AMOrderReductionAB::setInputSources()
{
	if (source_){

		disconnect(source_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		disconnect(source_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		disconnect(source_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
		source_ = 0;
	}

	int selectedIndex = indexOfInputSource(selectedName_);

	if (selectedIndex >= 0){

		source_ = inputDataSourceAt(selectedIndex);
		axes_.clear();

		for (int i = 0; i < source_->rank()-1; i++)
			axes_.append(AMAxisInfo("invalid", 0, "No input data"));

		updateAxes();

		cacheUpdateRequired_ = true;
		cachedData_ = QVector<double>(size().product());

		setDescription(QString("Reduced Order of %1").arg(source_->name()));
		connect(source_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		connect(source_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		connect(source_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
	}

	else{

		source_ = 0;
		setReducedAxis(-1);
	}

	reviewState();

	emitSizeChanged();
	emitValuesChanged();
	emitAxisInfoChanged();
	emitInfoChanged();
}

void AMOrderReductionAB::updateAxes()
{
	if (source_){

		if (reducedAxis_ == -1)
			for (int i = 0; i < source_->rank()-1; i++)
				axes_[i] = AMAxisInfo("invalid", 0, "No input data");

		else if (reducedAxis_ <= source_->rank()){

			int axisIndex = 0;

			for (int i = 0; i < source_->rank(); i++){
				if (i != reducedAxis_)
					axes_[axisIndex++] = source_->axisInfoAt(i);
			}
		}

		cacheUpdateRequired_ = true;
		cachedData_ = QVector<double>(size().product());

		emitSizeChanged();
		emitValuesChanged();
		emitAxisInfoChanged();
		emitInfoChanged();
	}
}

AMnDIndex AMOrderReductionAB::inputIndex(const AMnDIndex &input, int reducedIndex) const
{
	AMnDIndex newIndex = AMnDIndex(input.rank()+1, AMnDIndex::DoInit);

	for (int i = 0, size = newIndex.rank(); i < size; i++){

		if (i < reducedAxis_)
			newIndex[i] = input.at(i);

		else if (i == reducedAxis_)
			newIndex[i] = reducedIndex;

		else
			newIndex[i] = input.at(i-1);
	}

	return newIndex;
}

int AMOrderReductionAB::inputAxisIndex(int axisId) const
{
	int newAxis = axisId;

	// We skip over the reduced axis, so that's why we add one to the input axis.
	if (newAxis >= reducedAxis_)
		newAxis++;

	return newAxis;
}

AMnDIndex AMOrderReductionAB::outputIndex(const AMnDIndex &input) const
{
	AMnDIndex output = AMnDIndex(rank(), AMnDIndex::DoInit);

	for (int i = 0, index = 0, size = input.rank(); i < size; i++)
		if (i != reducedAxis_)
			output[index++] = input.at(i);

	return output;
}

void AMOrderReductionAB::computeCachedValues() const
{
	AMnDIndex start = AMnDIndex(source_->rank(), AMnDIndex::DoInit);
	AMnDIndex end = source_->size()-1;
	int totalPoints = start.totalPointsTo(end);
	int sumRange = source_->size(reducedAxis_);
	QVector<double> data = QVector<double>(totalPoints);
	source_->values(start, end, data.data());
	cachedData_.fill(-1);

	for (int i = 0; i < totalPoints; i++){

		int insertIndex = int(i/sumRange);

		if (data.at(i) == -1)
			cachedData_[insertIndex] = -1;

		else {
			if ((i%sumRange) == 0)
				cachedData_[insertIndex] = 0;

			cachedData_[insertIndex] += data.at(i);
		}
	}

	cachedDataRange_ = AMUtility::rangeFinder(cachedData_, -1);
	cacheUpdateRequired_ = false;
}

AMNumber AMOrderReductionAB::value(const AMnDIndex &indexes) const
{
	if (indexes.rank() == 0)
		return AMNumber(AMNumber::DimensionError);

	if (!isValid())
		return AMNumber(AMNumber::InvalidError);

#ifdef AM_ENABLE_BOUNDS_CHECKING
	for (int i = 0, size = indexes.rank(); i < size; i++)
		if((unsigned)indexes.at(i) >= (unsigned)axes_.at(i).size)
			return AMNumber(AMNumber::OutOfBoundsError);
#endif

	if (cacheUpdateRequired_)
		computeCachedValues();

	int flatIndex = 0;

	switch (rank()){

	case 0:
		flatIndex = 0;
		break;

	case 1:
		flatIndex = indexes.i();
		break;

	case 2:
		flatIndex = indexes.i()+size(1)+indexes.j();
		break;

	case 3:
		flatIndex = indexes.i()+size(1)*size(2)+indexes.j()*size(2)+indexes.k();
		break;
	}

	return cachedData_.at(flatIndex);
}

bool AMOrderReductionAB::values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const
{
	if (indexStart.rank() == 0 || indexEnd.rank() == 0 || indexStart.rank() != axes_.size())
		return false;

	if (!isValid())
		return false;

#ifdef AM_ENABLE_BOUNDS_CHECKING
	for (int i = 0, size = indexStart.rank(); i < size; i++)
		if((unsigned)indexStart.at(i) >= (unsigned)axes_.at(i).size || (unsigned)indexStart.at(i) > (unsigned)indexEnd.at(i))
			return false;
#endif

	if (cacheUpdateRequired_)
		computeCachedValues();

	int totalSize = indexStart.totalPointsTo(indexEnd);
	memcpy(outputValues, cachedData_.constData()+indexStart.flatIndexInArrayOfSize(size()), totalSize*sizeof(double));

	return true;
}

AMNumber AMOrderReductionAB::axisValue(int axisNumber, int index) const
{
	if (!isValid())
		return AMNumber(AMNumber::InvalidError);

	if (axisNumber >= rank())
		return AMNumber(AMNumber::DimensionError);

	int actualAxis = inputAxisIndex(axisNumber);

	if (index >= axes_.at(actualAxis).size)
		return AMNumber(AMNumber::DimensionError);

	return source_->axisValue(actualAxis, index);
}

bool AMOrderReductionAB::axisValues(int axisNumber, int startIndex, int endIndex, double *outputValues) const
{
	if (!isValid())
		return false;

	if (axisNumber >= rank())
		return false;

	int actualAxis = inputAxisIndex(axisNumber);

	if (startIndex >= source_->size(actualAxis) || endIndex >= source_->size(actualAxis))
		return false;

	return source_->axisValues(actualAxis, startIndex, endIndex, outputValues);
}

void AMOrderReductionAB::onInputSourceValuesChanged(const AMnDIndex &start, const AMnDIndex &end)
{
	cacheUpdateRequired_ = true;

	if (start.rank() == axes_.size())
		emitValuesChanged(start, end);

	else
		emitValuesChanged(outputIndex(start), outputIndex(end));
}

void AMOrderReductionAB::onInputSourceSizeChanged()
{
	updateAxes();
}

void AMOrderReductionAB::onInputSourceStateChanged()
{
	onInputSourceSizeChanged();
	reviewState();
}

void AMOrderReductionAB::setInputDataSourcesImplementation(const QList<AMDataSource *> &dataSources)
{
	if (source_){

		disconnect(source_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		disconnect(source_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		disconnect(source_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
		source_ = 0;
	}

	if (dataSources.isEmpty()){

		source_ = 0;
		sources_.clear();

		setReducedAxis(-1);
		axes_.clear();
		setDescription("Order Reduction Data Source");
	}

	else{

		sources_ = dataSources;
		setInputSources();
	}

	reviewState();

	emitSizeChanged();
	emitValuesChanged();
	emitAxisInfoChanged();
	emitInfoChanged();
}

void AMOrderReductionAB::reviewState()
{
	if (source_ == 0  || !source_->isValid() || reducedAxis_ == -1)
		setState(AMDataSource::InvalidFlag);

	else
		setState(0);
}

#include "analysis/AMOrderReductionABEditor.h"

QWidget *AMOrderReductionAB::createEditorWidget()
{
	return new AMOrderReductionABEditor(this);
}
