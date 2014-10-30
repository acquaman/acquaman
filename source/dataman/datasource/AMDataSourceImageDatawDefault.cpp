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

AMDataSourceImageDatawDefault::AMDataSourceImageDatawDefault(double defaultValue, QObject *parent)
	: AMDataSourceImageData(parent)
{
	defaultValue_ = defaultValue;
}

AMDataSourceImageDatawDefault::~AMDataSourceImageDatawDefault()
{

}

void AMDataSourceImageDatawDefault::setDefaultValue(double value)
{
	defaultValue_ = value;
	emitDataChanged();
}

void AMDataSourceImageDatawDefault::updateCachedValues() const
{
	QVector<double> newData = QVector<double>(dirtyRectBottomLeft_.totalPointsTo(dirtyRectTopRight_));

	if (source_->values(dirtyRectBottomLeft_, dirtyRectTopRight_, newData.data())){

		int iOffset = dirtyRectBottomLeft_.i()*ySize_;
		int jOffset = dirtyRectBottomLeft_.j();
		double rangeMinimum = newData.first();
		double rangeMaximum = newData.first();

		for (int j = 0, jSize = dirtyRectTopRight_.j()-dirtyRectBottomLeft_.j()+1; j < jSize; j++){

			for (int i = 0, iSize = dirtyRectTopRight_.i()-dirtyRectBottomLeft_.i()+1; i < iSize; i++){

				double newValue = newData.at(i*jSize+j);

				if (newValue > rangeMaximum && newValue != defaultValue_)
					rangeMaximum = newValue;

				if (newValue < rangeMinimum && newValue != defaultValue_)
					rangeMinimum = newValue;

				data_[i*ySize_ + iOffset + j + jOffset] = newValue;
			}
		}

		// The default range is invalid.
		if (range_.isNull() && rangeMinimum != defaultValue_ && rangeMaximum != defaultValue_)
			range_ = MPlotRange(rangeMinimum, rangeMaximum);

		else {

			if (range_.x() > rangeMinimum && rangeMinimum != defaultValue_)
				range_.setX(rangeMinimum);

			if (range_.y() < rangeMaximum && rangeMaximum != defaultValue_)
				range_.setY(rangeMaximum);
		}

		dirtyRectBottomLeft_ = AMnDIndex();
		dirtyRectTopRight_ = AMnDIndex();
		updateCacheRequired_ = false;
	}
}
