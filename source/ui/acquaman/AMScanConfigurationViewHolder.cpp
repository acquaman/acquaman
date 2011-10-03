/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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

#include <QDebug>
#include <QRadioButton>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QButtonGroup>

#include "AMScanConfigurationViewHolder.h"
#include "actions/AMBeamlineScanAction.h"
#include "acquaman/AMScanConfiguration.h"
#include "ui/acquaman/AMScanConfigurationView.h"
#include "ui/AMWorkflowManagerView.h"

#include "dataman/AMXASScan.h"
#include "dataman/AMScanDictionary.h"
#include <QTimer>


AMScanConfigurationViewHolder::AMScanConfigurationViewHolder(AMWorkflowManagerView* workflow, AMScanConfigurationView* view, QWidget *parent) :
	QWidget(parent)
{
	view_ = view;
	workflow_ = workflow;

	testDefaultScan_ = 0; //NULL
	testDictionary_ = 0; //NULL

	scanNameLabel_ = new QLabel("Scan Name:");
	//scanNameLineEdit_ = new QLineEdit();
	scanNameLineEdit_ = new AMDictionaryLineEdit();
	scanNameExampleLabel_ = new QLabel("ex,, ");

	autoExportLabel_ = new QLabel("---->");
	doExportNameCheckBox_ = new QCheckBox("Set Name");
	doAutoExportCheckBox_ = new QCheckBox("Auto Export");

	exportNameLabel_ = new QLabel("Export Name:");
	exportNameDictionaryLineEdit_ = new AMDictionaryLineEdit();
	exportNameExampleLabel_ = new QLabel("ex,, ");

	doExportNameCheckBox_->setChecked(false);
	doAutoExportCheckBox_->setChecked(true);
	exportNameDictionaryLineEdit_->setEnabled(false);

	whenDoneLabel_ = new QLabel("When I'm done here:");

	startScanButton_ = new QPushButton("Start Scan");
	addToQueueButton_ = new QPushButton("Add to Workflow");

	goToWorkflowOption_ = new QRadioButton("Show me the workflow");
	setupAnotherScanOption_ = new QRadioButton("Setup another scan");

	QButtonGroup* bg = new QButtonGroup(this);
	bg->addButton(goToWorkflowOption_, 0);
	bg->addButton(setupAnotherScanOption_, 1);
	goToWorkflowOption_->setChecked(true);


	layout_ = new QVBoxLayout();
	if(view_)
		layout_->addWidget(view_);

	QVBoxLayout *vl1 = new QVBoxLayout();
	QHBoxLayout *hl1 = new QHBoxLayout();
	hl1->addWidget(scanNameLabel_);
	hl1->addWidget(scanNameLineEdit_);
	vl1->addLayout(hl1);
	vl1->addWidget(scanNameExampleLabel_);
	vl1->setContentsMargins(10, 10, 0, 0);

	QVBoxLayout *vl2 = new QVBoxLayout();
	QHBoxLayout *hl2 = new QHBoxLayout();
	hl2->addWidget(exportNameLabel_);
	hl2->addWidget(exportNameDictionaryLineEdit_);
	vl2->addLayout(hl2);
	vl2->addWidget(exportNameExampleLabel_);
	vl2->setContentsMargins(0, 10, 10, 0);

	QHBoxLayout *hl3 = new QHBoxLayout();
	QVBoxLayout *vl3 = new QVBoxLayout();
	vl3->addWidget(doExportNameCheckBox_);
	vl3->addWidget(doAutoExportCheckBox_);
	QVBoxLayout *vl4 = new QVBoxLayout();
	vl4->addWidget(new QLabel());
	vl4->addWidget(autoExportLabel_);
	hl3->addLayout(vl4);
	hl3->addLayout(vl3);

	QHBoxLayout *namingHL = new QHBoxLayout();
	namingHL->addLayout(vl1);
	namingHL->addLayout(hl3);
	namingHL->addLayout(vl2);

	QHBoxLayout* optionsHL = new QHBoxLayout();
	optionsHL->addWidget(whenDoneLabel_);
	optionsHL->addWidget(goToWorkflowOption_);
	optionsHL->addWidget(setupAnotherScanOption_);
	optionsHL->addStretch();
	optionsHL->addWidget(addToQueueButton_);
	optionsHL->addWidget(startScanButton_);
	optionsHL->setContentsMargins(10, 0, 10, 20);

	layout_->addLayout(namingHL);
	layout_->addLayout(optionsHL);
	layout_->setContentsMargins(0,0,0,0);

	setLayout(layout_);

	connect(startScanButton_, SIGNAL(clicked()), this, SLOT(onStartScanRequested()));
	connect(addToQueueButton_, SIGNAL(clicked()), this, SLOT(onAddToQueueRequested()));

	connect(workflow_, SIGNAL(workflowStatusChanged(bool,bool,bool)), this, SLOT(reviewStartScanButtonState()));


	connect(scanNameLineEdit_, SIGNAL(textEdited(QString)), this, SLOT(onScanNameLineEditTextEdited(QString)));

	reviewStartScanButtonState();

	QTimer::singleShot(500, this, SLOT(delayedDbLoad()));
}



