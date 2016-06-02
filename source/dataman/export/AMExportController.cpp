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


#include "AMExportController.h"

#include <QComboBox>
#include <QHashIterator>
#include <QDir>
#include "acquaman.h"
#include <QTimer>
#include <QStandardItemModel>
#include <QStringBuilder>

#include "dataman/export/AMExporter.h"
#include "dataman/export/AMExporterOption.h"
#include "dataman/AMUser.h"

#include "util/AMErrorMonitor.h"
#include "dataman/database/AMDbObjectSupport.h"
#include "dataman/AMScan.h"
#include "application/AMAppControllerSupport.h"

QHash<QString, AMExporterInfo> AMExportController::registeredExporters_;

AMExportController::AMExportController(const QList<QUrl> &scansToExport, QObject *parent)
	: QObject(parent)
{
	usingScanURLs_ = true;
	usingScanObjects_ = false;
	exporter_ = 0;
	option_ = 0;
	scanURLsToExport_ = scansToExport;
	state_ = Preparing;
	availableDataSourcesModel_ = new QStandardItemModel(this);

	searchScanIndex_ = -1;
	exportScanIndex_ = -1;
	succeededCount_ = failedCount_ = 0;

	if(!scansToExport.isEmpty())
	{
		AMScan* scan = scanFromUrl(scansToExport.at(0));
		if (scan && scan->scanConfiguration())
			option_ = AMAppControllerSupport::createExporterOption(scan->scanConfiguration());
	}

	destinationFolderPath_ = AMUser::user()->lastExportDestination();
	if(destinationFolderPath_.isEmpty())
		destinationFolderPath_ = QDir::toNativeSeparators(QDir::homePath());
}

AMExportController::AMExportController(const QList<AMScan*>& scansToExport, QObject *parent)
	: QObject(parent)
{
	usingScanObjects_ = true;
	usingScanURLs_ = false;
	exporter_ = 0;
	option_ = 0;
	scanObjectsToExport_ = scansToExport;
	state_ = Preparing;
	availableDataSourcesModel_ = new QStandardItemModel(this);

	searchScanIndex_ = -1;
	exportScanIndex_ = -1;
	succeededCount_ = failedCount_ = 0;

	if(!scansToExport.isEmpty())
	{
		AMScan* scan = scansToExport.at(0);
		if(scan && scan->scanConfiguration())
			option_ = AMAppControllerSupport::createExporterOption(scan->scanConfiguration());
	}

	destinationFolderPath_ = AMUser::user()->lastExportDestination();
	if(destinationFolderPath_.isEmpty())
		destinationFolderPath_ = QDir::toNativeSeparators(QDir::homePath());
}

bool AMExportController::chooseExporter(const QString &exporterClassName)
{
	if(!registeredExporters_.contains(exporterClassName))
		return false;

	if(exporter_)
		exporter_->deleteLater();

	exporter_ = qobject_cast<AMExporter*>(registeredExporters_.value(exporterClassName).metaObject->newInstance());
	exporter_->setParent(this);

	return true;
}

void AMExportController::setDefaultDestinationFolderPath()
{
	QDir exportDir;
	if(!AMUserSettings::remoteDataFolder.isEmpty())
		exportDir.setCurrent(AMUserSettings::remoteDataFolder);
	else
		exportDir.setCurrent(AMUserSettings::userDataFolder);
	exportDir.cdUp();

	if(!exportDir.entryList(QDir::AllDirs).contains("exportData")){
		if(!exportDir.mkdir("exportData")){

			AMErrorMon::alert(this, AMSCANACTION_CANT_CREATE_EXPORT_FOLDER, "Could not create the auto-export folder.");
			return;
		}
	}

	exportDir.cd("exportData");

	setDestinationFolderPath(exportDir.absolutePath());
}

