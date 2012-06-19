#include "AMChangeRunActionEditor.h"

#include <QRadioButton>
#include <QBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include "ui/dataman/AMRunSelector.h"


AMChangeRunActionEditor::AMChangeRunActionEditor(AMChangeRunActionInfo *info, QWidget *parent) : QFrame(parent) {
	info_ = info;
	setFrameStyle(QFrame::StyledPanel);

	QRadioButton* newRunButton = new QRadioButton("Create New Run");
	QRadioButton* existingRunButton = new QRadioButton("Choose Existing Run");

	runSelector_ = new AMRunSelector(AMDatabase::database("user"));
	newNameEdit_ = new QLineEdit(info_->newRunName());


	QHBoxLayout* hl = new QHBoxLayout(this);
	QVBoxLayout* vl = new QVBoxLayout(this);

	vl->addWidget(newRunButton);
	vl->addWidget(existingRunButton);
	hl->addLayout(vl);
	hl->addWidget(runSelector_);
	hl->addWidget(newNameEdit_);

	if(info_->runId() < 1) {
		newRunButton->setChecked(true);
		existingRunButton->setChecked(false);
		runSelector_->hide();
	}
	else {
		newRunButton->setChecked(false);
		existingRunButton->setChecked(true);
		newNameEdit_->hide();
		runSelector_->setCurrentRunId(info_->runId());
	}


	connect(newRunButton, SIGNAL(toggled(bool)), this, SLOT(onNewRunRadioButtonToggled(bool)));
	connect(runSelector_, SIGNAL(currentRunIdChanged(int)), this, SLOT(onExistingRunChanged(int)));
	connect(newNameEdit_, SIGNAL(textChanged(QString)), this, SLOT(onNewRunNameChanged(QString)));
}

void AMChangeRunActionEditor::onNewRunRadioButtonToggled(bool newRun)
{
	runSelector_->setVisible(!newRun);
	newNameEdit_->setVisible(newRun);

	if(newRun) {
		runSelector_->hide();
		newNameEdit_->show();
		info_->setRunId(-1);
	}
	else {
		runSelector_->show();
		newNameEdit_->hide();
		info_->setRunId(runSelector_->currentRunId());
	}
}

void AMChangeRunActionEditor::onExistingRunChanged(int id)
{
	info_->setRunId(id);
}

void AMChangeRunActionEditor::onNewRunNameChanged(const QString &newRunName)
{
	info_->setNewRunName(newRunName);
}
