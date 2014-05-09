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
	setWindowTitle(QString("Search Scans"));
	QHBoxLayout* layout = new QHBoxLayout();

	QTableView* searchResults = new QTableView();
	searchResults->setModel(new AMScanListModel());
	layout->addWidget(searchResults);

	this->setLayout(layout);
}

// Definitions for AMScanListModel
///////////////////////////////////
AMScanListModel::AMScanListModel(QObject *parent) :
	QAbstractTableModel(parent)
{
	this->scans_ = new QList<AMScan*>();
}


QVariant AMScanListModel::data(const QModelIndex &index, int role) const
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
	if(index.row() >= scans_->count())
		return QVariant();

	QVariant dataVal = QVariant();
	AMScan *scan = scans_->at(index.row());

	switch(index.column()){

	case 0: // The scan id.
		dataVal = QString::number(scan->id());
		break;
	case 1: // The scan name.
		dataVal = scan->name();
		break;
	case 2: // The scan number.
		dataVal = QString::number(scan->number());
		break;
	case 3:  // Date the scan was performed.
		dataVal = scan->dateTime().toString(QString("ddd MMMM d yy"));
		break;
	case 4: // The name of the sample.
		dataVal = scan->sample()->name();
		break;
	case 5: // The sample time.
		dataVal = scan->sample()->dateTime().toString(QString("ddd MMMM d yy"));
		break;
	default:
		break; // Return null if not a specific case.
	}

	return dataVal;
}

QVariant AMScanListModel::headerData(int section, Qt::Orientation orientation, int role) const
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
	}

	return header;
}
