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


#include "AMRegionsList.h"

// AMRegionsList
///////////////////////////////////////////////////////////

 AMRegionsList::~AMRegionsList(){}
AMRegionsList::AMRegionsList(QObject *parent, bool setup)
	: QObject(parent)
{
	defaultControl_ = NULL;
	regions_ = NULL;
	defaultUnits_ = "";
	defaultTimeUnits_ = "";

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

	if(retVal.isValid()){

		QString val = retVal.toString();
		val.chop(units(index).size());
		return val.toDouble();
	}

	return -1;
}

// Returns the delta value of the region refered to by index. If an invalid index is given, returns 0 (not a valid delta value).
double AMRegionsList::delta(int index) const
{
	QVariant retVal = regions_->data(regions_->index(index, 2), Qt::DisplayRole);

	if(retVal.isValid()){

		QString val = retVal.toString();
		val.chop(units(index).size());
		return val.toDouble();
	}

	return 0;
}

// Returns the end value of the region refered to by index. If an invalid index is given, returns -1 (not a valid energy value).
double AMRegionsList::end(int index) const
{
	QVariant retVal = regions_->data(regions_->index(index, 3), Qt::DisplayRole);

	if(retVal.isValid()){

		QString val = retVal.toString();
		val.chop(units(index).size());
		return val.toDouble();
	}

	return -1;
}

bool AMRegionsList::elasticStart(int index) const
{
	QVariant retVal = regions_->data(regions_->index(index, 4), Qt::DisplayRole);

	if(retVal.isValid())
		return retVal.toBool();

	return false;
}

bool AMRegionsList::elasticEnd(int index) const
{
	QVariant retVal = regions_->data(regions_->index(index, 5), Qt::DisplayRole);

	if(retVal.isValid())
		return retVal.toBool();

	return false;
}

double AMRegionsList::time(int index) const
{
	QVariant retVal = regions_->data(regions_->index(index, 7), Qt::DisplayRole);

	if(retVal.isValid()){

		QString val = retVal.toString();
		val.chop(timeUnits(index).size());
		return val.toDouble();
	}
	return -1;
}

QString AMRegionsList::units(int index) const
{
	if(!regions_ || !regions_->regions())
		return QString();

	if (index < regions_->regions()->size() && regions_->regions()->at(index))
		return regions_->regions()->at(index)->units();

	return QString();
}

QString AMRegionsList::timeUnits(int index) const
{
	if (index < regions_->regions()->size())
		return regions_->regions()->at(index)->timeUnits();

	return QString();
}

bool AMRegionsList::isValid() const
{
	for (int i = 0; i < regions_->regions()->size(); i++)
		if (!isValid(i))
			return false;

	return true;
}

