#include "AMControlSetInfo.h"

AMControlSetInfo::AMControlSetInfo(QObject *parent) :
	AMDbObject(parent)
{
	insertRowLatch = -1;
	setName("ControlSet");
	QStringList tmpList;
	metaData_["names"] = QStringList(tmpList);
	metaData_["ctrlValues"] = QStringList(tmpList);
	metaData_["minimums"] = QStringList(tmpList);
	metaData_["maximums"] = QStringList(tmpList);
	metaData_["units"] = QStringList(tmpList);
	ctrlInfoList_ = NULL;
	setupModel();
}

AMControlSetInfo::AMControlSetInfo(AMControlSetInfo *copyFrom, QObject *parent) :
		AMDbObject(parent)
{
	setName("ControlSet");
	QStringList tmpList;
	metaData_["names"] = QStringList(tmpList);
	metaData_["ctrlValues"] = QStringList(tmpList);
	metaData_["minimums"] = QStringList(tmpList);
	metaData_["maximums"] = QStringList(tmpList);
	metaData_["units"] = QStringList(tmpList);
	ctrlInfoList_ = NULL;
	setupModel();
	this->copyFrom(copyFrom);
}

AMControlSetInfoModel* AMControlSetInfo::model(){
	return ctrlInfoList_;
}

