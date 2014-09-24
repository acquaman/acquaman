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


#include "AMDataSourceImageDatawDefault.h"

AMDataSourceImageDatawDefault::AMDataSourceImageDatawDefault(const AMDataSource *dataSource, double defaultValue, QObject *parent)
	: AMDataSourceImageData(dataSource, parent)
{
	defaultValue_ = defaultValue;
}

AMDataSourceImageDatawDefault::~AMDataSourceImageDatawDefault()
{

}

void AMDataSourceImageDatawDefault::setDefaultValue(double value)
{
	defaultValue_ = value;
	MPlotAbstractImageData::emitDataChanged();
}

void AMDataSourceImageDatawDefault::onDataChanged(const AMnDIndex &start, const AMnDIndex &end)
{
	QVector<double> newData = QVector<double>(start.totalPointsTo(end));
	source_->values(start, end, newData.data());

	int xOffset = start.i()*ySize_;
	int yOffset = start.j();
	double rangeMinimum = newData.first();
	double rangeMaximum = newData.first();

	for (int j = 0, jSize = end.j()-start.j()+1; j < jSize; j++){

		for (int i = 0, iSize = end.i()-start.i()+1; i < iSize; i++){

			double newValue = newData.at(i+j*iSize);

			if (newValue > rangeMaximum)
				rangeMaximum = newValue;

			if (newValue < rangeMinimum && newValue != defaultValue_)
				rangeMinimum = newValue;

			data_[i*ySize_+xOffset + j+yOffset] = newValue;
		}
	}

	range_ = MPlotInterval(rangeMinimum, rangeMaximum);

	MPlotAbstractImageData::emitDataChanged();
}
