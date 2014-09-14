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


#include "AMScanDatabaseImportWizard.h"

#include <QProgressBar>
#include <QLabel>
#include <QLineEdit>
#include <QFormLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QStringBuilder>
#include <QGroupBox>
#include <QTimer>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMap>
#include <QMapIterator>
#include <QApplication>
#include <QStyle>

AMScanDatabaseImportWizard::AMScanDatabaseImportWizard(AMScanDatabaseImportController* controller, QWidget *parent) :
	QWizard(parent)
{
	controller_ = controller;
	addPage(new AMScanDatabaseImportWizardChooseSourcePage(controller_));
	addPage(new AMScanDatabaseImportWizardReviewRunsPage(controller_));
	addPage(new AMScanDatabaseImportWizardReviewExperimentsPage(controller_));
	addPage(new AMScanDatabaseImportWizardReviewSamplesPage(controller_));
	addPage(new AMScanDatabaseImportWizardDoImportPage(controller_));
}

#include <QMessageBox>
void AMScanDatabaseImportWizard::done(int result)
{
	// check for cancel while import in progress...
	if(result == QDialog::Rejected && controller_ && (controller_->state() == AMScanDatabaseImportController::Importing)) {

		if(QMessageBox::question(this,
								 "Import in Progress...",
								 "Are you sure you want to cancel importing?\n\nIf you say yes here, your database will be returned to its original condition.",
								 QMessageBox::Yes | QMessageBox::No,
								 QMessageBox::Yes)
				== QMessageBox::Yes) {
			controller_->cancelAndRollBack();
		}
		else {
			return;	// don't close the window... We're still going.
		}
	}

	QWizard::done(result);
}

 AMScanDatabaseImportWizardChooseSourcePage::~AMScanDatabaseImportWizardChooseSourcePage(){}
AMScanDatabaseImportWizardChooseSourcePage::AMScanDatabaseImportWizardChooseSourcePage(AMScanDatabaseImportController* controller, QWidget *parent) : QWizardPage(parent)
{
	controller_ = controller;
	isComplete_ = false;

	progressBar_ = new QProgressBar();
	progressBar_->setRange(0,100);
	progressBar_->setOrientation(Qt::Horizontal);
	progressLabel_ = new QLabel();

	setTitle("Import Acquaman Bundle");

	sourcePathLineEdit_ = new QLineEdit();
	browseButton_ = new QPushButton("Browse");
	feedbackLabel_ = new QLabel( QString("Please choose the folder of the Acquaman Bundle you want to import. It should have a '%1' file inside it.").arg(AMUserSettings::userDatabaseFilename));
	feedbackLabel_->setWordWrap(true);

	// layout:
	QHBoxLayout* hl = new QHBoxLayout();
	hl->addWidget(new QLabel("Import:"));
	hl->addWidget(sourcePathLineEdit_);
	hl->addWidget(browseButton_);

	QHBoxLayout* hl2 = new QHBoxLayout();
	hl2->addWidget(progressBar_);
	hl2->addWidget(progressLabel_);

	QGroupBox* gb = new QGroupBox();
	QVBoxLayout* vl2 = new QVBoxLayout();
	vl2->addWidget(feedbackLabel_);
	gb->setLayout(vl2);

	QVBoxLayout* vl = new QVBoxLayout();
	vl->addWidget(new QLabel("This tool lets you import a complete Acquaman Bundle (for example, from another computer or a beamline run) \ninto your database. All of the raw data and scan information will be copied, and you will have a chance to review \npossible duplicates.\n\nOn this page, you should choose the bundle folder. It should contain a file called '" % AMUserSettings::userDatabaseFilename % "'."));
	vl->addLayout(hl);
	vl->addWidget(gb);
	vl->addStretch();
	vl->addLayout(hl2);

	setLayout(vl);
	setMinimumHeight(400);

	connect(sourcePathLineEdit_, SIGNAL(textChanged(QString)), this, SLOT(reviewComplete()));
	connect(browseButton_, SIGNAL(clicked()), this, SLOT(displaySourcePathDialog()));
}

void AMScanDatabaseImportWizardChooseSourcePage::initializePage()
{
}

bool AMScanDatabaseImportWizardChooseSourcePage::validatePage()
{
	bool success = controller_->setSourceFolderAndLoadDatabase(sourcePathLineEdit_->text());
	if(!success) {
		feedbackLabel_->setText("There was a problem loading this database. Please check to make sure it's a valid Acquaman Bundle.");
		isComplete_ = false;
		emit completeChanged();
	}
	return success;
}


