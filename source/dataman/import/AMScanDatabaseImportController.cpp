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


#include "AMScanDatabaseImportController.h"

#include <QDir>
#include <QStringBuilder>
#include <QTime>
#include <QApplication>

#include "dataman/database/AMDatabase.h"
#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/database/AMDbObject.h"

#include "dataman/AMRun.h"
#include "dataman/AMExperiment.h"
#include "dataman/AMSamplePre2013.h"
#include "dataman/AMScan.h"

#include "util/AMSettings.h"
#include "util/AMDateTimeUtils.h"
#include "util/AMErrorMonitor.h"

 AMScanDatabaseImportController::~AMScanDatabaseImportController(){}
AMScanDatabaseImportController::AMScanDatabaseImportController(AMDatabase* destinationDb, const QString& destinationPath, QObject *parent) :
	QObject(parent)
{
	sourceDb_ = 0;
	destinationDb_ = destinationDb;
	destinationPath_ = destinationPath;
	state_ = Preparing;
}

bool AMScanDatabaseImportController::setSourceFolderAndLoadDatabase(const QString &absolutePathToSourceFolder)
{
	if(!(state_ == Preparing || state_ == Analyzing))
		return false;

	if(sourceDb_) {
		AMDatabase::deleteDatabase(sourceDb_->connectionName());
		sourceDb_ = 0;
	}

	// look for a database file in this folder
	QDir dir(absolutePathToSourceFolder);
	if(!dir.exists(AMUserSettings::userDatabaseFilename))
		return false;

	sourcePath_ = absolutePathToSourceFolder;
	QString connectionName = "ImportDb" % QTime::currentTime().toString("hhmmsszzz");	// a unique connection name
	sourceDb_ = AMDatabase::createDatabase(connectionName,
										   sourcePath_ % "/" % AMUserSettings::userDatabaseFilename);
	if(!sourceDb_)
		return false;


	if(!AMDbObjectSupport::s()->registerDatabase(sourceDb_)) {
		sourceDb_->deleteLater();
		sourceDb_ = 0;
		return false;
	}

	state_ = Analyzing;
	return true;
}

void AMScanDatabaseImportController::analyzeFacilitiesForDuplicates()
{
	if(state_ != Analyzing) {
		return;
	}

	sourceFacilities_.clear();
	s2dFacilityIds_.clear();

	emit progressDescription("Checking Facilities...");
	emit stepProgress(-1);

	// go through facilities in source, and make sure they all exist in destination. Map ids.
	QSqlQuery q = AMDbObjectSupport::s()->select<AMFacility>(sourceDb_, "id, name, description");
	q.exec();
	int stepIndex = 0;
	int totalSteps = q.size();
	while(q.next()) {
		emit stepProgress(int(100.0*(++stepIndex) / totalSteps));	// if size is not supported in db, then this will be negative... which will trigger a "undefined" progress bar. Perfect.

		int id = q.value(0).toInt();
		QString name = q.value(1).toString();
		QString description = q.value(2).toString();

		sourceFacilities_[id] = name % " " % description;

		// Find a matching one in destinationDb_.
		QSqlQuery q2 = AMDbObjectSupport::s()->select<AMFacility>(destinationDb_, "id", "name = ? AND description = ?");
		q2.bindValue(0, name);
		q2.bindValue(1, description);
		q2.exec();
		if(q2.first()) {
			// found one.
			s2dFacilityIds_[id] = q2.value(0).toInt();	// insert a mapping from source facility IDs to destination facility IDs
		}
		else {
			// none found in destination. Will need to create it.
			s2dFacilityIds_[id] = -1;
		}

		qApp->sendPostedEvents();
		qApp->processEvents();
	}

	emit stepProgress(100);
}

void AMScanDatabaseImportController::analyzeRunsForDuplicates()
{
	if(state_ != Analyzing)
		return;

	sourceRuns_.clear();
	s2dRunIds_.clear();

	emit progressDescription("Reviewing Runs...");
	emit stepProgress(-1);

	// go through runs in source, and see if any exist in destination
	QSqlQuery q = AMDbObjectSupport::s()->select<AMRun>(sourceDb_, "id, name, dateTime");
	q.exec();
	int stepIndex = 0;
	int totalSteps = q.size();
	while(q.next()) {
		emit stepProgress(int(100.0*(++stepIndex) / totalSteps));	// if size is not supported in db, then this will be negative... which will trigger a "undefined" progress bar. Perfect.

		int id = q.value(0).toInt();
		QString name = q.value(1).toString();
		QDateTime dateTime = q.value(2).toDateTime();

		sourceRuns_[id] = name % ", started " % AMDateTimeUtils::prettyDateTime(dateTime);

		// Find a matching one in destinationDb_.
		QSqlQuery q2 = AMDbObjectSupport::s()->select<AMRun>(destinationDb_, "id", "name = ? AND dateTime = ?");
		q2.bindValue(0, name);
		q2.bindValue(1, dateTime);
		q2.exec();
		if(q2.first()) {
			// found one.
			s2dRunIds_[id] = q2.value(0).toInt();	// insert a mapping from source run IDs to destination run IDs
		}
		else {
			// none found in destination. Will need to create it.
			s2dRunIds_[id] = -1;
		}

		qApp->sendPostedEvents();
		qApp->processEvents();
	}

	emit stepProgress(100);
}