void AMExportController::searchForAvailableDataSources()
{
	if(searchScanIndex_ < 0) {
		searchScanIndex_ = 0;
		QTimer::singleShot(10, this, SLOT(continueAvailableDataSourceSearch()));	// and we're off...
	}
}

void AMExportController::continueAvailableDataSourceSearch()
{
	if(searchScanIndex_ >= scanCount())
		return; // We're done!

	if(usingScanURLs_) {
		const QUrl& url = scanURLsToExport_.at(searchScanIndex_++);	// incrementing searchScanIndex_ here.

		AMDatabase* db = 0;
		QStringList path;
		QString tableName;
		int id = 0;
		bool idOkay = false;

		// parse the URL and make sure it's valid
		if(url.scheme() == "amd" &&
				(db = AMDatabase::database(url.host())) &&
				(path = url.path().split('/', QString::SkipEmptyParts)).count() == 2 &&
				(id = path.at(1).toInt(&idOkay)) > 0 &&
				idOkay == true &&
				(tableName = path.at(0)).isEmpty() == false
				) {

			// let's roll.  Find all the raw data sources for this scan
			QSqlQuery q = db->select(tableName % "_rawDataSources", "id2,table2", "id1='" % QString::number(id) % "'");	// note: checked that this is indeed using the index. Can go faster? Dunno.
			q.exec();
			while(q.next()) {
				// get name, description, rank for this data source
				QSqlQuery q2 = db->select( q.value(1).toString(),
										   "name,description,rank",
										   "id='" % q.value(0).toString() % "'");
				q2.exec();
				if(q2.next()) {
					addFoundAvailableDataSource(q2.value(0).toString(), q2.value(1).toString(), q2.value(2).toInt());
				}
			}

			// Find all the analyzed data sources for this scan
			q = db->select(tableName % "_analyzedDataSources", "id2,table2", "id1='" % QString::number(id) % "'");	// note: checked that this is indeed using the index. Can go faster? Dunno.
			q.exec();
			while(q.next()) {
				// get name, description, rank for this data source
				QSqlQuery q2 = db->select( q.value(1).toString(),
										   "name,description,rank",
										   "id='" % q.value(0).toString() % "'");
				q2.exec();
				if(q2.next()) {
					addFoundAvailableDataSource(q2.value(0).toString(), q2.value(1).toString(), q2.value(2).toInt());
				}
			}
		}
	}
	else if(usingScanObjects_){
		const AMScan *scan = scanObjectsToExport_.at(searchScanIndex_++);
		const AMRawDataSourceSet *rawSources = scan->rawDataSources();
		const AMRawDataSource *rds;
		for(int x = 0; x < rawSources->count(); x++){
			rds = rawSources->at(x);
			addFoundAvailableDataSource(rds->name(), rds->description(), rds->rank());
		}
		const AMAnalyzedDataSourceSet *analyzedSources = scan->analyzedDataSources();
		const AMAnalysisBlock *ads;
		for(int x = 0; x < analyzedSources->count(); x++){
			ads = analyzedSources->at(x);
			addFoundAvailableDataSource(ads->name(), ads->description(), ads->rank());
		}
	}

	// Schedule us to continue onto next scan.  This 10ms timer might need to be adjusted for acceptable performance.
	QTimer::singleShot(10, this, SLOT(continueAvailableDataSourceSearch()));
}

