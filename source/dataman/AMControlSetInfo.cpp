/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

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


#include "AMControlSetInfo.h"

AMControlInfoSet::AMControlInfoSet(QObject *parent) :
		AMDbObject(parent)
{
	insertRowLatch = -1;
	setName("ControlSet");
	ctrlInfoList_ = NULL;
	setupModel();
}

AMControlInfoSet::AMControlInfoSet(AMControlInfoSet *copyFrom, QObject *parent) :
		AMDbObject(parent)
{
	setName("ControlSet");
	ctrlInfoList_ = NULL;
	setupModel();
	this->copyFrom(copyFrom);
}

AMControlInfoSetModel* AMControlInfoSet::model(){
	return ctrlInfoList_;
}

int AMControlInfoSet::count() const{
	return ctrlInfoList_->rowCount(QModelIndex());
}

QString AMControlInfoSet::nameAt(size_t index) const{
	QVariant retVal = ctrlInfoList_->data(ctrlInfoList_->index(index, 0), Qt::DisplayRole);
	if(retVal.isValid())
		return retVal.toString();
	else
		return QString();
}

double AMControlInfoSet::valueAt(size_t index) const{
	QVariant retVal = ctrlInfoList_->data(ctrlInfoList_->index(index, 1), Qt::DisplayRole);
	if(retVal.isValid())
		return retVal.toDouble();
	else
		return -1;
}

double AMControlInfoSet::minimumAt(size_t index) const{
	QVariant retVal = ctrlInfoList_->data(ctrlInfoList_->index(index, 2), Qt::DisplayRole);
	if(retVal.isValid())
		return retVal.toDouble();
	else
		return -1;
}

double AMControlInfoSet::maximumAt(size_t index) const{
	QVariant retVal = ctrlInfoList_->data(ctrlInfoList_->index(index, 3), Qt::DisplayRole);
	if(retVal.isValid())
		return retVal.toDouble();
	else
		return -1;
}

QString AMControlInfoSet::unitsAt(size_t index) const{
	QVariant retVal = ctrlInfoList_->data(ctrlInfoList_->index(index, 4), Qt::DisplayRole);
	if(retVal.isValid())
		return retVal.toString();
	else
		return QString();
}





#include "dataman/AMDbObjectSupport.h"

bool AMControlInfoSet::loadFromDb(AMDatabase *db, int id){
	bool retVal = AMDbObject::loadFromDb(db, id);
	if(retVal){
		while(count() > 0)
			removeControlAt(count()-1);

		/// \todo migrate to use new/improved AMDatabase::retrieve, or ...
		QSqlQuery q = db->query();
		q.prepare(QString("SELECT name,ctrlValue,minimum,maximum,units,number FROM %1 WHERE csiId = ?").arg(AMDbObjectSupport::controlSetEntriesTableName()));
		q.bindValue(0, id);
		retVal = q.exec();
		while(q.next()) {
			addControlAt(q.value(5).toInt(), q.value(0).toString(), q.value(1).toDouble(), q.value(2).toDouble(), q.value(3).toDouble(), q.value(4).toString());
		}
	}
	return retVal;
}

bool AMControlInfoSet::storeToDb(AMDatabase *db){
	if(!AMDbObject::storeToDb(db) )
		return false;

	bool success = true;

	// delete all the old entries for this control set
	db->deleteRows(AMDbObjectSupport::controlSetEntriesTableName(), QString("csiId = '%1'").arg(id()));

	db->startTransaction();

	QSqlQuery q = db->query();
	q.prepare(QString("INSERT INTO %1 (csiId,name,ctrlValue,minimum,maximum,units,number) VALUES (?,?,?,?,?,?,?)").arg(AMDbObjectSupport::controlSetEntriesTableName()));

	for(int x=0; x<count(); x++) {
		q.bindValue(0,id());
		q.bindValue(1,nameAt(x));
		q.bindValue(2,valueAt(x));
		q.bindValue(3,minimumAt(x));
		q.bindValue(4,maximumAt(x));
		q.bindValue(5,unitsAt(x));
		q.bindValue(6,x);
		if(!q.exec())
			success = false;
	}

	if(!success) {
		db->rollbackTransaction();
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Serious, -1, "The ControlSet was saved to the database, but saving the individual control values failed."));
	}
	else
		db->commitTransaction();

	return success;
}


void AMControlInfoSet::setDescription(const QString& description) {
	description_ = description;
	setModified(true);
}

