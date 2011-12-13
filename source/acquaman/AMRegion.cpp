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
	start_ = 0;
	delta_ = 0;
	end_ = 0;
	time_ = 0;
	units_ = "";
	timeUnits_ = "";
	ctrl_ = 0;
	timeControl_ = 0;
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

bool AMRegion::setUnits(const QString &units)
{
	if (units.isNull())
		return false;

	units_ = units;
	return true;
}

bool AMRegion::setTimeUnits(const QString &units)
{
	if (units.isNull())
		return false;

	timeUnits_ = units;
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
	defaultUnits_ = "";
	defaultTimeUnits_ = "";
}

QVariant AMRegionsListModel::data(const QModelIndex &index, int role) const{
	// Invalid index:
	if(!index.isValid())
		return QVariant();

	// If handling the alignment.
	if (role == Qt::TextAlignmentRole)
		return Qt::AlignCenter;

	// If handling the background color.
	if (role == Qt::BackgroundRole)
		return regions_->at(index.row())->isValid() ? Qt::white : Qt::red;

	// We only answer to Qt::DisplayRole right now
	if(role != Qt::DisplayRole)
		return QVariant();

	// Out of range: (Just checking for too big.  isValid() checked for < 0)
	if(index.row() >= regions_->count())
		return QVariant();

	QVariant dataVal = QVariant();
	AMRegion *region = regions_->at(index.row());

	switch(index.column()){

	case 0: // The control.
		break; // Doing nothing.
	case 1: // The start value.
		dataVal = QString::number(region->start()) + region->units();
		break;
	case 2: // The delta value.
		dataVal = QString::number(region->delta()) + region->units();
		break;
	case 3: // The end value.
		dataVal = QString::number(region->end()) + region->units();
		break;
	case 4: // The state of whether the region has an elastic start value.
		dataVal = region->elasticStart();
		break;
	case 5: // The state of whether the region has an elastic end value.
		dataVal = region->elasticEnd();
		break;
	case 6: // The time control.
		break; // Doing nothing.
	case 7: // The time value.
		dataVal = QString::number(region->time()) + region->timeUnits();
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

		AMRegion *region = regions_->at(index.row());

		if(index.column() == 1 || index.column() == 2 || index.column() == 3){

			QString energy = value.toString();

			if (energy.contains(region->units()))
				energy.chop(region->units().size());

			dval = energy.toDouble(&conversionOK);
		}
		else if(index.column() == 4 || index.column() == 5){
			bval = value.toBool();
			conversionOK = true;
		}
		else if (index.column() == 7){

			QString time = value.toString();

			if (time.contains(region->timeUnits()))
				time.chop(region->timeUnits().size());

			dval = time.toDouble(&conversionOK);
		}

		// Check if any data is invalid.
		if(!conversionOK)
			return false;

		switch(index.column()){

		case 0: // Setting a control?
			retVal = false; // Doing nothing right now.
			break;

		case 1: // Setting a start value?
			retVal = region->setStart(dval);
			break;

		case 2: // Setting a delta value?
			retVal = region->setDelta(dval);
			break;

		case 3: // Setting an end value?
			retVal = region->setEnd(dval);
			break;

		case 4: // Setting the start elasticity?
			retVal = region->setElasticStart(bval);
			break;

		case 5: // Setting the end elasticity?
			retVal = region->setElasticEnd(bval);
			break;

		case 6: // Setting the time time control?
			retVal = false; // Doing nothing right now.
			break;

		case 7: // Setting a time value?
			retVal = region->setTime(dval);
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
			tmpRegion->setUnits(defaultUnits_);
			tmpRegion->setTimeUnits(defaultTimeUnits_);
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
			tmpRegion->setTimeControl(defaultTimeControl_);
			tmpRegion->setUnits(defaultUnits_);
			tmpRegion->setTimeUnits(defaultTimeUnits_);
			regions_->insert(position, tmpRegion); // Order doesn't matter because they are all identical, empty regions.
		}

		endInsertRows();
		return true;
	}

	return false;
}

