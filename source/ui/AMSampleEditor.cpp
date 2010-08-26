#include "AMSampleEditor.h"

#include <QVBoxLayout>
#include <QGridLayout>

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
	vl->addWidget(sampleSelector_);

	vl->addSpacing(24);
	vl->addStretch(0);

	setStyleSheet( "#AMSampleEditorLabel {"
			"color: rgb(121, 121, 121);"
			"font-weight: bold;"
			"font-family: \"Lucida Grande\"; }");



	// Make connections:
	connect(sampleSelector_, SIGNAL(currentIndexChanged(int)), this, SLOT(onCBCurrentIndexChanged(int)));

	connect(db_, SIGNAL(created(QString,int)), this, SLOT(onDatabaseCreated(QString,int)));
	connect(db_, SIGNAL(updated(QString,int)), this, SLOT(onDatabaseUpdated(QString,int)));
	connect(db_, SIGNAL(removed(QString,int)), this, SLOT(onDatabaseRemoved(QString,int)));
}


/// Returns the id of the currently-selected sample
int AMSampleEditor::currentSample() const {
	if(sample_.id() > 0)
		return sample_.id();
	else
		return -1;
}


/// Set the currently-selected sample, by ID
void AMSampleEditor::setCurrentSample(int id) {

}


/// Call this to refresh the list of samples in the ComboBox from the database
void AMSampleEditor::refreshSamples() {

}

/// This is called when a row in the database is updated:
void AMSampleEditor::onDatabaseUpdated(const QString& tableName, int id) {

}

/// This is called when a row in the database is created:
void AMSampleEditor::onDatabaseCreated(const QString& tableName, int id) {

}

void AMSampleEditor::onDatabaseRemoved(const QString& tableName, int id) {

}

/// Called when the current index of the combo box changes, indicating new sample selected
void AMSampleEditor::onCBCurrentIndexChanged(int) {

}
