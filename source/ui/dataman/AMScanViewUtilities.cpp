/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "AMScanViewUtilities.h"

#include "dataman/AMScan.h"
#include "ui/dataman/AMColoredTextToolButton.h"

#include <QStringBuilder>

AMScanViewScanBar::AMScanViewScanBar(AMScanSetModel* model, int scanIndex, QWidget* parent)
	: QFrame(parent)
{
	model_ = model;
	scanIndex_ = scanIndex;

	setObjectName("AMScanViewScanBar");
	setStyleSheet("QFrame#AMScanViewScanBar { "
				  "background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(81, 81, 81, 255), stop:0.494444 rgba(81, 81, 81, 255), stop:0.5 rgba(64, 64, 64, 255), stop:1 rgba(64, 64, 64, 255));"
				  "border-bottom: 1px solid black;"
				  "}");


	exclusiveModeOn_ = false;
	sourceButtons_.setExclusive(false);

	AMScan* source = model->scanAt(scanIndex_);


	// UI Setup:
	///////////////////////

	QHBoxLayout* hl = new QHBoxLayout();
	nameLabel_ = new QLabel();
	if(source)
		nameLabel_->setText(source->name() + QString(" #%1").arg(source->number()));
	nameLabel_->setStyleSheet("color: white;");
	hl->addWidget(nameLabel_);
	hl->addStretch(0);

	cramBar_ = new AMCramBarHorizontal();


	if(source) {
		for(int i=0; i<source->dataSourceCount(); i++) {
			QColor color = model->plotColor(scanIndex, i);

			QToolButton* sourceButton = new AMColoredTextToolButton(color); /// \todo special buttons, with lines underneath that show the line color / style, and differentiate 1D, 2D datasets.

			sourceButton->setMaximumHeight(18);
			sourceButton->setText(source->dataSourceAt(i)->name());	/// \todo description or name? both? name if description is empty?
			sourceButton->setCheckable(true);
			sourceButton->setChecked(model->isVisible(scanIndex, i));
			sourceButtons_.addButton(sourceButton, i);
			cramBar_->addWidget(sourceButton);
			// hide the button if this data source should be hidden from users:
			sourceButton->setHidden(model->isHiddenFromUsers(scanIndex, i));

			sourceButton->setContextMenuPolicy(Qt::CustomContextMenu);
			connect(sourceButton, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onDataSourceButtonRightClicked(QPoint)));
		}
	}

	hl->addWidget(cramBar_);
	hl->addStretch(1);

	hl->setContentsMargins(6, 0, 6, 0);
	hl->setSpacing(24);
	setLayout(hl);

	connect(model, SIGNAL(rowsInserted(QModelIndex, int, int)), this, SLOT(onRowInserted(QModelIndex,int,int)));
	connect(model, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)), this, SLOT(onRowAboutToBeRemoved(QModelIndex,int,int)));
	connect(model, SIGNAL(rowsRemoved(QModelIndex, int, int)), this, SLOT(onRowRemoved(QModelIndex,int,int)));
	connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onModelDataChanged(QModelIndex,QModelIndex)));
	connect(model, SIGNAL(exclusiveDataSourceChanged(QString)), this, SLOT(onExclusiveDataSourceChanged(QString)));

	connect(&sourceButtons_, SIGNAL(buttonClicked(int)), this, SLOT(onSourceButtonClicked(int)));

	// connect(closeButton_, SIGNAL(clicked()), this, SLOT(onCloseButtonClicked()));

}


void AMScanViewScanBar::onDataSourceButtonRightClicked(const QPoint& location) {

	QAbstractButton* button = qobject_cast<QAbstractButton*>(sender());
	if(!button)
		return;

	int dataSourceIndex = sourceButtons_.id(button);

	AMScanViewScanBarContextMenu* cm = new AMScanViewScanBarContextMenu(model_, scanIndex_, dataSourceIndex, button);
	cm->popup(button->mapToGlobal(location));
}


