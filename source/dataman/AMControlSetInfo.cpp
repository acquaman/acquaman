#include "AMControlSetInfo.h"

AMControlSetInfo::AMControlSetInfo(QObject *parent) :
	AMDbObject(parent)
{
	setName("ControlSet");
	QStringList tmpList;
	metaData_["names"] = QStringList(tmpList);
	metaData_["values"] = QStringList(tmpList);
	metaData_["minimums"] = QStringList(tmpList);
	metaData_["maximums"] = QStringList(tmpList);
}

AMControlSetInfo::AMControlSetInfo(AMControlSetInfo *copyFrom, QObject *parent) :
		AMDbObject(parent)
{
	setName("ControlSet");
	QStringList tmpList;
	metaData_["names"] = QStringList(tmpList);
	metaData_["values"] = QStringList(tmpList);
	metaData_["minimums"] = QStringList(tmpList);
	metaData_["maximums"] = QStringList(tmpList);
	if(copyFrom){
		setName(copyFrom->name());
		for(int x = 0; x < copyFrom->count(); x++)
			addControlAt(x, copyFrom->nameAt(x), copyFrom->valueAt(x), copyFrom->minimumAt(x), copyFrom->maximumAt(x));
	}
}

int AMControlSetInfo::count() const{
	return metaData("names").toStringList().count();
}

QString AMControlSetInfo::nameAt(int index) const{
	QStringList tmpList = metaData("names").toStringList();
	if(index < tmpList.count())
		return tmpList.at(index);
	else
		return QString();
}

double AMControlSetInfo::valueAt(int index) const{
	QStringList tmpList = metaData("values").toStringList();
	if(index < tmpList.count())
		return tmpList.at(index).toDouble();
	else
		return -1;
}

double AMControlSetInfo::minimumAt(int index) const{
	QStringList tmpList = metaData("minimums").toStringList();
	if(index < tmpList.count())
		return tmpList.at(index).toDouble();
	else
		return -1;
}

double AMControlSetInfo::maximumAt(int index) const{
	QStringList tmpList = metaData("maximums").toStringList();
	if(index < tmpList.count())
		return tmpList.at(index).toDouble();
	else
		return -1;
}

QList<AMMetaMetaData> AMControlSetInfo::metaDataUniqueKeys(){
	QList<AMMetaMetaData> rv;
	rv << AMMetaMetaData(QVariant::StringList, "names", true);
	rv << AMMetaMetaData(QVariant::StringList, "values", true);
	rv << AMMetaMetaData(QVariant::StringList, "minimums", true);
	rv << AMMetaMetaData(QVariant::StringList, "maximums", true);
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

bool AMControlSetInfo::setNameAt(int index, QString name){
	QStringList tmpList = metaData("names").toStringList();
	if(index < tmpList.count()){
		tmpList[index] = name;
		setMetaData("names", tmpList);
		return true;
	}
	else
		return false;
}

bool AMControlSetInfo::setValueAt(int index, double value){
	QStringList tmpList = metaData("values").toStringList();
	if(index < tmpList.count()){
		QString tmpStr;
		tmpStr.setNum(value);
		tmpList[index] = tmpStr;
		setMetaData("values", tmpList);
		emit valuesChanged();
		return true;
	}
	else
		return false;
}

bool AMControlSetInfo::setMinimumAt(int index, double minimum){
	QStringList tmpList = metaData("minimums").toStringList();
	if(index < tmpList.count()){
		QString tmpStr;
		tmpStr.setNum(minimum);
		tmpList[index] = tmpStr;
		setMetaData("minimums", tmpList);
		return true;
	}
	else
		return false;
}

bool AMControlSetInfo::setMaximumAt(int index, double maximum){
	QStringList tmpList = metaData("maximums").toStringList();
	if(index < tmpList.count()){
		QString tmpStr;
		tmpStr.setNum(maximum);
		tmpList[index] = tmpStr;
		setMetaData("maximums", tmpList);
		return true;
	}
	else
		return false;
}

bool AMControlSetInfo::setControlAt(int index, QString name, double value, double minimum, double maximum){
	QStringList tmpList = metaData("names").toStringList();
	if(index < tmpList.count()){
		setNameAt(index, name);
		setValueAt(index, value);
		setMinimumAt(index, minimum);
		setMaximumAt(index, maximum);
		return true;
	}
	else
		return false;
}

bool AMControlSetInfo::addControlAt(int index, QString name, double value, double minimum, double maximum){
	QStringList tmpList = metaData("names").toStringList();
	if(index <= tmpList.count()){
		QString tmpStr;
		tmpList.insert(index, name);
		setMetaData("names", tmpList);
		tmpList = metaData("values").toStringList();
		tmpStr.setNum(value);
		tmpList.insert(index, tmpStr);
		setMetaData("values", tmpList);
		tmpList = metaData("minimums").toStringList();
		tmpStr.setNum(minimum);
		tmpList.insert(index, tmpStr);
		setMetaData("minimums", tmpList);
		tmpList = metaData("maximums").toStringList();
		tmpStr.setNum(maximum);
		tmpList.insert(index, tmpStr);
		setMetaData("maximums", tmpList);
		emit valuesChanged();
		return true;
	}
	else
		return false;
}

bool AMControlSetInfo::removeControlAt(int index){
	if(index < count()){
		QStringList tmpList = metaData("names").toStringList();
		tmpList.removeAt(index);
		setMetaData("names", tmpList);
		tmpList = metaData("values").toStringList();
		tmpList.removeAt(index);
		setMetaData("values", tmpList);
		tmpList = metaData("minimums").toStringList();
		tmpList.removeAt(index);
		setMetaData("minimums", tmpList);
		tmpList = metaData("maximums").toStringList();
		tmpList.removeAt(index);
		setMetaData("maximums", tmpList);
		emit valuesChanged();
		return true;
	}
	else
		return false;
}

void AMControlSetInfo::copyFrom(AMControlSetInfo *copyFrom){
	while(count() > 0)
		removeControlAt(count()-1);
	if(copyFrom){
		setName(copyFrom->name());
		for(int x = 0; x < copyFrom->count(); x++)
			addControlAt(x, copyFrom->nameAt(x), copyFrom->valueAt(x), copyFrom->minimumAt(x), copyFrom->maximumAt(x));
	}
}
