#include "AMDataStore.h"

AMMeasurementInfo::AMMeasurementInfo(const QString& mName, const QString& mDescription, const QList<AMAxisInfo>& mAxes) {
	name = mName;
	description = mDescription;
	axes = mAxes;
}


AMDataStoreSignalSource::AMDataStoreSignalSource(AMDataStore *parent)
	: QObject() {
	data_ = parent;
}

AMDataStore::AMDataStore()
{
	signalSource_ = new AMDataStoreSignalSource(this);
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
		return true;
	}
}


void AMDataStore::endInsertRows() {
	if(!isInsertingRows_)
		return;

	endInsertRowsImplementation(insertingAxisId_, insertingNumRows_, insertingAtRowIndex_);

	isInsertingRows_ = false;

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
