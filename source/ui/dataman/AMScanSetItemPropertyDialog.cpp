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
	vl->setSizeConstraint(QLayout::SetFixedSize);
	setSizeGripEnabled( false ) ;
	vl->setContentsMargins(0,0,0,0);

	int rank = model_->data(di, AM::RankRole).toInt();
	if(rank == 1)
		vl->addWidget(new AMScanSetItem1DPropertyEditor(model_, pi_));
	else if(rank == 2)
		vl->addWidget(new AMScanSetItem2DPropertyEditor(model_, pi_));



	setLayout(vl);

	// Make connections:
	connect(model_, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)), this, SLOT(onRowsAboutToBeRemoved(QModelIndex,int,int)));
	connect(model_, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onModelDataChanged(QModelIndex,QModelIndex)));

	 setAttribute(Qt::WA_TranslucentBackground);

	 resize(minimumSizeHint());

}

AMScanSetItemPropertyDialog::~AMScanSetItemPropertyDialog() {
	// nothing required yet
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


 AMScanSetItem1DPropertyEditor::~AMScanSetItem1DPropertyEditor(){}
AMScanSetItem1DPropertyEditor::AMScanSetItem1DPropertyEditor(AMScanSetModel* model, const QPersistentModelIndex &di, QWidget* parent)
	: AMLinePropertyEditor(
		  model->data(di, AM::LinePenRole).value<QPen>(),
		  model->data(di, AMScanSetModel::FilledRole).toBool(),
		  model->data(di, AMScanSetModel::FillColorRole).value<QColor>(),
		  parent)
{
	model_ = model;
	pi_ = di;
	connect(this, SIGNAL(linePenChanged(QPen)), this, SLOT(onLinePenChanged(QPen)));
}



void AMScanSetItem1DPropertyEditor::onLinePenChanged(const QPen &pen)
{
	if(pi_.isValid()) {
		model_->setData(pi_, pen, AM::LinePenRole);
	}
}

 AMScanSetItem2DPropertyEditor::~AMScanSetItem2DPropertyEditor(){}
AMScanSetItem2DPropertyEditor::AMScanSetItem2DPropertyEditor(AMScanSetModel *model, const QPersistentModelIndex &di, QWidget *parent)
	: AMImagePropertyEditor(
		  model->plotColorMap(di.parent().row(), di.row()),
		  parent)
{
	model_ = model;
	pi_ = di;
	connect(this, SIGNAL(colorMapChanged(MPlotColorMap)), this, SLOT(onColorMapChanged(MPlotColorMap)));
}

void AMScanSetItem2DPropertyEditor::onColorMapChanged(const MPlotColorMap &map)
{
	if(pi_.isValid()) {
		model_->setData(pi_, qVariantFromValue(map), AMScanSetModel::ColorMapRole);
	}
}