void AMExportController::addFoundAvailableDataSource(const QString &name, const QString &description, int rank)
{
	// search for existing items that are duplicate. (This shouldn't be that expensive... we won't get many items in the available sources list, compared to the list of scans to export.  Short enough that we're better off linear searching than hashing and re-indexing)
	for(int row = availableDataSourcesModel_->rowCount()-1; row >=0; row--) {
		QStandardItem* searchItem = availableDataSourcesModel_->item(row);
		if(searchItem &&
				searchItem->data(AM::NameRole).toString() == name &&
				searchItem->data(AM::DescriptionRole).toString() == description &&
				searchItem->data(AM::UserRole).toInt() == rank
				)
			return;	// don't add to model... We've got it already.
	}

	QStandardItem* newItem;
	if(name == description)
		newItem = new QStandardItem(QString("[%1d] %2").arg(rank).arg(name));
	else
		newItem = new QStandardItem(QString("[%1d] %2: \"%3\"").arg(rank).arg(name).arg(description));

	newItem->setData(name, AM::NameRole);
	newItem->setData(description, AM::DescriptionRole);
	newItem->setData(rank, AM::UserRole);

	// to make sorting by rank easier, let's put the rank into column 1.
	QStandardItem* newItemRank = new QStandardItem(QString::number(rank));

	availableDataSourcesModel_->appendRow(QList<QStandardItem*>() << newItem << newItemRank);	// add to model
	availableDataSourcesModel_->sort(1, Qt::AscendingOrder); // and (re)-sort by rank.
}

bool AMExportController::start(bool autoExport)
{
	if(!exporter_)
		return false;
	if(!option_)
		return false;

	if(state_ != Preparing)
		return false;	// Can't start except from this state

	exportScanIndex_ = 0;
	emit stateChanged(state_ = Exporting);

	if (autoExport)
		continueScanExport();

	else
		QTimer::singleShot(0, this, SLOT(continueScanExport()));

	return true;
}

void AMExportController::continueScanExport()
{
	if(state_ != Exporting)
		return;	// done, or paused. Don't keep going.

	// 0. emit progress and signals
	emit progressChanged(exportScanIndex_, scanCount());

	// 1. Check for finished:
	if(exportScanIndex_ >= scanCount()) {
		emit stateChanged(state_ = Finished);

		// Reset whether the exporter should overwrite files with matching filenames.
		exporter_->setOverwriteOption(AMExporter::Default);

		QString message = "Exported " % QString::number(succeededCount()) % " scans.";
		if(failedCount())
			message.append("  (" % QString::number(failedCount()) % " scans could not be exported.)");
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Information, 0, message));

		AMUser::user()->setLastExportDestination(destinationFolderPath());
		AMUser::user()->storeToDb(AMUser::user()->database());

		deleteLater();
		return; // We're done!
	}

	try {
		// 2. Load scan from db and check loaded successfully


		AMScan* scan = 0;
		AMDbObject* databaseObject = 0;
		if(usingScanURLs_){
			const QUrl& url = scanURLsToExport_.at(exportScanIndex_);
			AMDatabase* db = 0;
			QStringList path;
			QString tableName;
			int id = 0;
			bool idOkay = false;

			// parse the URL and make sure it's valid
			if(!( url.scheme() == "amd" &&
				  (db = AMDatabase::database(url.host())) &&
				  (path = url.path().split('/', QString::SkipEmptyParts)).count() == 2 &&
				  (id = path.at(1).toInt(&idOkay)) > 0 &&
				  idOkay == true &&
				  (tableName = path.at(0)).isEmpty() == false
				  ))
				throw QString("The export system couldn't understand the scan URL '" % url.toString() % "', so this scan has not been exported.");
			emit statusChanged(status_ = "Opening: " % url.toString());

			databaseObject = AMDbObjectSupport::s()->createAndLoadObjectAt(db, tableName, id);
			scan = qobject_cast<AMScan*>(databaseObject);

			if(!scan) {
				databaseObject->deleteLater();
				throw QString("The export system couldn't load a scan out of the database (" % url.toString() % "), so this scan has not been exported.");
			}
		}

		else if(usingScanObjects_) {
			scan = scanObjectsToExport_.at(exportScanIndex_);
			if(!scan)
				throw QString("An invalid scan reference was provided, so this scan has not been exported.");
		}

		emit statusChanged(status_ = "Opening: " % scan->name());	// this is kinda pointless...
		emit statusChanged(status_ = "Writing: " % scan->fullName());

		// 3. Check that it can be exported with the exporter and option selected
		if(!exporter_->isValidFor(scan, option_)) {
			QString err("The exporter '" % exporter_->description() % "' and the template '" % option_->name() % "' are not compatible with this scan (" % scan->fullName() % "), so it has not been exported.");
			emit statusChanged(status_ = err);

			if (usingScanURLs_)
				scan->deleteLater();

			throw err;
		}


		// 4. Export
		//    4.1 check and create the export folder for the current run
		QString destinationFolderPathWithRun = destinationFolderPath_;

		exporter_->setCurrentScan(scan); // we must set this, otherwise we can't get the name of the current run
		QString currentRunExportFilePath = exporter_->currentRunExportFilePath();
		if(currentRunExportFilePath.length() > 0) {
			QDir exportDir;
			exportDir.setCurrent(destinationFolderPath_);
			if  (!exportDir.entryList(QDir::AllDirs).contains(currentRunExportFilePath)) {
				if(!exportDir.mkdir(currentRunExportFilePath)){
					QString err("Could not create the export folder." % exportDir.absolutePath());
					emit statusChanged(status_ = err);
					throw err;
				}
			}

			destinationFolderPathWithRun = destinationFolderPathWithRun % "/" % currentRunExportFilePath;
		}

		//    4.2 export
		QString writtenFile = exporter_->exportScan(scan, destinationFolderPathWithRun, option_, exportScanIndex_);
		if(writtenFile.isNull()) {
			QString err("Export failed for scan '" % scan->fullName() % " to " % destinationFolderPathWithRun % "'.");
			emit statusChanged(status_ = err);

			if (usingScanURLs_)
				scan->deleteLater();

			throw err;
		}

		emit statusChanged(status_ = "Wrote: " % writtenFile);
		succeededCount_++;

		if (usingScanURLs_)
			scan->deleteLater();
	}

	catch(QString errMsg) {
		failedCount_++;
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -1, errMsg));
	}


	// 5. increment exportScanIndex_ and re-schedule next one
	exportScanIndex_++;
	QTimer::singleShot(5, this, SLOT(continueScanExport()));

}

