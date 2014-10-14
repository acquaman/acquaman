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

AMDataSourceSeriesData::~AMDataSourceSeriesData(){}

AMDataSourceSeriesData::AMDataSourceSeriesData(const AMDataSource* dataSource, QObject* parent)
	: QObject(parent), MPlotAbstractSeriesData()
{
	source_ = 0;
	setDataSource(dataSource);
}

void AMDataSourceSeriesData::setDataSource(const AMDataSource* dataSource) {

	// disconnect the old source, if there is a valid old source.
	if(source_) {
		disconnect(source_->signalSource(), 0, this, 0);
	}

	source_ = dataSource;
	if(dataSource == 0 || dataSource->rank() != 1) {
		isValid_ = false;
	}
	else {
		isValid_ = true;
		connect(dataSource->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onDataSourceDataChanged()));
		connect(dataSource->signalSource(), SIGNAL(axisInfoChanged(int)), this, SLOT(onDataSourceDataChanged()));
		connect(dataSource->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onDataSourceDataChanged()));
		connect(dataSource->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onDataSourceDataChanged()));
		connect(dataSource->signalSource(), SIGNAL(deleted(void*)), this, SLOT(onDataSourceDeleted()));
	}

	onDataSourceDataChanged();
}

double AMDataSourceSeriesData::x(unsigned index) const
{
	return source_->axisValue(0, index);
}

void AMDataSourceSeriesData::xValues(unsigned indexStart, unsigned indexEnd, qreal *outputValues) const
{
	int size = int(indexEnd - indexStart + 1);
	QVector<AMNumber> axisData = QVector<AMNumber>(size, 0);
	source_->axisValues(0, int(indexStart), int(indexEnd), axisData.data());

	for(int i = 0; i < size; i++)
		outputValues[i] = qreal(axisData.at(i));
}

double AMDataSourceSeriesData::y(unsigned index) const
{
	return source_->value(AMnDIndex(index));
}

void AMDataSourceSeriesData::yValues(unsigned indexStart, unsigned indexEnd, qreal *outputValues) const
{
	source_->values(AMnDIndex(indexStart), AMnDIndex(indexEnd), outputValues);
}

int AMDataSourceSeriesData::count() const
{
	if(isValid_)
		return source_->size(0);
	else
		return 0;
}