bool AMControlInfoSet::setNameAt(size_t index, QString name){
	return ctrlInfoList_->setData(ctrlInfoList_->index(index, 0), name, Qt::EditRole);
}

bool AMControlInfoSet::setValueAt(size_t index, double value){
	return ctrlInfoList_->setData(ctrlInfoList_->index(index, 1), value, Qt::EditRole);
}

bool AMControlInfoSet::setMinimumAt(size_t index, double minimum){
	return ctrlInfoList_->setData(ctrlInfoList_->index(index, 2), minimum, Qt::EditRole);
}

bool AMControlInfoSet::setMaximumAt(size_t index, double maximum){
	return ctrlInfoList_->setData(ctrlInfoList_->index(index, 3), maximum, Qt::EditRole);
}

bool AMControlInfoSet::setUnitsAt(size_t index, QString units){
	bool retVal = ctrlInfoList_->setData(ctrlInfoList_->index(index, 4), units, Qt::EditRole);
	return retVal;
}

bool AMControlInfoSet::setControlAt(size_t index, QString name, double value, double minimum, double maximum, QString units){
	if( (int)index < count()){
		return setNameAt(index, name) && setValueAt(index, value) && setMinimumAt(index, minimum) && setMaximumAt(index, maximum) && setUnitsAt(index, units);
	}
	else
		return false;
}

bool AMControlInfoSet::addControlAt(size_t index, QString name, double value, double minimum, double maximum, QString units){
	if(!ctrlInfoList_->insertRows(index, 1))
		return false;
	return setControlAt(index, name, value, minimum, maximum, units);
}

bool AMControlInfoSet::removeControlAt(size_t index){
	if( (int)index >= count())
		return false;
	return ctrlInfoList_->removeRows(index, 1);
}

void AMControlInfoSet::copyFrom(AMControlInfoSet *copyFrom){
	while(count() > 0)
		removeControlAt(count()-1);
	if(copyFrom){
		setName(copyFrom->name());
		for(int x = 0; x < copyFrom->count(); x++)
			addControlAt(x, copyFrom->nameAt(x), copyFrom->valueAt(x), copyFrom->minimumAt(x), copyFrom->maximumAt(x), copyFrom->unitsAt(x));
	}
}

void AMControlInfoSet::onDataChanged(QModelIndex a, QModelIndex b){
	if(a.row() != b.row())
		return;
	if(insertRowLatch != -1 && insertRowLatch == a.row()){
		insertRowLatch = -1;
		emit controlAdded(a.row());
	}
	else
		emit valuesChanged(a.row());
}

void AMControlInfoSet::onRowsInserted(QModelIndex parent, int start, int end){
	Q_UNUSED(parent);
	if(start != end)
		return;
	insertRowLatch = start;
}

void AMControlInfoSet::onRowsRemoved(QModelIndex parent, int start, int end){
	Q_UNUSED(parent);
	if(start != end)
		return;
	emit controlRemoved(start);
}

bool AMControlInfoSet::setupModel(){
	ctrlInfoList_ = new AMControlInfoSetModel(this);
	if(ctrlInfoList_){
		connect(ctrlInfoList_, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onDataChanged(QModelIndex,QModelIndex)));
		connect(ctrlInfoList_, SIGNAL(rowsInserted(const QModelIndex,int, int)), this, SLOT(onRowsInserted(QModelIndex,int,int)));
		connect(ctrlInfoList_, SIGNAL(rowsRemoved(const QModelIndex,int,int)), this, SLOT(onRowsRemoved(QModelIndex,int,int)));
		return true;
	}
	return false;
}

AMControlInfoSetModel::AMControlInfoSetModel(QObject *parent) :
		QAbstractTableModel(parent)
{
	ctrlInfoList_ = new QList<AMControlInfo*>();
}

int AMControlInfoSetModel::rowCount(const QModelIndex & /*parent*/) const{
	return ctrlInfoList_->count();
}

int AMControlInfoSetModel::columnCount(const QModelIndex & /*parent*/) const{
	return 5;
}

