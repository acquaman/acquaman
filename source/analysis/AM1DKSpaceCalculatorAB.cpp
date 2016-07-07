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


#include "AM1DKSpaceCalculatorAB.h"

#include "util/AMEnergyToKSpaceCalculator.h"

AM1DKSpaceCalculatorAB::AM1DKSpaceCalculatorAB(const QString &outputName, QObject *parent)
	: AMStandardAnalysisBlock(outputName, parent)
{
	data_ = 0;
	edgeEnergy_ = -1.0;
	axes_ << AMAxisInfo("invalid", 0, "No input data");
	setState(AMDataSource::InvalidFlag);
}

bool AM1DKSpaceCalculatorAB::areInputDataSourcesAcceptable(const QList<AMDataSource *> &dataSources) const
{
	if (dataSources.isEmpty())
		return true;	// always acceptable, the null input.

	// Otherwise we need one input source with rank 1.
	if (dataSources.size() == 1 && dataSources.first()->rank() == 1)
		return true;

	return false;
}

void AM1DKSpaceCalculatorAB::setEdgeEnergy(double energy)
{
	if (energy != edgeEnergy_){

		edgeEnergy_ = energy;
		emitValuesChanged();
	}
}

AMNumber AM1DKSpaceCalculatorAB::value(const AMnDIndex &indexes) const
{
	if (indexes.rank() != 1)
		return AMNumber(AMNumber::DimensionError);

	if(!isValid())
		return AMNumber(AMNumber::InvalidError);

	if (edgeEnergy_ <= 0.0)
		return AMNumber(AMNumber::InvalidError);

	if((unsigned)indexes.i() >= (unsigned)axes_.at(0).size)
		return AMNumber(AMNumber::OutOfBoundsError);

	return AMEnergyToKSpaceCalculator::k(AMNumber(edgeEnergy_), data_->value(indexes));
}

bool AM1DKSpaceCalculatorAB::values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const
{
	if(indexStart.rank() != 1 || indexEnd.rank() != 1)
		return false;

	if(!isValid())
		return false;

	if (edgeEnergy_ <= 0)
		return false;

	if((unsigned)indexEnd.i() >= (unsigned)axes_.at(0).size || (unsigned)indexStart.i() > (unsigned)indexEnd.i())
		return false;

	int totalSize = indexStart.totalPointsTo(indexEnd);

	AMEnergyToKSpaceCalculator calculator = AMEnergyToKSpaceCalculator(AMNumber(edgeEnergy_));

	QVector<double> data = QVector<double>(totalSize);
	data_->values(indexStart, indexEnd, data.data());

	for (int i = 0; i < totalSize; i++)
		outputValues[i] = double(calculator.k(data.at(i)));

	return true;
}

AMNumber AM1DKSpaceCalculatorAB::axisValue(int axisNumber, int index) const
{
	if (!isValid())
		return AMNumber(AMNumber::InvalidError);

	if (axisNumber != 0)
		return AMNumber(AMNumber::DimensionError);

	if (index >= axes_.at(axisNumber).size)
		return AMNumber(AMNumber::DimensionError);

	return data_->axisValue(axisNumber, index);
}

bool AM1DKSpaceCalculatorAB::axisValues(int axisNumber, int startIndex, int endIndex, double *outputValues) const
{
	if (!isValid())
		return false;

	if (axisNumber != 0)
		return false;

	if (startIndex >= axes_.at(axisNumber).size || endIndex >= axes_.at(axisNumber).size)
		return false;

	return data_->axisValues(axisNumber, startIndex, endIndex, outputValues);
}

void AM1DKSpaceCalculatorAB::onInputSourceValuesChanged(const AMnDIndex& start, const AMnDIndex& end)
{
	emitValuesChanged(start, end);
}

void AM1DKSpaceCalculatorAB::onInputSourceSizeChanged()
{
	if(axes_.at(0).size != data_->size(0)){

		axes_[0].size = data_->size(0);
		emitSizeChanged(0);
	}
}

void AM1DKSpaceCalculatorAB::onInputSourceStateChanged() {

	// just in case the size has changed while the input source was invalid, and now it's going valid.  Do we need this? probably not, if the input source is well behaved. But it's pretty inexpensive to do it twice... and we know we'll get the size right everytime it goes valid.
	onInputSourceSizeChanged();
	reviewState();
}

void AM1DKSpaceCalculatorAB::setInputDataSourcesImplementation(const QList<AMDataSource *> &dataSources)
{
	if (data_){

		disconnect(data_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		disconnect(data_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		disconnect(data_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
		data_ = 0;
	}

	if (dataSources.isEmpty()){

		data_ = 0;
		sources_.clear();

		axes_[0] = AMAxisInfo("invalid", 0, "No input data");
		setDescription("k-Space 1D Data Source");
	}

	else if (dataSources.size() == 1){

		data_ = dataSources.at(0);
		sources_ = dataSources;

		axes_[0] = data_->axisInfoAt(0);

		setDescription(QString("k-Space of %1").arg(data_->name()));

		connect(data_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		connect(data_->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		connect(data_->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
	}

	reviewState();

	emitSizeChanged(0);
	emitValuesChanged();
	emitAxisInfoChanged(0);
	emitInfoChanged();
}

void AM1DKSpaceCalculatorAB::reviewState(){

	if(data_ == 0 || !data_->isValid() || edgeEnergy_ <= 0) {
		setState(AMDataSource::InvalidFlag);
		return;
	}
	else
		setState(0);
}
