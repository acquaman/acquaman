#include "AMRunSelector.h"
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QString>
#include <QList>
#include <QPixmap>
#include <QListView>

#include "acquaman.h"

#include "ui/AMDetailedItemDelegate.h"

AMRunSelector:: AMRunSelector(QWidget *parent)
	: QComboBox(parent)
{

	QListView* v = new QListView();
	setView(v);
	v->setItemDelegate(new AMDetailedItemDelegate());
	v->setResizeMode(QListView::Adjust);

	/* will need from database:
	   * run name
	   * run date
	   * run id
	*************/
	database_ = AMDatabase::userdb();
	autoAddRuns();

	if (count() > 1)
		setCurrentIndex(1);

	box = 0;
	connect(this, SIGNAL(activated(int)),this,SLOT(onComboBoxActivated(int)));

	setMinimumContentsLength(4);

	runUpdateScheduled_ = false;
	connect(database_, SIGNAL(created(QString,int)), this, SLOT(onDatabaseUpdate(QString, int)));
	connect(database_, SIGNAL(updated(QString,int)), this, SLOT(onDatabaseUpdate(QString, int)));

}



AMRunSelector::~AMRunSelector(){

}

/// This function searches through the database for existing runs and adds the run names into the database. In addition, this function also stores the facility thumbnail as a decoration role, the run id as a user role, and the facility description as a tool tip role.
void AMRunSelector::autoAddRuns() {

	// Turn off this flag because we're completing the update right now.
	runUpdateScheduled_ = false;

	clear();

	addItem("Add New Run...");
	setItemData(0, -1, AM::IdRole);  // IdRole stores the runId.  This entry doesn't represent a valid run, so the runId is -1.

	int i = 1;
	QSqlQuery q = database_->query();
	q.prepare(QString("SELECT Runs.id,Runs.name,Runs.dateTime,Facilities.description,Thumbnails.type,Thumbnails.thumbnail "
					  "FROM Runs,Facilities,Thumbnails "
					  "WHERE Runs.facilityId = Facilities.id AND Thumbnails.id = Facilities.thumbnailFirstId "
					  "ORDER BY Runs.dateTime DESC"));

	if (q.exec()) {
		while (q.next()){
			addItem(QString(q.value(1).toString()));
			setItemData(i, q.value(2).toDateTime().toString(" MMM d (yyyy)"), AM::DescriptionRole);
			setItemData(i, q.value(2), AM::DateTimeRole);
			if(q.value(4).toString() == "PNG") {
				QPixmap p;
				if(p.loadFromData(q.value(5).toByteArray(), "PNG"))
					setItemData(i, p.scaledToHeight(22, Qt::SmoothTransformation), Qt::DecorationRole);
			}

			setItemData(i,q.value(3).toString(), Qt::ToolTipRole);
			setItemData(i,q.value(0).toInt(), AM::IdRole);

			i++;
		}
	}
	else
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, 0, "Error retrieving run information from the database."));
	setCurrentIndex(1);

}

//need to create a function that will allow user to add runs manually to put in the public section

/// This function is called upon every time a combo box component is activated. It outputs the index of the activated component.
void AMRunSelector::onComboBoxActivated(int index) {

	if (index==0){
		newRunBox();
	}
	else
		emit currentRunIdChanged(itemData(index, AM::IdRole).toInt());

}

/// This function returns the ID of the run that is currently selected in the this combo box
int AMRunSelector::currentRunId() const {
	return itemData(currentIndex(), AM::IdRole).toInt();
}

/// This function opens a newRunDialog box and will activate the addNewRun function when the dialog box is closed.
void AMRunSelector::newRunBox() {
	if(box)
		delete box;
	box = new AMNewRunDialog;
	box->show();

	connect(box,SIGNAL(dialogBoxClosed()),this,SLOT(addNewRun()));
}

/// This function adds all runs from the database and sets the combo box index to select the 1st run down.
void AMRunSelector::addNewRun(){


	autoAddRuns();
	setCurrentIndex(2);
}

#include "dataman/AMDatabaseDefinition.h"
#include <QTimer>
void AMRunSelector::onDatabaseUpdate(const QString & tableName, int id) {

	Q_UNUSED(id)

	if(tableName == AMDatabaseDefinition::runTableName()) {
		if(!runUpdateScheduled_) {
			runUpdateScheduled_ = true;
			QTimer::singleShot(0, this, SLOT(autoAddRuns()));
		}
	}
}

/// This function sets the combo box selector to the run that has the designated runId. If not a valid runId, then this function does nothing.
void AMRunSelector::setCurrentRunId(int runId) {
	if (findData(runId, AM::IdRole)>0){
		setCurrentIndex(findData(runId, AM::IdRole));
		emit currentRunIdChanged(itemData(currentIndex(), AM::IdRole).toInt());
	}
}
