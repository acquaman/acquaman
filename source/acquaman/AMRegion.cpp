#include "AMRegion.h"

AMRegion::AMRegion(QObject *parent) :
	QObject(parent)
{
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
	qDebug() << "Running RegionsListModel constructor";
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
	qDebug() << "Doing insertRows in RegionsListModel";
	if (index.row() <= regions_->count() && defaultControl_) {
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
	qDebug() << "Doing removeRows in model";
	if (index.row() < regions_->count()) {
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

AMRegionsList::AMRegionsList(QObject *parent) : QObject(parent){
	qDebug() << "Running RegionsList constructor";
	defaultControl_ = NULL;
//	regions_ = new AMRegionsListModel(this);
	regions_ = NULL;
//	setupModel();
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
	qDebug() << "SETTING UP A REGIONSLISTMODEL";
	regions_ = new AMRegionsListModel(this);
	if(regions_)
		return true;
	return false;
}

/*
  *
  *
  *
  */

/// Sets the start value from the double passed in. Makes sure the energy is within the allowable range, otherwise returns false.
bool AMXASRegion::setStart(double start) {
	qDebug() << "Trying setStart in AMXASRegion";
	if(ctrl_->valueOutOfRange(start))
	{qDebug() << "In AMXASRegion setStart it's bad"; return FALSE;}
	qDebug() << "In AMXASRegion setStart it's good";
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

bool AMXASRegionsListModel::insertRows(int position, int rows, const QModelIndex &index){
	qDebug() << "Doing insertRows in XASRegionsListModel";
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


bool AMXASRegionsList::setupModel(){
	qDebug() << "SETTING UP A XASREGIONSLISTMODEL";
	regions_ = new AMXASRegionsListModel(this);
	if(regions_)
		return true;
	return false;
}















/// Returns the data at the index from the list of AMXASRegion available. For invalid requests an empty QVariant is returned.
QVariant AMXASRegionModel::data(const QModelIndex &index, int role) const {

	// Invalid index:
	if(!index.isValid())
		return QVariant();

	// We only answer to Qt::DisplayRole right now
	if(role != Qt::DisplayRole)
		return QVariant();

	// Out of range: (Just checking for too big.  isValid() checked for < 0)
	if(index.row() >= regions_->count())
		return QVariant();
	// Return Start:
	if(index.column() == 0)
		return regions_->at(index.row())->start() ;
	// Return Delta:
	if(index.column() == 1)
		return regions_->at(index.row())->delta() ;
	if(index.column() == 2)
		return regions_->at(index.row())->end() ;

	// Anything else:
	return QVariant();
}

/// Returns the header data at the row or column requested. For invalid requests an empty QVariant is returned.
QVariant AMXASRegionModel::headerData(int section, Qt::Orientation orientation, int role) const {

	if (role != Qt::DisplayRole) return QVariant();

	// Vertical headers:
	if(orientation == Qt::Vertical) {
		return section;
	}

	// Horizontal Headers: (Column labels)
	else {
		if(section == 0)
			return "Start";
		if(section == 1)
			return "Delta";
		if(section == 2)
			return "End";
	}
	return QVariant();
}

/// Sets the data at the requested index from a QVariant, which is converted to a double in all cases. Returns false if no value set or if the QVariant cannot be converted to a double.
bool AMXASRegionModel::setData(const QModelIndex &index, const QVariant &value, int role) {

	if (index.isValid()  && index.row() < regions_->count() && role == Qt::EditRole) {

		bool conversionOK;
		double dval = value.toDouble(&conversionOK);
		if(!conversionOK)
			return false;

		// Setting a start value?
		if(index.column() == 0) {
			regions_->at(index.row())->setStart(dval);
			emit dataChanged(index, index);
			return true;
		}
		// Setting a delta value?
		if(index.column() == 1) {
			regions_->at(index.row())->setDelta(dval);
			emit dataChanged(index, index);
			return true;
		}
		// Setting an end value?
		if(index.column() == 2) {
			regions_->at(index.row())->setEnd(dval);
			emit dataChanged(index, index);
			return true;
		}
	}
	return false;	// no value set
}

bool AMXASRegionModel::insertRows(int position, int rows, const QModelIndex &index){
	qDebug() << "Doing insertRows in model";
	if (/*index.isValid()  &&*/ index.row() <= regions_->count() /*&& role == Qt::EditRole*/ && beamlineEnergy_) {
		qDebug() << "Passed validity test in model insert";
		beginInsertRows(QModelIndex(), position, position+rows-1);

		AMXASRegion *tmpRegion;
		for (int row = 0; row < rows; ++row) {
			tmpRegion = new AMXASRegion(beamlineEnergy_, this);
			regions_->insert(position, tmpRegion);
//			stringList.insert(position, "");
		}

		endInsertRows();
		return true;
	}
	if(!index.isValid())
		qDebug() << "Failed index is valid";
	qDebug() << "Failed validity test in model insert";
	return false;
}

/// Returns the appropriate flags for an index, which are always the same if the index is valid.
Qt::ItemFlags AMXASRegionModel::flags(const QModelIndex &index) const {

	Qt::ItemFlags flags;
	if (index.isValid() && index.row() < regions_->count() && index.column()<3)
		flags = Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	return flags;
}
