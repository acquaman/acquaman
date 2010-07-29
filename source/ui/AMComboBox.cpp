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

	clear();

	addItem("Add New Run...");

	int i = 1;
	QSqlQuery q = database_->query();
	q.prepare(QString("SELECT Runs.runId,Runs.name,Runs.dateTime,Facilities.description,Thumbnails.thumbnailType,Thumbnails.thumbnail "
					  "FROM Runs,Facilities,Thumbnails "
					  "WHERE Runs.facilityId = Facilities.id AND Thumbnails.id = Facilities.thumbnailFirstId "
					  "ORDER BY Runs.dateTime DESC"));

	if (q.exec())
		while (q.next()) {
			addItem(q.value(1).toString());
			i++;
		}

}

//need to create a function that will allow user to add runs manually to put in the public section

void AMComboBox::onComboBoxActivated(int index) {
	if (index==0){
		addRun();
		autoAddRuns();
	}
}

