#include "AMComboBox.h"
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QString>
#include <QList>
#include <QPixmap>

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

	if (count() > 1)
		setCurrentIndex(1);

	connect(this, SIGNAL(activated(int)),this,SLOT(onComboBoxActivated(int)));

}

AMComboBox::~AMComboBox(){

}

// Searching and obtaining from the database, whatever is specified in the column name in a string array


int AMComboBox::addRun() {

	//getting text from input box
	bool ok;
	QString text = QInputDialog::getText(this, tr("Add Run"),
		tr("Please enter the name of the new run:"), QLineEdit::Normal,QString(), &ok);
	if (ok && !text.isEmpty()){
			AMRun newRun(text);
			newRun.storeToDb(database_);

			return newRun.id();
	}
	else
		return -1;
}

void AMComboBox::autoAddRuns() {

	clear();

	addItem("Add New Run...");
	setItemData(0, -1, Qt::UserRole);  // UserRole stores the runId.  This entry doesn't represent a valid run, so the runId is -1.

	int i = 1;
	QSqlQuery q = database_->query();
	q.prepare(QString("SELECT Runs.id,Runs.name,Runs.dateTime,Facilities.description,Thumbnails.type,Thumbnails.thumbnail "
					  "FROM Runs,Facilities,Thumbnails "
					  "WHERE Runs.facilityId = Facilities.id AND Thumbnails.id = Facilities.thumbnailFirstId "
					  "ORDER BY Runs.dateTime DESC"));

	if (q.exec()) {
		while (q.next()) {
			addItem(QString(q.value(1).toString()+" , "+q.value(2).toDateTime().toString(" MMM d (yyyy)")));
			if(q.value(4).toString() == "PNG") {
				QPixmap p;
				if(p.loadFromData(q.value(5).toByteArray(), "PNG"))
					setItemData(i, p.scaledToHeight(22, Qt::SmoothTransformation), Qt::DecorationRole);
			}

			setItemData(i,q.value(3).toString(), Qt::ToolTipRole);
			setItemData(i,q.value(0).toInt(), Qt::UserRole);

			i++;
		}
	}
	else
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, 0, "Error retrieving run information from the database."));

}

//need to create a function that will allow user to add runs manually to put in the public section

void AMComboBox::onComboBoxActivated(int index) {

	if (index==0){
		int id=addRun();
		autoAddRuns();
		setCurrentIndex(findData(id,Qt::UserRole));
	}

	emit currentRunIdChanged(itemData(index, Qt::UserRole).toInt());

}

int AMComboBox::currentRunId() const {
	return itemData(currentIndex(), Qt::UserRole).toInt();
}


