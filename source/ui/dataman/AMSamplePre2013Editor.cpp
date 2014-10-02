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


#include "AMSamplePre2013Editor.h"

#include <QComboBox>
#include <QLineEdit>
#include <QLabel>

#include "dataman/AMSamplePre2013.h"
#include "dataman/database/AMDatabase.h"

#include <QVBoxLayout>
#include <QGridLayout>
#include <QTimer>

#include "dataman/database/AMDbObjectSupport.h"
#include "ui/AMDetailedItemDelegate.h"

//#include "ui/AMElementListEdit.h"
#include <QListView>

AMSamplePre2013Editor::AMSamplePre2013Editor(AMDatabase* db, QWidget *parent) :
		QWidget(parent)
{
	db_ = db;
	sampleTableName_ = AMDbObjectSupport::s()->tableNameForClass<AMSamplePre2013>();

	vl_ = new QVBoxLayout();
	vl_->setContentsMargins(0, 0, 0, 0);
	vl_->setSpacing(6);

	setLayout(vl_);

	sampleSelector_ = new QComboBox();
	QListView* lview = new QListView();
	AMDetailedItemDelegate* del = new AMDetailedItemDelegate(this);
	del->setCloseButtonsEnabled(true);
	connect(del, SIGNAL(closeButtonClicked(QModelIndex)), this, SLOT(onSampleDeleteButtonClicked(QModelIndex)));
	lview->setItemDelegate(del);
	lview->setAlternatingRowColors(true);
	sampleSelector_->setView(lview);
	vl_->addWidget(sampleSelector_);


	QGridLayout* gl = new QGridLayout();
	vl_->addLayout(gl);

	QLabel* l = new QLabel("name");
	l->setObjectName("AMSampleEditorLabel");
	gl->addWidget(l, 0, 0);

	sampleName_ = new QLineEdit();
	gl->addWidget(sampleName_, 0, 1);

	l = new QLabel("created");
	l->setObjectName("AMSampleEditorLabel");
	gl->addWidget(l, 1, 0);
	sampleDate_ = new QLabel();
	gl->addWidget(sampleDate_, 1, 1);

	l = new QLabel("elements");
	l->setObjectName("AMSampleEditorLabel");
	gl->addWidget(l, 2, 0);

	sampleElements_ = new QLineEdit();
	gl->addWidget(sampleElements_, 2, 1);

	setStyleSheet( "#AMSampleEditorLabel {"
				   "color: rgb(121, 121, 121);"
				   "font-weight: bold;"
				   "font-family: \"Lucida Grande\"; }");

	sample_ = new AMSamplePre2013(this);
	newSampleActive_ = false;

	// Make connections:
	// responds to switching between samples, using the combo box
	connect(sampleSelector_, SIGNAL(activated(int)), this, SLOT(onCBCurrentIndexChanged(int)));

	// responds to updates from the database
	connect(db_, SIGNAL(created(QString,int)), this, SLOT(onDatabaseCreated(QString,int)));
	connect(db_, SIGNAL(updated(QString,int)), this, SLOT(onDatabaseUpdated(QString,int)));
	connect(db_, SIGNAL(removed(QString,int)), this, SLOT(onDatabaseRemoved(QString,int)));

	// responds to changes in the editor fields:
	connect(sampleName_, SIGNAL(editingFinished()), this, SLOT(saveCurrentSample()));
	connect(sampleElements_, SIGNAL(editingFinished()), this, SLOT(saveCurrentSample()));

	refreshScheduled_ = false;
	onDatabaseUpdated(sampleTableName_, -1);
}


/// Returns the id of the currently-selected sample, or -1 if it's a non-existent/invalid sample
int AMSamplePre2013Editor::currentSampleId() const {
	if(sample_->id() > 0)
		return sample_->id();
	else
		return -1;
}


