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

	// raw data sources shouldn't be visible in plots, usually.  This is just a default. Programmers can always call setVisibleInPlots(), or use the AMScan::addRawDataSource() version which calls this internally.
	visibleInPlots_ = false;
}


// This constructor re-loads a previously-stored source from the database.
AMRawDataSource::AMRawDataSource(const QString &outputName, QObject *parent)
	: AMDbObject(parent), AMDataSource(outputName)
{
	dataStore_ = 0;
	measurementId_ = 0;
	stateFlags_ = AMDataSource::InvalidFlag;

	// raw data sources shouldn't be visible in plots, usually. This is just a default. Programmers can always call setVisibleInPlots(), or use the AMScan::addRawDataSource() version which calls this internally.
	visibleInPlots_ = false;
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

	emitValuesChanged(start, end);
}

// Called when the size of a scan axis changes.  \c axisId is the id of the changing axis, or -1 if they all did.
void AMRawDataSource::onScanAxisSizeChanged() {
	// the nice thing is that within our axes, the scan axes appear first, followed by the measurement axes. So an AMDataStore scan axis id is the same as one of our axis numbers.

	QList<int> axesChanged;

	for (int i = 0; i < scanAxesCount_; i++){

		if (axes_.at(i).size != dataStore_->scanSize(i)){

			axes_[i].size = dataStore_->scanSize(i);
			axesChanged << i;
		}
	}

	for (int i = 0; i < measurementAxesCount_; i++){

		if (axes_.at(i+scanAxesCount_).size != measurementIndexEnd_.at(i)){

			axes_[i+scanAxesCount_].size = measurementIndexEnd_.at(i)+1;
			axesChanged << (i+scanAxesCount_);
		}
	}

	if (axesChanged.size() > 1)
		emitSizeChanged(-1);

	else
		emitSizeChanged(axesChanged.first());
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

bool AMRawDataSource::axisValues(int axisNumber, int startIndex, int endIndex, double *outputValues) const
{
	if (!isValid())
		return false;

	if (startIndex < 0 || startIndex >= size(axisNumber))
		return false;

	if (endIndex < 0 || endIndex >= size(axisNumber))
		return false;

	if (axisNumber < scanAxesCount_)
		return dataStore_->axisValues(axisNumber, startIndex, endIndex, outputValues);

	else{

		const AMAxisInfo& axis = axes_.at(axisNumber);

		for (int i = 0, size = endIndex-startIndex+1; i < size; i++)
            outputValues[i] = double(axis.start) + (startIndex+i)*double(axis.increment);
	}

	return true;
}

#include "ui/dataman/AMSimpleDataSourceEditor.h"

QWidget* AMRawDataSource::createEditorWidget()
{
	return new AMSimpleDataSourceEditor(this);
}

bool AMRawDataSource::loadFromDb(AMDatabase *db, int id)
{
	bool success = AMDbObject::loadFromDb(db, id);

	if(success)
		AMDataSource::name_ = AMDbObject::name();	/// \todo This might change the name of a data-source in mid-life, which is technically not allowed.

	return success;
}
