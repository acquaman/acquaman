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


#include "AMRegion.h"

// AMRegion
/////////////////////////////////////////////////////////

AMRegion::AMRegion(QObject *parent) :
	QObject(parent)
{
	elasticStart_ = false;
	elasticEnd_ = false;
	initiatedStartAdjust_ = false;
	initiatedEndAdjust_ = false;
}

// Sets the start value from the double passed in. Makes sure the energy is within the allowable range, otherwise returns false.
bool AMRegion::setStart(double start) {
	if(ctrl_->valueOutOfRange(start))
		return false;
	start_ = start;
	if(elasticStart_){
		initiatedStartAdjust_ = true;
		emit startChanged(start_);
	}
	return true;
}

// Sets the end value from the double passed in. Makes sure the energy is within the allowable range, otherwise returns false.
bool AMRegion::setEnd(double end) {
	if(ctrl_->valueOutOfRange(end))
		return false;
	end_ = end;
	if(elasticEnd_){
		initiatedEndAdjust_ = true;
		emit endChanged(end_);
	}
	return true;
}

// Sets the delta value from the double passed in. Makes sure the value is non-zero, otherwise it returns false.
bool AMRegion::setDelta(double delta) {
	if(delta == 0)
		return false;
	delta_ = delta;
	return true;
}

bool AMRegion::setTime(double time)
{
	if (time < 0)
		return false;

	time_ = time;

	return true;
}

bool AMRegion::adjustStart(double start){
	if(initiatedStartAdjust_){
		initiatedStartAdjust_ = false;
	}
	else{
		setStart(start);
		initiatedStartAdjust_= false;
	}
	return true;
}

bool AMRegion::adjustEnd(double end){
	if(initiatedEndAdjust_){
		initiatedEndAdjust_ = false;
	}
	else{
		setEnd(end);
		initiatedEndAdjust_ = false;
	}
	return true;
}

bool AMRegion::isValid() const
{
	if (start_ < end_ && delta_ > 0)
		return true;
	else if (start_ > end_ && delta_ < 0)
		return true;

	return false;
}
// AMRegionsListModel
/////////////////////////////////////////////////////////////////////////////

AMRegionsListModel::AMRegionsListModel(QObject *parent)
	: QAbstractTableModel(parent)
{
	regions_ = new QList<AMRegion*>();
	defaultControl_ = NULL;
	defaultTimeControl_ = NULL;
}

QVariant AMRegionsListModel::data(const QModelIndex &index, int role) const{
	// Invalid index:
	if(!index.isValid())
		return QVariant();

	// If handling the alignment.
	if (role == Qt::TextAlignmentRole)
		return Qt::AlignCenter;

	// If handling the background color.
	if (role == Qt::BackgroundRole && regions_->at(index.row())->isValid())
		return Qt::white;
	else if (role == Qt::BackgroundRole && !regions_->at(index.row())->isValid())
		return Qt::red;

	// We only answer to Qt::DisplayRole right now
	if(role != Qt::DisplayRole)
		return QVariant();

	// Out of range: (Just checking for too big.  isValid() checked for < 0)
	if(index.row() >= regions_->count())
		return QVariant();

	QVariant dataVal = QVariant();

	switch(index.column()){

	case 0: // The control.
		break; // Doing nothing.
	case 1: // The start value.
		dataVal = regions_->at(index.row())->start();
		break;
	case 2: // The delta value.
		dataVal = regions_->at(index.row())->delta();
		break;
	case 3: // The end value.
		dataVal = regions_->at(index.row())->end();
		break;
	case 4: // The state of whether the region has an elastic start value.
		dataVal = regions_->at(index.row())->elasticStart();
		break;
	case 5: // The state of whether the region has an elastic end value.
		dataVal = regions_->at(index.row())->elasticEnd();
		break;
	case 6: // The time control.
		break; // Doing nothing.
	case 7: // The time value.
		dataVal = regions_->at(index.row())->time();
		break;
	default:
		break; // Return null if not a specific case.
	}

	return dataVal;
}

// Retrieves the header data for a column or row and returns as a QVariant. Only valid role is Qt::DisplayRole right now.
QVariant AMRegionsListModel::headerData(int section, Qt::Orientation orientation, int role) const{

	if (role != Qt::DisplayRole)
		return QVariant();

	// Vertical headers:
	if(orientation == Qt::Vertical) {
		return section;
	}

	// Horizontal Headers: (Column labels)
	QVariant header = QVariant();

	switch(section){

	case 0:
		header = "Control";
		break;
	case 1:
		header = "Start";
		break;
	case 2:
		header = "Delta";
		break;
	case 3:
		header = "End";
		break;
	case 4:
		header = "Elastic Start";
		break;
	case 5:
		header = "Elastic End";
		break;
	case 6:
		header = "Time Control";
		break;
	case 7:
		header = "Time";
		break;
	}

	return header;
}

