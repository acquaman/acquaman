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


#include "AMProcessVariableDataSource.h"

// AM0DProcessVariableDataSource
///////////////////////////////////////////////////////

AM0DProcessVariableDataSource::~AM0DProcessVariableDataSource(){}

AM0DProcessVariableDataSource::AM0DProcessVariableDataSource(const AMProcessVariable *data, const QString &name, QObject *parent)
	: QObject(parent), AMDataSource(name)
{
	data_ = data;
	axes_ = QList<AMAxisInfo>();

	connect(data_, SIGNAL(valueChanged()), this, SLOT(onDataChanged()));
	connect(data_, SIGNAL(hasValuesChanged(bool)), this, SLOT(onStateChanged()));
}

QString AM0DProcessVariableDataSource::typeDescription() const
{
	return "Scalar Process Variable Data Source";
}

int AM0DProcessVariableDataSource::state() const
{
	return data_->hasValues() ? ProcessingFlag : InvalidFlag;
}

QList<AMAxisInfo> AM0DProcessVariableDataSource::axes() const
{
	return axes_;
}

int AM0DProcessVariableDataSource::rank() const
{
	return 0;
}

AMnDIndex AM0DProcessVariableDataSource::size() const
{
	return AMnDIndex();
}

int AM0DProcessVariableDataSource::size(int axisNumber) const
{
	Q_UNUSED(axisNumber)
	return 1;
}

AMAxisInfo AM0DProcessVariableDataSource::axisInfoAt(int axisNumber) const
{
	Q_UNUSED(axisNumber)
	return AMAxisInfo("scalar", 1);
}

int AM0DProcessVariableDataSource::idOfAxis(const QString& axisName) const
{
	Q_UNUSED(axisName)
	return -1;
}

AMNumber AM0DProcessVariableDataSource::value(const AMnDIndex &indexes) const
{
	if(!data_->isConnected())
		return AMNumber();

	if (indexes.rank() != 0)
		return AMNumber(AMNumber::DimensionError);

	return data_->lastValue();
}

AMNumber AM0DProcessVariableDataSource::axisValue(int axisNumber, int index) const
{
	Q_UNUSED(axisNumber)
	Q_UNUSED(index)

	return 0;
}

void AM0DProcessVariableDataSource::onDataChanged()
{
	emitValuesChanged();
}

void AM0DProcessVariableDataSource::onStateChanged()
{
	emitStateChanged(state());
}

// AM1DProcessVariableDataSource
///////////////////////////////////////////////////////

AM1DProcessVariableDataSource::~AM1DProcessVariableDataSource(){}

AM1DProcessVariableDataSource::AM1DProcessVariableDataSource(const AMProcessVariable *data, const QString &name, QObject *parent)
	: QObject(parent), AMDataSource(name)
{
	data_ = data;

	connect(data_, SIGNAL(initialized()), this, SLOT(onInitialized()));
	connect(data_, SIGNAL(valueChanged()), this, SLOT(onDataChanged()));
	connect(data_, SIGNAL(hasValuesChanged(bool)), this, SLOT(onStateChanged()));

	AMAxisInfo ai("xAxis", 0, "Array X-axis");
	ai.isUniform = true;
	ai.increment = 1.0;
	ai.start = 0.0;
	axes_ << ai;
}

void AM1DProcessVariableDataSource::onInitialized()
{
	axes_[0].size = data_->count();
	emitAxisInfoChanged(0);
	emitSizeChanged(0);
}

QString AM1DProcessVariableDataSource::typeDescription() const
{
	return "Array Process Variable Data Source";
}

int AM1DProcessVariableDataSource::state() const
{
	return data_->hasValues() ? ProcessingFlag : InvalidFlag;
}

QList<AMAxisInfo> AM1DProcessVariableDataSource::axes() const
{
	return axes_;
}

int AM1DProcessVariableDataSource::rank() const
{
	return 1;
}

AMnDIndex AM1DProcessVariableDataSource::size() const
{
	return AMnDIndex(axes_.at(0).size);
}

int AM1DProcessVariableDataSource::size(int axisNumber) const
{
	if (axisNumber != 0)
		return -1;

	return axes_.at(axisNumber).size;
}

AMAxisInfo AM1DProcessVariableDataSource::axisInfoAt(int axisNumber) const
{
	return axes_.at(axisNumber);
}

int AM1DProcessVariableDataSource::idOfAxis(const QString &axisName) const
{
	if(axisName == axes_.at(0).name)
		return 0;

	return -1;
}

AMNumber AM1DProcessVariableDataSource::value(const AMnDIndex &indexes) const
{
	if(!data_->isConnected())
		return AMNumber();
	if(indexes.rank() != 1)
		return AMNumber(AMNumber::DimensionError);
	if((unsigned)indexes.i() >= data_->count())
		return AMNumber(AMNumber::OutOfBoundsError);

	return data_->lastValue(indexes.i());
}

