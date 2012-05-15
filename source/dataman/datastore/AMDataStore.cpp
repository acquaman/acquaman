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


#include "AMDataStore.h"




AMDataStoreSignalSource::AMDataStoreSignalSource(AMDataStore *parent)
	: QObject() {
	data_ = parent;
}

AMDataStore::AMDataStore()
{
	signalSource_ = new AMDataStoreSignalSource(this);
	isInsertingRows_ = false;
}

AMDataStore::~AMDataStore() {
	delete signalSource_;
	signalSource_ = 0;
}


bool AMDataStore::beginInsertRows(int axisId, int numRows, int atRowIndex) {
	if(isInsertingRows_)
		return false;	// nested inserts not supported.

	// put atRowIndex within range. (-1, or anything beyond the rowCount, should append)
	int rowCount = scanAxisAt(axisId).size;
	if(atRowIndex < 0 || atRowIndex > rowCount)
		atRowIndex = rowCount;

	if(beginInsertRowsImplementation(axisId, numRows, atRowIndex) == false) {
		return false;
	}
	else {
		isInsertingRows_ = true;
		insertingAxisId_ = axisId;
		insertingNumRows_ = numRows;
		insertingAtRowIndex_ = atRowIndex;
		multiAxisInsertInProgress_ = false;
		return true;
	}
}

bool AMDataStore::beginInsertRowsAsNecessaryForScanPoint(const AMnDIndex& scanIndex) {

	if(isInsertingRows_)
		return false; // nested inserts not supported.

	if(scanIndex.rank() != scanRank())
		return false;	// wrong dimensions of scanIndex.

	/// \todo For now we trust that the inserts will succeed, when calling begingInsertRowsImplementation. Check on this...

	isInsertingRows_ = true;
	multiAxisInsertInProgress_ = true;
	// the starting index of our insert (inclusive)
	scanSpaceStartIndex_ = scanSize();
	scanSpaceEndIndex_ = scanIndex;

	// go through each axis and insert the number of rows required.
	for(int mu=0; mu<scanRank(); mu++) {
		if(scanIndex.at(mu) >= scanSpaceStartIndex_[mu]) {
			int numRows = scanIndex.at(mu)-scanSpaceStartIndex_[mu]+1;
			int atRowIndex = scanSpaceStartIndex_[mu];
			beginInsertRowsImplementation(mu, numRows, atRowIndex);
			endInsertRowsImplementation(mu, numRows, atRowIndex);	// need to do this here, so as not to nest inserts.
		}
	}
	return true;
}


void AMDataStore::endInsertRows() {
	if(!isInsertingRows_)
		return;

	isInsertingRows_ = false;

	// case of multi-axis insert
	/////////////////////////////
	if(multiAxisInsertInProgress_) {

//		needs to be done after each beginInsertRowsImplementation instead:
//		for(int mu=0; mu<scanRank(); mu++) {
//			if(scanSpaceEndIndex_[mu] >= scanSpaceStartIndex_[mu])
//				endInsertRowsImplementation(mu, scanSpaceEndIndex_[mu]-scanSpaceStartIndex_[mu]+1, scanSpaceStartIndex_[mu]);
//		}

		emitSizeChanged(-1);	// multiple axes might have changed size.
		for(int d=0; d<measurementCount(); d++)	// emit dataChanged for each measurement
			emitDataChanged(scanSpaceStartIndex_, scanSpaceEndIndex_, d);
	}

	// case of single-axis insert
	///////////////////////
	else {
		endInsertRowsImplementation(insertingAxisId_, insertingNumRows_, insertingAtRowIndex_);

		// determine a region that covers the whole new scan space. It will be from min to max index in all axes, except for the axis along which we're inserting.
		AMnDIndex start, end;
		for(int mu=0; mu<scanAxesCount(); mu++) {	// mu is a "meta-index"... iterating over the axis indexes.
			if(mu == insertingAxisId_) {
				start.append(insertingAtRowIndex_);	// any data that got pushed down should also be considered changed. end should be the max index for this axis.
				end.append(scanAxisAt(mu).size-1);
			}
			else {	// in all other axes, it's the whole range (since these are all new points)
				start.append(0);
				end.append(scanAxisAt(mu).size-1);
			}
		}

		emitSizeChanged(insertingAxisId_);
		for(int d=0; d<measurementCount(); d++)
			emitDataChanged(start, end, d);	// emitted once for each set of measurements.
	}
}
