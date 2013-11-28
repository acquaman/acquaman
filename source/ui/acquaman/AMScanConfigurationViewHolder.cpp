/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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

//#include <QRadioButton>
//#include <QPushButton>
//#include <QLabel>
//#include <QLineEdit>
//#include <QCheckBox>
//#include <QVBoxLayout>
//#include <QButtonGroup>

//#include "AMScanConfigurationViewHolder.h"
//#include "actions/AMBeamlineScanAction.h"
//#include "acquaman/AMScanConfiguration.h"
//#include "ui/acquaman/AMScanConfigurationView.h"
//#include "ui/AMWorkflowManagerView.h"
//#include "ui/dataman/AMDictionaryLineEdit.h"

//#include "dataman/AMXASScan.h"
//#include "dataman/AMScanExemplarDictionary.h"
//#include <QTimer>


//AMScanConfigurationViewHolder::AMScanConfigurationViewHolder(AMWorkflowManagerView* workflow, AMScanConfigurationView* view, QWidget *parent) :
//	QWidget(parent)
//{
//	view_ = view;
//	workflow_ = workflow;

//	/*
//	testExemplar_.setName("");
//	testExemplar_.setDateTime(QDateTime::currentDateTime());
//	testExemplar_.setRunName("SGM");
//	testExemplar_.setRunStartDateTime(QDateTime::currentDateTime());
//	testExemplar_.setRunEndDateTime(QDateTime::currentDateTime());
//	testExemplar_.setFacilityName("SGM");
//	testExemplar_.setFacilityDescription("CLS SGM Beamline");
//	if(view_ && view_->configuration())
//		testExemplar_.setScanConfiguration(view->configuration());
//	testExemplar_.setSampleName("Eu Doped ZnO");
//	testExemplar_.setSampleElements("Eu Zn O");
//	testExemplar_.setSampleDateTime(QDateTime::currentDateTime());
//	exemplarNameDictionary_ = new AMScanExemplarDictionary(&testExemplar_, this);
//	exemplarNameDictionary_->setOperatingOnName(true);
//	exemplarExportNameDictionary_ = new AMScanExemplarDictionary(&testExemplar_, this);
//	exemplarExportNameDictionary_->setOperatingOnExportName(true);


//	scanNameLabel_ = new QLabel("Scan Name:");
//	scanNameExampleLabel_ = new QLabel("   ex,, ");
//	scanNameDictionaryLineEdit_ = new AMDictionaryLineEdit(exemplarNameDictionary_, scanNameExampleLabel_);

//	autoExportLabel_ = new QLabel("---->");
//	doExportNameCheckBox_ = new QCheckBox("Set Name");
//	doAutoExportCheckBox_ = new QCheckBox("Auto Export");

//	exportNameLabel_ = new QLabel("Export Name:");
//	exportNameExampleLabel_ = new QLabel("   ex,, ");
//	exportNameDictionaryLineEdit_ = new AMDictionaryLineEdit(exemplarExportNameDictionary_, exportNameExampleLabel_);

//	doExportNameCheckBox_->setChecked(false);
//	doAutoExportCheckBox_->setChecked(true);
//	exportNameLabel_->setEnabled(false);
//	exportNameDictionaryLineEdit_->setEnabled(false);
//	*/

//	whenDoneLabel_ = new QLabel("When I'm done here:");

//	startScanButton_ = new QPushButton("Start Scan");
//	addToQueueButton_ = new QPushButton("Add to Workflow");

//	goToWorkflowOption_ = new QRadioButton("Show me the workflow");
//	setupAnotherScanOption_ = new QRadioButton("Setup another scan");

//	QButtonGroup* bg = new QButtonGroup(this);
//	bg->addButton(goToWorkflowOption_, 0);
//	bg->addButton(setupAnotherScanOption_, 1);
//	goToWorkflowOption_->setChecked(true);


//	layout_ = new QVBoxLayout();
//	if(view_)
//		layout_->addWidget(view_);

//	QHBoxLayout* optionsHL = new QHBoxLayout();
//	optionsHL->addWidget(whenDoneLabel_);
//	optionsHL->addWidget(goToWorkflowOption_);
//	optionsHL->addWidget(setupAnotherScanOption_);
//	optionsHL->addStretch();
//	optionsHL->addWidget(addToQueueButton_);
//	optionsHL->addWidget(startScanButton_);
//	optionsHL->setContentsMargins(10, 0, 10, 20);

//	layout_->addLayout(optionsHL);
//	layout_->setContentsMargins(0,0,0,0);

//	setLayout(layout_);

//	connect(startScanButton_, SIGNAL(clicked()), this, SLOT(onStartScanRequested()));
//	connect(addToQueueButton_, SIGNAL(clicked()), this, SLOT(onAddToQueueRequested()));

//	connect(workflow_, SIGNAL(workflowStatusChanged(bool,bool,bool)), this, SLOT(reviewStartScanButtonState()));

//	/*
//	connect(doExportNameCheckBox_, SIGNAL(stateChanged(int)), this, SLOT(onDoExportNameCheckBoxStatedChanged(int)));
//	*/

//	reviewStartScanButtonState();
//}