// AMEXAFSRegion
///////////////////////////////////////////////////

bool AMEXAFSRegion::setType(AMEXAFSRegion::RegionType type)
{
	if (type == Energy || type == kSpace){

		type_ = type;
		return true;
	}

	return false;
}

// Sets the start value from the double passed in. Assumes the value passed in is in the space of the region.  Makes sure the energy is within the allowable range, otherwise returns false.
bool AMEXAFSRegion::setStart(double start) {
	if(control()->valueOutOfRange(start))
		return false;

	// k values can't be negative.
	if (type_ == kSpace && start < 0)
		return false;

	start_ = (type_ == Energy) ? start : toEnergy(start);

	if(elasticStart_){
		initiatedStartAdjust_ = true;
		emit startChanged(start_);
	}

	return true;
}

// Sets the end value from the double passed in. Assumes the value passed in is in the space of the region.  Makes sure the energy is within the allowable range, otherwise returns false.
bool AMEXAFSRegion::setEnd(double end) {
	if(control()->valueOutOfRange(end))
		return false;

	// k values can't be negative.
	if (type_ == kSpace && end < 0)
		return false;

	end_ = (type_ == Energy) ? end : toEnergy(end);

	if(elasticEnd_){
		initiatedEndAdjust_ = true;
		emit endChanged(end_);
	}

	return true;
}

// Sets the start value from the double and the method assumes that the value is in the space of the type passed in it.  For example, if you choose Energy, it will assume it is a value in eV.
bool AMEXAFSRegion::setStartByType(double start, RegionType type)
{
	if(control()->valueOutOfRange(start))
		return false;

	// k values can't be negative.
	if (type == kSpace && start < 0)
		return false;

	start_ = (type == Energy) ? start : toEnergy(start);

	if(elasticStart_){
		initiatedStartAdjust_ = true;
		emit startChanged(start_);
	}

	return true;
}

// Sets the end value from the double and the method assumes that the value is in the space of the type passed in it.  For example, if you choose Energy, it will assume it is a value in eV.
bool AMEXAFSRegion::setEndByType(double end, RegionType type)
{
	if(control()->valueOutOfRange(end))
		return false;

	// k values can't be negative.
	if (type == kSpace && end < 0)
		return false;

	end_ = (type == Energy) ? end : toEnergy(end);

	if(elasticEnd_){
		initiatedEndAdjust_ = true;
		emit endChanged(end_);
	}

	return true;
}

double AMEXAFSRegion::toKSpace(double energy) const
{
	// Energy must be greater than the reference energy.
	if (energy < edgeEnergy_)
		return -1;

	// k = sqrt((E - E0)/a) ; a = 3.810 945 497 eV * Angstrom
	return sqrt((energy-edgeEnergy_)/3.810945497);
}

double AMEXAFSRegion::toEnergy(double kSpace) const
{
	// kSpace must be greater than 0.
	if (kSpace < 0)
		return -1;

	// E = E0 + a*k^2 ; a = 3.810 945 497 eV * Angstrom
	return edgeEnergy_ + 3.810945497*kSpace*kSpace;
}

// AMEXAFSRegionsListModel
////////////////////////////////////////////////////////

