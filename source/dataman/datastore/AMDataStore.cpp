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


#include "AMDataStore.h"



AMDataStore::AMDataStore(QObject* parent) : QObject(parent)
{
	isInsertingRows_ = false;
}

AMDataStore::~AMDataStore() {
}


bool AMDataStore::beginInsertRows(long numRows, long atRowIndex) {
	if(isInsertingRows_)
		return false;	// nested inserts not supported. Finish the last insert first, and then call endInsertRows().

	// Cannot insert rows if we don't have any scan axes.
	if(scanAxesCount() == 0)
		return false;

	// put atRowIndex within range. (-1, or anything beyond the rowCount, should append)
	long rowCount = scanSize(0);
	if(atRowIndex < 0 || atRowIndex > rowCount)
		atRowIndex = rowCount;

	if(beginInsertRowsImplementation(numRows, atRowIndex) == false) {
		return false;
	}
	else {
		isInsertingRows_ = true;
		insertingNumRows_ = numRows;
		insertingAtRowIndex_ = atRowIndex;
		return true;
	}
}

void AMDataStore::endInsertRows() {
	if(!isInsertingRows_)
		return;

	isInsertingRows_ = false;

	endInsertRowsImplementation(insertingNumRows_, insertingAtRowIndex_);

	// determine a region that covers the whole new scan space. It will be from min to max index in all axes, except for the first axis, which we are inserting into.
	AMnDIndex start(scanAxesCount(), AMnDIndex::DoNotInit);
	AMnDIndex end(scanAxesCount(), AMnDIndex::DoNotInit);

	start[0] = insertingAtRowIndex_;
	// any data that got pushed down by the insert should also be considered changed. Therefore, end should be the new maximum index
	end[0] = scanSize(0)-1;
	// Go through remaining axes and set start/end to cover the full range
	for(int mu=scanAxesCount()-1; mu>=1; --mu) {
		start[mu] = 0;
		end[mu] = scanSize(mu);
	}

	emit sizeChanged();
	for(int d=0; d<measurementCount(); d++)
		emitDataChanged(start, end, d);	// emitted once for each set of measurements.
}

// This function calculates the total size of the array required for values(), spanned by \c scanIndexStart, \c scanIndexEnd, \c measurementIndexStart, and \c measurementIndexEnd.
int AMDataStore::valuesSize(const AMnDIndex &scanIndexStart, const AMnDIndex &scanIndexEnd, const AMnDIndex &measurementIndexStart, const AMnDIndex &measurementIndexEnd) const
{
	if(scanIndexStart.rank() != scanIndexEnd.rank())
		return -1;
	if(measurementIndexStart.rank() != measurementIndexEnd.rank())
		return -1;


	int totalSize = 1;
	for(int mu = 0; mu < scanIndexStart.rank(); mu++) {
		if(scanIndexEnd.at(mu) < scanIndexStart.at(mu))
			return -1;
		totalSize *= (scanIndexEnd.at(mu) - scanIndexStart.at(mu) + 1);
	}

	for(int mu = 0; mu < measurementIndexStart.rank(); mu++) {
		if(measurementIndexEnd.at(mu) < measurementIndexStart.at(mu))
			return -1;
		totalSize *= (measurementIndexEnd.at(mu) - measurementIndexStart.at(mu) + 1);
	}

	return totalSize;
}

QVector<double> AMDataStore::values(const AMnDIndex &scanIndexStart, const AMnDIndex &scanIndexEnd, int measurementId, const AMnDIndex &measurementIndexStart, const AMnDIndex &measurementIndexEnd) const {
	   QVector<double> rv(valuesSize(scanIndexStart, scanIndexEnd, measurementIndexStart, measurementIndexEnd));
	   if(!values(scanIndexStart, scanIndexEnd, measurementId, measurementIndexStart, measurementIndexEnd, rv.data()))
		   return QVector<double>();
	   return rv;
   }
