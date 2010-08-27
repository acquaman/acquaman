#include "AMControlSetInfo.h"

AMControlSetInfo::AMControlSetInfo(QObject *parent) :
		AMDbObject(parent)
{
	insertRowLatch = -1;
	setName("ControlSet");
	QStringList tmpList;
	metaData_["names"] = QStringList(tmpList);
	metaData_["units"] = QStringList(tmpList);
	AMDoubleList tmpDList;
	metaData_["ctrlValues"].setValue(tmpDList);
	metaData_["minimums"].setValue(tmpDList);
	metaData_["maximums"].setValue(tmpDList);
	ctrlInfoList_ = NULL;
	setupModel();
}

AMControlSetInfo::AMControlSetInfo(AMControlSetInfo *copyFrom, QObject *parent) :
		AMDbObject(parent)
{
	setName("ControlSet");
	QStringList tmpList;
	metaData_["names"] = QStringList(tmpList);
	metaData_["units"] = QStringList(tmpList);
	AMDoubleList tmpDList;
	metaData_["ctrlValues"].setValue(tmpDList);
	metaData_["minimums"].setValue(tmpDList);
	metaData_["maximums"].setValue(tmpDList);
	ctrlInfoList_ = NULL;
	setupModel();
	this->copyFrom(copyFrom);
}

AMControlSetInfoModel* AMControlSetInfo::model(){
	return ctrlInfoList_;
}

int AMControlSetInfo::count() const{
	return ctrlInfoList_->rowCount(QModelIndex());
}

QString AMControlSetInfo::nameAt(size_t index) const{
	QVariant retVal = ctrlInfoList_->data(ctrlInfoList_->index(index, 0), Qt::DisplayRole);
	if(retVal.isValid())
		return retVal.toString();
	else
		return QString();
}

double AMControlSetInfo::valueAt(size_t index) const{
	QVariant retVal = ctrlInfoList_->data(ctrlInfoList_->index(index, 1), Qt::DisplayRole);
	if(retVal.isValid())
		return retVal.toDouble();
	else
		return -1;
}

double AMControlSetInfo::minimumAt(size_t index) const{
	QVariant retVal = ctrlInfoList_->data(ctrlInfoList_->index(index, 2), Qt::DisplayRole);
	if(retVal.isValid())
		return retVal.toDouble();
	else
		return -1;
}

double AMControlSetInfo::maximumAt(size_t index) const{
	QVariant retVal = ctrlInfoList_->data(ctrlInfoList_->index(index, 3), Qt::DisplayRole);
	if(retVal.isValid())
		return retVal.toDouble();
	else
		return -1;
}

QString AMControlSetInfo::unitsAt(size_t index) const{
	QVariant retVal = ctrlInfoList_->data(ctrlInfoList_->index(index, 4), Qt::DisplayRole);
	if(retVal.isValid())
		return retVal.toString();
	else
		return QString();
}



QString AMControlSetInfo::databaseTableName() const {
	return AMDatabaseDefinition::controlSetTableName();
}


bool AMControlSetInfo::loadFromDb(AMDatabase *db, int id){
	bool retVal = AMDbObject::loadFromDb(db, id);
	if(retVal){
		while(count() > 0)
			removeControlAt(count()-1);

		QSqlQuery q = db->query();
		q.prepare(QString("SELECT name,ctrlValue,minimum,maximum,units,number FROM %1 WHERE csiId = ?").arg(AMDatabaseDefinition::controlSetEntriesTableName()));
		q.bindValue(0, id);
		retVal = q.exec();
		while(q.next()) {
			addControlAt(q.value(5).toInt(), q.value(0).toString(), q.value(1).toDouble(), q.value(2).toDouble(), q.value(3).toDouble(), q.value(4).toString());
		}
	}
	return retVal;
}

bool AMControlSetInfo::storeToDb(AMDatabase *db){
	if(!AMDbObject::storeToDb(db) )
		return false;

	bool success = true;

	// delete all the old entries for this control set
	db->deleteRows(AMDatabaseDefinition::controlSetEntriesTableName(), QString("csiId = '%1'").arg(id()));

	db->startTransation();

	QSqlQuery q = db->query();
	q.prepare(QString("INSERT INTO %1 (csiId,name,ctrlValue,minimum,maximum,units,number) VALUES (?,?,?,?,?,?,?)").arg(AMDatabaseDefinition::controlSetEntriesTableName()));

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
		db->rollbackTransation();
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Serious, -1, "The ControlSet was saved to the database, but saving the individual control values failed."));
	}
	else
		db->commitTransation();

	return success;
}

QString AMControlSetInfo::typeDescription() const {
	return "Generic Control Set";
}

void AMControlSetInfo::setDescription(const QString& description) {
	setMetaData("description", description);
}

bool AMControlSetInfo::setNameAt(size_t index, QString name){
	return ctrlInfoList_->setData(ctrlInfoList_->index(index, 0), name, Qt::EditRole);
}

bool AMControlSetInfo::setValueAt(size_t index, double value){
	return ctrlInfoList_->setData(ctrlInfoList_->index(index, 1), value, Qt::EditRole);
}

bool AMControlSetInfo::setMinimumAt(size_t index, double minimum){
	return ctrlInfoList_->setData(ctrlInfoList_->index(index, 2), minimum, Qt::EditRole);
}

bool AMControlSetInfo::setMaximumAt(size_t index, double maximum){
	return ctrlInfoList_->setData(ctrlInfoList_->index(index, 3), maximum, Qt::EditRole);
}

