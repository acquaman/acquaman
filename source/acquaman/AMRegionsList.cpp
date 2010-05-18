#include "AMRegionsList.h"

AMRegionsList::AMRegionsList(QObject *parent, bool setup) : QObject(parent){
	defaultControl_ = NULL;
//	regions_ = new AMRegionsListModel(this);
	regions_ = NULL;
	if(setup)
		setupModel();
}

/// Returns the start value of the region refered to by index. If an invalid index is given, returns -1 (not a valid energy value).
double AMRegionsList::start(size_t index) const{
	QVariant retVal = regions_->data(regions_->index(index, 1), Qt::DisplayRole);
	if(retVal.isValid())
		return retVal.toDouble();
	return -1;
}
/// Returns the delta value of the region refered to by index. If an invalid index is given, returns 0 (not a valid delta value).
double AMRegionsList::delta(size_t index) const{
	QVariant retVal = regions_->data(regions_->index(index, 2), Qt::DisplayRole);
	if(retVal.isValid())
		return retVal.toDouble();
	return 0;
}
/// Returns the end value of the region refered to by index. If an invalid index is given, returns -1 (not a valid energy value).
double AMRegionsList::end(size_t index) const{
	QVariant retVal = regions_->data(regions_->index(index, 3), Qt::DisplayRole);
	if(retVal.isValid())
		return retVal.toDouble();
	return -1;
}

bool AMRegionsList::addRegion(size_t index, double start, double delta, double end){
	if(!defaultControl_)
		return false;
	bool retVal;
	regions_->insertRows(index, 1);
	retVal = setStart(index, start) && setDelta(index, delta) && setEnd(index, end);
	if(!retVal)
		regions_->removeRows(index, 1);
	return retVal;
}

bool AMRegionsList::setupModel(){
	regions_ = new AMRegionsListModel(this);
	if(regions_)
		return true;
	return false;
}

bool AMXASRegionsList::setupModel(){
	regions_ = new AMXASRegionsListModel(this);
	if(regions_)
		return true;
	return false;
}