void AMScanDatabaseImportWizardChooseSourcePage::displaySourcePathDialog()
{
	sourcePathLineEdit_->setText(QFileDialog::getExistingDirectory(this,
																   QString("Please choose the folder of the Acquaman Bundle you want to import. It should have a '%1' file inside it.").arg(AMUserSettings::userDatabaseFilename)));
}

void AMScanDatabaseImportWizardChooseSourcePage::reviewComplete()
{
	QString sourcePath = sourcePathLineEdit_->text();
	QDir sourcePathDir(sourcePath);
	if(sourcePathDir.exists(AMUserSettings::userDatabaseFilename)) {
		isComplete_ = true;
		feedbackLabel_->setText("Ready to import.");
	}
	else {
		isComplete_ = false;
		feedbackLabel_->setText(QString("Please choose a folder with the Acquaman Bundle you want to import. It should have a '%1' file inside it.  (This one doesn't.)").arg(AMUserSettings::userDatabaseFilename));
	}

	emit completeChanged();
}

void AMScanDatabaseImportWizardReviewRunsPage::initializePage()
{
	controller_->analyzeFacilitiesForDuplicates();
	controller_->analyzeRunsForDuplicates();

	runsFoundList_->clear();
	runsToMergeList_->clear();

	QStyle* style = QApplication::style();
	QIcon icon = style->standardIcon(QStyle::SP_DirIcon);

	QMap<int, int> runIdMapping = controller_->runIdMapping();
	QMapIterator<int, int> i(runIdMapping);
	while(i.hasNext()) {
		i.next();
		runsFoundList_->addItem(new QListWidgetItem(icon, controller_->runName(i.key())));
		if(i.value() != -1) {
			QListWidgetItem* item = new QListWidgetItem(icon, controller_->runName(i.key()));
			item->setData(Qt::UserRole, i.key());
			item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
			item->setData(Qt::CheckStateRole, false);
			runsToMergeList_->addItem(item);
		}
	}
}

bool AMScanDatabaseImportWizardReviewRunsPage::validatePage()
{
	// if they have checked any runs in the runsToMergeList, they want to keep these separate (ie: don't merge). We should tell the controller.
	for(int i=0, cc=runsToMergeList_->count(); i<cc; i++) {
		QListWidgetItem* item = runsToMergeList_->item(i);
		if(item->data(Qt::CheckStateRole).toBool())
			controller_->doNotMergeRun(item->data(Qt::UserRole).toInt());
	}
	return true;
}

 AMScanDatabaseImportWizardReviewRunsPage::~AMScanDatabaseImportWizardReviewRunsPage(){}
AMScanDatabaseImportWizardReviewRunsPage::AMScanDatabaseImportWizardReviewRunsPage(AMScanDatabaseImportController *controller, QWidget *parent) : QWizardPage(parent)
{
	controller_ = controller;

	progressBar_ = new QProgressBar();
	progressBar_->setRange(0, 100);
	progressBar_->setOrientation(Qt::Horizontal);
	progressLabel_ = new QLabel();

	setTitle("Review Runs");

	runsFoundList_ = new QListWidget();
	runsToMergeList_ = new QListWidget();
	QGroupBox* gb1 = new QGroupBox("Runs found");
	QGroupBox* gb2 = new QGroupBox("Already in your library\n(Check to keep separate)");
	gb1->setLayout(new QVBoxLayout());
	gb1->layout()->addWidget(runsFoundList_);
	gb2->setLayout(new QVBoxLayout());
	gb2->layout()->addWidget(runsToMergeList_);

	QHBoxLayout* hl = new QHBoxLayout();
	hl->addWidget(gb1);
	hl->addWidget(gb2);

	QHBoxLayout* hl2 = new QHBoxLayout();
	hl2->addWidget(progressLabel_);
	hl2->addWidget(progressBar_);


	QVBoxLayout* vl = new QVBoxLayout();
	vl->addWidget(new QLabel("These are the runs we found in the bundle.\n\nAny runs that are already in your library are shown on the right and will be merged automatically. \nIf you want to keep these separate instead, check the 'Keep Separate' box and a new run will be added.\n"));
	vl->addLayout(hl);
	vl->addStretch();
	vl->addLayout(hl2);

	setLayout(vl);
	setMinimumHeight(400);

	connect(controller_, SIGNAL(stepProgress(int)), progressBar_, SLOT(setValue(int)));
	connect(controller_, SIGNAL(progressDescription(QString)), progressLabel_, SLOT(setText(QString)));
}