// Sets the data value at an index (row and column). Only valid role is Qt::DisplayRole right now.
bool AMRegionsListModel::setData(const QModelIndex &index, const QVariant &value, int role){

	if (index.isValid() && index.row() < regions_->count() && role == Qt::EditRole) {

		bool conversionOK = false;
		bool retVal;
		double dval;
		bool bval;

		if(index.column() == 1 || index.column() == 2 || index.column() == 3 || index.column() == 7)
			dval  = value.toDouble(&conversionOK);
		else if(index.column() == 4 || index.column() == 5){
			bval = value.toBool();
			conversionOK = true;
		}

		// Check if any data is invalid.
		if(!conversionOK)
			return false;

		switch(index.column()){

		case 0: // Setting a control?
			retVal = false; // Doing nothing right now.
			break;

		case 1: // Setting a start value?
			retVal = regions_->at(index.row())->setStart(dval);
			break;

		case 2: // Setting a delta value?
			retVal = regions_->at(index.row())->setDelta(dval);
			break;

		case 3: // Setting an end value?
			retVal = regions_->at(index.row())->setEnd(dval);
			break;

		case 4: // Setting the start elasticity?
			retVal = regions_->at(index.row())->setElasticStart(bval);
			break;

		case 5: // Setting the end elasticity?
			retVal = regions_->at(index.row())->setElasticEnd(bval);
			break;

		case 6: // Setting the time time control?
			retVal = false; // Doing nothing right now.
			break;

		case 7: // Setting a time value?
			retVal = regions_->at(index.row())->setTime(dval);
			break;

		default: // Not a valid index.
			retVal = false;
			break;
		}

		// If something actually changed, we need to notify others.
		if (retVal)
			emit dataChanged(index, index);

		return retVal;
	}

	return false;	// no value set
}

bool AMRegionsListModel::insertRows(int position, int rows, const QModelIndex &index){

	if (index.row() <= regions_->count() && position <= regions_->count() && defaultControl_ && defaultTimeControl_) {

		beginInsertRows(QModelIndex(), position, position+rows-1);

		AMRegion *tmpRegion;

		for (int row = 0; row < rows; ++row) {

			tmpRegion = new AMRegion(this);
			tmpRegion->setControl(defaultControl_);
			tmpRegion->setTimeControl(defaultTimeControl_);
			regions_->insert(position, tmpRegion); // Order doesn't matter because they are all identical, empty regions.
		}

		endInsertRows();
		return true;
	}

	return false;
}

bool AMRegionsListModel::removeRows(int position, int rows, const QModelIndex &index){

	if (index.row() < regions_->count() && position < regions_->count()) {

		beginRemoveRows(QModelIndex(), position, position+rows-1);

		for (int row = 0; row < rows; ++row) {
			delete regions_->takeAt(position);
		}

		endRemoveRows();
		return true;
	}

	return false;
}

// This allows editing of values within range (for ex: in a QTableView)
Qt::ItemFlags AMRegionsListModel::flags(const QModelIndex &index) const{

	Qt::ItemFlags flags = Qt::NoItemFlags;

	if (index.isValid() && index.row() < regions_->count()
			&& index.column() != 0 && index.column() != 4 && index.column() != 5 && index.column() != 6
			&& index.column() < 8)
		flags = Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;

	return flags;
}

// AMXASRegionsListModel
////////////////////////////////////////////////////////

bool AMXASRegionsListModel::insertRows(int position, int rows, const QModelIndex &index){

	if (index.row() <= regions_->count() && position <= regions_->count() && defaultControl_ && defaultTimeControl_) {

		beginInsertRows(QModelIndex(), position, position+rows-1);

		AMXASRegion *tmpRegion;

		for (int row = 0; row < rows; ++row) {

			tmpRegion = new AMXASRegion(defaultControl_, this);
			regions_->insert(position, tmpRegion); // Order doesn't matter because they are all identical, empty regions.
		}

		endInsertRows();
		return true;
	}

	return false;
}

// AMEXAFSRegion
///////////////////////////////////////////////////

bool AMEXAFSRegion::setType(RegionType type)
{
	if (type == Energy || type == kSpace){

		type_ = type;
		return true;
	}

	return false;
}
