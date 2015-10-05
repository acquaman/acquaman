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

#include "util/AMUtility.h"

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
	AMnDIndex start = AMnDIndex(0, 0);
	AMnDIndex end = AMnDIndex(xSize_-1, ySize_-1);

	if (source_->values(start, end, cachedData_.data())){

		AMRange dataSourceRange = AMUtility::rangeFinder(cachedData_, defaultValue_);
		range_ = MPlotRange(dataSourceRange.minimum(), dataSourceRange.maximum());
		cacheUpdateRequired_ = false;
	}
}