void AMScanDatabaseImportController::analyzeExperimentsForDuplicates()
{
	if(state_ != Analyzing)
		return;

	sourceExperiments_.clear();
	s2dExperimentIds_.clear();

	emit progressDescription("Reviewing Experiments...");
	emit stepProgress(-1);

	// go through experiments in source, and see if any exist in destination
	QSqlQuery q = AMDbObjectSupport::s()->select<AMExperiment>(sourceDb_, "id, name");
	q.exec();
	int stepIndex = 0;
	int totalSteps = q.size();
	while(q.next()) {
		emit stepProgress(int(100.0*(++stepIndex) / totalSteps));	// if size is not supported in db, then this will be negative... which will trigger a "undefined" progress bar. Perfect.

		int id = q.value(0).toInt();
		QString name = q.value(1).toString();

		sourceExperiments_[id] = name;

		// Find a matching one in destinationDb_.
		QSqlQuery q2 = AMDbObjectSupport::s()->select<AMExperiment>(destinationDb_, "id", "name = ?");
		q2.bindValue(0, name);
		q2.exec();
		if(q2.first()) {
			// found one.
			s2dExperimentIds_[id] = q2.value(0).toInt();	// insert a mapping from source experiment IDs to destination experiment IDs
		}
		else {
			// none found in destination. Will need to create it.
			s2dExperimentIds_[id] = -1;
		}
		qApp->sendPostedEvents();
		qApp->processEvents();
	}

	emit stepProgress(100);
}

void AMScanDatabaseImportController::analyzeSamplesForDuplicates()
{
	if(state_ != Analyzing)
		return;

	sourceSamples_.clear();
	s2dSampleIds_.clear();

	emit progressDescription("Reviewing Samples...");
	emit stepProgress(-1);

	// go through samples in source, and see if any exist in destination
	QSqlQuery q = AMDbObjectSupport::s()->select<AMSamplePre2013>(sourceDb_, "id, name, dateTime");
	q.exec();
	int stepIndex = 0;
	int totalSteps = q.size();
	while(q.next()) {
		emit stepProgress(int(100.0*(++stepIndex) / totalSteps));	// if size is not supported in db, then this will be negative... which will trigger a "undefined" progress bar. Perfect.

		int id = q.value(0).toInt();
		QString name = q.value(1).toString();
		QDateTime dateTime = q.value(2).toDateTime();

		sourceSamples_[id] = name % ", created " % AMDateTimeUtils::prettyDateTime(dateTime);

		// Find a matching one in destinationDb_.
		QSqlQuery q2 = AMDbObjectSupport::s()->select<AMSamplePre2013>(destinationDb_, "id", "name = ? AND dateTime = ?");
		q2.bindValue(0, name);
		q2.bindValue(1, dateTime);
		q2.exec();
		if(q2.first()) {
			// found one.
			s2dSampleIds_[id] = q2.value(0).toInt();	// insert a mapping from source sample IDs to destination sample IDs
		}
		else {
			// none found in destination. Will need to create it.
			s2dSampleIds_[id] = -1;
		}
		qApp->sendPostedEvents();
		qApp->processEvents();
	}

	emit stepProgress(100);
	state_ = ReadyToImport;
}

void AMScanDatabaseImportController::startDatabaseOperations()
{
	if(state_ != ReadyToImport)
		return;

	destinationDb_->startTransaction();
	state_ = Importing;
	copyFacilities();
	copyRuns();
	copyExperiments();
	copySamples();
	copyScans();
	if(state_ == Importing) {	// if we were cancelled, don't commit the transaction.
		state_ = Finished;
		destinationDb_->commitTransaction();
		emit progressDescription("Import complete.");
		emit stepProgress(100);
	}
}

