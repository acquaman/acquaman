#include "AMRunSelector.h"
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QString>
#include <QList>
#include <QPixmap>
#include <QListView>

#include "acquaman.h"

#include "ui/AMDetailedItemDelegate.h"

AMRunSelector:: AMRunSelector(AMDatabase* db, QWidget *parent)
	: QComboBox(parent)
{

	QListView* v = new QListView();
	setView(v);
	v->setItemDelegate(new AMDetailedItemDelegate());
	v->setResizeMode(QListView::Adjust);

	database_ = db;
	lastValidRunId_ = -1;
	populateRuns();

	if (count() > 1)
		setCurrentRunId(itemData(1, AM::IdRole).toInt());

	newRunDialog_ = 0;
	connect(this, SIGNAL(activated(int)),this,SLOT(onComboBoxActivated(int)));

	setMinimumContentsLength(4);

	runUpdateScheduled_ = false;
	connect(database_, SIGNAL(created(QString,int)), this, SLOT(onDatabaseUpdate(QString, int)));
	connect(database_, SIGNAL(updated(QString,int)), this, SLOT(onDatabaseUpdate(QString, int)));

}



AMRunSelector::~AMRunSelector(){

}

/// This function searches through the database for existing runs and adds the run names into the database. In addition, this function also stores the facility thumbnail as a decoration role, the run id as a user role, and the facility description as a tool tip role.
void AMRunSelector::populateRuns() {

	// Turn off this flag because we're completing the update right now.
	runUpdateScheduled_ = false;

	// old run index: (save for backup)
	int oldRunId = currentRunId();

	// empty all run entries
	clear();

	// The first entry is just a 'Add New Run...' entry.
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
			addItem(QString("%1, started %2").arg(q.value(1).toString()).arg(AMDateTimeUtils::prettyDate(q.value(2).toDateTime())));
			setItemData(i, q.value(3).toString() + " - " + q.value(2).toDateTime().toString("MMM d (yyyy)"), AM::DescriptionRole);
			setItemData(i, q.value(2).toDateTime(), AM::DateTimeRole);
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

	// restore. If we didn't have anything selected previously, and we have at least one valid run now, select it
	if(oldRunId < 1 && count() > 1) {
		setCurrentRunId(itemData(1, AM::IdRole).toInt());
	}
	else
		setCurrentRunId(oldRunId);

}

//need to create a function that will allow user to add runs manually to put in the public section

/// This function is called upon every time a combo box component is activated. It outputs the index of the activated component.
void AMRunSelector::onComboBoxActivated(int index) {

	if(index < 0)
		emit currentRunIdChanged(-1);
	else if (index==0)	// open dialogue to create a new run
		showAddRunDialog();
	else {
		lastValidRunId_ = itemData(index, AM::IdRole).toInt();
		emit currentRunIdChanged(lastValidRunId_);
	}

}

/// This function returns the ID of the run that is currently selected in the this combo box
int AMRunSelector::currentRunId() const {
	if(currentIndex() < 1)
		return -1;
	else
		return itemData(currentIndex(), AM::IdRole).toInt();
}

/// This function opens a new AddRunDialog box and will activate the onAddRunDialogClosed function when the dialog box is closed.
void AMRunSelector::showAddRunDialog() {
	if(newRunDialog_) {
		newRunDialog_->deleteLater();
		newRunDialog_ = 0;
	}
	newRunDialog_ = new AMNewRunDialog(database_, this);
	newRunDialog_->show();
	connect(newRunDialog_,SIGNAL(dialogBoxClosed(int)),this,SLOT(onAddRunDialogClosed(int)));
}

/// This function adds all runs from the database and sets the combo box index to select the 1st run down.
void AMRunSelector::onAddRunDialogClosed(int newRunId){

	if(newRunDialog_) {
		newRunDialog_->deleteLater();;
		newRunDialog_ = 0;
	}
	if(newRunId > 0) {
		populateRuns();
		setCurrentRunId(newRunId);
	}
	else {
		setCurrentRunId(lastValidRunId_);
	}
}

#include "dataman/AMDatabaseDefinition.h"
#include <QTimer>
void AMRunSelector::onDatabaseUpdate(const QString & tableName, int id) {

	Q_UNUSED(id)

	if(tableName == AMDatabaseDefinition::runTableName()) {
		if(!runUpdateScheduled_) {
			runUpdateScheduled_ = true;
			QTimer::singleShot(0, this, SLOT(populateRuns()));
		}
	}
}

/// This function sets the combo box selector to the run that has the designated runId. If the run id is invalid (ie: runId<1, or non-existent), it sets the combo box to have nothing selected, and emits the currentRunIdChanged() signal with an invalid value (-1).
void AMRunSelector::setCurrentRunId(int runId) {
	int runIndex;
	if(runId < 1 || (runIndex = findData(runId, AM::IdRole)) == -1 ) {
		setCurrentIndex(-1);
		emit currentRunIdChanged(-1);
	}
	else {
		setCurrentIndex(runIndex);
		emit currentRunIdChanged(runId);
	}
}
