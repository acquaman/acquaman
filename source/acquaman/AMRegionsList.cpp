#include "AMRegionsList.h"

AMRegionsList::AMRegionsList(QObject *parent, bool setup) : QObject(parent){
	defaultControl_ = NULL;
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
	if(!regions_->insertRows(index, 1))
		return false;
	retVal = setStart(index, start) && setDelta(index, delta) && setEnd(index, end);
	if(retVal){
		if( (index == 0) && (count() != 1) ){
			regions_->setData(regions_->index(index, 5), true, Qt::EditRole);
			regions_->setData(regions_->index(index+1, 4), true, Qt::EditRole);
			connect(regions_->regions()->at(index), SIGNAL(endChanged(double)), regions_->regions()->at(index+1), SLOT(adjustStart(double)));
			connect(regions_->regions()->at(index+1), SIGNAL(startChanged(double)), regions_->regions()->at(index), SLOT(adjustEnd(double)));
			regions_->regions()->at(index+1)->setStart(end);
		}
		else if( (index == (count()-1)) && (count() != 1) ){
			regions_->setData(regions_->index(index, 4), true, Qt::EditRole);
			regions_->setData(regions_->index(index-1, 5), true, Qt::EditRole);
			connect(regions_->regions()->at(index), SIGNAL(startChanged(double)), regions_->regions()->at(index-1), SLOT(adjustEnd(double)));
			connect(regions_->regions()->at(index-1), SIGNAL(endChanged(double)), regions_->regions()->at(index), SLOT(adjustStart(double)));
			regions_->regions()->at(index-1)->setEnd(start);
		}
		else if(count() != 1){
			if( regions_->data(regions_->index(index-1, 5), Qt::DisplayRole).toBool()){
				regions_->setData(regions_->index(index+1, 4), false, Qt::EditRole);
				regions_->setData(regions_->index(index-1, 5), false, Qt::EditRole);
				disconnect(regions_->regions()->at(index-1), SIGNAL(endChanged(double)), regions_->regions()->at(index+1), SLOT(adjustStart(double)));
				disconnect(regions_->regions()->at(index+1), SIGNAL(startChanged(double)), regions_->regions()->at(index-1), SLOT(adjustEnd(double)));
			}
			regions_->setData(regions_->index(index, 5), true, Qt::EditRole);
			regions_->setData(regions_->index(index+1, 4), true, Qt::EditRole);
			connect(regions_->regions()->at(index), SIGNAL(endChanged(double)), regions_->regions()->at(index+1), SLOT(adjustStart(double)));
			connect(regions_->regions()->at(index+1), SIGNAL(startChanged(double)), regions_->regions()->at(index), SLOT(adjustEnd(double)));
			regions_->regions()->at(index+1)->setStart(end);
			regions_->setData(regions_->index(index, 4), true, Qt::EditRole);
			regions_->setData(regions_->index(index-1, 5), true, Qt::EditRole);
			connect(regions_->regions()->at(index), SIGNAL(startChanged(double)), regions_->regions()->at(index-1), SLOT(adjustEnd(double)));
			connect(regions_->regions()->at(index-1), SIGNAL(endChanged(double)), regions_->regions()->at(index), SLOT(adjustStart(double)));
			regions_->regions()->at(index-1)->setEnd(start);
		}
	}
	if(!retVal)
		regions_->removeRows(index, 1);
	return retVal;
}

bool AMRegionsList::addRegionSqueeze(size_t index){
	double nextStart, nextEnd, prevStart, prevEnd;
	double sensibleStart = 200;
	double sensibleEnd = 2000;
	if(count() == 0){
		return addRegion(index, sensibleStart, 1, sensibleEnd);
	}
	else if(index == 0){
		nextStart = start(index);
		return addRegion(index, sensibleStart, 1, nextStart);
	}
	else if(index == count()){
		prevEnd = end(index-1);
		return addRegion(index, prevEnd, 1, sensibleEnd);
	}
	else{
		prevStart = start(index-1);
		prevEnd = end(index-1);
		nextStart = start(index);
		nextEnd = end(index);
		return addRegion(index, prevStart+(prevEnd-prevStart)/2, 1, nextStart+(nextEnd-nextStart)/2 );
	}
}

bool AMRegionsList::deleteRegion(size_t index){
	if(count() == 0)
		return false;
	else if( (index == 0) && (count() != 1) ){
		regions_->setData(regions_->index(index+1, 4), false, Qt::EditRole);
		disconnect(regions_->regions()->at(index), SIGNAL(endChanged(double)), regions_->regions()->at(index+1), SLOT(adjustStart(double)));
		disconnect(regions_->regions()->at(index+1), SIGNAL(startChanged(double)), regions_->regions()->at(index), SLOT(adjustEnd(double)));
//		regions_->regions()->at(index+1)->setStart(end);
	}
	else if( (index == (count()-1)) && (count() != 1) ){
		regions_->setData(regions_->index(index-1, 5), false, Qt::EditRole);
		disconnect(regions_->regions()->at(index), SIGNAL(startChanged(double)), regions_->regions()->at(index-1), SLOT(adjustEnd(double)));
		disconnect(regions_->regions()->at(index-1), SIGNAL(endChanged(double)), regions_->regions()->at(index), SLOT(adjustStart(double)));
//		regions_->regions()->at(index-1)->setEnd(start);
	}
	else if(count() != 1){
		if( regions_->data(regions_->index(index-1, 5), Qt::DisplayRole).toBool()){
			regions_->setData(regions_->index(index+1, 4), false, Qt::EditRole);
			regions_->setData(regions_->index(index-1, 5), false, Qt::EditRole);
			disconnect(regions_->regions()->at(index), SIGNAL(endChanged(double)), regions_->regions()->at(index+1), SLOT(adjustStart(double)));
			disconnect(regions_->regions()->at(index+1), SIGNAL(startChanged(double)), regions_->regions()->at(index), SLOT(adjustEnd(double)));
			disconnect(regions_->regions()->at(index), SIGNAL(startChanged(double)), regions_->regions()->at(index-1), SLOT(adjustEnd(double)));
			disconnect(regions_->regions()->at(index-1), SIGNAL(endChanged(double)), regions_->regions()->at(index), SLOT(adjustStart(double)));
		}
		regions_->setData(regions_->index(index-1, 5), true, Qt::EditRole);
		regions_->setData(regions_->index(index+1, 4), true, Qt::EditRole);
		connect(regions_->regions()->at(index-1), SIGNAL(endChanged(double)), regions_->regions()->at(index+1), SLOT(adjustStart(double)));
		connect(regions_->regions()->at(index+1), SIGNAL(startChanged(double)), regions_->regions()->at(index-1), SLOT(adjustEnd(double)));
	}
	return regions_->removeRows(index, 1);
}

bool AMRegionsList::deleteRegionSqueeze(size_t index){
	bool retVal = deleteRegion(index);
	if(retVal){
		if( (index != 0) && (index != count()) ){
			double prevEnd, nextStart;
			prevEnd = end(index-1);
			nextStart = start(index);
			setEnd(index-1, prevEnd+(nextStart-prevEnd)/2);
		}
	}
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