AMScanConfigurationViewHolder::~AMScanConfigurationViewHolder()
{
}

void AMScanConfigurationViewHolder::setView(AMScanConfigurationView *view) {
	// delete old view, if it exists
	if(view_)
		delete view_;

	view_ = view;
	if(view_) {
		layout_->insertWidget(0, view_);
	}

	reviewStartScanButtonState();
}



void AMScanConfigurationViewHolder::reviewStartScanButtonState() {

	// if the scan configuration view, or its actual configuration, is not valid...
	if(!view_ || !view_->configuration()) {
		startScanButton_->setEnabled(false);
		addToQueueButton_->setEnabled(false);
		startScanButton_->setText("No valid Scan Configuration");
	}

	// if we can't start the workflow because it's already running (ie: our scans / actions)
	else if(workflow_->isRunning()) {
		startScanButton_->setEnabled(false);
		addToQueueButton_->setEnabled(true);
		startScanButton_->setText("Scans in progress");
	}
	// if we can't start the workflow because the beamline is busy/locked out (ie: another program is using it)
	else if(workflow_->beamlineBusy()) {
		startScanButton_->setEnabled(false);
		addToQueueButton_->setEnabled(true);
		startScanButton_->setText("Beamline Busy");
	}
	// Good to go. The workflow isn't running, and the beamline is not busy. Watch out: there may or may not still be scan actions already in the queue.
	else {
		startScanButton_->setEnabled(true);
		addToQueueButton_->setEnabled(true);
		startScanButton_->setText("Start Scan");
	}
}


#include <QMessageBox>
void AMScanConfigurationViewHolder::onStartScanRequested(){

	if(!view_ || !view_->configuration())
		return;

	if(workflow_->isRunning() || workflow_->beamlineBusy())
		return;

	bool startNow = true;
	int position = 0;

	// check first: if there's already items in the workflow, we need to find out if they want to add this action to the end of the queue, add this action to the beginning of the queue,
	if(workflow_->actionItemCount()) {

		qDebug() << workflow_->actionItemCount();

		QMessageBox questionBox;
		questionBox.setText("There are already scans waiting in the workflow queue.");
		questionBox.setInformativeText("What do you want to do with this scan?");
		questionBox.setIcon(QMessageBox::Question);
		QPushButton* cancel = questionBox.addButton("Cancel", QMessageBox::RejectRole);
		QPushButton* addToBeginningAndStart = questionBox.addButton("Add to beginning and start", QMessageBox::YesRole);
		QPushButton* addToEndAndStart = questionBox.addButton("Add to end and start workflow", QMessageBox::YesRole);
		QPushButton* addToEnd = questionBox.addButton("Add to end", QMessageBox::YesRole);
		questionBox.setDefaultButton(addToEndAndStart);

		questionBox.exec();

		QAbstractButton* result = questionBox.clickedButton();
		if(result == cancel) {
			return;
		}
		else if(result == addToEnd) {
			position = -1;
			startNow = false;
		}
		else if(result == addToEndAndStart) {
			position = -1;
			startNow = false;
		}
		else if(result == addToBeginningAndStart) {
			position = 0;
			startNow = true;
		}
	}

	AMScanConfiguration *config = view_->configuration()->createCopy();
	config->setUserScanName(scanNameLineEdit_->text());
	//AMBeamlineScanAction* action = new AMBeamlineScanAction(view_->configuration()->createCopy());
	AMBeamlineScanAction* action = new AMBeamlineScanAction(config);
	workflow_->insertBeamlineAction(position, action, startNow);
}

