#include "AMRegion.h"

AMRegion::AMRegion(QObject *parent) :
	QObject(parent)
{
}

/// Sets the start value from the double passed in. Makes sure the energy is within the allowable range, otherwise returns false.
bool AMRegion::setStart(double start) {
	if(ctrl_->valueOutOfRange(start))
		return FALSE;
	start_ = start;
	return TRUE;
}

/// Sets the end value from the double passed in. Makes sure the energy is within the allowable range, otherwise returns false.
bool AMRegion::setEnd(double end) {
	if(ctrl_->valueOutOfRange(end))
		return FALSE;
	end_ = end;
	return TRUE;
}

/// Sets the delta value from the double passed in. Makes sure the value is non-zero, otherwise it returns false.
bool AMRegion::setDelta(double delta) {
	if(delta == 0)
		return false;
	delta_ = delta;
	return true;
}

AMRegionsListModel::AMRegionsListModel(QObject *parent) : QAbstractTableModel(parent) {
	regions_ = new QList<AMRegion*>();
	defaultControl_ = NULL;
}

QVariant AMRegionsListModel::data(const QModelIndex &index, int role) const{
	// Invalid index:
	if(!index.isValid())
		return QVariant();

	// We only answer to Qt::DisplayRole right now
	if(role != Qt::DisplayRole)
		return QVariant();

	// Out of range: (Just checking for too big.  isValid() checked for < 0)
	if(index.row() >= regions_->count())
		return QVariant();
	// Return Control:
	if(index.column() == 0)
		return QVariant(); // Doing nothing
//			return regions_->at(index.row())->control();
	// Return Start:
	if(index.column() == 1)
		return regions_->at(index.row())->start() ;
	// Return Delta:
	if(index.column() == 2)
		return regions_->at(index.row())->delta() ;
	if(index.column() == 3)
		return regions_->at(index.row())->end() ;

	// Anything else:
	return QVariant();
}

/// Retrieves the header data for a column or row and returns as a QVariant. Only valid role is Qt::DisplayRole right now.
QVariant AMRegionsListModel::headerData(int section, Qt::Orientation orientation, int role) const{

	if (role != Qt::DisplayRole) return QVariant();

	// Vertical headers:
	if(orientation == Qt::Vertical) {
		return section;
	}

	// Horizontal Headers: (Column labels)
	else {
		if(section == 0)
			return "Control";
		if(section == 1)
			return "Start";
		if(section == 2)
			return "Delta";
		if(section == 3)
			return "End";
	}
	return QVariant();
}

/// Sets the data value at an index (row and column). Only valid role is Qt::DisplayRole right now.
bool AMRegionsListModel::setData(const QModelIndex &index, const QVariant &value, int role){

	if (index.isValid()  && index.row() < regions_->count() && role == Qt::EditRole) {

		bool conversionOK, retVal;
		double dval = value.toDouble(&conversionOK);
		if(!conversionOK)
			return false;

		// Setting a control?
		if(index.column() == 0){
			return false; //Doing nothing right now
		}
		// Setting a start value?
		if(index.column() == 1) {
			retVal = regions_->at(index.row())->setStart(dval);
			if(retVal)
				emit dataChanged(index, index);
			return retVal;
		}
		// Setting a delta value?
		if(index.column() == 2) {
			retVal = regions_->at(index.row())->setDelta(dval);
			if(retVal)
				emit dataChanged(index, index);
			return retVal;
		}
		// Setting an end value?
		if(index.column() == 3) {
			retVal = regions_->at(index.row())->setEnd(dval);
			if(retVal)
				emit dataChanged(index, index);
			return retVal;
		}
	}
	return false;	// no value set
}

bool AMRegionsListModel::insertRows(int position, int rows, const QModelIndex &index){
	if (index.row() <= regions_->count() && position <= regions_->count() && defaultControl_) {
		beginInsertRows(QModelIndex(), position, position+rows-1);

		AMRegion *tmpRegion;
		for (int row = 0; row < rows; ++row) {
			tmpRegion = new AMRegion(this);
			tmpRegion->setControl(defaultControl_);
			regions_->insert(position, tmpRegion);
//			stringList.insert(position, "");
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
			regions_->removeAt(position);
//			stringList.insert(position, "");
		}

		endRemoveRows();
		return true;
	}
	return false;
}

/// This allows editing of values within range (for ex: in a QTableView)
Qt::ItemFlags AMRegionsListModel::flags(const QModelIndex &index) const{

	Qt::ItemFlags flags;
	if (index.isValid() && index.row() < regions_->count() && index.column()<4)
		flags = Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	return flags;
}

/*
/// Sets the start value from the double passed in. Makes sure the energy is within the allowable range, otherwise returns false.
bool AMXASRegion::setStart(double start) {
	if(ctrl_->valueOutOfRange(start))
		return FALSE;
	start_ = start;
	return TRUE;
}

/// Sets the end value from the double passed in. Makes sure the energy is within the allowable range, otherwise returns false.
bool AMXASRegion::setEnd(double end) {
	if(ctrl_->valueOutOfRange(end))
		return FALSE;
	end_ = end;
	return TRUE;
}
*/

bool AMXASRegionsListModel::insertRows(int position, int rows, const QModelIndex &index){
	if (index.row() <= regions_->count() && defaultControl_) {
		beginInsertRows(QModelIndex(), position, position+rows-1);

		AMXASRegion *tmpRegion;
		for (int row = 0; row < rows; ++row) {
			tmpRegion = new AMXASRegion(defaultControl_, this);
			regions_->insert(position, tmpRegion);
		}

		endInsertRows();
		return true;
	}
	return false;
}
