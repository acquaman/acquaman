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
