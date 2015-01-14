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


#include "AMImportController.h"


#include "dataman/AMXASScan.h"
#include "dataman/SGM/SGM2004FileLoader.h"
#include "dataman/ALSBL8XASFileLoader.h"
#include "ui/dataman/AMImportControllerWidget.h"
#include "util/AMErrorMonitor.h"

#include <QDir>
#include <QFileDialog>
#include <QSignalMapper>

#include <QStringBuilder>

#include <QTimer>

/// Attempt to import, and return a pointer to a new scan object, or 0 if the import fails.  It's the caller's responsibility to delete the scan when done with it.
AMScan* SGMLegacyImporter::createScanAndImport(const QString& fullPath) {

	AMXASScan* rv = new AMXASScan();
	rv->rawData()->addScanAxis( AMAxisInfo("eV", 0, "Incident Energy", "eV") );

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
		rv->deleteLater();
		return 0;
	}
}

#include <QFileInfo>
AMScan* ALSBL8XASImporter::createScanAndImport(const QString& fullPath) {

	AMXASScan* rv = new AMXASScan();
	rv->rawData()->addScanAxis( AMAxisInfo("eV", 0, "Incident Energy", "eV") );

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
		rv->deleteLater();
		return 0;
	}
}



AMImportController::~AMImportController() {
	for(int i=0; i<importers_.count(); i++)
		delete importers_[i];

	fileDialog_->deleteLater();
	importWidget_->deleteLater();
}

AMImportController::AMImportController(QObject *parent) :
	QObject(parent)
{
	installImporters();

	fileDialog_ = new QFileDialog();
	importWidget_ = new AMImportControllerWidget();

	foreach(AMImporter* importer, importers_)
		importWidget_->addFormat(importer->description());


	connect(fileDialog_, SIGNAL(finished(int)), this, SLOT(onFileDialogFinished(int)));

	connect(importWidget_, SIGNAL(nextClicked()), this, SLOT(onNextButtonClicked()));
	connect(importWidget_, SIGNAL(applyAllClicked()), this, SLOT(onApplyAllButtonClicked()));
	connect(importWidget_, SIGNAL(cancelClicked()), this, SLOT(onCancelButtonClicked()));
	connect(importWidget_, SIGNAL(selectedFormatChanged(int)), this, SLOT(onFileTypeComboBoxChanged(int)));

	connect(importWidget_, SIGNAL(customizeNameChanged(bool)), this, SLOT(onCustomizeNameChanged(bool)));
	connect(importWidget_, SIGNAL(customizeNumberChanged(bool)), this, SLOT(onCustomizeNumberChanged(bool)));
	connect(importWidget_, SIGNAL(customizeDateTimeChanged(bool)), this, SLOT(onCustomizeDateTimeChanged(bool)));

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

	importWidget_->show();
	importWidget_->resize(importWidget_->minimumSizeHint());
	setupNextFile();
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

	// short-term hack: identify what looks like a "spectra" file (an auxiliary file for CLS dacq scans), and move on/skip it
	while(isAuxiliaryFile(filesToImport_.at(currentFile_))) {
		currentFile_++;

		if(currentFile_ >= filesToImport_.count()) {
			// all done:
			onFinished();
			return;
		}
	}

	importWidget_->progressBar()->setValue(currentFile_);
	importWidget_->progressBar()->setMaximum(filesToImport_.count());
	importWidget_->progressBar()->setMinimum(0);
	importWidget_->setProgressLabel(QString("File %1 of %2").arg(currentFile_+1).arg(filesToImport_.count()));

	// separate file name and path
	QFileInfo fi(filesToImport_.at(currentFile_));
	QString name = fi.fileName();
	QString path = fi.path();

	importWidget_->thumbnailViewer()->setCaption1(name);
	importWidget_->thumbnailViewer()->setCaption2(path);

	/// \todo: switch importer automatically if current one doesn't match filter, and another does.

	AMImporter* importer = importers_.at(importWidget_->selectedFormatIndex());
	currentScan_ = importer->createScanAndImport(filesToImport_.at(currentFile_));

	importWidget_->thumbnailViewer()->setSource(currentScan_);

	if(currentScan_ == 0) {

		importWidget_->setLoadingStatus("Could not load this file.");
	}

	else {
		importWidget_->setLoadingStatus("Loaded successfully.");
	}


	/// todo: you could generalize this for meta-data types, instead of doing them all separately.
	///////////////////////
	if(!importWidget_->isCustomizeNameChecked() && currentScan_)
		importWidget_->setName(currentScan_->name());

	if(!importWidget_->isCustomizeNumberChecked() && currentScan_)
		importWidget_->setNumber(currentScan_->number());

	if(!importWidget_->isCustomizeDataTimeChecked() && currentScan_)
		importWidget_->setDateTime(currentScan_->dateTime());
	/////////////////////////////


	// This is our magic way of handling "Apply to All", while keeping the progress bar for free and stopping if we ever get an import error:  After setting up the next file, we just schedule a signal to automatically push the next button, as if the user was doing it really fast.
	if(applyToAll_) {
		QTimer::singleShot(10, this, SLOT(onNextButtonClicked()));
	}
}

#include <QMessageBox>
#include "dataman/database/AMDatabase.h"

