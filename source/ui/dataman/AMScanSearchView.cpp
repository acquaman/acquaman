#include "AMScanSearchView.h"

// Definitions for AMScanSearchView
///////////////////////////////////

AMScanSearchView::AMScanSearchView(QWidget *parent) :
	QWidget(parent)
{
	initDialog();
}

void AMScanSearchView::initDialog()
{
	// Set Window properties
	setWindowTitle(QString("Search Scans"));
	resize(750, 800);
	// Set layout and widgets
	QHBoxLayout* layout = new QHBoxLayout();

	QTableView* searchResults = new QTableView();
	searchResults->setModel(new AMScanSearchInfoListModel());
	layout->addWidget(searchResults);

	this->setLayout(layout);
}

void AMScanSearchView::refreshScanList()
{


}

// Definitions for AMScanSearchInfo
///////////////////////////////////
AMScanSearchInfo::AMScanSearchInfo(int scanID, QObject *parent)
	:QObject(parent)
{
	AMDatabase* database = AMDatabase::database("user");

	configuration_ = 0;
	sample_ = 0;

	if(database == 0)
		return;

	QStringList columns;
	columns.append(QString("id"));
	columns.append(QString("name"));
	columns.append(QString("number"));
	columns.append(QString("dateTime"));
	columns.append(QString("scanConfiguration"));
	columns.append(QString("sample"));

	QVariantList results = database->retrieve(scanID, "AMScan_table", columns);

	if(results.count()!=0)
	{
		scanID_ = results.at(0).toInt();
		scanName_ = results.at(1).toString();
		scanNumber_ = results.at(2).toInt();
		scanDateTime_ = results.at(3).toDateTime();
		QStringList configResult = results.at(4).toString().split(";");
		if(configResult.count() > 1 && configResult.at(0).compare(QString("SGMXASScanConfiguration_table"))==0)
		{
			initializeConfig(configResult.at(1).toInt());
		}
		QStringList sampleResult = results.at(5).toString().split(";");
		if(sampleResult.count() > 1)
		{
			initializeSample(sampleResult.at(1).toInt());
		}
	}


}

int AMScanSearchInfo::scanID()
{
	return scanID_;
}


int AMScanSearchInfo::scanNumber()
{
	return scanNumber_;
}

QDateTime AMScanSearchInfo::scanDateTime()
{
	return scanDateTime_;
}

QString AMScanSearchInfo::scanName()
{
	return scanName_;
}

SGMXASScanConfiguration* AMScanSearchInfo::configuration()
{
	return configuration_;
}

AMSample *AMScanSearchInfo::sample()
{
	return sample_;
}

void AMScanSearchInfo::initializeConfig(int configID)
{

	AMDatabase* database = AMDatabase::database("user");
	if(database == 0)
		return;

	configuration_ = new SGMXASScanConfiguration();
	configuration_->loadFromDb(database, configID);
}

void AMScanSearchInfo::initializeSample(int sampleID)
{

	AMDatabase* database = AMDatabase::database("user");
	if(database == 0)
		return;

	sample_ = new AMSample(sampleID, database);

}


// Definitions for AMScanListModel
///////////////////////////////////
AMScanSearchInfoListModel::AMScanSearchInfoListModel(QObject *parent) :
	QAbstractTableModel(parent)
{
	this->scanInfos_ = new QList<AMScanSearchInfo*>();
	// Load Scans
	AMDatabase* database = AMDatabase::database("user");
	if(database == 0)
		return;
	QVariantList scanIds = database->retrieve("AMScan_table", "id");
	for(int iScanID = 0; iScanID < scanIds.count(); iScanID++)
	{
		scanInfos_->append(new AMScanSearchInfo(iScanID));
	}
}


QVariant AMScanSearchInfoListModel::data(const QModelIndex &index, int role) const
{
	// Invalid index:
	if(!index.isValid())
		return QVariant();

	// If handling the alignment.
	if (role == Qt::TextAlignmentRole)
		return Qt::AlignCenter;

	// If handling the background color.
	//if (role == Qt::BackgroundRole)
		//return scans_->at(index.row())->isValid() ? Qt::white : Qt::red;

	// We only answer to Qt::DisplayRole right now
	if(role != Qt::DisplayRole)
		return QVariant();

	// Out of range: (Just checking for too big.  isValid() checked for < 0)
	if(index.row() >= this->scanInfos_->count())
		return QVariant();

	QVariant dataVal = QVariant();
	AMScanSearchInfo *scanInfo = scanInfos_->at(index.row());

	switch(index.column()){

	case 0: // The scan id.
		dataVal = QString::number(scanInfo->scanID());
		break;
	case 1: // The scan name.
		dataVal = scanInfo->scanName();
		break;
	case 2: // The scan number.
		dataVal = QString::number(scanInfo->scanNumber());
		break;
	case 3:  // Date the scan was performed.
		dataVal = scanInfo->scanDateTime().toString(QString("MM/dd/yyyy hh:mm:ss"));
		break;
	case 4: // The name of the sample.
		if(scanInfo->sample() != 0)
			dataVal = scanInfo->sample()->name();
		break;
	case 5: // The sample time.
		if(scanInfo->sample() != 0)
			dataVal = scanInfo->sample()->dateTime().toString(QString("MM/dd/yyyy hh:mm:ss"));
		break;
	case 6: // The configuration exit slit
		if(scanInfo->configuration() != 0)
			dataVal = scanInfo->configuration()->exitSlitGap();
		break;
	case 7: // The configuration grating
		if(scanInfo->configuration() != 0)
			dataVal = scanInfo->configuration()->grating();
		break;
	case 8: // The configuration harmonic
		if(scanInfo->configuration() != 0)
			dataVal = scanInfo->configuration()->harmonic();
		break;
	default:
		break; // Return null if not a specific case.
	}

	return dataVal;
}

QVariant AMScanSearchInfoListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return QVariant();

	// Vertical headers:
	if(orientation == Qt::Vertical) {
		return section;
	}

	// Horizontal Headers: (Column labels)
	QVariant header = QVariant();

	switch(section){

	case 0:
		header = "id";
		break;
	case 1:
		header = "Name";
		break;
	case 2:
		header = "#";
		break;
	case 3:
		header = "When";
		break;
	case 4:
		header = "Sample Name";
		break;
	case 5:
		header = "Sample Time";
		break;
	case 6:
		header = "Exit Slit";
		break;
	case 7:
		header = "Grating";
		break;
	case 8:
		header = "Harmonic";
	}

	return header;
}



