/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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

 AMDataSourceImageData::~AMDataSourceImageData(){}
AMDataSourceImageData::AMDataSourceImageData(const AMDataSource* dataSource, QObject* parent)
	: QObject(parent), MPlotAbstractImageData()
{
	source_ = 0;
	setDataSource(dataSource);
}

/// Call this to switch to representing a different data source
void AMDataSourceImageData::setDataSource(const AMDataSource* dataSource) {

	// disconnect the old source, if there is a valid old source.
	if(source_) {
		disconnect(source_->signalSource(), 0, this, 0);
	}

	source_ = dataSource;
	if(dataSource == 0 || dataSource->rank() != 2) {
		isValid_ = false;
	}
	else {
		isValid_ = true;
		connect(dataSource->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onDataSourceDataChanged()));
		connect(dataSource->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onDataSourceBoundsChanged()));

		connect(dataSource->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onDataSourceDataChanged()));

		connect(dataSource->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onDataSourceBoundsChanged()));
		connect(dataSource->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onDataSourceDataChanged()));

		connect(dataSource->signalSource(), SIGNAL(deleted(void*)), this, SLOT(onDataSourceDeleted()));
	}

	onDataSourceBoundsChanged();
	onDataSourceDataChanged();
}
