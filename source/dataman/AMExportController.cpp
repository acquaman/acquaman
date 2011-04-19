#include "AMExportController.h"

#include <QComboBox>
#include <QHashIterator>
#include <QDir>
#include "acquaman.h"
#include <QTimer>
#include <QStandardItemModel>
#include <QStringBuilder>

#include "dataman/AMExporter.h"
#include "dataman/AMUser.h"



QHash<QString, AMExporterInfo> AMExportController::registeredExporters_;

AMExportController::AMExportController(const QList<QUrl>& scansToExport) :
	QObject()
{
	exporter_ = 0;
	scansToExport_ = scansToExport;
	availableDataSourcesModel_ = new QStandardItemModel(this);

	searchScanIndex_ = -1;

	destinationFolderPath_ = AMUser::user()->lastExportDestination();
	if(destinationFolderPath_.isEmpty())
		destinationFolderPath_ = QDir::toNativeSeparators(QDir::homePath());
}

bool AMExportController::chooseExporter(const QString &exporterClassName)
{
	if(!registeredExporters_.contains(exporterClassName))
		return false;

	if(exporter_)
		delete exporter_;

	exporter_ = qobject_cast<AMExporter*>(registeredExporters_.value(exporterClassName).metaObject->newInstance());
	exporter_->setParent(this);

	return true;
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
	if(searchScanIndex_ >= scansToExport_.count())
		return; // We're done!

	const QUrl& url = scansToExport_.at(searchScanIndex_++);	// incrementing searchScanIndex_ here.

	AMDatabase* db;
	QStringList path;
	QString tableName;
	int id;
	bool idOkay;

	// parse the URL and make sure it's valid
	if(url.scheme() == "amd" &&
			(db = AMDatabase::dbByName(url.host())) &&
			(path = url.path().split('/', QString::SkipEmptyParts)).count() == 2 &&
			(id = path.at(1).toInt(&idOkay)) > 0 &&
			idOkay == true &&
			(tableName = path.at(0)).isEmpty() == false
			) {

		// let's roll.  Find all the raw data sources for this scan
		QSqlQuery q = db->select(tableName % "_rawDataSources", "id2,table2", "id1='" % QString::number(id) % "'");	// note: checked that this is indeed using the index. Can go faster? Dunno.
		while(q.next()) {
			// get name, description, rank for this data source
			QSqlQuery q2 = db->select( q.value(1).toString(),
									  "name,description,rank",
									  "id='" % q.value(0).toString() % "'");
			if(q2.next()) {
				addFoundAvailableDataSource(q2.value(0).toString(), q2.value(1).toString(), q2.value(2).toInt());
			}
		}

		// Find all the analyzed data sources for this scan
		q = db->select(tableName % "_analyzedDataSources", "id2,table2", "id1='" % QString::number(id) % "'");	// note: checked that this is indeed using the index. Can go faster? Dunno.
		while(q.next()) {
			// get name, description, rank for this data source
			QSqlQuery q2 = db->select( q.value(1).toString(),
									  "name,description,rank",
									  "id='" % q.value(0).toString() % "'");
			if(q2.next()) {
				addFoundAvailableDataSource(q2.value(0).toString(), q2.value(1).toString(), q2.value(2).toInt());
			}
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
