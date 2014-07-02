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


#include "AMRawDataSource.h"


 AMRawDataSource::~AMRawDataSource(){}
AMRawDataSource::AMRawDataSource(const AMDataStore* dataStore, int measurementId, QObject* parent)
	: AMDbObject(parent), AMDataSource( dataStore->measurementAt(measurementId).name )
{

	dataStore_ = dataStore;
	measurementId_ = measurementId;
	AMMeasurementInfo measurementInfo = dataStore_->measurementAt(measurementId);

	AMDbObject::setName( AMDataSource::name() );
	setDescription( measurementInfo.description );

	scanAxesCount_ = dataStore_->scanRank();
	measurementAxesCount_ = measurementInfo.rank();

	// create the correct number of axes
	for(int i=0; i<scanAxesCount_; i++)
		axes_ << dataStore_->scanAxisAt(i);	// adds scan axes
	axes_ << measurementInfo.axes;	// adds detector axes

	measurementIndexEnd_ = measurementIndexStart_ = measurementInfo.size();
	for(int a=0; a<measurementIndexEnd_.rank(); a++) {	// a: dimension index.
		measurementIndexEnd_[a]--;	// end index should be size-1 in all dimensions.
		measurementIndexStart_[a] = 0;	// start index should be 0 in all dimensions.
	}

	// \todo Add ProcessingFlag when acquiring...
	stateFlags_ = 0;

	// create connections to datastore:
	connect(dataStore_, SIGNAL(dataChanged(AMnDIndex,AMnDIndex,int)), this, SLOT(onDataChanged(AMnDIndex, AMnDIndex,int)) );
	connect(dataStore_, SIGNAL(sizeChanged()), this, SLOT(onScanAxisSizeChanged()));

	// raw data sources shouldn't be visible in plots, usually.  This is just a default; programmers can always call setVisibleInPlots(), or use the AMScan::addRawDataSource() version which calls this internally.
	visibleInPlots_ = false;
}


// This constructor re-loads a previously-stored source from the database.
AMRawDataSource::AMRawDataSource(AMDatabase* db, int id)
	: AMDbObject(0), AMDataSource("tempName")
{
	dataStore_ = 0;
	measurementId_ = 0;
	stateFlags_ = AMDataSource::InvalidFlag;

	// raw data sources shouldn't be visible in plots, usually.  This is just a default; programmers can always call setVisibleInPlots(), or use the AMScan::addRawDataSource() version which calls this internally.
	visibleInPlots_ = false;

	// restore the description(), rank(), measurementId(), scanRank(), and measurementRank() as stored in the database.  Our state() will remain Invalid until you call setDataStore() with a valid datastore that matches these dimensions.
	loadFromDb(db, id);
	AMDataSource::name_ = AMDbObject::name();

}

bool AMRawDataSource::setDataStore(const AMDataStore *dataStore) {
	if(dataStore_ == dataStore)
		return true;	// nothing to do.

	if(!isDataStoreCompatible(dataStore))
		return false;

	// ok, you're good.

	// existing data store? Disconnect signals.
	if(dataStore_) {
		disconnect(dataStore_, 0, this, 0);
	}

	AMMeasurementInfo measurementInfo = dataStore->measurementAt(measurementId_);

	dataStore_ = dataStore;
	axes_.clear();
	for(int i=0; i<scanAxesCount_; i++)
		axes_ << dataStore->scanAxisAt(i);	// adds scan axes
	axes_ << measurementInfo.axes;			// appends measurement axes

	measurementIndexEnd_ = measurementIndexStart_ = measurementInfo.size();
	for(int a=0; a<measurementIndexEnd_.rank(); a++) {	// a: dimension index.
		measurementIndexEnd_[a]--;	// end index should be size-1 in all dimensions.
		measurementIndexStart_[a] = 0;	// start index should be 0 in all dimensions.
	}

	// create connections to the new datastore:
	connect(dataStore_, SIGNAL(dataChanged(AMnDIndex,AMnDIndex,int)), SLOT(onDataChanged(AMnDIndex, AMnDIndex,int)) );
	connect(dataStore_, SIGNAL(sizeChanged()), this, SLOT(onScanAxisSizeChanged()));

	setValid();
	emitSizeChanged(-1);
	emitAxisInfoChanged(-1);
	return true;
}


// Forwarding signals from the data store:
//////////////////////
// Called when the data changes within the region described. We only care when \c measurementId matches our measurementId_
void AMRawDataSource::onDataChanged(const AMnDIndex& scanIndexStart, const AMnDIndex scanIndexEnd, int measurementId) {
	if(measurementId != measurementId_)
		return;	// doesn't concern us. None of our data changed

	AMnDIndex start = scanIndexStart;
	AMnDIndex end = scanIndexEnd;

	start.append(measurementIndexStart_);
	end.append(measurementIndexEnd_);

	emitValuesChanged(start, end);	/// \todo evaluate performance of creating these start/end indices on every valuesChanged. If no one uses the regions, would be faster not to bother.
}

// Called when the size of a scan axis changes.  \c axisId is the id of the changing axis, or -1 if they all did.
void AMRawDataSource::onScanAxisSizeChanged() {
	// the nice thing is that within our axes, the scan axes appear first, followed by the measurement axes. So an AMDataStore scan axis id is the same as one of our axis numbers.

	// Changed in May 2012: the first axis is the only axis of a data store that can change size:
	// removed:
//	if(axisId < 0) {	// all axes changed size?
//		for(int a=0; a<scanAxesCount_; a++)
//			axes_[a].size = dataStore_->scanSize(a);
//		emitSizeChanged(-1);
//	}
//	else if(axisId < scanAxesCount_) {	// just this axis changed size
//		axes_[axisId].size = dataStore_->scanSize(axisId);
//		emitSizeChanged(axisId);
//	}

	if(scanAxesCount_)
		axes_[0].size = dataStore_->scanSize(0);
	emitSizeChanged(0);
}

bool AMRawDataSource::isDataStoreCompatible(const AMDataStore *dataStore) const
{
	// verify that this new dataStore is suitable. It needs to match our old sizes, and measurementId_ must be a valid for it.
	if(!dataStore)
		return false;
	if(dataStore->scanRank() != scanAxesCount_)
		return false;
	if(measurementId_ >= dataStore->measurementCount())
		return false;
	if(dataStore->measurementAt(measurementId_).rank() != measurementAxesCount_)
		return false;

	return true;
}

#include "ui/dataman/AMSimpleDataSourceEditor.h"

QWidget* AMRawDataSource::createEditorWidget()
{
	return new AMSimpleDataSourceEditor(this);
}


