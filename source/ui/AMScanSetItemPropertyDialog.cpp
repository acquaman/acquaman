#include "AMScanSetItemPropertyDialog.h"

#include "dataman/AMScanSetModel.h"

#include <QBoxLayout>
#include "acquaman.h"
#include <QStringBuilder>

#include <QLabel>

AMScanSetItemPropertyDialog::AMScanSetItemPropertyDialog(AMScanSetModel* model, int scanIndex, int dataSourceIndex, QWidget* parent) :
	QDialog(parent)
{
	// delete when accepted or rejected
	setAttribute(Qt::WA_DeleteOnClose, true);

	model_ = model;

	QModelIndex di = model_->indexForDataSource(scanIndex, dataSourceIndex);
	pi_ = QPersistentModelIndex(di);

	QString scanName = model_->data(model_->indexForScan(scanIndex), AM::NameRole).toString();
	QString dataSourceName = model_->data(di, AM::NameRole).toString();


	// Setup UI:
	setWindowTitle(scanName % ": " % dataSourceName);

	QVBoxLayout* vl = new QVBoxLayout();

	int rank = model_->data(di, AM::RankRole).toInt();
//	if(rank == 1)
//		vl->addWidget(new AMScanSetItem1DPropertyEditor(model_, pi_));
//	else if(rank == 2)
//		vl->addWidget(new AMScanSetItem2DPropertyEditor(model_, pi_));

	vl->addWidget(new QLabel("Hi!"));

	setLayout(vl);

	// Make connections:
	connect(model_, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)), this, SLOT(onRowsAboutToBeRemoved(QModelIndex,int,int)));
	connect(model_, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onModelDataChanged(QModelIndex,QModelIndex)));
}

AMScanSetItemPropertyDialog::~AMScanSetItemPropertyDialog() {
	qDebug() << "Deleting property dialog" << windowTitle();
}

void AMScanSetItemPropertyDialog::onRowsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
	// if parent is invalid: this is a top-level (scan) index
	if(!parent.isValid()) {
		for(int row=start; row<=end; row++)
			// pi_.parent() will be the index of our scan. If that matches the index being removed...
			if(pi_.parent() == model_->indexForScan(row)) {
				pi_ = QPersistentModelIndex();	// make pi_ invalid so we don't try to keep tracking a soon-non-existent index
				reject();// get us outta here
				deleteLater();
			}
	}
	// is the parent our scan? Then all the removed rows will be our data sources
	else if(parent == pi_.parent()) {
		for(int row=start; row<=end; row++) {
			// are we being removed?
			if(pi_ == model_->index(row,0,parent)) {
				pi_ = QPersistentModelIndex(); // make pi_ invalid so we don't try to keep tracking a soon-non-existent index
				reject();// get us outta here
				deleteLater();
			}
		}
	}
}

void AMScanSetItemPropertyDialog::onModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
	Q_UNUSED(topLeft)
	Q_UNUSED(bottomRight)
}



