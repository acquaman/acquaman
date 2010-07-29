#include "AMComboBox.h"
#include <QSqlQuery>
#include <QSqlDatabase>
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
	database_ = AMDatabase::userdb();
	autoAddRuns();
	connect(this, SIGNAL(activated(int)),this,SLOT(onComboBoxActivated(int)));

}

AMComboBox::~AMComboBox(){

}

// Searching and obtaining from the database, whatever is specified in the column name in a string array
QList<QString> AMComboBox::searchDbRuns(const QString& tableName, const QString& colName) const{

	QList<QString> rv;


	QVariant value;
	value = "?";


	QSqlQuery q = database_->query();
		q.prepare(QString("SELECT %1 FROM %2 ").arg(colName).arg(tableName));

	if (q.exec())
		while (q.next()) {
		rv << q.value(0).toString();
		}

	return rv;
}
void AMComboBox::addRun() {

	//getting text from input box
	bool ok;
	QString text = QInputDialog::getText(this, tr("Add Run"),
		tr("Please enter the name of the new run:"), QLineEdit::Normal,QString(), &ok);
	if (ok && !text.isEmpty()){
			AMRun newRun(text);
			newRun.storeToDb(database_);
	}
}

void AMComboBox::autoAddRuns() {

	// Checking that database exists:

	clear();
	//getting different items from database
	QList<QString> runName = searchDbRuns("Runs","name");
	QList<QString> runDate = searchDbRuns("Runs", "dateTime");
	//int i = numberOfRuns;
	//int i = runCount();

	int i = runName.count();
	//putting those items into the combobox one by one while collating

	addItem("Add New Run...");
	for (int j=0; j<i; j++) {
		QString item = runName.at(j);
		item.append(" , ");
		item.append(runDate.at(j));

		addItem(item);

	}

}

//need to create a function that will allow user to add runs manually to put in the public section

void AMComboBox::onComboBoxActivated(int index) {
	if (index==0){
		addRun();
		autoAddRuns();
	}
}

