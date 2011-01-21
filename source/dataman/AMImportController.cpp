/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

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


#include "AMImportController.h"


#include "dataman/AMXASScan.h"
#include "dataman/SGM2004FileLoader.h"
#include "dataman/ALSBL8XASFileLoader.h"
#include "ui/AMImportControllerWidget.h"
#include "AMErrorMonitor.h"

#include <QDir>
#include <QFileDialog>
#include <QSignalMapper>

/// Attempt to import, and return a pointer to a new scan object, or 0 if the import fails.  It's the caller's responsibility to delete the scan when done with it.
AMScan* SGMLegacyImporter::import(const QString& fullPath) {

	AMXASScan* rv = new AMXASScan();
	SGM2004FileLoader loader(rv);

	// check for spectra file, and set rv->additionalFilePaths()
	if( fullPath.indexOf(".dat") >= 0){
		QString spectraPath = fullPath;
		spectraPath.insert(spectraPath.indexOf(".dat"), "_spectra");
		if( QFile::exists(spectraPath) ){
			QStringList additionalFilePaths;
			additionalFilePaths << spectraPath;
			rv->setAdditionalFilePaths(additionalFilePaths);
		}
	}

	if(loader.loadFromFile(fullPath, true, true, true)) {

		// what should we name this scan?
		QFileInfo fileInfo(fullPath);
		QString name = fileInfo.completeBaseName();
		// find a number?
		int indexOfNumber = name.indexOf(QRegExp("\\d+$"));
		// will be -1 if not found, or 0 if the whole filename is a number. In either case, don't use it as the number.
		if(indexOfNumber > 0) {
			rv->setName(name.left(indexOfNumber));
			rv->setNumber(name.mid(indexOfNumber).toInt());
		}
		else {
			rv->setName(name);
			rv->setNumber(0);
		}

		// remember the file format, for re-loading.
		rv->setFileFormat( loader.formatTag() );

		return rv;
	}
	else {
		delete rv;
		return 0;
	}
}

#include <QFileInfo>
AMScan* ALSBL8XASImporter::import(const QString& fullPath) {

	AMXASScan* rv = new AMXASScan();
	ALSBL8XASFileLoader loader(rv);
	// load meta-data AND raw data, please...
	if(loader.loadFromFile(fullPath, true, true, true)) {

		// what should we name this scan?
		QFileInfo fileInfo(fullPath);
		QString name = fileInfo.completeBaseName();	// SigScan23777.txt ===> SigScan23777
		bool numberOk;
		int number = name.right(3).toInt(&numberOk);	// SigScan23777 ===> 777
		// find a number?
		if(numberOk) {
			rv->setNumber(number);
		}
		else {
			rv->setNumber(0);
		}

		// use the first line of the description for the name, if it's filled out. (Otherwise, use the file name...)
		QStringList notes = rv->notes().split('\n', QString::SkipEmptyParts);
		if(!notes.isEmpty())
			rv->setName(notes.at(0).trimmed());
		else
			rv->setName(name);

		// remember the file format, for re-loading.
		rv->setFileFormat( loader.formatTag() );

		return rv;
	}
	else {
		delete rv;
		return 0;
	}
}



AMImportController::~AMImportController() {
	for(int i=0; i<importers_.count(); i++)
		delete importers_[i];

	delete fileDialog_;
	delete w_;
}

AMImportController::AMImportController(QObject *parent) :
		QObject(parent)
{
	installImporters();

	fileDialog_ = new QFileDialog();
	w_ = new AMImportControllerWidget();

	foreach(AMImporter* importer, importers_)
		w_->formatComboBox->addItem(importer->description());


	connect(fileDialog_, SIGNAL(finished(int)), this, SLOT(onFileDialogFinished(int)));

	connect(w_->nextButton, SIGNAL(clicked()), this, SLOT(onNextButtonClicked()));
	connect(w_->applyAllButton, SIGNAL(clicked()), this, SLOT(onApplyAllButtonClicked()));
	connect(w_->cancelButton, SIGNAL(clicked()), this, SLOT(onCancelButtonClicked()));
	connect(w_->formatComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onFileTypeComboBoxChanged(int)));

	QSignalMapper* mapper = new QSignalMapper(this);
	connect(w_->checkName, SIGNAL(clicked()), mapper, SLOT(map()));
	connect(w_->checkNumber, SIGNAL(clicked()), mapper, SLOT(map()));
	connect(w_->checkDateTime, SIGNAL(clicked()), mapper, SLOT(map()));

	mapper->setMapping(w_->checkName, "name");
	mapper->setMapping(w_->checkNumber, "number");
	mapper->setMapping(w_->checkDateTime, "dateTime");
	connect(mapper, SIGNAL(mapped(QString)), this, SLOT(onGetInfoFromFileBoxChecked(QString)));


	onStart();
}