void AMScanConfigurationViewHolder::onAddToQueueRequested() {

	if(!view_ || !view_->configuration())
		return;

	AMScanConfiguration *config = view_->configuration()->createCopy();
	config->setUserScanName(scanNameLineEdit_->text());
	//AMBeamlineScanAction* action = new AMBeamlineScanAction(view_->configuration()->createCopy());
	AMBeamlineScanAction* action = new AMBeamlineScanAction(config);
	workflow_->insertBeamlineAction(-1, action);

	if(goToWorkflowOption_->isChecked())
		emit showWorkflowRequested();
}

void AMScanConfigurationViewHolder::onScanNameLineEditTextEdited(const QString &text){
	scanNameExampleLabel_->setText("ex,, "+testDictionary_->parseKeywordString(text)+".dat");
}

void AMScanConfigurationViewHolder::delayedDbLoad(){
	testDefaultScan_ = new AMXASScan(this);
	testDefaultScan_->loadFromDb(AMDatabase::userdb(), 1);
	testDictionary_ = new AMScanDictionary(testDefaultScan_, this);

	scanNameLineEdit_->setText(view_->configuration()->userScanName());
	onScanNameLineEditTextEdited(scanNameLineEdit_->text());
}

AMDictionaryLineEdit::AMDictionaryLineEdit(QWidget *parent) :
	QLineEdit(parent)
{

}

#include <QKeyEvent>

void AMDictionaryLineEdit::keyPressEvent(QKeyEvent *e){
	if(e->key() == Qt::Key_BracketLeft){
		int lastCursorPosition = cursorPosition();
		setText(text().insert(lastCursorPosition, "[]"));
		setCursorPosition(lastCursorPosition+1);
	}
	else if(e->key() == Qt::Key_BracketRight && cursorPosition() != text().length() && text().at(cursorPosition()) == ']')
		setCursorPosition(cursorPosition()+1);
	else if(e->key() == Qt::Key_Backspace){
		if(hasSelectedText())
			QLineEdit::keyPressEvent(e);
		else if(text().length() >= 2 && cursorPosition() != 0 && text().at(cursorPosition()-1) == ']' && text().at(cursorPosition()-2) == '[')
			setText(text().remove(cursorPosition()-2,2));
		else if(text().length() >= 2 && cursorPosition() != 0 && text().at(cursorPosition()-1) == '[' && cursorPosition() != text().length() && text().at(cursorPosition()) == ']')
			setText(text().remove(cursorPosition()-1,2));
		else if(text().length() >= 3 && cursorPosition() != 0 && text().at(cursorPosition()-1) == ']'){
			int otherBracket = text().lastIndexOf('[', cursorPosition()-1);
			int difference = cursorPosition()-otherBracket;
			if(otherBracket != -1)
				setSelection(otherBracket, difference);
		}
		else if(text().length() >= 3 && cursorPosition() != 0 && text().at(cursorPosition()-1) == '['){
			int otherBracket = text().indexOf(']', cursorPosition()-1);
			int difference = otherBracket-(cursorPosition()-1)+1;
			if(otherBracket != -1)
				setSelection(cursorPosition()-1, difference);
		}
		else
			QLineEdit::keyPressEvent(e);
	}
	else
		QLineEdit::keyPressEvent(e);
}