bool AMEXAFSRegionsListModel::insertRows(int position, int rows, const QModelIndex &index)
{
	if (index.row() <= regions_->count() && position <= regions_->count()
			&& defaultControl_ && defaultKControl_ && defaultTimeControl_) {

		beginInsertRows(QModelIndex(), position, position+rows-1);

		AMEXAFSRegion *tmpRegion;

		// Only need to check for kSpace because the region defaults to Energy.
		// New regions should only be preset as kSpace regions if they are being appended after a kSpace region or if the only regions are kSpace regions.
		if ((index.row() != 0 && ((AMEXAFSRegion *)regions_->at(index.row()))->type() == AMEXAFSRegion::kSpace && position > index.row())
				|| (((AMEXAFSRegion *)regions_->at(0))->type() == AMEXAFSRegion::kSpace)){

			for (int row = 0; row < rows; ++row) {

				tmpRegion = new AMEXAFSRegion(defaultControl_, defaultKControl_, this);
				tmpRegion->setTimeControl(defaultTimeControl_);
				tmpRegion->setUnits(defaultUnits_);
				tmpRegion->setTimeUnits(defaultTimeUnits_);
				tmpRegion->setType(AMEXAFSRegion::kSpace);
				tmpRegion->setEdgeEnergy(defaultEdgeEnergy_);
				regions_->insert(position, tmpRegion); // Order doesn't matter because they are all identical, empty regions.
			}
		}
		else {

			for (int row = 0; row < rows; ++row) {

				tmpRegion = new AMEXAFSRegion(defaultControl_, defaultKControl_, this);
				tmpRegion->setTimeControl(defaultTimeControl_);
				tmpRegion->setUnits(defaultUnits_);
				tmpRegion->setTimeUnits(defaultTimeUnits_);
				tmpRegion->setType(AMEXAFSRegion::Energy);
				tmpRegion->setEdgeEnergy(defaultEdgeEnergy_);
				regions_->insert(position, tmpRegion); // Order doesn't matter because they are all identical, empty regions.
			}
		}

		endInsertRows();
		return true;
	}

	return false;
}

// Sets the data value at an index (row and column). Only valid role is Qt::DisplayRole right now.
bool AMEXAFSRegionsListModel::setData(const QModelIndex &index, const QVariant &value, int role){

	if (index.isValid() && index.row() < regions_->count() && role == Qt::EditRole) {

		bool conversionOK = false;
		bool retVal;
		double dval;
		bool bval;

		AMEXAFSRegion *region = qobject_cast<AMEXAFSRegion *>(regions_->at(index.row()));

		if(index.column() == 1 || index.column() == 2 || index.column() == 3){

			QString energy = value.toString();

			if (energy.contains(region->units()))
				energy.chop(region->units().size());

			dval = energy.toDouble(&conversionOK);
		}
		else if(index.column() == 4 || index.column() == 5){
			bval = value.toBool();
			conversionOK = true;
		}
		else if (index.column() == 7){

			QString time = value.toString();

			if (time.contains(region->timeUnits()))
				time.chop(region->timeUnits().size());

			dval = time.toDouble(&conversionOK);
		}

		// Check if any data is invalid.
		if(!conversionOK)
			return false;

		QString energy;

		// Need to be using an AMEXAFSRegion.
		if (!region)
			return false;

		switch(index.column()){

		case 0: // Setting a control?
			retVal = false; // Doing nothing right now.
			break;

		case 1: // Setting a start value?
			energy = value.toString();

			if (energy.contains("k") && region->type() == AMEXAFSRegion::Energy)
				region->setType(AMEXAFSRegion::kSpace);

			else if (!energy.contains("k") && region->type() == AMEXAFSRegion::kSpace)
				region->setType(AMEXAFSRegion::Energy);

			retVal = regions_->at(index.row())->setStart(dval);

			break;

		case 2: // Setting a delta value?
			energy = value.toString();

			if (energy.contains("k") && region->type() == AMEXAFSRegion::Energy)
				region->setType(AMEXAFSRegion::kSpace);

			else if (!energy.contains("k") && region->type() == AMEXAFSRegion::kSpace)
				region->setType(AMEXAFSRegion::Energy);

			retVal = regions_->at(index.row())->setDelta(dval);

			break;

		case 3: // Setting an end value?
			energy = value.toString();

			if (energy.contains("k") && region->type() == AMEXAFSRegion::Energy)
				region->setType(AMEXAFSRegion::kSpace);

			else if (!energy.contains("k") && region->type() == AMEXAFSRegion::kSpace)
				region->setType(AMEXAFSRegion::Energy);

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