void AMImportController::installImporters() {
	importers_ << new SGMLegacyImporter();
	importers_ << new ALSBL8XASImporter();
}

/// Used to start the whole import process. (Called automatically from the constructor)
void AMImportController::onStart() {

	applyToAll_ = false;
	numSuccess_ = 0;

	fileDialog_->setFileMode(QFileDialog::ExistingFiles);

	QStringList filters;
	for(int i=0; i<importers_.count(); i++)
		filters << QString("%1 (%2)").arg(importers_[i]->description()).arg(importers_[i]->fileNameFilter());
	filters << "All files (*.*)";
	fileDialog_->setFilters(filters);

	fileDialog_->show();
}




/// Called when the file dialog has finished (and files have been selected)
void AMImportController::onFileDialogFinished(int result) {
	if(result == QDialog::Rejected) {
		onFinished();
		return;
	}

	filesToImport_ = fileDialog_->selectedFiles();
	fileDialog_->hide();
	currentFile_ = 0;

	setupNextFile();
	w_->show();
	w_->resize(w_->minimumSizeHint());
}

/// Called when the 'next' button is clicked while reviewing
void AMImportController::onNextButtonClicked() {

	finalizeImport();

	currentFile_++;

	setupNextFile();
}



void AMImportController::setupNextFile() {
	if(currentFile_ >= filesToImport_.count()) {
		// all done:
		onFinished();
		return;
	}

	w_->progressBar->setValue(currentFile_);
	w_->progressBar->setMaximum(filesToImport_.count());
	w_->progressBar->setMinimum(0);
	w_->progressLabel->setText(QString("File %1 of %2").arg(currentFile_+1).arg(filesToImport_.count()));

	// extract names
	QString path = QDir::fromNativeSeparators(filesToImport_.at(currentFile_));
	// remove folders
	QString name = path.split(QChar('/')).last();
	path.chop(name.length()+1);

	w_->thumbnailViewer->setCaption1(name);
	w_->thumbnailViewer->setCaption2(path);

	/// \todo: switch importer automatically if current one doesn't match filter, and another does.

	AMImporter* importer = importers_.at(w_->formatComboBox->currentIndex());
	currentScan_ = importer->import(filesToImport_.at(currentFile_));

	w_->thumbnailViewer->setSource(currentScan_);

	if(currentScan_ == 0) {
		w_->loadingStatusLabel->setText("Could not load this file.");
		w_->loadingStatusLabel->setStyleSheet("color: red;");
	}

	else {
		w_->loadingStatusLabel->setText("Loaded successfully.");
		w_->loadingStatusLabel->setStyleSheet("color: black;");
	}


	/// todo: you could generalize this for meta-data types, instead of doing them all separately.
	///////////////////////
	if(w_->checkName->isChecked()) {
		w_->nameEdit->setEnabled(false);
		if(currentScan_)
			w_->nameEdit->setText(currentScan_->name());
	}
	else
		w_->nameEdit->setEnabled(true);


	if(w_->checkNumber->isChecked()) {
		w_->numberEdit->setEnabled(false);
		if(currentScan_)
			w_->numberEdit->setValue(currentScan_->number());
	}
	else
		w_->numberEdit->setEnabled(true);


	if(w_->checkDateTime->isChecked()) {
		w_->dateTimeEdit->setEnabled(false);
		if(currentScan_)
			w_->dateTimeEdit->setDateTime(currentScan_->dateTime());
	}
	else
		w_->dateTimeEdit->setEnabled(true);
	/////////////////////////////


	// This is our magic way of handling "Apply to All", while keeping the progress bar for free and stopping if we ever get an import error:  After setting up the next file, we just schedule a signal to automatically push the next button, as if the user was doing it really fast.
	if(applyToAll_) {
		QTimer::singleShot(0, this, SLOT(onNextButtonClicked()));
	}
}

#include <QMessageBox>
#include "dataman/AMDatabase.h"