void AMScanViewScanBar::onRowInserted(const QModelIndex& parent, int start, int end) {
	// not for us...
	if(parent.internalId() != -1 || parent.row() != scanIndex_) {
		return;
	}


	// it is for us... (parent index is our Scan, and it is a new data source)
	AMScan* source = model_->scanAt(scanIndex_);
	// note: AMScanSetModel guarantees only one row inserted at a time, but we don't depend on that...
	for(int i=start; i<=end; i++) {
		AMColoredTextToolButton *newButton = new AMColoredTextToolButton(model_->plotColor(scanIndex_, i));
		newButton->setText(source->dataSourceAt(i)->name());
		newButton->setCheckable(true);
		newButton->setMaximumHeight(18);
		sourceButtons_.addButton(newButton, i);
		cramBar_->insertWidget(i, newButton);
		if(exclusiveModeOn_)
			newButton->setChecked( (model_->exclusiveDataSourceName() == source->dataSourceAt(i)->name()) );
		else
			newButton->setChecked(model_->isVisible(scanIndex_, i));
		// If this data source should be hidden from users, don't show the button to toggle its visibility.  (Above, isVisible() will automatically return false if hiddenFromUsers() is true.)
		if (source->dataSourceAt(i)->hiddenFromUsers())
			newButton->hide();

		newButton->setContextMenuPolicy(Qt::CustomContextMenu);
		connect(newButton, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onDataSourceButtonRightClicked(QPoint)));
		// qdebug() << "added a data source. exclusiveModeOn is: " << exclusiveModeOn_ << ", source name is:" << source->dataSourceAt(i)->name() << ", exclusiveDataSourceName is:" << model_->exclusiveDataSourceName();
	}

}

// before a scan or data source is deleted in the model.
void AMScanViewScanBar::onRowAboutToBeRemoved(const QModelIndex& parent, int start, int end) {

	// check if this isn't one of our data sources being deleted.
	if(parent.internalId() != -1 || parent.row() != scanIndex_) {
		return;
	}


	// (AMScanSetModel guarantees only one removed at once -- ie: start == end --, but we don't depend on that)
	for(int di = end; di>=start; di-- ) {

		sourceButtons_.button(di)->disconnect();
		delete sourceButtons_.button(di);
		// the button group's id's from "start+1" to "count+1" are too high now...
		for(int i=di+1; i<sourceButtons_.buttons().count()+1; i++)
			sourceButtons_.setId(sourceButtons_.button(i), i-1);
	}
}

// after a scan or data source is deleted in the model:
void AMScanViewScanBar::onRowRemoved(const QModelIndex& parent, int start, int end) {
	Q_UNUSED(parent)
	Q_UNUSED(start)
	Q_UNUSED(end)
}

// when model data changes.  Possibilities we care about: scan name, and data sources visible/not visible.
/*! This assumes that topLeft == bottomRight, which is ok, given that AMScanSetModel always emits dataChanged() for single items. */
void AMScanViewScanBar::onModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight) {

	Q_UNUSED(bottomRight)

	// changes to our scan:
	if(topLeft.internalId() == -1 && topLeft.row() == scanIndex_) {

		nameLabel_->setText(model_->scanAt(scanIndex_)->name() + QString(" #%1").arg(model_->scanAt(scanIndex_)->number()));
	}

	// changes to one of our data sources:
	else if(topLeft.internalId() == scanIndex_) {

		int dataSourceIndex = topLeft.row();
		AMDataSource* dataSource = model_->dataSourceAt(scanIndex_, dataSourceIndex);
		QAbstractButton* button = sourceButtons_.button(dataSourceIndex);
		button->setText(dataSource->name());
		// setting visibility: depends on whether exclusiveMode is on or not
		if(exclusiveModeOn_)
			button->setChecked( (model_->exclusiveDataSourceName() == dataSource->name()) );
		else
			button->setChecked(model_->isVisible(scanIndex_, dataSourceIndex));

		// hide the button to toggle visibility, if this data source should be hidden from users.
		button->setHidden(model_->isHiddenFromUsers(scanIndex_, dataSourceIndex));

		qobject_cast<AMColoredTextToolButton*>(button)->setTextColor( model_->plotColor(scanIndex_, dataSourceIndex) );
	}
}

void AMScanViewScanBar::onSourceButtonClicked(int id) {
	// if in "exclusive" mode, when clicked, set that data source as the exclusive data source
	if(exclusiveModeOn_) {
		//if(visible == Qt::Checked)
		model_->setExclusiveDataSourceByName(model_->dataSourceAt(scanIndex_, id)->name());
		sourceButtons_.button(id)->setChecked(true);
	}
	else {
		model_->setVisible(scanIndex_, id, sourceButtons_.button(id)->isChecked());
	}
}


void AMScanViewScanBar::onExclusiveDataSourceChanged(const QString& exlusiveDataSourceName) {
	// when exclusive mode is on, set checked only when data source name matches the exclusive one
	if(exclusiveModeOn_) {
		int numSourceButtons = sourceButtons_.buttons().count();
		for(int di=0; di<numSourceButtons; di++)
			sourceButtons_.button(di)->setChecked( (model_->dataSourceAt(scanIndex_, di)->name() == exlusiveDataSourceName) );
	}
}


