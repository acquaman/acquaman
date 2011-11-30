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
	if (units_.isNull())
		return false;

	units_ = units;
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
		dataVal = QString::number(region->time()) + " s";
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

			if (time.contains(" s"))
				time.chop(2);

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

// AMEXAFSRegionsListModel
////////////////////////////////////////////////////////

bool AMEXAFSRegionsListModel::insertRows(int position, int rows, const QModelIndex &index)
{
	if (index.row() <= regions_->count() && position <= regions_->count()
			&& defaultControl_ && defaultKControl_ && defaultTimeControl_) {

		beginInsertRows(QModelIndex(), position, position+rows-1);

		AMEXAFSRegion *tmpRegion;

		for (int row = 0; row < rows; ++row) {

			tmpRegion = new AMEXAFSRegion(defaultControl_, defaultKControl_, this);
			regions_->insert(position, tmpRegion); // Order doesn't matter because they are all identical, empty regions.
		}

		endInsertRows();
		return true;
	}

	return false;
}

QVariant AMEXAFSRegionsListModel::data(const QModelIndex &index, int role) const{
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

	AMEXAFSRegion *region = qobject_cast<AMEXAFSRegion *>(regions_->at(index.row()));

	// If this region is not an AMEXAFSRegion, then we are in trouble.
	if (!region)
		return QVariant();

	QVariant dataVal = QVariant();

	switch(index.column()){

	case 0: // The control.
		break; // Doing nothing.
	case 1: // The start value.
		dataVal = region->type() == AMEXAFSRegion::Energy ? QString::number(region->start()) : QString::number(region->start()) + "k";
		break;
	case 2: // The delta value.
		dataVal = region->type() == AMEXAFSRegion::Energy ? QString::number(region->delta()) : QString::number(region->delta()) + "k";
		break;
	case 3: // The end value.
		dataVal = region->type() == AMEXAFSRegion::Energy ? QString::number(region->end()) : QString::number(region->end()) + "k";
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

// Sets the data value at an index (row and column). Only valid role is Qt::DisplayRole right now.
bool AMEXAFSRegionsListModel::setData(const QModelIndex &index, const QVariant &value, int role){

	if (index.isValid() && index.row() < regions_->count() && role == Qt::EditRole) {

		bool conversionOK = false;
		bool retVal;
		double dval;
		bool bval;

		if(index.column() == 1 || index.column() == 2 || index.column() == 3){

			QString energy = value.toString();

			if (energy.contains("k"))
				energy.chop(1);

			dval = energy.toDouble(&conversionOK);
		}

		else if (index.column() == 7)
			dval  = value.toDouble(&conversionOK);

		else if(index.column() == 4 || index.column() == 5){
			bval = value.toBool();
			conversionOK = true;
		}

		// Check if any data is invalid.
		if(!conversionOK)
			return false;

		QString energy;
		AMEXAFSRegion *region = qobject_cast<AMEXAFSRegion *>(regions_->at(index.row()));

		// Need to be using an AMEXAFSRegion.
		if (!region)
			return false;

		switch(index.column()){

		case 0: // Setting a control?
			retVal = false; // Doing nothing right now.
			break;

		case 1: // Setting a start value?
			energy = value.toString();

			if (energy.contains("k") && region->type() == AMEXAFSRegion::Energy){

				region->setType(AMEXAFSRegion::kSpace);
				retVal = region->setStart(dval) && region->setDelta(0.1) && region->setEnd(toKSpace(region->end()));
			}

			else if (!energy.contains("k") && region->type() == AMEXAFSRegion::kSpace){

				region->setType(AMEXAFSRegion::Energy);
				retVal = region->setStart(dval) && region->setDelta(1) && region->setEnd(toEnergy(region->end()));
			}

			else
				retVal = regions_->at(index.row())->setStart(dval);

			break;

		case 2: // Setting a delta value?
			energy = value.toString();

			if (energy.contains("k") && region->type() == AMEXAFSRegion::Energy){

				region->setType(AMEXAFSRegion::kSpace);
				retVal = region->setStart(toKSpace(region->start())) && region->setDelta(dval) && region->setEnd(toKSpace(region->end()));
			}

			else if (!energy.contains("k") && region->type() == AMEXAFSRegion::kSpace){

				region->setType(AMEXAFSRegion::Energy);
				retVal = region->setStart(toEnergy(region->start())) && region->setDelta(dval) && region->setEnd(toEnergy(region->end()));
			}

			else
				retVal = regions_->at(index.row())->setDelta(dval);

			break;

		case 3: // Setting an end value?
			energy = value.toString();

			if (energy.contains("k") && region->type() == AMEXAFSRegion::Energy){

				region->setType(AMEXAFSRegion::kSpace);
				retVal = region->setStart(toKSpace(region->start())) && region->setDelta(0.1) && region->setEnd(dval);
			}

			else if (!energy.contains("k") && region->type() == AMEXAFSRegion::kSpace){

				region->setType(AMEXAFSRegion::Energy);
				retVal = region->setStart(toEnergy(region->start())) && region->setDelta(1) && region->setEnd(dval);
			}

			else
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

double AMEXAFSRegionsListModel::toKSpace(double energy)
{
	// k = sqrt((E - E0)/a) ; a = 3.810 945 497 eV * Angstrom
	return sqrt((energy-edgeEnergy_)/3.810945497);
}

double AMEXAFSRegionsListModel::toEnergy(double kSpace)
{
	// E = E0 + a*k^2 ; a = 3.810 945 497 eV * Angstrom
	return edgeEnergy_ + 3.810945497*kSpace*kSpace;
}