bool AM1DProcessVariableDataSource::values(const AMnDIndex& indexStart, const AMnDIndex& indexEnd, double* outputValues) const
{
	if (!data_->isConnected())
		return false;

	if (indexStart.rank() != 1 || indexEnd.rank() != 1)
		return false;

	if((unsigned)indexStart.i() >= data_->count() || indexStart.i() > indexEnd.i())
		return false;

	if (data_->dataType() == PVDataType::FloatingPoint)
		memcpy(outputValues, data_->lastFloatingPointValues().constData(), indexStart.totalPointsTo(indexEnd)*sizeof(double));

	else{

		int totalSize = indexStart.totalPointsTo(indexEnd);
		QVector<double> val = QVector<double>(totalSize);
		for (int i = 0, offset = indexStart.i(); i < totalSize; i++)
			val[i] = double(data_->lastIntegerValues().at(i+offset));

		memcpy(outputValues, val.constData(), indexStart.totalPointsTo(indexEnd)*sizeof(double));
	}
	return true;
}

AMNumber AM1DProcessVariableDataSource::axisValue(int axisNumber, int index) const
{
	if(!data_->isConnected())
		return AMNumber();

	if(axisNumber != 0)
		return AMNumber(AMNumber::DimensionError);

	if ((unsigned)index >= data_->count())
		return AMNumber(AMNumber::OutOfBoundsError);

	return double(axes_.at(0).start) + index*double(axes_.at(0).increment);
}

bool AM1DProcessVariableDataSource::axisValues(int axisNumber, int startIndex, int endIndex, double *outputValues) const
{
	if (!data_->isConnected())
		return false;

	if (!isValid())
		return false;

	if (axisNumber != 0)
		return false;

	if (startIndex < 0 || startIndex >= size(axisNumber))
		return false;

	if (endIndex < 0 || endIndex >= size(axisNumber))
		return false;

	for (int i = 0, size = endIndex-startIndex+1; i < size; i++)
        outputValues[i] = double(axes_.at(0).start) + (i+startIndex)*double(axes_.at(0).increment);

	return true;
}

double AM1DProcessVariableDataSource::scale() const
{
	return double(axes_.at(0).increment);
}

double AM1DProcessVariableDataSource::offset() const
{
	return double(axes_.at(0).start);
}

void AM1DProcessVariableDataSource::setScale(double scale)
{
	if(double(axes_.at(0).increment) != scale){

		axes_[0].increment = scale;
		emitValuesChanged(AMnDIndex(0), size()-1);
		emitAxisInfoChanged();
	}
}

void AM1DProcessVariableDataSource::setOffset(double offset)
{
	if(double(axes_.at(0).start) != offset){

		axes_[0].start = offset;
		emitValuesChanged(AMnDIndex(0), size()-1);
		emitAxisInfoChanged();
	}
}

void AM1DProcessVariableDataSource::onDataChanged()
{
	emitValuesChanged(AMnDIndex(0), size()-1);
}

void AM1DProcessVariableDataSource::onStateChanged()
{
	emitStateChanged(state());
}

// AM2DProcessVariableDataSource
///////////////////////////////////////////////////////

AM2DProcessVariableDataSource::~AM2DProcessVariableDataSource(){}

AM2DProcessVariableDataSource::AM2DProcessVariableDataSource(const AMProcessVariable *data, const QString &name, int rowLength, QObject *parent)
	: QObject(parent), AMDataSource(name)
{
	data_ = data;
	sx_ = 1;
	sy_ = 1;
	length_ = rowLength;
	connect(data_, SIGNAL(initialized()), this, SLOT(onInitialized()));
	connect(data_, SIGNAL(valueChanged()), this, SLOT(onDataChanged()));
	connect(data_, SIGNAL(hasValuesChanged(bool)), this, SLOT(onStateChanged()));

	AMAxisInfo xaxis("xAxis", length_, "Image X-axis");
	xaxis.isUniform = true;
	xaxis.increment = sx_;
	AMAxisInfo yaxis("yAxis", data_->count()/length_, "Image Y-axis");
	yaxis.isUniform = true;
	yaxis.increment = sy_;
	axes_ << xaxis << yaxis;
}

void AM2DProcessVariableDataSource::onInitialized()
{
	axes_[0].size = length_;
	emitAxisInfoChanged(0);
	emitSizeChanged(0);

	axes_[1].size = data_->count()/length_;
	emitAxisInfoChanged(1);
	emitSizeChanged(1);
}

QString AM2DProcessVariableDataSource::typeDescription() const
{
	return "Image Process Variable Data Source";
}

