#include "ComboBox.h"
#include <QSqlQuery>

ComboBox:: comboBox(QWidget *parent)
	: QComboBox(parent)
{
/// Getting required information from database
	/* will need from database:
	   * run name
	   * run date
	   * run id
	*/
	//Checking that database exists:
	if (database()==0)
		return 0;
	QList <string> runName = searchDbRuns(Runs,name);
	QList <string> runDate = searchDbRuns(Runs, dateTime);
	QList <string> runId = searchDbRuns(Runs, id);

	for (int j=0; j<i; j++)
		string& item = runName++;
		comboBox->addItem(item);

}
	// acquiring run name (will be in String format)


QList <string> ComboBox::searchDbRuns(const QString& tableName, const QString& colName) const{

	QList<string> rv;

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