void AMScanDatabaseImportWizardReviewExperimentsPage::initializePage()
{
	controller_->analyzeExperimentsForDuplicates();

	itemsFoundList_->clear();
	itemsToMergeList_->clear();

	QStyle* style = QApplication::style();
	QIcon icon = style->standardIcon(QStyle::SP_DirOpenIcon);

	QMap<int, int> itemIdMapping = controller_->experimentIdMapping();
	QMapIterator<int, int> i(itemIdMapping);
	while(i.hasNext()) {
		i.next();
		itemsFoundList_->addItem(new QListWidgetItem(icon, controller_->experimentName(i.key())));
		if(i.value() != -1) {
			QListWidgetItem* item = new QListWidgetItem(icon, controller_->experimentName(i.key()));
			item->setData(Qt::UserRole, i.key());
			item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
			item->setData(Qt::CheckStateRole, false);
			itemsToMergeList_->addItem(item);
		}
	}
}

bool AMScanDatabaseImportWizardReviewExperimentsPage::validatePage()
{
	// if they have checked any experiments in the itemsToMergeList, they want to keep these separate (ie: don't merge). We should tell the controller.
	for(int i=0, cc=itemsToMergeList_->count(); i<cc; i++) {
		QListWidgetItem* item = itemsToMergeList_->item(i);
		if(item->data(Qt::CheckStateRole).toBool())
			controller_->doNotMergeExperiment(item->data(Qt::UserRole).toInt());
	}
	return true;
}

 AMScanDatabaseImportWizardReviewExperimentsPage::~AMScanDatabaseImportWizardReviewExperimentsPage(){}
AMScanDatabaseImportWizardReviewExperimentsPage::AMScanDatabaseImportWizardReviewExperimentsPage(AMScanDatabaseImportController *controller, QWidget *parent) : QWizardPage(parent)
{
	controller_ = controller;

	progressBar_ = new QProgressBar();
	progressBar_->setRange(0, 100);
	progressBar_->setOrientation(Qt::Horizontal);
	progressLabel_ = new QLabel();

	setTitle("Review Experiments");

	itemsFoundList_ = new QListWidget();
	itemsToMergeList_ = new QListWidget();
	QGroupBox* gb1 = new QGroupBox("Experiments found");
	QGroupBox* gb2 = new QGroupBox("Already in your library\n(Check to keep separate)");
	gb1->setLayout(new QVBoxLayout());
	gb1->layout()->addWidget(itemsFoundList_);
	gb2->setLayout(new QVBoxLayout());
	gb2->layout()->addWidget(itemsToMergeList_);

	QHBoxLayout* hl = new QHBoxLayout();
	hl->addWidget(gb1);
	hl->addWidget(gb2);

	QHBoxLayout* hl2 = new QHBoxLayout();
	hl2->addWidget(progressLabel_);
	hl2->addWidget(progressBar_);


	QVBoxLayout* vl = new QVBoxLayout();
	vl->addWidget(new QLabel("These are the experiments we found in the bundle.\n\nAny experiments that are already in your library are shown on the right and will be merged automatically. \nIf you want to keep these separate instead, check the 'Keep Separate' box and a new experiment will be added.\n"));
	vl->addLayout(hl);
	vl->addStretch();
	vl->addLayout(hl2);

	setLayout(vl);
	setMinimumHeight(400);

	connect(controller_, SIGNAL(stepProgress(int)), progressBar_, SLOT(setValue(int)));
	connect(controller_, SIGNAL(progressDescription(QString)), progressLabel_, SLOT(setText(QString)));
}

void AMScanDatabaseImportWizardReviewSamplesPage::initializePage()
{
	controller_->analyzeSamplesForDuplicates();

	itemsFoundList_->clear();
	itemsToMergeList_->clear();

	QStyle* style = QApplication::style();
	QIcon icon = style->standardIcon(QStyle::SP_FileIcon);

	QMap<int, int> itemIdMapping = controller_->sampleIdMapping();
	QMapIterator<int, int> i(itemIdMapping);
	while(i.hasNext()) {
		i.next();
		itemsFoundList_->addItem(new QListWidgetItem(icon, controller_->sampleName(i.key())));
		if(i.value() != -1) {
			QListWidgetItem* item = new QListWidgetItem(icon, controller_->sampleName(i.key()));
			item->setData(Qt::UserRole, i.key());
			item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
			item->setData(Qt::CheckStateRole, false);
			itemsToMergeList_->addItem(item);
		}
	}
}