void AMExportController::setOption(AMExporterOption *option)
{
	option_->deleteLater();
	option_ = option;
	option_->setParent(this);

	if(option_)
		option_->setAvailableDataSourcesModel(availableDataSourcesModel());
}

bool AMExportController::cancel()
{
	if(!(state_ == Exporting || state_ == Paused))
		return false;

	emit stateChanged(state_ = Cancelled);
	deleteLater();
	return true;
}

AMExportController::~AMExportController() {
	// nothing for now
}

AMScan *AMExportController::scanFromUrl(const QUrl &scanUrl)
{
			AMDatabase* db = 0;
			QString tableName;
			int id = 0;
			bool idOkay = false;
			QStringList path;

			if(!( scanUrl.scheme() == "amd" &&
				  (db = AMDatabase::database(scanUrl.host())) &&
				  (path = scanUrl.path().split('/', QString::SkipEmptyParts)).count() == 2 &&
				  (id = path.at(1).toInt(&idOkay)) > 0 &&
				  idOkay == true &&
				  (tableName = path.at(0)).isEmpty() == false
				  ))
			{
				return 0;
			}

			AMDbObject* databaseObject = AMDbObjectSupport::s()->createAndLoadObjectAt(db, tableName, id);
			AMScan* scan = qobject_cast<AMScan*>(databaseObject);

			return scan;
}

bool AMExportController::pause()
{
	if(state_ != Exporting)
		return false;

	emit stateChanged(state_ = Paused);
	return true;
}

bool AMExportController::resume()
{
	if(state_ != Paused)
		return false;

	emit stateChanged(state_ = Exporting);
	QTimer::singleShot(0, this, SLOT(continueScanExport()));

	return true;
}

 AMExporterInfo::~AMExporterInfo(){}