void AMImportController::finalizeImport() {
	// error loading:
	if(currentScan_ == 0) {
		int doSkip = QMessageBox::question(
					importWidget_,
					"Skip this file?",
					QString("Couldn't load this file (%1) with the '%2' format.\n\nDo you want to skip this file?").arg(filesToImport_.at(currentFile_)).arg(importWidget_->format()),
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
		if(importWidget_->isCustomizeNameChecked())
			currentScan_->setName(importWidget_->name());

		if(importWidget_->isCustomizeNumberChecked())
			currentScan_->setNumber(importWidget_->number());

		if(importWidget_->isCustomizeDataTimeChecked())
			currentScan_->setDateTime(importWidget_->dateTime());

		currentScan_->setSampleId(importWidget_->sampleId());
		currentScan_->setRunId(importWidget_->runEdit()->currentRunId());

		QString copyErrorMessage;
		if(!copyRawDataFiles(copyErrorMessage)) {
			AMErrorMon::report(AMErrorReport(0, AMErrorReport::Serious, -2, "AMImportController: Could not copy the imported file into the library. Maybe this file exists already?"));
			int doSkip = QMessageBox::question(
						importWidget_,
						"Skip this file?",
						QString("The file '%1' was opened correctly, but it couldn't be copied to the data library. (%2) \n\nDo you want to skip this file?").arg(filesToImport_.at(currentFile_)).arg(copyErrorMessage),
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

			if( currentScan_->storeToDb(AMDatabase::database("user")) )
				numSuccess_++;
			else
				AMErrorMon::report(AMErrorReport(this, AMErrorReport::Serious, -1, "The file was loaded correctly, but it could not be saved to the database."));
		}

		importWidget_->thumbnailViewer()->setSource(0);
		currentScan_->deleteLater();
		currentScan_ = 0;
	}
}


bool AMImportController::copyRawDataFiles(QString &errorMessage)
{
	// copy the raw data file to the library:
	QFileInfo file(filesToImport_.at(currentFile_));
	QDir dir;
	QString relativePath;
	QDateTime dt = currentScan_->dateTime();
	if(dt.isValid())
		relativePath = currentScan_->dateTime().toString("yyyy/MM");
	else
		relativePath = "UnknownDate";

	dir.mkpath(AMUserSettings::userDataFolder + "/" + relativePath);
	currentScan_->setFilePath(relativePath + "/" + file.fileName());

	QString destination = AMUserSettings::userDataFolder + "/" + currentScan_->filePath();

	if(!QFile::copy(filesToImport_.at(currentFile_), destination)) {
		errorMessage = QString("Maybe the destination file '%1' already exists?").arg(destination);
		return false;
	}

	// Do we need to copy over additionalFilePaths()? [These would be extra raw data files affiliated with this scan, aside from the main raw data file at fileName().] They start out as absolute paths, but we'll change them to relative paths as we copy them.
	QStringList additionalFilePaths = currentScan_->additionalFilePaths();
	QStringList relativeAdditionalFilePaths;

	foreach(QString additionalFilePath, additionalFilePaths) {
		QFileInfo additionalFileInfo(additionalFilePath);
		QString relativeAdditionalFilePath  = relativePath + "/" + additionalFileInfo.fileName();
		relativeAdditionalFilePaths << relativeAdditionalFilePath;

		if(!QFile::copy(additionalFilePath, AMUserSettings::userDataFolder + "/" + relativeAdditionalFilePath)) {
			errorMessage = QString("Could not copy '%1' to '%2/%3'.").arg(additionalFilePath).arg(AMUserSettings::userDataFolder).arg(relativeAdditionalFilePath);
			return false;
		}
	}

	currentScan_->setAdditionalFilePaths(relativeAdditionalFilePaths);
	return true;
}



/// Called when the 'apply to all' button is clicked while reviewing
void AMImportController::onApplyAllButtonClicked() {
	applyToAll_ = true;
	QTimer::singleShot(0, this, SLOT(onNextButtonClicked()));
}

/// called when the cancel button is clicked while reviewing
void AMImportController::onCancelButtonClicked() {
	int doCancel = QMessageBox::question(
				importWidget_,
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

void AMImportController::onCustomizeNameChanged(bool)
{
	if(currentScan_)
		importWidget_->setName(currentScan_->name());
}

void AMImportController::onCustomizeNumberChanged(bool)
{
	if(currentScan_)
		importWidget_->setNumber(currentScan_->number());

}

void AMImportController::onCustomizeDateTimeChanged(bool)
{
	if(currentScan_)
		importWidget_->setDateTime(currentScan_->dateTime());
}

/// called when the whole process is finished.
void AMImportController::onFinished() {
	importWidget_->hide();
	AMErrorMon::report(AMErrorReport(0, AMErrorReport::Information, 0, QString("Imported %1 files").arg(numSuccess_)));
	deleteLater();
}

bool AMImportController::isAuxiliaryFile(const QString &fullFileName)
{
	QFileInfo fi(fullFileName);
	QString name = fi.fileName();
	QString path = fi.path();

	if(name.endsWith("_spectra.dat")) {
		// looks right. check for non-spectra file.
		name.chop(12); // removes _spectra.dat
		name.append(".dat");
		QFileInfo parentFile(path % "/" % name);
		if(parentFile.exists() && parentFile.isFile())
			return true;	// we found the parent file. This is definitely a spectra file.
	}

	return false;
}