/// Set the currently-selected sample, by ID
void AMSamplePre2013Editor::setCurrentSampleFromId(int id) {

	int oldSampleId = currentSampleId();

	if(sample_->loadFromDb(db_, id)) {
		sampleName_->setReadOnly(false);
		sampleName_->setText(sample_->name());
		sampleDate_->setText(AMDateTimeUtils::prettyDateTime(sample_->dateTime()));

		sampleElements_->setText(sample_->elementString());


		// don't cause infinite loop here... disable signal first?
		sampleSelector_->blockSignals(true);
		if(sampleId2Index_.contains(id))
			sampleSelector_->setCurrentIndex( sampleId2Index_.value(id) );
		sampleSelector_->blockSignals(false);

		if(newSampleActive_) {
			sampleName_->setFocus();
			sampleName_->selectAll();
		}

		if(newSampleActive_ || id != oldSampleId){
			emit currentSampleChanged(id);
			emit currentSamplePointerChanged(sample_);
		}
	}
	else {
		sampleName_->setText("[no sample]");
		sampleName_->setReadOnly(true);
		sampleDate_->setText(QString());
		sampleElements_->setText(QString());
		// todo: notes

		// don't cause infinite loop here... disable signal first?
		sampleSelector_->blockSignals(true);
		sampleSelector_->setCurrentIndex(-1);
		sampleSelector_->blockSignals(false);


		if(oldSampleId != -1){
			emit currentSampleChanged(-1);
			emit currentSamplePointerChanged(0);
		}
	}

	newSampleActive_ = false;
}

/// Call this to refresh the list of samples in the ComboBox from the database
void AMSamplePre2013Editor::refreshSamples() {


	// Refresh-scheduled flag is now turned off, because we're completing the refresh
	refreshScheduled_ = false;

	// Mode 1: Precision update of a single sample:
	///////////////////////////////////////////
	if(refreshId_ > 0 && sampleId2Index_.contains(refreshId_)) {

		int index = sampleId2Index_.value(refreshId_);
		QSqlQuery q = db_->query();
		q.prepare(QString("SELECT id,name,dateTime FROM %1 WHERE id = ?").arg(sampleTableName_));
		q.bindValue(0, refreshId_);
		if(q.exec() && q.first()) {
			sampleSelector_->setItemData(index, q.value(1).toString(), Qt::DisplayRole);
			sampleSelector_->setItemData(index, q.value(2), AM::DateTimeRole);
			sampleSelector_->setItemData(index, QString("created ") + AMDateTimeUtils::prettyDateTime(q.value(2).toDateTime()), AM::DescriptionRole);
			// todo: decorations... thumbnails...
		}
	}


	// Full model update, from scratch:
	/////////////////////////////////////
	else {

		// clear the old map from sample IDs to combo-box indexes
		sampleId2Index_.clear();

		// block the signals out of the combo box for now, while we fill it. Don't want to bother with handling currentIndexChanged() until this is all over...
		sampleSelector_->blockSignals(true);

		// clear the old combo box entries
		sampleSelector_->clear();

		// Add the first entry, which is always "create new sample".
		sampleSelector_->addItem("Create New Sample");

		// Fill the combo box with all samples in the db:
		QSqlQuery q = db_->query();
		q.prepare(QString("SELECT id,name,dateTime FROM %1 ORDER BY dateTime DESC").arg(sampleTableName_));
		q.exec();
		int index = 0;
		while(q.next()) {
			index++;
			sampleSelector_->addItem(q.value(1).toString());
			sampleSelector_->setItemData(index, true, AM::CanCloseRole);	// enables views to show the close/delete button
			sampleSelector_->setItemData(index, q.value(0).toInt(), AM::IdRole);
			sampleSelector_->setItemData(index, q.value(2), AM::DateTimeRole);
			sampleSelector_->setItemData(index, QString("created ") + AMDateTimeUtils::prettyDateTime(q.value(2).toDateTime()), AM::DescriptionRole);
			// todo: decorations... thumbnails?
			sampleId2Index_.insert(q.value(0).toInt(), index);
		}

		sampleSelector_->blockSignals(false);
	}


	// Do we need to update the info on the existing sample?
	int currentSamp = currentSampleId();
	if(refreshId_ == currentSamp || refreshId_ < 1)
		setCurrentSampleFromId(currentSamp);

	refreshId_ = -1;
}