void AMScanDatabaseImportController::copyFacilities()
{
	if(state_ != Importing) return;

	emit progressDescription("Copying Facilities...");
	emit stepProgress(-1);
	int totalSteps = s2dFacilityIds_.count();
	int currentStep = 0;

	QMutableMapIterator<int, int> i(s2dFacilityIds_);
	while (i.hasNext()) {
		if(state_ != Importing) return;
		i.next();
		emit stepProgress(int(100.0*(++currentStep)/totalSteps));
		// key is id. Value is id in destination database, or -1 if not there yet. (if not there, need to insert)
		int sourceId = i.key(), destinationId = i.value();
		if(destinationId<1) {
			AMFacility f;
			f.loadFromDb(sourceDb_, 1); //AMFacility table will have only one entry, which is the definition of this facility
			if(f.storeToDb(destinationDb_))
				i.setValue(f.id());
			else
				AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -2, "Could not import the facility '" % sourceFacilities_.value(1) % "' into the database."));
		}
		qApp->sendPostedEvents();
		qApp->processEvents();
	}
}

void AMScanDatabaseImportController::copyRuns()
{
	if(state_ != Importing) return;

	emit progressDescription("Copying Runs...");
	emit stepProgress(-1);
	int totalSteps = s2dRunIds_.count();
	int currentStep = 0;

	QMutableMapIterator<int, int> i(s2dRunIds_);
	while (i.hasNext()) {
		if(state_ != Importing) return;
		i.next();
		emit stepProgress(int(100.0*(++currentStep)/totalSteps));
		// key is id. Value is id in destination database, or -1 if not there yet. (if not there, need to insert)
		int sourceId = i.key(), destinationId = i.value();
		if(destinationId<1) {
			AMRun r;
			r.loadFromDb(sourceDb_, 1); //AMFacility table will have only one entry, which is the definition of this facility
			if(r.storeToDb(destinationDb_))
				i.setValue(r.id());
			else
				AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -2, "Could not import the run '" % sourceRuns_.value(sourceId) % "' into the database."));
		}
		qApp->sendPostedEvents();
		qApp->processEvents();
	}
}

void AMScanDatabaseImportController::copyExperiments()
{
	if(state_ != Importing) return;
	emit progressDescription("Copying Experiments...");
	emit stepProgress(-1);
	int totalSteps = s2dExperimentIds_.count();
	int currentStep = 0;

	QMutableMapIterator<int, int> i(s2dExperimentIds_);
	while (i.hasNext()) {
		if(state_ != Importing) return;
		i.next();
		emit stepProgress(int(100.0*(++currentStep)/totalSteps));
		// key is id. Value is id in destination database, or -1 if not there yet. (if not there, need to insert)
		int sourceId = i.key(), destinationId = i.value();
		if(destinationId<1) {
			AMExperiment e;
			e.loadFromDb(sourceDb_, sourceId);
			if(e.storeToDb(destinationDb_))
				i.setValue(e.id());
			else
				AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -2, "Could not import the experiment '" % sourceExperiments_.value(sourceId) % "' into the database."));
		}
		// TODO: need to update the scans that should be in this experiment. This will be tricky... don't know new scan IDs yet.
		qApp->sendPostedEvents();
		qApp->processEvents();
	}
}

void AMScanDatabaseImportController::copySamples()
{
	if(state_ != Importing) return;
	emit progressDescription("Copying Samples...");
	emit stepProgress(-1);
	int totalSteps = s2dSampleIds_.count();
	int currentStep = 0;

	QMutableMapIterator<int, int> i(s2dSampleIds_);
	while (i.hasNext()) {
		if(state_ != Importing) return;
		i.next();
		emit stepProgress(int(100.0*(++currentStep)/totalSteps));
		// key is id. Value is id in destination database, or -1 if not there yet. (if not there, need to insert)
		int sourceId = i.key(), destinationId = i.value();
		if(destinationId<1) {
			AMSamplePre2013 s;
			s.loadFromDb(sourceDb_, sourceId);
			if(s.storeToDb(destinationDb_))
				i.setValue(s.id());
			else
				AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -2, "Could not import the sample '" % sourceSamples_.value(sourceId) % "' into the database."));
		}
		qApp->sendPostedEvents();
		qApp->processEvents();
	}
}