int AM2DProcessVariableDataSource::state() const
{
	return data_->hasValues() ? ProcessingFlag : InvalidFlag;
}

QList<AMAxisInfo> AM2DProcessVariableDataSource::axes() const
{
	return axes_;
}

int AM2DProcessVariableDataSource::rank() const
{
	return 2;
}

AMnDIndex AM2DProcessVariableDataSource::size() const
{
	return AMnDIndex(axes_.at(0).size, axes_.at(1).size);
}

int AM2DProcessVariableDataSource::size(int axisNumber) const
{
	if (axisNumber != 0 && axisNumber != 1)
		return -1;

	return axes_.at(axisNumber).size;
}

AMAxisInfo AM2DProcessVariableDataSource::axisInfoAt(int axisNumber) const
{
	return axes_.at(axisNumber);
}

int AM2DProcessVariableDataSource::idOfAxis(const QString& axisName) const
{
	if(axisName == axes_.at(0).name)
		return 0;
	if(axisName == axes_.at(1).name)
		return 1;

	return -1;
}

AMNumber AM2DProcessVariableDataSource::value(const AMnDIndex &indexes) const
{
	if(!data_->isConnected())
		return AMNumber();
	if(indexes.rank() != 2)
		return AMNumber(AMNumber::DimensionError);
	if(((unsigned)indexes.i() >= (unsigned)length_ || (unsigned)indexes.j() >= data_->count()/length_))
		return AMNumber(AMNumber::OutOfBoundsError);

	return data_->lastValue(indexes.i() + indexes.j()*length_);	/// \todo Acquaman normally uses the opposite convention: the first index varies the slowest while iterating through a flat array.
}

bool AM2DProcessVariableDataSource::values(const AMnDIndex& indexStart, const AMnDIndex& indexEnd, double* outputValues) const
{
	if (!data_->isConnected())
		return false;

	if (indexStart.rank() != 2 || indexEnd.rank() != 2)
		return false;

	if((unsigned)indexStart.totalPointsTo(indexEnd) >= data_->count() || indexStart.i() > indexEnd.i() || indexStart.j() > indexEnd.j())
		return false;

	memcpy(outputValues, data_->lastFloatingPointValues().constData(), indexStart.totalPointsTo(indexEnd)*sizeof(double));
	return true;
}

AMNumber AM2DProcessVariableDataSource::axisValue(int axisNumber, int index) const
{
	if(!data_->isConnected())
		return AMNumber();
	if (axisNumber > 1)
		return AMNumber(AMNumber::DimensionError);


	if (axisNumber == 0) {
		if ((unsigned)index >= (unsigned)length_)
			return AMNumber(AMNumber::OutOfBoundsError);
		return index*sx_;
	}
	else {
		if ((unsigned)index >= data_->count()/length_)
			return AMNumber(AMNumber::OutOfBoundsError);
		return index*sy_;
	}
}

bool AM2DProcessVariableDataSource::axisValues(int axisNumber, int startIndex, int endIndex, double *outputValues) const
{
	if (!data_->isConnected())
		return false;

	if (!isValid())
		return false;

	if (axisNumber > 1)
		return false;

	if (startIndex < 0 || startIndex >= size(axisNumber))
		return false;

	if (endIndex < 0 || endIndex >= size(axisNumber))
		return false;

	double scaler = axisNumber == 0 ? sx_ : sy_;

	for (int i = 0, size = endIndex-startIndex+1; i < size; i++)
        outputValues[i] = (i+startIndex)*scaler;

	return true;
}

QPair<double, double> AM2DProcessVariableDataSource::scale() const
{
	return qMakePair(sx_, sy_);
}

double AM2DProcessVariableDataSource::scaleX() const
{
	return sx_;
}

double AM2DProcessVariableDataSource::scaleY() const
{
	return sy_;
}

void AM2DProcessVariableDataSource::setScale(double sx, double sy)
{
	sx_ = sx;
	sy_ = sy;
	axes_[0].increment = sx_;
	axes_[1].increment = sy_;
	emitAxisInfoChanged();
	emitValuesChanged();
}

void AM2DProcessVariableDataSource::setScaleX(double sx)
{
	sx_ = sx;
	axes_[0].increment = sx;
	emitAxisInfoChanged(0);
	emitValuesChanged();
}

void AM2DProcessVariableDataSource::setScaleY(double sy)
{
	sy_ = sy;
	axes_[1].increment = sy;
	emitAxisInfoChanged(1);
	emitValuesChanged();
}

void AM2DProcessVariableDataSource::onDataChanged()
{
	emitValuesChanged();
}

void AM2DProcessVariableDataSource::onStateChanged() { emitStateChanged(state()); }
