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


#include "AMDataSourceSeriesData.h"

#include "util/AMUtility.h"

AMDataSourceSeriesData::~AMDataSourceSeriesData(){}

AMDataSourceSeriesData::AMDataSourceSeriesData(const AMDataSource* dataSource, QObject* parent)
	: QObject(parent), MPlotAbstractSeriesData()
{
	source_ = 0;
	isValid_ = false;
	axisSize_ = 0;
	axis_ = QVector<qreal>();
	cachedData_ = QVector<qreal>();
	cachedDataRectUpdateRequired_ = false;
	setDataSource(dataSource);
}

void AMDataSourceSeriesData::setDataSource(const AMDataSource* dataSource) {

	// disconnect the old source, if there is a valid old source.
	if(source_)
		disconnect(source_->signalSource(), 0, this, 0);

	source_ = dataSource;

	if(dataSource == 0 || dataSource->rank() != 1) {

		isValid_ = false;
		axisSize_ = 0;
		axis_ = QVector<qreal>();
		cachedData_ = QVector<qreal>();
	}

	else {

		isValid_ = true;
		connect(dataSource->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onSizeChanged()));
		connect(dataSource->signalSource(), SIGNAL(axisInfoChanged(int)), this, SLOT(onAxisValuesChanged()));
		connect(dataSource->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onDataChanged(AMnDIndex,AMnDIndex)));
		connect(dataSource->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onSizeChanged()));
		connect(dataSource->signalSource(), SIGNAL(deleted(void*)), this, SLOT(onDataSourceDeleted()));
	}

	onSizeChanged();
	onDataChanged(AMnDIndex(0), AMnDIndex(axisSize_-1));
}

double AMDataSourceSeriesData::x(unsigned index) const
{
	return axis_.at(index);
}

void AMDataSourceSeriesData::xValues(unsigned indexStart, unsigned indexEnd, qreal *outputValues) const
{
	memcpy(outputValues, (axis_.constData()+indexStart), (indexEnd-indexStart+1)*sizeof(qreal));
}

double AMDataSourceSeriesData::y(unsigned index) const
{
	if (cacheUpdateRequired_)
		updateCachedValues();

	return cachedData_.at(index);
}

void AMDataSourceSeriesData::yValues(unsigned indexStart, unsigned indexEnd, qreal *outputValues) const
{
	if (cacheUpdateRequired_)
		updateCachedValues();

	memcpy(outputValues, (cachedData_.constData()+indexStart), (indexEnd-indexStart+1)*sizeof(qreal));
}

int AMDataSourceSeriesData::count() const
{
	if(isValid_)
		return source_->size(0);
	else
		return 0;
}

QRectF AMDataSourceSeriesData::boundingRect() const
{
	if (cachedDataRectUpdateRequired_){

		if (axisSize_ == 0)
			cachedDataRect_ = QRectF();

		else {

			if (cacheUpdateRequired_)
				updateCachedValues();

			cachedDataRect_ = QRectF(axis_.first(),
						 range_.minimum(),
						 qMax(axis_.last()-axis_.first(), std::numeric_limits<qreal>::min()),
						 qMax(range_.maximum()-range_.minimum(), std::numeric_limits<qreal>::min()));
		}

		cachedDataRectUpdateRequired_ = false;
	}

	return cachedDataRect_;
}

void AMDataSourceSeriesData::onDataSourceDeleted()
{
	setDataSource(0);
}

void  AMDataSourceSeriesData::onDataChanged(const AMnDIndex &start, const AMnDIndex &end)
{
    Q_UNUSED(start)
    Q_UNUSED(end)
	cacheUpdateRequired_ = true;
	cachedDataRectUpdateRequired_ = true;
	MPlotAbstractSeriesData::emitDataChanged();
}

void AMDataSourceSeriesData::onAxisValuesChanged()
{
	source_->axisValues(0, 0, axisSize_-1, axis_.data());
	cachedDataRectUpdateRequired_ = true;
	onDataChanged(AMnDIndex(0), AMnDIndex(axisSize_-1));
}

void AMDataSourceSeriesData::onSizeChanged()
{
	if (source_ && source_->isValid()){

		axisSize_ = source_->size(0);
		axis_ = QVector<qreal>(axisSize_, 0);
		cachedData_ = QVector<qreal>(axisSize_, 0);
		onAxisValuesChanged();
		onDataChanged(AMnDIndex(0), AMnDIndex(axisSize_-1));
	}
}

void AMDataSourceSeriesData::updateCachedValues() const
{
	if (source_->values(0, axisSize_-1, cachedData_.data())){

		range_ = AMUtility::rangeFinder(cachedData_);
		cacheUpdateRequired_ = false;
	}
}