bool AMControlSetInfo::setUnitsAt(size_t index, QString units){
	bool retVal = ctrlInfoList_->setData(ctrlInfoList_->index(index, 4), units, Qt::EditRole);
	return retVal;
}

bool AMControlSetInfo::setControlAt(size_t index, QString name, double value, double minimum, double maximum, QString units){
	if( (int)index < count()){
		return setNameAt(index, name) && setValueAt(index, value) && setMinimumAt(index, minimum) && setMaximumAt(index, maximum) && setUnitsAt(index, units);
	}
	else
		return false;
}

bool AMControlSetInfo::addControlAt(size_t index, QString name, double value, double minimum, double maximum, QString units){
	if(!ctrlInfoList_->insertRows(index, 1))
		return false;
	return setControlAt(index, name, value, minimum, maximum, units);
}

bool AMControlSetInfo::removeControlAt(size_t index){
	if( (int)index >= count())
		return false;
	return ctrlInfoList_->removeRows(index, 1);
}

void AMControlSetInfo::copyFrom(AMControlSetInfo *copyFrom){
	while(count() > 0)
		removeControlAt(count()-1);
	if(copyFrom){
		setName(copyFrom->name());
		for(int x = 0; x < copyFrom->count(); x++)
			addControlAt(x, copyFrom->nameAt(x), copyFrom->valueAt(x), copyFrom->minimumAt(x), copyFrom->maximumAt(x), copyFrom->unitsAt(x));
	}
}

void AMControlSetInfo::onDataChanged(QModelIndex a, QModelIndex b){
	if(a.row() != b.row())
		return;
	if(insertRowLatch != -1 && insertRowLatch == a.row()){
		insertRowLatch = -1;
		emit controlAdded(a.row());
	}
	else
		emit valuesChanged(a.row());
}

void AMControlSetInfo::onRowsInserted(QModelIndex parent, int start, int end){
	Q_UNUSED(parent);
	if(start != end)
		return;
	insertRowLatch = start;
}

void AMControlSetInfo::onRowsRemoved(QModelIndex parent, int start, int end){
	Q_UNUSED(parent);
	if(start != end)
		return;
	emit controlRemoved(start);
}

bool AMControlSetInfo::setupModel(){
	ctrlInfoList_ = new AMControlSetInfoModel(this);
	if(ctrlInfoList_){
		connect(ctrlInfoList_, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onDataChanged(QModelIndex,QModelIndex)));
		connect(ctrlInfoList_, SIGNAL(rowsInserted(const QModelIndex,int, int)), this, SLOT(onRowsInserted(QModelIndex,int,int)));
		connect(ctrlInfoList_, SIGNAL(rowsRemoved(const QModelIndex,int,int)), this, SLOT(onRowsRemoved(QModelIndex,int,int)));
		return true;
	}
	return false;
}

AMControlSetInfoModel::AMControlSetInfoModel(QObject *parent) :
		QAbstractTableModel(parent)
{
	ctrlInfoList_ = new QList<AMControlSlimInfo*>();
}

int AMControlSetInfoModel::rowCount(const QModelIndex & /*parent*/) const{
	return ctrlInfoList_->count();
}

int AMControlSetInfoModel::columnCount(const QModelIndex & /*parent*/) const{
	return 5;
}

QVariant AMControlSetInfoModel::data(const QModelIndex &index, int role) const{
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

QVariant AMControlSetInfoModel::headerData(int section, Qt::Orientation orientation, int role) const{
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

bool AMControlSetInfoModel::setData(const QModelIndex &index, const QVariant &value, int role){
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

bool AMControlSetInfoModel::insertRows(int position, int rows, const QModelIndex &index){
	if (index.row() <= ctrlInfoList_->count() && position <= ctrlInfoList_->count() ) {
		beginInsertRows(QModelIndex(), position, position+rows-1);
		AMControlSlimInfo *tmpCSI;
		for (int row = 0; row < rows; ++row) {
			tmpCSI = new AMControlSlimInfo("Unnamed", 0, 0, 0, "", this);
			ctrlInfoList_->insert(position, tmpCSI);
		}
		endInsertRows();
		return true;
	}
	return false;
}

bool AMControlSetInfoModel::removeRows(int position, int rows, const QModelIndex &index){
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

Qt::ItemFlags AMControlSetInfoModel::flags(const QModelIndex &index) const{
	Qt::ItemFlags flags;
	if (index.isValid() && index.row() < ctrlInfoList_->count() && index.column()<4)
		flags = Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	return flags;
}

AMControlSlimInfo::AMControlSlimInfo(QString name, double value, double minimum, double maximum, QString units, QObject *parent) :
		QObject(parent)
{
	name_ = name;
	value_ = value;
	minimum_ = minimum;
	maximum_ = maximum;
	units_ = units;
}

const QString& AMControlSlimInfo::name() const{
	return name_;
}

double AMControlSlimInfo::value() const{
	return value_;
}

double AMControlSlimInfo::minimum() const{
	return minimum_;
}

double AMControlSlimInfo::maximum() const{
	return maximum_;
}

const QString& AMControlSlimInfo::units() const{
	return units_;
}

void AMControlSlimInfo::setName(const QString &name){
	name_ = name;
}

void AMControlSlimInfo::setValue(double value){
	value_ = value;
}

void AMControlSlimInfo::setMinimum(double minimum){
	minimum_ = minimum;
}

void AMControlSlimInfo::setMaximum(double maximum){
	maximum_ = maximum;
}

void AMControlSlimInfo::setUnits(const QString &units){
	units_ = units;
}