bool AMScanDatabaseImportWizardReviewSamplesPage::validatePage()
{
	// if they have checked any samples in the itemsToMergeList, they want to keep these separate (ie: don't merge). We should tell the controller.
	for(int i=0, cc=itemsToMergeList_->count(); i<cc; i++) {
		QListWidgetItem* item = itemsToMergeList_->item(i);
		if(item->data(Qt::CheckStateRole).toBool())
			controller_->doNotMergeSample(item->data(Qt::UserRole).toInt());
	}
	return true;
}

 AMScanDatabaseImportWizardReviewSamplesPage::~AMScanDatabaseImportWizardReviewSamplesPage(){}
AMScanDatabaseImportWizardReviewSamplesPage::AMScanDatabaseImportWizardReviewSamplesPage(AMScanDatabaseImportController *controller, QWidget *parent) : QWizardPage(parent)
{
	controller_ = controller;

	progressBar_ = new QProgressBar();
	progressBar_->setRange(0, 100);
	progressBar_->setOrientation(Qt::Horizontal);
	progressLabel_ = new QLabel();

	setTitle("Review Samples");

	itemsFoundList_ = new QListWidget();
	itemsToMergeList_ = new QListWidget();
	QGroupBox* gb1 = new QGroupBox("Samples found");
	QGroupBox* gb2 = new QGroupBox("Already in your library\n(Check to keep separate)");
	gb1->setLayout(new QVBoxLayout());
	gb1->layout()->addWidget(itemsFoundList_);
	gb2->setLayout(new QVBoxLayout());
	gb2->layout()->addWidget(itemsToMergeList_);

	QHBoxLayout* hl = new QHBoxLayout();
	hl->addWidget(gb1);
	hl->addWidget(gb2);

	QHBoxLayout* hl2 = new QHBoxLayout();
	hl2->addWidget(progressLabel_);
	hl2->addWidget(progressBar_);


	QVBoxLayout* vl = new QVBoxLayout();
	vl->addWidget(new QLabel("These are the samples we found in the bundle.\n\nAny samples that are already in your library are shown on the right and will be merged automatically. \nIf you want to keep these separate instead, check the 'Keep Separate' box and a new sample will be added.\n"));
	vl->addLayout(hl);
	vl->addStretch();
	vl->addLayout(hl2);

	setLayout(vl);
	setMinimumHeight(400);

	connect(controller_, SIGNAL(stepProgress(int)), progressBar_, SLOT(setValue(int)));
	connect(controller_, SIGNAL(progressDescription(QString)), progressLabel_, SLOT(setText(QString)));

	setCommitPage(true);
}

void AMScanDatabaseImportWizardDoImportPage::initializePage()
{
	QTimer::singleShot(0, this, SLOT(startImport()));
}

bool AMScanDatabaseImportWizardDoImportPage::validatePage()
{
	controller_->deleteLater();
	wizard()->deleteLater();
	return true;
}

 AMScanDatabaseImportWizardDoImportPage::~AMScanDatabaseImportWizardDoImportPage(){}
AMScanDatabaseImportWizardDoImportPage::AMScanDatabaseImportWizardDoImportPage(AMScanDatabaseImportController *controller, QWidget *parent) : QWizardPage(parent)
{
	controller_ = controller;

	progressBar_ = new QProgressBar();
	progressBar_->setRange(0, 100);
	progressBar_->setOrientation(Qt::Horizontal);
	progressLabel_ = new QLabel();

	setTitle("Importing...");

	QHBoxLayout* hl2 = new QHBoxLayout();
	hl2->addWidget(progressLabel_);
	hl2->addWidget(progressBar_);


	QVBoxLayout* vl = new QVBoxLayout();
	vl->addWidget(new QLabel("Pleases wait while this Acquaman Bundle is imported into your library."));
	vl->addStretch();
	vl->addLayout(hl2);

	setLayout(vl);

	connect(controller_, SIGNAL(stepProgress(int)), progressBar_, SLOT(setValue(int)));
	connect(controller_, SIGNAL(progressDescription(QString)), progressLabel_, SLOT(setText(QString)));
	connect(controller_, SIGNAL(stepProgress(int)), this, SIGNAL(completeChanged()));
	connect(controller_, SIGNAL(progressDescription(QString)), this, SIGNAL(completeChanged()));
}

void AMScanDatabaseImportWizardDoImportPage::startImport()
{
	controller_->startDatabaseOperations();
}

bool AMScanDatabaseImportWizardDoImportPage::isComplete() const
{
	return (controller_->state() != AMScanDatabaseImportController::Importing);
}