/// This is called when a row in the database is updated/created/removed:
/*! In the future, it might be good to detect when our current sample has been deleted from the DB, and handle that situation intelligently.  Right now what happens is that the refreshSamples() will run like always; at the end, the loadFromDb() for the current sample will fail, it will become an invalid sample. */
void AMSamplePre2013Editor::onDatabaseUpdated(const QString& tableName, int id) {
	Q_UNUSED(id);

	if(tableName != sampleTableName_)
		return;

	// single-row precision update? That's cool if this :
	// is a valid id
	// AND we have this id already in the model
	// AND (it's the first/only update scheduled, OR there's an update scheduled already but its for the same id)
	if( id > 0
		&& sampleId2Index_.contains(id)
		&& (!refreshScheduled_ || (refreshScheduled_ && refreshId_ == id) )
		)
		refreshId_ = id;
	else
		refreshId_ = -1;

	if(!refreshScheduled_) {
		refreshScheduled_ = true;
		QTimer::singleShot(0, this, SLOT(refreshSamples()));
	}

}


/// Called when the current index of the combo box changes, indicating new sample selected
void AMSamplePre2013Editor::onCBCurrentIndexChanged(int index) {

	// Top index is the 'add new sample' entry
	if(index == 0)
		createNewSample();

	else
		setCurrentSampleFromId(sampleSelector_->itemData(index, AM::IdRole).toInt());

}

void AMSamplePre2013Editor::createNewSample() {
	static int sampleNum = 1;

	delete sample_;
	sample_ = new AMSamplePre2013(QString("New Sample %1").arg(sampleNum++), this);
	sample_->setDateTime(QDateTime::currentDateTime());
	sample_->storeToDb(db_);
	newSampleActive_ = true;
	// this will trigger a database update... causing re-loading of the combo-box list, followed by re-setting the current sample id as the current sample. Luckily, the deferred processing of the onDatabaseUpdate() --> refreshSamples() link will let sample_->id() become set by storeToDb() before refreshSamples() is called. That's fantastic, because it means this sample will become the current sample automatically.
}

void AMSamplePre2013Editor::saveCurrentSample() {

	/// clear focus on the editors. \todo Move this to subclass of qlineedit?
	sampleName_->blockSignals(true);
	sampleElements_->blockSignals(true);
	sampleName_->clearFocus();
	sampleElements_->clearFocus();
	sampleName_->blockSignals(false);
	sampleElements_->blockSignals(false);

	if(currentSampleId() > 0) {
		sample_->setName(sampleName_->text());
		sample_->setElementList(parseElementString(sampleElements_->text()));
		sample_->storeToDb(db_);
	}
}

#include "util/AMPeriodicTable.h"
QList<int> AMSamplePre2013Editor::parseElementString(const QString &elementString) {
	QList<int> rv;


	QStringList elements = elementString.split(QRegExp("[\\,\\;\\s]+"), QString::SkipEmptyParts);

	foreach(QString s, elements) {
		s = s.toLower();
		s = s.left(1).toUpper() + s.mid(1);	// Capitalize as Cl, Chlorine, etc.
		const AMElement* element;
		if((element = AMPeriodicTable::table()->elementByName(s)))
			rv << element->atomicNumber();
		else if((element = AMPeriodicTable::table()->elementBySymbol(s)))
			rv << element->atomicNumber();
	}

	return rv;
	/// \todo Validator / red-green hinting for element editor box
}

#include <QMessageBox>
void AMSamplePre2013Editor::onSampleDeleteButtonClicked(const QModelIndex &index) {

	if(!index.isValid() || index.row() < 1)
		return;

	int sampleId = sampleSelector_->itemData(index.row(), AM::IdRole).toInt();

	QString sampleName = sampleSelector_->itemData(index.row(), Qt::DisplayRole).toString();
	QString dt = AMDateTimeUtils::prettyDateTime(sampleSelector_->itemData(index.row(), AM::DateTimeRole).toDateTime());

	QMessageBox confirmBox(this);
	confirmBox.setText(QString("Delete the sample '%1'', created %2?").arg(sampleName).arg(dt));
	confirmBox.setInformativeText("All scans on this sample will be kept, but they will lose their affiliation with this sample.");
	confirmBox.setIcon(QMessageBox::Question);
	confirmBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
	if(confirmBox.exec() == QMessageBox::Ok)
		AMSamplePre2013::destroySample(db_, sampleId);
}
