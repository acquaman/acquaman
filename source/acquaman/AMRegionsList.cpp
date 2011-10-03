/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "AMRegionsList.h"

// AMRegionsList
///////////////////////////////////////////////////////////

AMRegionsList::AMRegionsList(QObject *parent, bool setup)
	: QObject(parent)
{
	defaultControl_ = NULL;
	regions_ = NULL;

	// Setting the sensible start and end values to the SGM values to preserve existing code.
	sensibleStart_ = 200;
	sensibleEnd_ = 2000;

	if(setup)
		setupModel();
}

// Returns the start value of the region refered to by index. If an invalid index is given, returns -1 (not a valid energy value).
double AMRegionsList::start(int index) const
{
	QVariant retVal = regions_->data(regions_->index(index, 1), Qt::DisplayRole);

	if(retVal.isValid())
		return retVal.toDouble();

	return -1;
}

// Returns the delta value of the region refered to by index. If an invalid index is given, returns 0 (not a valid delta value).
double AMRegionsList::delta(int index) const
{
	QVariant retVal = regions_->data(regions_->index(index, 2), Qt::DisplayRole);

	if(retVal.isValid())
		return retVal.toDouble();

	return 0;
}

// Returns the end value of the region refered to by index. If an invalid index is given, returns -1 (not a valid energy value).
double AMRegionsList::end(int index) const
{
	QVariant retVal = regions_->data(regions_->index(index, 3), Qt::DisplayRole);

	if(retVal.isValid())
		return retVal.toDouble();

	return -1;
}

double AMRegionsList::time(int index) const
{
	QVariant retVal = regions_->data(regions_->index(index, 7), Qt::DisplayRole);

	if(retVal.isValid())
		return retVal.toDouble();

	return -1;
}

bool AMRegionsList::addRegion(int index, double start, double delta, double end, double time)
{
	if(!defaultControl_ || !defaultTimeControl_)
		return false;

	bool retVal;

	if(!regions_->insertRows(index, 1))
		return false;

	retVal = setStart(index, start) && setDelta(index, delta) && setEnd(index, end) && setTime(index, time);

	if(retVal){

		if( (index == 0) && (count() != 1) ){

			regions_->setData(regions_->index(index, 5), true, Qt::EditRole);
			regions_->setData(regions_->index(index+1, 4), true, Qt::EditRole);
			connect(regions_->regions()->at(index), SIGNAL(endChanged(double)), regions_->regions()->at(index+1), SLOT(adjustStart(double)));
			connect(regions_->regions()->at(index+1), SIGNAL(startChanged(double)), regions_->regions()->at(index), SLOT(adjustEnd(double)));
			regions_->regions()->at(index+1)->setStart(end);
		}

		else if( (index == count()-1) && (count() != 1) ){

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

	// If we couldn't setup the region, take it out.
	else
		regions_->removeRows(index, 1);

	return retVal;
}

bool AMRegionsList::addRegionSqueeze(int index){

	double nextStart, nextEnd, prevStart, prevEnd;
	double sensibleStart = sensibleStart_;
	double sensibleEnd = sensibleEnd_;

	if(count() == 0)
		return addRegion(index, sensibleStart, 1, sensibleEnd);

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

bool AMRegionsList::deleteRegion(int index){

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

bool AMRegionsList::deleteRegionSqueeze(int index){

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

	if(regions_){

		connect(regions_, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onDataChanged(QModelIndex,QModelIndex)) );
		return true;
	}

	return false;
}

// AMXASRegionsList
//////////////////////////////////////////

bool AMXASRegionsList::setupModel(){

	regions_ = new AMXASRegionsListModel(this);
	if(regions_){

		connect(regions_, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onDataChanged(QModelIndex,QModelIndex)) );
		return true;
	}

	return false;
}