void AMImportController::finalizeImport() {
	// error loading:
	if(currentScan_ == 0) {
		int doSkip = QMessageBox::question(
				w_,
				"Skip this file?",
				QString("Couldn't load this file (%1) with the '%2' format.\n\nDo you want to skip this file?").arg(filesToImport_.at(currentFile_)).arg(w_->formatComboBox->currentText()),
				QMessageBox::Ok | QMessageBox::Cancel);

		if(doSkip == QMessageBox::Ok) {
			// do nothing... We'll move on automatically
		}
		else {
			// This will force us to re-load the same one and try again.
			currentFile_--;
			applyToAll_ = false;
		}
	}

	// success:
	else {
		if(!w_->checkName->isChecked())
			currentScan_->setName(w_->nameEdit->text());

		if(!w_->checkNumber->isChecked())
			currentScan_->setNumber(w_->numberEdit->value());

		if(!w_->checkDateTime->isChecked())
			currentScan_->setDateTime(w_->dateTimeEdit->dateTime());

		currentScan_->setSampleId(w_->sampleEdit->value());
		currentScan_->setRunId(w_->runEdit->currentRunId());

		// copy the raw data file to the library:
		QFileInfo file(filesToImport_.at(currentFile_));
		QDir dir;
		QString path = AMUserSettings::userDataFolder + currentScan_->dateTime().toString("/yyyy/MM");
		dir.mkpath(path);
		path.append("/").append(file.fileName());

		currentScan_->setFilePath(path);

		if(!QFile::copy(filesToImport_.at(currentFile_), currentScan_->filePath())) {
			AMErrorMon::report(AMErrorReport(0, AMErrorReport::Serious, -2, "SGM2004FileImporter: Could not copy the imported file into the library. Maybe this file exists already?"));
			int doSkip = QMessageBox::question(
					w_,
					"Skip this file?",
					QString("The file '%1' was opened correctly, but it couldn't be copied to the data library. Maybe the destination file (%2) exists already? \n\nDo you want to skip this file?").arg(filesToImport_.at(currentFile_)).arg(currentScan_->filePath()),
					QMessageBox::Ok | QMessageBox::Cancel);

			if(doSkip == QMessageBox::Ok) {
				// do nothing... We'll move on automatically
			}
			else {
				// This will force us to re-load the same one and try again.
				currentFile_--;
				applyToAll_ = false;
			}
		}
		else {
			if( currentScan_->storeToDb(AMDatabase::userdb()) )
				numSuccess_++;
			else
				AMErrorMon::report(AMErrorReport(this, AMErrorReport::Serious, -1, "The file was loaded correctly, but it could not be saved to the database."));
		}

		w_->thumbnailViewer->setSource(0);
		delete currentScan_;
		currentScan_ = 0;
	}
}

/// Called when the 'apply to all' button is clicked while reviewing
void AMImportController::onApplyAllButtonClicked() {
	applyToAll_ = true;
	QTimer::singleShot(0, this, SLOT(onNextButtonClicked()));
}

/// called when the cancel button is clicked while reviewing
void AMImportController::onCancelButtonClicked() {
	int doCancel = QMessageBox::question(
			w_,
			"Cancel import?",
			QString("You've imported %1 of %2 files. If you stop now, the remaining %3 will not be imported.\n\nAre you sure you want to stop?").arg(qMax(0, currentFile_ -1)).arg(filesToImport_.count()).arg(filesToImport_.count()-currentFile_),
			QMessageBox::Cancel | QMessageBox::Ok);
	if(doCancel == QMessageBox::Ok) {
		applyToAll_ = false;
		onFinished();
	}
}

/// while reviewing, the file type sets the method used to import. If it's changed, need to retry.
void AMImportController::onFileTypeComboBoxChanged(int index) {
	Q_UNUSED(index)

	setupNextFile();
}

/// This slot is called from the signal mapper whenever any of the check-boxes are changed, indicating whether to get
void AMImportController::onGetInfoFromFileBoxChecked(const QString& key) {

	/// \todo change UI components to an array of the same type of object, indexed by meta-data key. Handle generally.

	if(key == "name") {
		if(w_->checkName->isChecked()) {
			w_->nameEdit->setEnabled(false);
			if(currentScan_)
				w_->nameEdit->setText(currentScan_->name());
		}
		else
			w_->nameEdit->setEnabled(true);
	}

	if(key == "number") {
		if(w_->checkNumber->isChecked()) {
			w_->numberEdit->setEnabled(false);
			if(currentScan_)
				w_->numberEdit->setValue(currentScan_->number());
		}
		else
			w_->numberEdit->setEnabled(true);
	}

	if(key == "dateTime") {
		if(w_->checkDateTime->isChecked()) {
			w_->dateTimeEdit->setEnabled(false);
			if(currentScan_)
				w_->dateTimeEdit->setDateTime(currentScan_->dateTime());
		}
		else
			w_->dateTimeEdit->setEnabled(true);
	}

}

/// called when the whole process is finished.
void AMImportController::onFinished() {
	w_->hide();
	AMErrorMon::report(AMErrorReport(0, AMErrorReport::Information, 0, QString("Imported %1 files").arg(numSuccess_)));
	deleteLater();
}