//AMScanConfigurationViewHolder::~AMScanConfigurationViewHolder()
//{
//}

//void AMScanConfigurationViewHolder::setView(AMScanConfigurationView *view) {
//	// delete old view, if it exists
//	if(view_)
//		delete view_;

//	view_ = view;
//	if(view_) {
//		layout_->insertWidget(0, view_);
//		/*
//		if(view_->configuration()){
//			testExemplar_.setScanConfiguration(view_->configuration());
//			scanNameDictionaryLineEdit_->setTextAndOperate(view_->configuration()->userScanName());
//			exportNameDictionaryLineEdit_->setTextAndOperate(view_->configuration()->userExportName());
//		}
//		*/
//	}

//	reviewStartScanButtonState();
//}



//void AMScanConfigurationViewHolder::reviewStartScanButtonState() {

//	// if the scan configuration view, or its actual configuration, is not valid...
//	if(!view_ || !view_->configuration()) {
//		startScanButton_->setEnabled(false);
//		addToQueueButton_->setEnabled(false);
//		startScanButton_->setText("No valid Scan Configuration");
//	}

//	// if we can't start the workflow because it's already running (ie: our scans / actions)
//	else if(workflow_->isRunning()) {
//		startScanButton_->setEnabled(false);
//		addToQueueButton_->setEnabled(true);
//		startScanButton_->setText("Scans in progress");
//	}
//	// if we can't start the workflow because the beamline is busy/locked out (ie: another program is using it)
//	else if(workflow_->beamlineBusy()) {
//		startScanButton_->setEnabled(false);
//		addToQueueButton_->setEnabled(true);
//		startScanButton_->setText("Beamline Busy");
//	}
//	// Good to go. The workflow isn't running, and the beamline is not busy. Watch out: there may or may not still be scan actions already in the queue.
//	else {
//		startScanButton_->setEnabled(true);
//		addToQueueButton_->setEnabled(true);
//		startScanButton_->setText("Start Scan");
//	}
//}


//#include <QMessageBox>
//void AMScanConfigurationViewHolder::onStartScanRequested(){

//	if(!view_ || !view_->configuration())
//		return;

//	if(workflow_->isRunning() || workflow_->beamlineBusy())
//		return;

//	bool startNow = true;
//	int position = 0;

//	// check first: if there's already items in the workflow, we need to find out if they want to add this action to the end of the queue, add this action to the beginning of the queue,
//	if(workflow_->actionItemCount()) {

//		QMessageBox questionBox;
//		questionBox.setText("There are already scans waiting in the workflow queue.");
//		questionBox.setInformativeText("What do you want to do with this scan?");
//		questionBox.setIcon(QMessageBox::Question);
//		QPushButton* cancel = questionBox.addButton("Cancel", QMessageBox::RejectRole);
//		QPushButton* addToBeginningAndStart = questionBox.addButton("Add to beginning and start", QMessageBox::YesRole);
//		QPushButton* addToEndAndStart = questionBox.addButton("Add to end and start workflow", QMessageBox::YesRole);
//		QPushButton* addToEnd = questionBox.addButton("Add to end", QMessageBox::YesRole);
//		questionBox.setDefaultButton(addToEndAndStart);

//		questionBox.exec();

//		QAbstractButton* result = questionBox.clickedButton();
//		if(result == cancel) {
//			return;
//		}
//		else if(result == addToEnd) {
//			position = -1;
//			startNow = false;
//		}
//		else if(result == addToEndAndStart) {
//			position = -1;
//			startNow = false;
//		}
//		else if(result == addToBeginningAndStart) {
//			position = 0;
//			startNow = true;
//		}
//	}

//	AMScanConfiguration *config = view_->configuration()->createCopy();
//	/*
//	config->setUserScanName(scanNameDictionaryLineEdit_->text());
//	config->setUserExportNmae(exportNameDictionaryLineEdit_->text());
//	*/
//	AMBeamlineScanAction* action = new AMBeamlineScanAction(config);
//	workflow_->insertBeamlineAction(position, action, startNow);
//}

//#include "acquaman/AMXASScanConfiguration.h"

//void AMScanConfigurationViewHolder::onAddToQueueRequested() {

//	if(!view_ || !view_->configuration())
//		return;

//	AMScanConfiguration *config = view_->configuration()->createCopy();
//	/*
//	config->setUserScanName(scanNameDictionaryLineEdit_->text());
//	config->setUserExportNmae(exportNameDictionaryLineEdit_->text());
//	*/
//	AMBeamlineScanAction* action = new AMBeamlineScanAction(config);
//	workflow_->insertBeamlineAction(-1, action);

//	if(goToWorkflowOption_->isChecked())
//		emit showWorkflowRequested();
//}

///*
//void AMScanConfigurationViewHolder::onDoExportNameCheckBoxStatedChanged(int state){
//	if(state == Qt::Unchecked){
//		exportNameLabel_->setEnabled(false);
//		exportNameDictionaryLineEdit_->setEnabled(false);
//	}
//	else if(state == Qt::Checked){
//		exportNameLabel_->setEnabled(true);
//		exportNameDictionaryLineEdit_->setEnabled(true);
//	}
//}
//*/
