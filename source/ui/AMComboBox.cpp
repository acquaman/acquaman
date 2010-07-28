#include "AMComboBox.h"
#include <QSqlQuery>
#include <QString>
#include <QList>
AMComboBox:: AMComboBox(QWidget *parent)
	: QComboBox(parent)
{
/// Getting required information from database
	/* will need from database:
	   * run name
	   * run date
	   * run id
	*************/
	autoAddRuns();
	database_ = AMDatabase::userdb();
}

AMComboBox::~AMComboBox(){

}

// Searching and obtaining from the database, whatever is specified in the column name in a string array
QList<QString> AMComboBox::searchDbRuns(const QString& tableName, const QString& colName) const{

	QList<QString> rv;

	if (database()==0)
		return QList<QString>();

	QVariant value;
	value = "?";
	QSqlQuery q = database()->query();

	q.prepare (QString("SELECT %1 FROM %2 ").arg(colName).arg(tableName));
	q.bindValue(0, value);
	if (q.exec())
		while (q.next()) {
		rv << q.value(0).toString();
		}

	return rv;
}

void AMComboBox::autoAddRuns(){

	// Checking that database exists:


	//getting different items from database
	/*QList<QString> runName = searchDbRuns("Runs","name");
	QList<QString> runDate = searchDbRuns("Runs", "dateTime");
	QList<QString> runId = searchDbRuns("Runs", "id"); */
	//int i = numberOfRuns;
	//int i = runCount();
	QList<QString> runName;
	runName<<"run1"<<"run2"<<"run3";
	QList<QString> runDate;
	runDate<<"Jan 21, 2010"<<"Feb 1, 2010"<<"Mar 6, 2010";


	int i = runName.count();
	//putting those items into the combobox one by one while collating


	for (int j=0; j<i; j++) {
		QString item = runName.at(j);
		item.append(" , ");
		item.append(runDate.at(j));
		//item.append(" , ");
		//item.append(runId.at(j));
		addItem(item);

	}
}

//need to create a function that will allow user to add runs manually to put in the public section