void AMScanViewScanBar::setExclusiveModeOn(bool exclusiveModeOn) {

	// turning exclusiveMode on:
	if(exclusiveModeOn && !exclusiveModeOn_) {
		exclusiveModeOn_ = true;
		int numSourceButtons = sourceButtons_.buttons().count();
		for(int di=0; di<numSourceButtons; di++) {
			sourceButtons_.button(di)->setChecked( (model_->dataSourceAt(scanIndex_, di)->name() == model_->exclusiveDataSourceName()) );
		}
	}

	// turning exclusiveMode off:
	if(!exclusiveModeOn && exclusiveModeOn_) {
		exclusiveModeOn_ = false;
		//chButtons_.setExclusive(false);
		int numSourceButtons = sourceButtons_.buttons().count();
		for(int di=0; di<numSourceButtons; di++) {
			sourceButtons_.button(di)->setChecked( model_->isVisible(scanIndex_, di) );
		}
	}
}

AMScanViewScanBarContextMenu::AMScanViewScanBarContextMenu(AMScanSetModel *model, int scanIndex, int dataSourceIndex, QWidget* parent)
	: QMenu(parent)
{
	model_ = model;
	scanIndex_ = scanIndex;
	dataSourceIndex_ = dataSourceIndex;
	QModelIndex di = model_->indexForDataSource(scanIndex, dataSourceIndex);
	pi_ = QPersistentModelIndex(di);

	QString scanName = model_->data(di.parent(), AM::NameRole).toString();
	QString dataSourceName = model_->data(di, AM::NameRole).toString();
	QString dataSourceDescription = model_->data(di, AM::DescriptionRole).toString();
	QString title = scanName % ": " % dataSourceName;
	setTitle(title);

	addAction(title)->setDisabled(true);
	if(dataSourceDescription != dataSourceName)
		addAction(dataSourceDescription)->setDisabled(true);
	addSeparator();
	connect(addAction("Hide all except " % dataSourceName), SIGNAL(triggered()), this, SLOT(hideAllExceptDataSource()));
	if(model_->scanCount() > 1)
		connect(addAction("Show all " % dataSourceName), SIGNAL(triggered()), this, SLOT(showAllDataSource()));
	connect(addAction("Show all"), SIGNAL(triggered()), this, SLOT(showAll()));
	addSeparator();
	connect(addAction("Color and style..."), SIGNAL(triggered()), this, SLOT(editColorAndStyle()));
	addSeparator();
	connect(addAction("Remember Visibility"), SIGNAL(triggered()), model_, SLOT(saveVisibility()));

	connect(this, SIGNAL(aboutToHide()), this, SLOT(deleteLater()));
}

AMScanViewScanBarContextMenu::~AMScanViewScanBarContextMenu() {
	// nothing required yet
}

void AMScanViewScanBarContextMenu::hideAllExceptDataSource()
{
	AMScan *scan = model_->scanAt(scanIndex_);
	if (!scan)
		return;

	int dataSourceCount = scan->dataSourceCount();

	for (int i = 0; i < dataSourceCount; i++){

		if (i == dataSourceIndex_)
			model_->setVisible(scanIndex_, i, true);

		else
			model_->setVisible(scanIndex_, i, false);
	}
}

void AMScanViewScanBarContextMenu::showAllDataSource()
{
	QString nameOfDataSource(model_->dataSourceAt(scanIndex_, dataSourceIndex_)->name());
	int scanCount = model_->scanCount();
	int dataSourceCount = 0;

	for (int i = 0; i < scanCount; i++){

		dataSourceCount = model_->scanAt(i)->dataSourceCount();

		for (int j = 0; j < dataSourceCount; j++){

			if (model_->dataSourceAt(i, j)->name() == nameOfDataSource)
				model_->setVisible(i, j, true);

			else
				model_->setVisible(i, j, false);
		}
	}
}

void AMScanViewScanBarContextMenu::showAll()
{
	AMScan *scan = model_->scanAt(scanIndex_);
	if (!scan)
		return;

	int dataSourceCount = scan->dataSourceCount();

	for (int i = 0; i < dataSourceCount; i++)
		model_->setVisible(scanIndex_, i, true);
}

#include "ui/dataman/AMScanSetItemPropertyDialog.h"
void AMScanViewScanBarContextMenu::editColorAndStyle()
{
	if(pi_.isValid()) {
		AMScanSetItemPropertyDialog* pd = new AMScanSetItemPropertyDialog(model_, pi_.parent().row(), pi_.row(), parentWidget());
		pd->show();
	}
}


