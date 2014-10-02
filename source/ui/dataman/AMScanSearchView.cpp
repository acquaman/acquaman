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


#include "AMScanSearchView.h"
#include <QHeaderView>
#include <QDebug>
#include <QMenu>

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

	searchFields_->addItem(QString("Serial#"));
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



	searchResults_ = new QTableView(this);

	searchResults_->setAlternatingRowColors(true);
	searchResults_->setSelectionBehavior(QAbstractItemView::SelectRows);
	searchResults_->setSelectionMode(QAbstractItemView::SingleSelection);
	searchResults_->setSelectionMode(QAbstractItemView::ExtendedSelection);
	searchResults_->verticalHeader()->setVisible(false);
	searchResults_->setShowGrid(false);

	AMScanSearchInfoListModel* infoListModel = new AMScanSearchInfoListModel(this);
	proxyModel_ = new QSortFilterProxyModel(this);
	proxyModel_->setSourceModel(infoListModel);
	searchResults_->setModel(proxyModel_);
	mainLayout->addWidget(searchResults_);

	this->setLayout(mainLayout);

	searchResults_->setColumnWidth(1, 210);
	searchResults_->setColumnWidth(2, 40);
	searchResults_->setColumnWidth(3, 180);
	searchResults_->setColumnWidth(4, 120);
	searchResults_->setColumnWidth(5, 120);
	searchResults_->setColumnWidth(6, 120);
	searchResults_->setSortingEnabled(true);
	QFont font = searchResults_->font();
	font.setPointSize(11);
	searchResults_->setFont(font);
	searchResults_->setContextMenuPolicy(Qt::CustomContextMenu);

	connect(searchCriteria_, SIGNAL(editingFinished()), this, SLOT(onSearchCriteriaChanged()));
	connect(searchFields_, SIGNAL(currentIndexChanged(int)), this, SLOT(onSearchFieldChanged()));
	connect(searchResults_, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onTableDoubleClicked(QModelIndex)));
	connect(searchResults_, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onContextMenuRequested(QPoint)));

	proxyModel_->setFilterKeyColumn(0);

}

void AMScanSearchView::onContextMenuRequested(QPoint pos)
{
	QModelIndexList selectedItems = searchResults_->selectionModel()->selectedIndexes();
	if(!selectedItems.count())
		return;

	QModelIndex selectedItem = selectedItems.at(0);
	QVariant id = proxyModel_->data(selectedItem.sibling(selectedItem.row(), 0));
	QList<QUrl> urls;
	urls << QUrl(QString("amd://user/AMScan_table/%1").arg(id.toString()));

	QMenu popup(this);
	popup.addAction(QString("Show Scan Configuration"));
	popup.addAction(QString("Edit"));
	popup.addAction(QString("Export"));

	QPoint globalPos = mapToGlobal(pos);
	globalPos.setX(globalPos.x() + searchResults_->x());
	globalPos.setY(globalPos.y() + searchResults_->y());

	QAction* selectedAction = popup.exec(globalPos);
	if(selectedAction)
	{
		if(selectedAction->text() == "Show Scan Configuration")
			emit editConfigurationRequested(urls);
		else if(selectedAction->text() == "Edit")
			emit editScanRequested(urls);
		else if(selectedAction->text() == "Export")
			emit exportScanRequested(urls);
	}
}


void AMScanSearchView::onTableDoubleClicked(QModelIndex index)
{
	QVariant id = proxyModel_->data(index.sibling(index.row(), 0));
	QList<QUrl> urls;
	urls << QUrl(QString("amd://user/AMScan_table/%1").arg(id.toString()));
	emit editScanRequested(urls);
}

void AMScanSearchView::onSearchFieldChanged()
{

	proxyModel_->setFilterKeyColumn(searchFields_->currentIndex());
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
			configID_ = configResult.at(1).toInt();
		}
		QStringList sampleResult = results.at(5).toString().split(";");
		if(sampleResult.count() > 1)
		{
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
		header = "Serial#";
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
