#include "AMScanSearchView.h"
#include <QHeaderView>
#include <QDebug>
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
	resize(1032, 640);
	// Set layout and widgets
	QVBoxLayout* mainLayout = new QVBoxLayout();

	QFormLayout* filterLayout = new QFormLayout();

	searchCriteria_ = new QLineEdit();
	searchFields_ = new QComboBox();

	searchFields_->addItem(QString("Name"));
	searchFields_->addItem(QString("#"));
	searchFields_->addItem(QString("When"));
	searchFields_->addItem(QString("Sample Name"));
	searchFields_->addItem(QString("Sample Time"));
	searchFields_->addItem(QString("Exit Slit"));
	searchFields_->addItem(QString("Grating"));
	searchFields_->addItem(QString("Harmonic"));


	filterLayout->addRow(new QLabel(QString("Search:")), searchCriteria_);
	filterLayout->addRow(new QLabel(QString("Filter Column:")), searchFields_);
	mainLayout->addLayout(filterLayout);



	QTableView* searchResults = new QTableView();

	searchResults->setAlternatingRowColors(true);
	searchResults->setSelectionBehavior(QAbstractItemView::SelectRows);
	searchResults->setSelectionMode(QAbstractItemView::ExtendedSelection);
	searchResults->verticalHeader()->setVisible(false);
	searchResults->setShowGrid(false);

	AMScanSearchInfoListModel* infoListModel = new AMScanSearchInfoListModel(this);
	proxyModel_ = new QSortFilterProxyModel(this);
	proxyModel_->setSourceModel(infoListModel);
	searchResults->setModel(proxyModel_);
	mainLayout->addWidget(searchResults);

	this->setLayout(mainLayout);

	//searchResults->setColumnHidden(0, true);
	searchResults->setColumnWidth(1, 210);
	searchResults->setColumnWidth(2, 40);
	searchResults->setColumnWidth(3, 180);
	searchResults->setColumnWidth(4, 120);
	searchResults->setColumnWidth(5, 120);
	searchResults->setColumnWidth(6, 120);
	searchResults->setSortingEnabled(true);
	QFont font = searchResults->font();
	font.setPointSize(11);
	searchResults->setFont(font);

	connect(searchCriteria_, SIGNAL(editingFinished()), this, SLOT(onSearchCriteriaChanged()));
	connect(searchFields_, SIGNAL(currentIndexChanged(int)), this, SLOT(onSearchFieldChanged()));
	connect(searchResults, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onTableDoubleClicked(QModelIndex)));
	proxyModel_->setFilterKeyColumn(1);

}

void AMScanSearchView::onTableDoubleClicked(QModelIndex index)
{
	QVariant id = proxyModel_->data(index.sibling(index.row(), 0));
	qDebug() << QString("%1 clicked").arg(id.toString());
	emit searchDoubleClicked(id.toInt());
}

void AMScanSearchView::onSearchFieldChanged()
{
	//+1 offset as id not searchable
	proxyModel_->setFilterKeyColumn(searchFields_->currentIndex()+1);
}

void AMScanSearchView::onSearchCriteriaChanged()
{
	QRegExp regExp(searchCriteria_->text(), Qt::CaseInsensitive, QRegExp::Wildcard);
	proxyModel_->setFilterRegExp(regExp);
}

// Definitions for AMScanSearchInfo
///////////////////////////////////
AMScanSearchInfo::AMScanSearchInfo(int scanID, QObject *parent)
	:QObject(parent)
{
	AMDatabase* database = AMDatabase::database("user");

	configID_ = -1;
	configuration_ = 0;
	sampleID_ = -1;
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
			//initializeConfig(configResult.at(1).toInt());
			configID_ = configResult.at(1).toInt();
		}
		QStringList sampleResult = results.at(5).toString().split(";");
		if(sampleResult.count() > 1)
		{
			//initializeSample(sampleResult.at(1).toInt());
			sampleID_ = sampleResult.at(1).toInt();
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
	if(configuration_ == 0 && configID_ > -1)
		initializeConfig(configID_);
	return configuration_;
}

AMSample *AMScanSearchInfo::sample()
{
	if(sample_ == 0 && sampleID_ > -1)
		initializeSample(sampleID_);
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
		// Load Scans
	AMDatabase* database = AMDatabase::database("user");
	if(database == 0)
		return;
	scanIds_ = database->retrieve("AMScan_table", "id");
	scanCache_ = new AMScanSearchInfo*[scanIds_.count()];
	for(int iCachePosition = 0; iCachePosition < scanIds_.count(); iCachePosition++)
	{
		scanCache_[iCachePosition] = 0;
	}
}

AMScanSearchInfoListModel::~AMScanSearchInfoListModel()
{
	for (int iCache = 0; iCache < scanIds_.count(); iCache++)
		delete scanCache_[iCache];
	delete[] scanCache_;
}


QVariant AMScanSearchInfoListModel::data(const QModelIndex &index, int role) const
{
	// Invalid index:
	if(!index.isValid())
		return QVariant();

	// If handling the alignment.
	if (role == Qt::TextAlignmentRole)
		return Qt::AlignCenter;

	// We only answer to Qt::DisplayRole right now
	if(role != Qt::DisplayRole)
		return QVariant();

	// Out of range: (Just checking for too big.  isValid() checked for < 0)
	if(index.row() >= scanIds_.count())
		return QVariant();

	QVariant dataVal = QVariant();
	int scanID = scanIds_.at(index.row()).toInt();
	int cacheIndex = index.row();
	AMScanSearchInfo *scanInfo = this->scanInfoAt(cacheIndex, scanID);

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
			dataVal = SGMBeamlineInfo::sgmInfo()->sgmGratingDescription(scanInfo->configuration()->grating());
		break;
	case 8: // The configuration harmonic
		if(scanInfo->configuration() != 0)
			dataVal = SGMBeamlineInfo::sgmInfo()->sgmHarmonicDescription(scanInfo->configuration()->harmonic());
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

int AMScanSearchInfoListModel::getID(QModelIndex &index)
{
	return scanIds_.at(index.row()).toInt();
}

AMScanSearchInfo *AMScanSearchInfoListModel::scanInfoAt(int index, int id) const
{
	if(!scanCache_[index])
	{
		scanCache_[index] = new AMScanSearchInfo(id);
	}
	return scanCache_[index];

}







