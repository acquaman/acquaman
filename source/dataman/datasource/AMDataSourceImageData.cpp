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


#include "AMDataSourceImageData.h"

AMDataSourceImageData::AMDataSourceImageData(QObject* parent)
	: QObject(parent), MPlotAbstractImageData()
{
	source_ = 0;
	updateCacheRequired_ = true;
}

AMDataSourceImageData::~AMDataSourceImageData()
{

}

void AMDataSourceImageData::setDataSource(const AMDataSource* dataSource) {

	// disconnect the old source, if there is a valid old source.
	if(source_) {
		disconnect(source_->signalSource(), 0, this, 0);
	}

	source_ = dataSource;

	if(dataSource == 0 || dataSource->rank() != 2){

		isValid_ = false;
		xSize_ = 0;
		ySize_ = 0;
		boundingRect_ = QRect();
		xAxis_ = QVector<double>();
		yAxis_ = QVector<double>();
		data_ = QVector<double>();
	}

	else {

		isValid_ = true;

		connect(dataSource->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onSizeChanged(int)));
		connect(dataSource->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onSizeChanged(int)));
		connect(dataSource->signalSource(), SIGNAL(axisInfoChanged(int)), this, SLOT(onAxisValuesChanged(int)));
		connect(dataSource->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onDataChanged(AMnDIndex,AMnDIndex)));
	}

	onSizeChanged(-1);
	onDataChanged(AMnDIndex(0,0), AMnDIndex(xSize_-1, ySize_-1));
}

double AMDataSourceImageData::x(int index) const
{
	return xAxis_.at(index);
}

void AMDataSourceImageData::xValues(int startIndex, int endIndex, double *outputValues)
{
	memcpy(outputValues, (xAxis_.constData()+startIndex), (endIndex-startIndex+1)*sizeof(double));
}

double AMDataSourceImageData::y(int index) const
{
	return yAxis_.at(index);
}

void AMDataSourceImageData::yValues(int startIndex, int endIndex, double *outputValues)
{
	memcpy(outputValues, (yAxis_.constData()+startIndex), (endIndex-startIndex+1)*sizeof(double));
}

double AMDataSourceImageData::z(int xIndex, int yIndex) const
{
	return data_.at(xIndex + yIndex*xSize_);
}

void AMDataSourceImageData::zValues(int xStart, int yStart, int xEnd, int yEnd, double* outputValues) const
{
	if (updateCacheRequired_)
		updateCachedValues();

	// If we want the whole array, just use memcpy.
	if (xStart == 0 && yStart == 0 && xEnd == (xSize_-1) && yEnd == (ySize_-1))
		memcpy(outputValues, data_.constData(), (xSize_*ySize_)*sizeof(double));

	else{

		for (int j = 0, jSize = yEnd-yStart+1; j < jSize; j++)
			for (int i = 0, iSize = xEnd-xStart+1; i < iSize; i++)
				outputValues[i+j*iSize] = data_.at(i+xStart + (j+yStart)*xSize_);
	}
}

QPoint AMDataSourceImageData::count() const
{
	return QPoint(xSize_, ySize_); // Returns 0,0 when invalid so we are still okay.
}

QRectF AMDataSourceImageData::boundingRect() const
{
	return boundingRect_;
}

void AMDataSourceImageData::onAxisValuesChanged(int axisId)
{
	if (axisId == -1){

		source_->axisValues(0, 0, xSize_-1, xAxis_.data());
		source_->axisValues(1, 0, ySize_-1, yAxis_.data());
	}

	else if (axisId == 0){

		source_->axisValues(0, 0, xSize_-1, xAxis_.data());
	}

	else if (axisId == 1) {

		source_->axisValues(1, 0, ySize_-1, yAxis_.data());
	}

    recomputeBoundingRect();
	emitBoundsChanged();
}

void AMDataSourceImageData::onDataChanged(const AMnDIndex &start, const AMnDIndex &end)
{
    Q_UNUSED(start)
    Q_UNUSED(end)
	updateCacheRequired_ = true;
	emitDataChanged();
}

void AMDataSourceImageData::onSizeChanged(int axisId)
{
	if (axisId == -1){

		xSize_ = source_->size(0);
		ySize_ = source_->size(1);
		xAxis_ = QVector<double>(xSize_, 0);
		yAxis_ = QVector<double>(ySize_, 0);
	}

	else if (axisId == 0){

		xSize_ = source_->size(0);
		xAxis_ = QVector<double>(xSize_, 0);
	}

	else if (axisId == 1){

		ySize_ = source_->size(1);
		yAxis_ = QVector<double>(ySize_);
	}

	data_ = QVector<double>(xSize_*ySize_, 0);
	onAxisValuesChanged(axisId);
	onDataChanged(AMnDIndex(0,0), AMnDIndex(xSize_-1, ySize_-1));
}

void AMDataSourceImageData::recomputeBoundingRect()
{
	if(!isValid_)
		boundingRect_ = QRectF();

	if(xSize_ == 0 || ySize_ == 0)
		boundingRect_ = QRectF();

    else {

		double minimumX = xAxis_.at(0);
		double maximumX = xAxis_.at(xSize_-1);
		double minimumY = yAxis_.at(0);
		double maximumY = yAxis_.at(ySize_-1);

		if(maximumX < minimumX)
			qSwap(minimumX, maximumX);

		if(maximumY < minimumY)
			qSwap(minimumY, maximumY);

		boundingRect_ = QRectF(minimumX, minimumY, maximumX-minimumX, maximumY-minimumY);
	}
}

void AMDataSourceImageData::onDataSourceDeleted()
{
	setDataSource(0);
}

MPlotRange AMDataSourceImageData::range() const
{
	if (updateCacheRequired_)
		updateCachedValues();

	return range_;
}

void AMDataSourceImageData::updateCachedValues() const
{
    AMnDIndex start = AMnDIndex(0, 0);
    AMnDIndex end = AMnDIndex(xSize_-1, ySize_-1);

    if (source_->values(start, end, data_.data())){

        double rangeMinimum = data_.first();
        double rangeMaximum = data_.first();

        for (int i = 0, size = data_.size(); i < size; i++){

            double newValue = data_.at(i);

            if (newValue > rangeMaximum)
                rangeMaximum = newValue;

            if (newValue < rangeMinimum)
                rangeMinimum = newValue;
        }

        range_ = MPlotRange(rangeMinimum, rangeMaximum);
		updateCacheRequired_ = false;
	}
}
