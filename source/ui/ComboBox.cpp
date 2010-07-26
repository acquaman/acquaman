#include "ComboBox.h"
#include <QSqlQuery>
#include <QString>
#include <QList>
ComboBox:: ComboBox(QWidget *parent)
	: QComboBox(parent)
{
/// Getting required information from database
	/* will need from database:
	   * run name
	   * run date
	   * run id
	*************/


	// Setting up UI:
	/////////////////////

}

// Searching and obtaining from the database, whatever is specified in the column name in a string array
QList<QString> ComboBox::searchDbRuns(const QString& tableName, const QString& colName) const{

	QList<QString> rv;

	if (database()==0)
		return QList<QString>();

	QSqlQuery q = database()->query();

	q.prepare (QString("SELECT %1 FROM %2 ").arg(colName).arg(tableName));
	q.bindValue(0, value);
	if (q.exec)
		while (q.next()) {
		rv<<q.value(0).toString;
		int i++;    // Will this variable exist outside of this function?
		}

	return rv;
}

// Obtaining the number of runs in the database
int ComboBox::runCount(){
	return searchDbRuns().count();
}

void ComboBox::autoAddRuns(const int numberOfRuns){

	// Checking that database exists:


	//getting different items from database
	QList<QString> runName = searchDbRuns("Runs","name");
	QList<QString> runDate = searchDbRuns("Runs", "dateTime");
	QList<QString> runId = searchDbRuns("Runs", "id");
	int i = numberOfRuns;
	//putting those items into the combobox one by one while collating
	for (int j=0; j<=i; j++) {
		QString& item = runName++;
		item.append("\n");
		item.append(runDate++);
		item.append("\n");
		item.append(runId++);

		ComboBox->addItem(item);
	}
}

//need to create a function that will allow user to add runs manually to put in the public section
