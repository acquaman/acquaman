/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "AMRunSelector.h"
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QString>
#include <QList>
#include <QPixmap>
#include <QListView>

#include "acquaman.h"
#include "util/AMErrorMonitor.h"

#include "ui/AMDetailedItemDelegate.h"

#include "dataman/database/AMDbObjectSupport.h"

AMRunSelector:: AMRunSelector(AMDatabase* db, QWidget *parent)
	: QComboBox(parent)
{

	QListView* v = new QListView();
	setView(v);
	itemDelegate_ = new AMDetailedItemDelegate();
	v->setItemDelegate(itemDelegate_);
	v->setResizeMode(QListView::Adjust);

	database_ = db;
	lastValidRunId_ = -1;
	populateRuns();

	if (count() > 1)
		setCurrentIndex(1);

	newRunDialog_ = 0;
	connect(this, SIGNAL(activated(int)),this,SLOT(onComboBoxActivated(int)));

	setMinimumContentsLength(4);

	runUpdateScheduled_ = false;
	connect(database_, SIGNAL(created(QString,int)), this, SLOT(onDatabaseUpdate(QString, int)));
	connect(database_, SIGNAL(updated(QString,int)), this, SLOT(onDatabaseUpdate(QString, int)));

}



AMRunSelector::~AMRunSelector(){
	itemDelegate_->deleteLater();
	if(newRunDialog_)
		newRunDialog_->deleteLater();
}

/// This function searches through the database for existing runs and adds the run names into the database. In addition, this function also stores the facility thumbnail as a decoration role, the run id as a user role, and the facility description as a tool tip role. \todo Performance question: check how often this is getting called. It might be being called more often than necessary.
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

	/* NTBA - September 1st, 2011 (David Chevrier)
	"Hard-coded database table names. Down to only "AMDbObjectThumbnails_table."
	*/
	// get the Thumbnails
	q.prepare(QString("SELECT %1.id,%1.name,%1.description,AMDbObjectThumbnails_table.type,AMDbObjectThumbnails_table.thumbnail "
					"FROM %1,AMDbObjectThumbnails_table "
					"WHERE %1.name=AMDbObjectThumbnails_table.title"
				).arg(AMDbObjectSupport::s()->tableNameForClass<AMFacility>()));

	// Load the facility information and the facility icons
	bool facilityIconLoaded = false;
	QString facilityDescrition = "";
	QPixmap faciliytIcon;
	if (q.exec() && q.next()) {
		facilityDescrition = q.value(2).toString();
		if(q.value(3).toString() == "PNG")
			facilityIconLoaded = faciliytIcon.loadFromData(q.value(4).toByteArray(), "PNG");
	}

	// load the AMRuns
	q.prepare(QString("SELECT %1.id,%1.name,%1.dateTime "
						"FROM %1 "
						"ORDER BY %1.dateTime DESC").arg(AMDbObjectSupport::s()->tableNameForClass<AMRun>()));
	if (q.exec()) {
		while (q.next()){
			addItem(QString("%1").arg(q.value(1).toString()));
			setItemData(i, QString("started %1, %2").arg(AMDateTimeUtils::prettyDate(q.value(2).toDateTime())).arg(facilityDescrition), AM::DescriptionRole);
			setItemData(i, q.value(2).toDateTime(), AM::DateTimeRole);
			if (facilityIconLoaded) {
				setItemData(i, faciliytIcon.scaledToHeight(22, Qt::SmoothTransformation), Qt::DecorationRole);
			}

			setItemData(i,q.value(2).toString(), Qt::ToolTipRole);
			setItemData(i,q.value(0).toInt(), AM::IdRole);
			i++;
		}
	}
	else {
		q.finish();
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, 0, "Error retrieving run information from the database."));
	}

	// restore the previous id. If we didn't have anything selected previously, and we have at least one valid run now, select it, and notify
	if(oldRunId == -1 && count() > 1) {
		lastValidRunId_ = itemData(1, AM::IdRole).toInt();
		setCurrentIndex(1);
		emit currentRunIdChanged(lastValidRunId_);
	}
	else {	// we had a previously valid run. It might have been moved (up or down) in index, or even deleted.  If we still have it, set it as the current index (but don't signal, because the current run id hasn't changed. It it's gone, select nothing, and notify that the current run id HAS changed.
		int indexOfOldRun = findIndexForRunId(oldRunId);
		if(indexOfOldRun == -1) {
			setCurrentIndex(-1);
			emit currentRunIdChanged(-1);
		}
		else
			setCurrentIndex(indexOfOldRun);
	}

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
	newRunDialog_ = new AMNewRunDialog(database_);
	newRunDialog_->show();
	connect(newRunDialog_, SIGNAL(dialogBoxClosed(int)), this, SLOT(onAddRunDialogClosed(int)));
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

#include "dataman/database/AMDbObjectSupport.h"
#include <QTimer>
void AMRunSelector::onDatabaseUpdate(const QString & tableName, int id) {

	Q_UNUSED(id)

	if(tableName == AMDbObjectSupport::s()->tableNameForClass<AMRun>()) {
		if(!runUpdateScheduled_) {
			runUpdateScheduled_ = true;
			QTimer::singleShot(0, this, SLOT(populateRuns()));
		}
	}
}

/// This function sets the combo box selector to the run that has the designated runId. If the run id is invalid (ie: runId<1, or non-existent), it sets the combo box to have nothing selected, and emits the currentRunIdChanged() signal with an invalid value (-1).
void AMRunSelector::setCurrentRunId(int runId) {
	int oldRunId = currentRunId();

	int runIndex = findIndexForRunId(runId);

	setCurrentIndex(runIndex);

	if(runIndex == -1)
		emit currentRunIdChanged(-1);

	else if(runId != oldRunId)
		emit currentRunIdChanged(runId);

}

int AMRunSelector::findIndexForRunId(int runId) const {
	int runIndex;

	if(runId < 1 || (runIndex = findData(runId, AM::IdRole)) == -1 )
		return -1;

	return runIndex;
}