QVariant AMControlInfoSetModel::data(const QModelIndex &index, int role) const{
	// Invalid index:
	if(!index.isValid())
		return QVariant();
	// We only answer to Qt::DisplayRole right now
	if(role != Qt::DisplayRole)
		return QVariant();
	// Out of range: (Just checking for too big.  isValid() checked for < 0)
	if(index.row() >= ctrlInfoList_->count())
		return QVariant();
	// Return Control:
	if(index.column() == 0)
		return ctrlInfoList_->at(index.row())->name();
	// Return Start:
	if(index.column() == 1)
		return ctrlInfoList_->at(index.row())->value();
	// Return Delta:
	if(index.column() == 2)
		return ctrlInfoList_->at(index.row())->minimum();
	if(index.column() == 3)
		return ctrlInfoList_->at(index.row())->maximum();
	if(index.column() == 4)
		return ctrlInfoList_->at(index.row())->units();
	// Anything else:
	return QVariant();
}

QVariant AMControlInfoSetModel::headerData(int section, Qt::Orientation orientation, int role) const{
	if (role != Qt::DisplayRole)
		return QVariant();
	// Vertical headers:
	if(orientation == Qt::Vertical) {
		return section;
	}
	// Horizontal Headers: (Column labels)
	else {
		if(section == 0)
			return "Name";
		if(section == 1)
			return "Value";
		if(section == 2)
			return "Minimum";
		if(section == 3)
			return "Maximum";
		if(section == 4)
			return "Units";
	}
	return QVariant();
}

bool AMControlInfoSetModel::setData(const QModelIndex &index, const QVariant &value, int role){
	if (index.isValid()  && index.row() < ctrlInfoList_->count() && role == Qt::EditRole) {
		bool conversionOK = false;
		double dval;
		QString sval;
		if(index.column() == 1 || index.column() == 2 || index.column() == 3)
			dval  = value.toDouble(&conversionOK);
		else if(index.column() == 0 || index.column() == 4){
			sval = value.toString();
			conversionOK = true;
		}
		if(!conversionOK)
			return false;
		if(index.column() == 0)
			ctrlInfoList_->at(index.row())->setName(sval);
		if(index.column() == 1)
			ctrlInfoList_->at(index.row())->setValue(dval);
		if(index.column() == 2)
			ctrlInfoList_->at(index.row())->setMinimum(dval);
		if(index.column() == 3)
			ctrlInfoList_->at(index.row())->setMaximum(dval);
		if(index.column() == 4)
			ctrlInfoList_->at(index.row())->setUnits(sval);

		emit dataChanged(index, index);
		return true;
	}
	return false;	// no value set
}

bool AMControlInfoSetModel::insertRows(int position, int rows, const QModelIndex &index){
	if (index.row() <= ctrlInfoList_->count() && position <= ctrlInfoList_->count() ) {
		beginInsertRows(QModelIndex(), position, position+rows-1);
		AMControlInfo *tmpCSI;
		for (int row = 0; row < rows; ++row) {
			tmpCSI = new AMControlInfo("Unnamed", 0, 0, 0, "", this);
			ctrlInfoList_->insert(position, tmpCSI);
		}
		endInsertRows();
		return true;
	}
	return false;
}

bool AMControlInfoSetModel::removeRows(int position, int rows, const QModelIndex &index){
	if (index.row() < ctrlInfoList_->count() && position < ctrlInfoList_->count()) {
		beginRemoveRows(QModelIndex(), position, position+rows-1);
		for (int row = 0; row < rows; ++row) {
			ctrlInfoList_->removeAt(position);
		}
		endRemoveRows();
		return true;
	}
	return false;
}

Qt::ItemFlags AMControlInfoSetModel::flags(const QModelIndex &index) const{
	Qt::ItemFlags flags;
	if (index.isValid() && index.row() < ctrlInfoList_->count() && index.column()<4)
		flags = Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	return flags;
}

AMControlInfo::AMControlInfo(QString name, double value, double minimum, double maximum, QString units, QObject *parent) :
		QObject(parent)
{
	name_ = name;
	value_ = value;
	minimum_ = minimum;
	maximum_ = maximum;
	units_ = units;
}

const QString& AMControlInfo::name() const{
	return name_;
}

double AMControlInfo::value() const{
	return value_;
}

double AMControlInfo::minimum() const{
	return minimum_;
}

double AMControlInfo::maximum() const{
	return maximum_;
}

const QString& AMControlInfo::units() const{
	return units_;
}

void AMControlInfo::setName(const QString &name){
	name_ = name;
}

void AMControlInfo::setValue(double value){
	value_ = value;
}

void AMControlInfo::setMinimum(double minimum){
	minimum_ = minimum;
}

void AMControlInfo::setMaximum(double maximum){
	maximum_ = maximum;
}

void AMControlInfo::setUnits(const QString &units){
	units_ = units;
}
