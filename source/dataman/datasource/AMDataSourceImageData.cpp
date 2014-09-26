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

AMDataSourceImageData::AMDataSourceImageData(const AMDataSource* dataSource, QObject* parent)
	: QObject(parent), MPlotAbstractImageData()
{
	source_ = 0;
	setDataSource(dataSource);
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

double AMDataSourceImageData::y(int index) const
{
	return yAxis_.at(index);
}

double AMDataSourceImageData::z(int xIndex, int yIndex) const
{
	return data_.at(xIndex + yIndex*xSize_);
}

void AMDataSourceImageData::zValues(int xStart, int yStart, int xEnd, int yEnd, double* outputValues) const
{
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

		for (int i = 0; i < xSize_; i++)
			xAxis_[i] = source_->axisValue(0, i);

		for (int j = 0; j < ySize_; j++)
			yAxis_[j] = source_->axisValue(1, j);
	}

	else if (axisId == 0){

		for (int i = 0; i < xSize_; i++)
			xAxis_[i] = source_->axisValue(0, i);

	}

	else if (axisId == 1) {

		for (int j = 0; j < ySize_; j++)
			yAxis_[j] = source_->axisValue(1, j);
	}

	recomputeBoundingRect(axisId);
	MPlotAbstractImageData::emitBoundsChanged();
}

void AMDataSourceImageData::onDataChanged(const AMnDIndex &start, const AMnDIndex &end)
{
	QVector<double> newData = QVector<double>(start.totalPointsTo(end));
	source_->values(start, end, newData.data());

	int xOffset = start.i()*ySize_;
	int yOffset = start.j();
	double rangeMinimum = newData.first();
	double rangeMaximum = newData.first();

	for (int j = 0, jSize = end.j()-start.j()+1; j < jSize; j++){

		for (int i = 0, iSize = end.i()-start.i()+1; i < iSize; i++){

			double newValue = newData.at(i*jSize+j);

			if (newValue > rangeMaximum)
				rangeMaximum = newValue;

			if (newValue < rangeMinimum)
				rangeMinimum = newValue;

			data_[i*ySize_+xOffset + j+yOffset] = newValue;
		}
	}

	if (range_.first > rangeMinimum)
		range_.first = rangeMinimum;

	if (range_.second < rangeMaximum)
		range_.second = rangeMaximum;

	MPlotAbstractImageData::emitDataChanged();
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

void AMDataSourceImageData::recomputeBoundingRect(int axisId)
{
	if(!isValid_)
		boundingRect_ = QRectF();

	if(xSize_ == 0 || ySize_ == 0)
		boundingRect_ = QRectF();

	else if (axisId == -1){

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

	else if (axisId == 0){

		double minimumX = xAxis_.at(0);
		double maximumX = xAxis_.at(xSize_-1);

		if(maximumX < minimumX)
			qSwap(minimumX, maximumX);

		boundingRect_.setX(minimumX);
		boundingRect_.setWidth(maximumX-minimumX);
	}

	else if (axisId == 1){

		double minimumY = yAxis_.at(0);
		double maximumY = yAxis_.at(ySize_-1);

		if(maximumY < minimumY)
			qSwap(minimumY, maximumY);

		boundingRect_.setY(minimumY);
		boundingRect_.setHeight(maximumY-minimumY);
	}
}

void AMDataSourceImageData::onDataSourceDeleted()
{
	source_ = 0;
	setDataSource(0);
}
