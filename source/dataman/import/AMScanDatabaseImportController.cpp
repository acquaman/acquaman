#include "AMScanDatabaseImportController.h"

#include <QDir>
#include "util/AMSettings.h"

#include "dataman/database/AMDatabase.h"
#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/database/AMDbObject.h"

#include "util/AMDateTimeUtils.h"

#include <QStringBuilder>
#include <QTime>

#include "dataman/AMRun.h"
#include "dataman/AMExperiment.h"
#include "dataman/AMSample.h"

#include <QApplication>

AMScanDatabaseImportController::AMScanDatabaseImportController(AMDatabase* destinationDb, QObject *parent) :
	QObject(parent)
{
	sourceDb_ = 0;
	destinationDb_ = destinationDb;
}

bool AMScanDatabaseImportController::setSourceFolderAndLoadDatabase(const QString &absolutePathToSourceFolder)
{
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
		delete sourceDb_;
		sourceDb_ = 0;
		return false;
	}

	return true;
}

void AMScanDatabaseImportController::analyzeFacilitiesForDuplicates()
{
	sourceFacilities_.clear();
	s2dFacilityIds_.clear();

	emit progressDescription("Checking Facilities...");
	emit stepProgress(0);

	// go through facilities in source, and make sure they all exist in destination. Map ids.
	QSqlQuery q = AMDbObjectSupport::s()->select<AMFacility>(sourceDb_, "id, name, description");
	int stepIndex = 0;
	int totalSteps = q.size();
	while(q.next()) {
		emit stepProgress(++stepIndex / totalSteps);	// if size is not supported in db, then this will be negative... which will trigger a "undefined" progress bar. Perfect.

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
	sourceRuns_.clear();
	s2dRunIds_.clear();

	emit progressDescription("Reviewing Runs...");
	emit stepProgress(0);

	// go through runs in source, and see if any exist in destination
	QSqlQuery q = AMDbObjectSupport::s()->select<AMRun>(sourceDb_, "id, name, dateTime");
	int stepIndex = 0;
	int totalSteps = q.size();
	while(q.next()) {
		emit stepProgress(++stepIndex / totalSteps);	// if size is not supported in db, then this will be negative... which will trigger a "undefined" progress bar. Perfect.

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
	sourceExperiments_.clear();
	s2dExperimentIds_.clear();

	emit progressDescription("Reviewing Experiments...");
	emit stepProgress(0);

	// go through experiments in source, and see if any exist in destination
	QSqlQuery q = AMDbObjectSupport::s()->select<AMExperiment>(sourceDb_, "id, name");
	int stepIndex = 0;
	int totalSteps = q.size();
	while(q.next()) {
		emit stepProgress(++stepIndex / totalSteps);	// if size is not supported in db, then this will be negative... which will trigger a "undefined" progress bar. Perfect.

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
	sourceSamples_.clear();
	s2dSampleIds_.clear();

	emit progressDescription("Reviewing Samples...");
	emit stepProgress(0);

	// go through samples in source, and see if any exist in destination
	QSqlQuery q = AMDbObjectSupport::s()->select<AMSample>(sourceDb_, "id, name, dateTime");
	int stepIndex = 0;
	int totalSteps = q.size();
	while(q.next()) {
		emit stepProgress(++stepIndex / totalSteps);	// if size is not supported in db, then this will be negative... which will trigger a "undefined" progress bar. Perfect.

		int id = q.value(0).toInt();
		QString name = q.value(1).toString();
		QDateTime dateTime = q.value(2).toDateTime();

		sourceSamples_[id] = name % ", created " % AMDateTimeUtils::prettyDateTime(dateTime);

		// Find a matching one in destinationDb_.
		QSqlQuery q2 = AMDbObjectSupport::s()->select<AMSample>(destinationDb_, "id", "name = ? AND dateTime = ?");
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
}

void AMScanDatabaseImportController::startDatabaseOperations()
{
	destinationDb_->startTransaction();
	copyFacilities();
	copyRuns();
	copyExperiments();
	copySamples();
	copyScans();
	destinationDb_->commitTransaction();
}

void AMScanDatabaseImportController::copyFacilities()
{
	emit progressDescription("Copying Facilities...");
	emit stepProgress(0);
	int totalSteps = s2dFacilityIds_.count();
	int currentStep = 0;

	QMutableMapIterator<int, int> i(s2dFacilityIds_);
	while (i.hasNext()) {
		i.next();
		emit stepProgress(++currentStep/totalSteps);
		// key is id; value is id in destination database, or -1 if not there yet. (if not there, need to insert)
		int sourceId = i.key(), destinationId = i.value();
		if(destinationId<1) {
			AMFacility f;
			f.loadFromDb(sourceDb_, sourceId);
			if(f.storeToDb(destinationDb_))
				i.setValue(f.id());
			else
				AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -2, "Could not import the facility '" % sourceFacilities_.value(sourceId) % "' into the database."));
		}
		qApp->sendPostedEvents();
		qApp->processEvents();
	}
}

void AMScanDatabaseImportController::copyRuns()
{
	emit progressDescription("Copying Runs...");
	emit stepProgress(0);
	int totalSteps = s2dRunIds_.count();
	int currentStep = 0;

	QMutableMapIterator<int, int> i(s2dRunIds_);
	while (i.hasNext()) {
		i.next();
		emit stepProgress(++currentStep/totalSteps);
		// key is id; value is id in destination database, or -1 if not there yet. (if not there, need to insert)
		int sourceId = i.key(), destinationId = i.value();
		if(destinationId<1) {
			AMRun r;
			r.loadFromDb(sourceDb_, sourceId);
			r.setFacilityId(s2dFacilityIds_.value(r.facilityId()));
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
	emit progressDescription("Copying Experiments...");
	emit stepProgress(0);
	int totalSteps = s2dExperimentIds_.count();
	int currentStep = 0;

	QMutableMapIterator<int, int> i(s2dExperimentIds_);
	while (i.hasNext()) {
		i.next();
		emit stepProgress(++currentStep/totalSteps);
		// key is id; value is id in destination database, or -1 if not there yet. (if not there, need to insert)
		int sourceId = i.key(), destinationId = i.value();
		if(destinationId<1) {
			AMExperiment e;
			e.loadFromDb(sourceDb_, sourceId);
			if(e.storeToDb(destinationDb_))
				i.setValue(e.id());
			else
				AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -2, "Could not import the experiment '" % sourceExperiments_.value(sourceId) % "' into the database."));
		}
		qApp->sendPostedEvents();
		qApp->processEvents();
	}
}

void AMScanDatabaseImportController::copySamples()
{
	emit progressDescription("Copying Samples...");
	emit stepProgress(0);
	int totalSteps = s2dSampleIds_.count();
	int currentStep = 0;

	QMutableMapIterator<int, int> i(s2dSampleIds_);
	while (i.hasNext()) {
		i.next();
		emit stepProgress(++currentStep/totalSteps);
		// key is id; value is id in destination database, or -1 if not there yet. (if not there, need to insert)
		int sourceId = i.key(), destinationId = i.value();
		if(destinationId<1) {
			AMSample s;
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
}