bool AMRegionsList::addRegion(int index, double start, double delta, double end, double time)
{
//	if(!defaultControl_ || !defaultTimeControl_)
//		return false;

	bool retVal;

	if(!regions_->insertRows(index, 1))
		return false;

	retVal = setStart(index, start) && setDelta(index, delta) && setEnd(index, end) && setTime(index, time) && setUnits(index, defaultUnits_) && setTimeUnits(index, defaultTimeUnits_);

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

bool AMRegionsList::addRegion(int index, double start, double delta, double end)
{
	// Default the time at 1 second if no regions exist currently.
	if (count() == 0)
		return addRegion(index, start, delta, end, 1);
	// If appending to the first region, look at the old front for the time value.
	else if (index == 0 && count() != 0)
		return addRegion(index, start, delta, end, time(index));
	// If adding to any other part of the list, use the previous regions time value.
	else
		return addRegion(index, start, delta, end, time(index - 1));
}

bool AMRegionsList::addRegionSqueeze(int index){

	double nextStart, nextEnd, prevStart, prevEnd;
	double sensibleStart = sensibleStart_;
	double sensibleEnd = sensibleEnd_;

	if(count() == 0)
		return addRegion(index, sensibleStart, 1, sensibleEnd);

	else if(index == 0){

		nextStart = start(index);

		if (sensibleStart == nextStart)
			return addRegion(index, sensibleStart - 10, 1, nextStart);
		else if (nextStart < sensibleStart)
			return addRegion(index, nextStart - 10, 1, nextStart);
		else
			return addRegion(index, sensibleStart, 1, nextStart);
	}

	else if(index == count()){

		prevEnd = end(index-1);

		if (sensibleEnd == prevEnd)
			return addRegion(index, prevEnd, 1, sensibleEnd + 10);
		else if (prevEnd > sensibleEnd)
			return addRegion(index, prevEnd, 1, prevEnd + 10);
		else
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

		emit regionsChanged();
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

// AMEXAFSRegionsList
///////////////////////////////////////////

bool AMEXAFSRegionsList::setupModel()
{
	regions_ = new AMEXAFSRegionsListModel(this);

	if (regions_){

		connect(regions_, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onDataChanged(QModelIndex,QModelIndex)));
		connect(regions_, SIGNAL(typeChanged()), this, SLOT(onTypeChanged()));
		return true;
	}

	return false;
}

AMEXAFSRegion::RegionType AMEXAFSRegionsList::type(int index) const
{
	return exafsRegion(index)->type();
}

double AMEXAFSRegionsList::edgeEnergy(int index) const
{
	QVariant retVal = regions_->data(regions_->index(index, 9), Qt::DisplayRole);

	if(retVal.isValid()){

		QString val = retVal.toString();
		val.chop(units(index).size());
		return val.toDouble();
	}

	return -1;
}

bool AMEXAFSRegionsList::addRegionSqueeze(int index){

	double nextStart, nextEnd, prevStart, prevEnd;
	double sensibleStart = sensibleStart_;
	double sensibleEnd = sensibleEnd_;

	if(count() == 0)
		return addRegion(index, sensibleStart, 1, sensibleEnd);

	else if(index == 0){

		nextStart = startByType(index, AMEXAFSRegion::Energy);

		if (sensibleStart == nextStart)
			return addRegion(index, sensibleStart - 10, 1, nextStart);
		else if (nextStart < sensibleStart)
			return addRegion(index, nextStart - 10, 1, nextStart);
		else
			return addRegion(index, sensibleStart, 1, nextStart);
	}

	else if(index == count() && type(index-1) == AMEXAFSRegion::Energy){

		prevEnd = endByType(index-1, AMEXAFSRegion::Energy);

		if (sensibleEnd == prevEnd)
			return addRegion(index, prevEnd, 1, sensibleEnd + 10);
		else if (prevEnd > sensibleEnd)
			return addRegion(index, prevEnd, 1, prevEnd + 10);
		else
			return addRegion(index, prevEnd, 1, sensibleEnd);
	}

	else if(index == count() && type(index-1) == AMEXAFSRegion::kSpace){

		prevEnd = endByType(index-1, AMEXAFSRegion::kSpace);

		// Sensible end for EXAFS is 10k.
		if (prevEnd == 10)
			return addRegion(index, prevEnd, 1, 15);
		else if (prevEnd > 10)
			return addRegion(index, prevEnd, 1, prevEnd + 5);
		else
			return addRegion(index, prevEnd, 1, 10);
	}

	else{

		prevStart = startByType(index-1, AMEXAFSRegion::Energy);
		prevEnd = endByType(index-1, AMEXAFSRegion::Energy);
		nextStart = startByType(index, AMEXAFSRegion::Energy);
		nextEnd = endByType(index, AMEXAFSRegion::Energy);
		return addRegion(index, prevStart+(prevEnd-prevStart)/2, 1, nextStart+(nextEnd-nextStart)/2 );
	}
}

bool AMEXAFSRegionsList::deleteRegionSqueeze(int index){

	bool retVal = deleteRegion(index);

	if(retVal){

		if( (index != 0) && (index != count()) ){

			double prevEnd, nextStart;
			prevEnd = endByType(index-1, AMEXAFSRegion::Energy);
			nextStart = startByType(index, AMEXAFSRegion::Energy);
			setEnd(index-1, prevEnd+(nextStart-prevEnd)/2);
		}

		emit regionsChanged();
	}

	return retVal;
}

double AMRegionsList::totalAcquisitionTime() const
{
	double rv = 0;
	for(int i=0, cc=count(); i<cc; ++i) {
		AMRegion* r = region(i);
		rv += (int((r->end() - r->start())/r->delta())+1)*r->time();
	}
	return rv;
}
 AMXASRegionsList::~AMXASRegionsList(){}
 AMEXAFSRegionsList::~AMEXAFSRegionsList(){}