bool AMControlSetInfo::checkSync(){
	QStringList names = metaData("names").toStringList();
	QStringList values = metaData("ctrlValues").toStringList();
	QStringList minimums = metaData("minimums").toStringList();
	QStringList maximums = metaData("maximums").toStringList();
	QStringList units = metaData("units").toStringList();
	bool retVal = true;
	for(int x = 0; x < count(); x++){
		qDebug() << nameAt(x) << "=" << ctrlInfoList_->data(ctrlInfoList_->index(x, 0), Qt::DisplayRole).toString() << "=" << names.at(x)
				<< valueAt(x) << "=" << ctrlInfoList_->data(ctrlInfoList_->index(x, 1), Qt::DisplayRole).toDouble() << "=" << values.at(x)
				<< minimumAt(x) << "=" << ctrlInfoList_->data(ctrlInfoList_->index(x, 2), Qt::DisplayRole).toDouble() << "=" << minimums.at(x)
				<< maximumAt(x) << "=" << ctrlInfoList_->data(ctrlInfoList_->index(x, 3), Qt::DisplayRole).toDouble() << "=" << maximums.at(x)
				<< unitsAt(x) << "=" << ctrlInfoList_->data(ctrlInfoList_->index(x, 4), Qt::DisplayRole).toString() << "=" << units.at(x);
		retVal = retVal && ( ctrlInfoList_->data(ctrlInfoList_->index(x, 0), Qt::DisplayRole).toString() == names.at(x) );
		retVal = retVal && ( ctrlInfoList_->data(ctrlInfoList_->index(x, 1), Qt::DisplayRole).toDouble() == values.at(x).toDouble() );
		retVal = retVal && ( ctrlInfoList_->data(ctrlInfoList_->index(x, 2), Qt::DisplayRole).toDouble() == minimums.at(x).toDouble() );
		retVal = retVal && ( ctrlInfoList_->data(ctrlInfoList_->index(x, 3), Qt::DisplayRole).toDouble() == maximums.at(x).toDouble() );
		retVal = retVal && ( ctrlInfoList_->data(ctrlInfoList_->index(x, 4), Qt::DisplayRole).toString() == units.at(x) );
	}
	return retVal;
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

QList<AMMetaMetaData> AMControlSetInfo::metaDataUniqueKeys(){
	QList<AMMetaMetaData> rv;
	rv << AMMetaMetaData(QVariant::StringList, "names", true);
	rv << AMMetaMetaData(QVariant::StringList, "ctrlValues", true);
	rv << AMMetaMetaData(QVariant::StringList, "minimums", true);
	rv << AMMetaMetaData(QVariant::StringList, "maximums", true);
	rv << AMMetaMetaData(QVariant::StringList, "units", true);
	return rv;
}

QList<AMMetaMetaData> AMControlSetInfo::metaDataKeys() {
	return AMDbObject::metaDataKeys() << metaDataUniqueKeys();
}

QList<AMMetaMetaData> AMControlSetInfo::metaDataAllKeys() const {
	return this->metaDataKeys();
}

QString AMControlSetInfo::databaseTableName() const {
	return AMDatabaseDefinition::controlSetTableName();
}

QString AMControlSetInfo::typeDescription() const {
	return "Generic Control Set";
}

void AMControlSetInfo::setDescription(const QString& description) {
	setMetaData("description", description);
}

bool AMControlSetInfo::setNameAt(size_t index, QString name){
	bool retVal = ctrlInfoList_->setData(ctrlInfoList_->index(index, 0), name, Qt::EditRole);
	if(retVal){
		QStringList tmpList = metaData("names").toStringList();
		tmpList[index] = name;
		setMetaData("names", tmpList);
	}
	return retVal;
}

bool AMControlSetInfo::setValueAt(size_t index, double value){
	bool retVal = ctrlInfoList_->setData(ctrlInfoList_->index(index, 1), value, Qt::EditRole);
	if(retVal){
		QStringList tmpList = metaData("ctrlValues").toStringList();
		QString tmpStr;
		tmpStr.setNum(value);
		tmpList[index] = tmpStr;
		setMetaData("ctrlValues", tmpList);
	}
	return retVal;
}

bool AMControlSetInfo::setMinimumAt(size_t index, double minimum){
	bool retVal = ctrlInfoList_->setData(ctrlInfoList_->index(index, 2), minimum, Qt::EditRole);
	if(retVal){
		QStringList tmpList = metaData("minimums").toStringList();
		QString tmpStr;
		tmpStr.setNum(minimum);
		tmpList[index] = tmpStr;
		setMetaData("minimums", tmpList);
	}
	return retVal;
}

bool AMControlSetInfo::setMaximumAt(size_t index, double maximum){
	bool retVal = ctrlInfoList_->setData(ctrlInfoList_->index(index, 3), maximum, Qt::EditRole);
	if(retVal){
		QStringList tmpList = metaData("maximums").toStringList();
		QString tmpStr;
		tmpStr.setNum(maximum);
		tmpList[index] = tmpStr;
		setMetaData("maximums", tmpList);
	}
	return retVal;
}

bool AMControlSetInfo::setUnitsAt(size_t index, QString units){
	bool retVal = ctrlInfoList_->setData(ctrlInfoList_->index(index, 4), units, Qt::EditRole);
	if(retVal){
		QStringList tmpList = metaData("units").toStringList();
		tmpList[index] = units;
		setMetaData("units", tmpList);
	}
	return retVal;
}

bool AMControlSetInfo::setControlAt(size_t index, QString name, double value, double minimum, double maximum, QString units){
	if( (int)index < count()){
		/*
		bool t1 = setNameAt(index, name);
		bool t2 = setValueAt(index, value);
		bool t3 = setMinimumAt(index, minimum);
		bool t4 = setMaximumAt(index, maximum);
		bool t5 = setUnitsAt(index, units);
		return t1 && t2 && t3 && t4 && t5;
		*/
		return setNameAt(index, name) && setValueAt(index, value) && setMinimumAt(index, minimum) && setMaximumAt(index, maximum) && setUnitsAt(index, units);
	}
	else
		return false;
}

bool AMControlSetInfo::addControlAt(size_t index, QString name, double value, double minimum, double maximum, QString units){
	if(!ctrlInfoList_->insertRows(index, 1))
		return false;
	QStringList tmpList = metaData("names").toStringList();
	QString tmpStr;
	tmpList.insert(index, "");
	setMetaData("names", tmpList);
	tmpList = metaData("ctrlValues").toStringList();
	tmpStr.setNum(value);
	tmpList.insert(index, "");
	setMetaData("ctrlValues", tmpList);
	tmpList = metaData("minimums").toStringList();
	tmpStr.setNum(minimum);
	tmpList.insert(index, "");
	setMetaData("minimums", tmpList);
	tmpList = metaData("maximums").toStringList();
	tmpStr.setNum(maximum);
	tmpList.insert(index, "");
	setMetaData("maximums", tmpList);
	tmpList = metaData("units").toStringList();
	tmpList.insert(index, "");
	setMetaData("units", tmpList);
	bool retVal = setControlAt(index, name, value, minimum, maximum, units);
/*	if(retVal){
		QStringList tmpList = metaData("names").toStringList();
		QString tmpStr;
		tmpList.insert(index, name);
		setMetaData("names", tmpList);
		tmpList = metaData("ctrlValues").toStringList();
		tmpStr.setNum(value);
		tmpList.insert(index, tmpStr);
		setMetaData("ctrlValues", tmpList);
		tmpList = metaData("minimums").toStringList();
		tmpStr.setNum(minimum);
		tmpList.insert(index, tmpStr);
		setMetaData("minimums", tmpList);
		tmpList = metaData("maximums").toStringList();
		tmpStr.setNum(maximum);
		tmpList.insert(index, tmpStr);
		setMetaData("maximums", tmpList);
	}
*/
	return retVal;
}

bool AMControlSetInfo::removeControlAt(size_t index){
	if( (int)index >= count())
		return false;
	bool retVal = ctrlInfoList_->removeRows(index, 1);
	if(retVal){
		QStringList tmpList = metaData("names").toStringList();
		tmpList.removeAt(index);
		setMetaData("names", tmpList);
		tmpList = metaData("ctrlValues").toStringList();
		tmpList.removeAt(index);
		setMetaData("ctrlValues", tmpList);
		tmpList = metaData("minimums").toStringList();
		tmpList.removeAt(index);
		setMetaData("minimums", tmpList);
		tmpList = metaData("maximums").toStringList();
		tmpList.removeAt(index);
		setMetaData("maximums", tmpList);
	}
	return retVal;
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