void AMScanDatabaseImportController::copyScans()
{
	if(state_ != Importing) return;

	emit progressDescription("Copying Scans...");
	emit stepProgress(-1);

	QString scanTableName = AMDbObjectSupport::s()->tableNameForClass<AMScan>();


	QList<int> scanIds = sourceDb_->objectsWhere(scanTableName, QString());
	AMScan::setAutoLoadData(false);	// disable loading data for performance. It will also fail to load because it's in the wrong spot.
	for(int i=0, cc=scanIds.count(); i<cc; i++) {
		if(state_ != Importing) return;

		emit stepProgress(int(100.0*(i+1)/cc));

		AMDbObject* object = AMDbObjectSupport::s()->createAndLoadObjectAt(sourceDb_, scanTableName, scanIds.at(i));
		if(!object) {
			AMErrorMon::report(AMErrorReport(this,
											 AMErrorReport::Alert,
											 -3,
											 QString("Error copying the scan with ID '%1' out of the import database. Your database might be corrupted. Please report this problem to the Acquaman developers.").arg(scanIds.at(i))));
			continue;
		}
		AMScan* scan = qobject_cast<AMScan*>(object);
		if(!scan) {
			AMErrorMon::report(AMErrorReport(this,
											 AMErrorReport::Alert,
											 -4,
											 QString("Error copying the scan with ID '%1' out of the import database: it wasn't a scan object. Your database might be corrupted. Please report this problem to the Acquaman developers.").arg(scanIds.at(i))));
			object->deleteLater();
			continue;
		}

		// update scan fields:
		scan->setRunId(s2dRunIds_.value(scan->runId(), -1));
		scan->setSampleId(s2dSampleIds_.value(scan->sampleId(), -1));

		// copy the raw data: filePath and additionalFilePaths
		QDir destinationDir(destinationPath_);
		// Assuming paths always in relative file format.
		QString filePath = scan->filePath();

		// Does the file exist already in the destination?
		if(destinationDir.exists(filePath)) {
			// I know we're not supposed to be a GUI module... but is it okay to prompt here for what to do?
			if(userSaysShouldSkipDuplicateScan(scan)) {
				scan->deleteLater();
				continue;
			}
			filePath = makeUniqueFileName(destinationPath_, filePath);
		}
		copyFileAndMakeFoldersAsRequired(sourcePath_ % "/" % scan->filePath(),
										 destinationPath_ % "/" % filePath);
		scan->setFilePath(filePath);

		// Copy additional file paths:
		QStringList extraPaths = scan->additionalFilePaths();
		for(int j=0, cc=extraPaths.count(); j<cc; j++) {
			QString destinationFilePath = extraPaths.at(j);
			if(destinationDir.exists(destinationFilePath)) {
				destinationFilePath = makeUniqueFileName(destinationPath_, destinationFilePath);
			}
			copyFileAndMakeFoldersAsRequired(sourcePath_ % "/" % extraPaths.at(j),
											 destinationPath_ % "/" % destinationFilePath);
			extraPaths[j] = destinationFilePath;
		}
		scan->setAdditionalFilePaths(extraPaths);


		// OK. raw data copied and file paths adjusted. Just store this scan in the new db:
		if(!scan->storeToDb(destinationDb_, false)) {	// HOLD UP! Don't generate those thumbnails... We'll do a direct copy.
			AMErrorMon::report(AMErrorReport(this,
											 AMErrorReport::Alert,
											 -4,
											 QString("Error copying the scan with ID '%1' into the new database. Your database might be corrupted. Please report this problem to the Acquaman developers.").arg(scanIds.at(i))));
			scan->deleteLater();
			continue;
		}



		// Copy the thumbnails from the old DB to the new DB:
		// find the old thumbnails:
		QStringList thumbnailCols;
		thumbnailCols << "objectId" << "objectTableName" << "number" << "type" << "title" << "subtitle" << "thumbnail";
		int thumbnailFirstId = -1;
		int thumbnailCount = 0;
		QSqlQuery q = sourceDb_->select(AMDbObjectSupport::thumbnailTableName(),
										thumbnailCols.join(","),
										"objectId = ? AND objectTableName = ?");
		q.bindValue(0, scanIds.at(i));
		q.bindValue(1, scanTableName);
		if(!q.exec()) {
			AMErrorMon::report(AMErrorReport(this,
											 AMErrorReport::Alert,
											 -4,
											 QString("Error copying the the thumbnails for scan with ID '%1' out of the old database. Please report this problem to the Acquaman developers.").arg(scanIds.at(i))));
		}
		else {
			if(q.first()) {
				QVariantList thumbnailVals;
				thumbnailVals << q.value(0) << q.value(1) << q.value(2) << q.value(3) << q.value(4) << q.value(5) << q.value(6);
				thumbnailVals[0] = scan->id();	// update the scan id from old DB to new DB's value
				thumbnailFirstId = destinationDb_->insertOrUpdate(0, AMDbObjectSupport::thumbnailTableName(), thumbnailCols, thumbnailVals);
				if(thumbnailFirstId > 0)
					thumbnailCount++;
				else
					AMErrorMon::report(AMErrorReport(this,
													 AMErrorReport::Alert,
													 -4,
													 QString("Error copying the the thumbnails for scan with ID '%1' into the new database. Please report this problem to the Acquaman developers.").arg(scanIds.at(i))));
			}
			while(q.next()) {
				QVariantList thumbnailVals;
				thumbnailVals << q.value(0) << q.value(1) << q.value(2) << q.value(3) << q.value(4) << q.value(5) << q.value(6);
				thumbnailVals[0] = scan->id(); // update the scan id from old DB to new DB's value
				if(destinationDb_->insertOrUpdate(0, AMDbObjectSupport::thumbnailTableName(), thumbnailCols, thumbnailVals) > 0)
					thumbnailCount++;
			}
		}

		// Finally, update the thumbnail references in the scan object:
		// now that we know where the thumbnails are, update this in the object table
		if(!destinationDb_->update(scan->id(),
								   scanTableName,
								   QStringList() << "thumbnailCount" << "thumbnailFirstId",
								   QVariantList() << thumbnailCount << thumbnailFirstId)) {
			AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, AMSCANDATABASEIMPORTCONTROLLER_ERROR_STORING_UPDATED_THUMBNAIL_COUNT_AND_FIRST_ID, QString("While importing, error trying to store the updated thumbnail count and firstThumbnailId for database object %1. Please report this problem to the Acquaman developers.").arg(scanIds.at(i))));
		}


		// What experiments was this scan in? Need to map and add in new database.
		QList<int> experiments = AMExperiment::experimentsContainingScan(scanIds.at(i), sourceDb_);
		foreach(int oldExperimentId, experiments) {
			int newExperimentId = s2dExperimentIds_.value(oldExperimentId);
			if(newExperimentId > 0)
				AMExperiment::addScanToExperiment(scan->id(), newExperimentId, destinationDb_);
		}

		// All done!
		scan->deleteLater();
		qApp->sendPostedEvents();
		qApp->processEvents();
	}
	AMScan::setAutoLoadData(true);	// turn it back on, so we don't interfere with anyone else...
}

