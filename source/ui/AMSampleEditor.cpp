#include "AMSampleEditor.h"

#include <QVBoxLayout>
#include <QGridLayout>
#include <QTimer>

#include "dataman/AMDatabaseDefinition.h"
#include "ui/AMDetailedItemDelegate.h"

AMSampleEditor::AMSampleEditor(AMDatabase* db, QWidget *parent) :
	QWidget(parent)
{
	db_ = db;

	QVBoxLayout* vl = new QVBoxLayout();
	vl->setContentsMargins(12,12,12,12);
	vl->setSpacing(6);

	setLayout(vl);

	QGridLayout* gl = new QGridLayout();
	vl->addLayout(gl);

	QLabel* l = new QLabel("name");
	l->setObjectName("AMSampleEditorLabel");
	gl->addWidget(l, 0, 0);

	sampleName_ = new QLineEdit();
	sampleName_->setFrame(false);
	gl->addWidget(sampleName_, 0, 1);

	l = new QLabel("date");
	l->setObjectName("AMSampleEditorLabel");
	gl->addWidget(l, 1, 0);
	sampleDate_ = new QLabel();
	gl->addWidget(sampleDate_, 1, 1);

	l = new QLabel("time");
	l->setObjectName("AMSampleEditorLabel");
	gl->addWidget(l, 2, 0);
	sampleTime_ = new QLabel();
	gl->addWidget(sampleTime_, 2, 1);

	l = new QLabel("elements");
	l->setObjectName("AMSampleEditorLabel");
	gl->addWidget(l, 3, 0);

	sampleElements_ = new QLineEdit();
	sampleElements_->setFrame(false);
	gl->addWidget(sampleElements_, 3, 1);

	l = new QLabel("Choose a different sample...");
	// l->setObjectName("AMSampleEditorLabel");
	gl->addWidget(l, 4, 0, 1, 2);

	sampleSelector_ = new QComboBox();
	sampleSelector_->setItemDelegate(new AMDetailedItemDelegate(this));
	vl->addWidget(sampleSelector_);

	vl->addSpacing(24);
	vl->addStretch(0);

	setStyleSheet( "#AMSampleEditorLabel {"
			"color: rgb(121, 121, 121);"
			"font-weight: bold;"
			"font-family: \"Lucida Grande\"; }");

	sample_ = new AMSample(this);
	newSampleActive_ = false;

	// Make connections:
	connect(sampleSelector_, SIGNAL(activated(int)), this, SLOT(onCBCurrentIndexChanged(int)));

	connect(db_, SIGNAL(created(QString,int)), this, SLOT(onDatabaseCreated(QString,int)));
	connect(db_, SIGNAL(updated(QString,int)), this, SLOT(onDatabaseUpdated(QString,int)));
	connect(db_, SIGNAL(removed(QString,int)), this, SLOT(onDatabaseRemoved(QString,int)));

	refreshScheduled_ = false;
	onDatabaseUpdated(AMDatabaseDefinition::sampleTableName(), -1);
}


/// Returns the id of the currently-selected sample, or -1 if it's a non-existent/invalid sample
int AMSampleEditor::currentSample() const {
	if(sample_->id() > 0)
		return sample_->id();
	else
		return -1;
}


/// Set the currently-selected sample, by ID
void AMSampleEditor::setCurrentSample(int id) {

	if(sample_->loadFromDb(db_, id)) {
		sampleName_->setReadOnly(false);
		sampleName_->setText(sample_->name());
		sampleDate_->setText(sample_->dateTime().toString("MMM d (yyyy)"));
		sampleTime_->setText(sample_->dateTime().toString("h:mmap"));
		// todo: elements .  bwaa?
		// todo: notes

		// don't cause infinite loop here... disable signal first?
		sampleSelector_->blockSignals(true);
		if(sampleId2Index_.contains(id))
			sampleSelector_->setCurrentIndex( sampleId2Index_.value(id) );
		sampleSelector_->blockSignals(false);

		if(newSampleActive_) {
			sampleName_->setFocus();
			sampleName_->selectAll();
		}

		emit currentSampleChanged(id);
	}
	else {
		sampleName_->setText("[no sample]");
		sampleName_->setReadOnly(true);
		sampleDate_->setText(QString());
		sampleTime_->setText(QString());
		sampleElements_->setText(QString());
		// todo: notes

		emit currentSampleChanged(-1);
	}

	newSampleActive_ = false;
}

#include <QDebug>

/// Call this to refresh the list of samples in the ComboBox from the database
void AMSampleEditor::refreshSamples() {

	qDebug() << "Calling AMSampleEditor::refreshSamples";

	// Refresh-scheduled flag is now turned off, because we're completing the refresh
	refreshScheduled_ = false;

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
	q.prepare(QString("SELECT id,name,dateTime FROM %1 ORDER BY dateTime DESC").arg(AMDatabaseDefinition::sampleTableName()));
	q.exec();
	int index = 0;
	while(q.next()) {
		index++;
		sampleSelector_->addItem(q.value(1).toString());
		sampleSelector_->setItemData(index, q.value(0).toInt(), AM::IdRole);
		sampleSelector_->setItemData(index, q.value(2), AM::DateTimeRole);
		sampleSelector_->setItemData(index, QString("created ") + q.value(2).toDateTime().toString("h:mmap, MMM d (yyyy)"), AM::DescriptionRole);
		// todo: decorations... thumbnails?
		sampleId2Index_.insert(q.value(0).toInt(), index);
	}

	sampleSelector_->blockSignals(false);

	// Any changes to our existing sample?
	setCurrentSample(currentSample());

}



/// This is called when a row in the database is updated/created/removed:
/*! In the future, it might be good to detect when our current sample has been deleted from the DB, and handle that situation intelligently.  Right now what happens is that the refreshSamples() will run like always; at the end, the loadFromDb() for the current sample will fail, it will become an invalid sample. */
void AMSampleEditor::onDatabaseUpdated(const QString& tableName, int id) {
	Q_UNUSED(id);

	if(tableName == AMDatabaseDefinition::sampleTableName() && !refreshScheduled_) {
		refreshScheduled_ = true;
		QTimer::singleShot(0, this, SLOT(refreshSamples()));
	}
}


/// Called when the current index of the combo box changes, indicating new sample selected
void AMSampleEditor::onCBCurrentIndexChanged(int index) {

	// Top index is the 'add new sample' entry
	if(index == 0)
		createNewSample();

	else
		setCurrentSample(sampleSelector_->itemData(index, AM::IdRole).toInt());

}

void AMSampleEditor::createNewSample() {
	static int sampleNum = 1;

	delete sample_;
	sample_ = new AMSample(QString("New Sample %1").arg(sampleNum++), this);
	sample_->setDateTime(QDateTime::currentDateTime());
	sample_->storeToDb(db_);
	newSampleActive_ = true;
	// this will trigger a database update... causing re-loading of the combo-box list, followed by re-setting the current sample id as the current sample. Luckily, the deferred processing of the onDatabaseUpdate() --> refreshSamples() link will let sample_->id() become set by storeToDb() before refreshSamples() is called. That's fantastic, because it means this sample will become the current sample automatically.
}