#include <QFileInfo>
QString AMScanDatabaseImportController::makeUniqueFileName(const QString &parentFolder, const QString &file)
{
	QDir parentDir(parentFolder);
	if(!parentDir.exists(file))
		return file;

	QFileInfo fileInfo(file);
	QString completeSuffix = fileInfo.completeSuffix();
	QString baseName = fileInfo.baseName();
	QString path = fileInfo.path();

	int numberAppended = 1;
	QString adjustedName;
	while(parentDir.exists((adjustedName = path % "/" % baseName % "_c" % QString::number(numberAppended) % "." % completeSuffix)))
		numberAppended++;

	return adjustedName;
}

#include <QMessageBox>
#include <QPushButton>
bool AMScanDatabaseImportController::userSaysShouldSkipDuplicateScan(AMScan *scan)
{
	QMessageBox mb;
	mb.setText(QString("It looks like a similar scan already exists in your database. Do you want to skip importing this (possibly duplicated) scan?"));
	mb.setInformativeText(QString("Scan to import: %1 (#%2)\nRaw data file name: %3")
						  .arg(scan->name())
						  .arg(scan->number())
						  .arg(scan->filePath()));
	mb.setIcon(QMessageBox::Question);
	QPushButton* skipButton = mb.addButton("Skip this scan", QMessageBox::AcceptRole);
	QPushButton* importDuplicateButton = mb.addButton("Import (possible) duplicate", QMessageBox::RejectRole);
	mb.setDefaultButton(importDuplicateButton);
	mb.setEscapeButton(importDuplicateButton);

	mb.exec();
	return (mb.clickedButton() == skipButton);
}

bool AMScanDatabaseImportController::copyFileAndMakeFoldersAsRequired(const QString &sourcePath, const QString &destinationPath)
{
	QFileInfo destinationFileInfo(destinationPath);
	QString destinationFolder = destinationFileInfo.path();
	if(!QDir(destinationFolder).exists())
		if(!QDir::root().mkpath(destinationFolder))
			return false;
	return QFile::copy(sourcePath, destinationPath);
}

void AMScanDatabaseImportController::cancelAndRollBack()
{
	if(state_ == Importing) {
		state_ = Cancelled;	// this will trigger all functions to give up
		destinationDb_->rollbackTransaction();
		emit progressDescription("Cancelled. Your database has not been changed.");
		emit stepProgress(0);
	}
}
