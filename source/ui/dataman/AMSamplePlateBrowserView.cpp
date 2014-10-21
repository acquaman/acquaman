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


#include "AMSamplePlateBrowserView.h"

#include <QListView>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>

#include "AMQEvents.h"

#include "dataman/AMSamplePlate.h"

AMSamplePlateBrowserView::AMSamplePlateBrowserView(AMSamplePlateBrowser *samplePlateBrowser, QWidget *parent) :
	QWidget(parent)
{
	samplePlateBrowser_ = samplePlateBrowser;
	samplePlatesListView_ = new QListView();
	samplePlatesListView_->setModel(samplePlateBrowser_);
	samplePlatesListView_->setAlternatingRowColors(true);

	selectionLabel_ = new QLabel("No Plate Selected");
	selectButton_ = new QPushButton("Select");
	selectButton_->setEnabled(false);
	cancelButton_ = new QPushButton("Cancel");

	QVBoxLayout *vl = new QVBoxLayout();
	vl->addWidget(new QLabel("Select a Sample Plate"));
	vl->addWidget(samplePlatesListView_);
	vl->addWidget(selectionLabel_);

	QHBoxLayout *hl = new QHBoxLayout();
	hl->addWidget(selectButton_);
	hl->addWidget(cancelButton_);
	vl->addLayout(hl);

	setLayout(vl);

	connect(cancelButton_, SIGNAL(clicked()), this, SLOT(close()));
	connect(selectButton_, SIGNAL(clicked()), this, SLOT(onSelectButtonClicked()));
	connect(samplePlatesListView_->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onSelectionModelCurrentChanged(QModelIndex,QModelIndex)));
	connect(samplePlatesListView_, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onSamplePlatesListViewDoubleClicked(QModelIndex)));
}

void AMSamplePlateBrowserView::clearViewSelection(){
	samplePlatesListView_->selectionModel()->clearSelection();
	selectButton_->setEnabled(false);
	selectionLabel_->setText("No Plate Selected");
}

void AMSamplePlateBrowserView::onSelectButtonClicked(){
	QModelIndex selectedPlate = samplePlatesListView_->selectionModel()->currentIndex();
	if(selectedPlate.isValid()){
		AMSamplePlate *selectedSamplePlate = qobject_cast<AMSamplePlate*>(samplePlateBrowser_->data(selectedPlate, AM::PointerRole).value<QObject*>());
		if(selectedSamplePlate){
			emit samplePlateSelected(selectedSamplePlate);
			close();
		}
	}
}

void AMSamplePlateBrowserView::onSelectionModelCurrentChanged(const QModelIndex &current, const QModelIndex &previous){
	if(current == previous)
		return;

	if(!current.isValid()){
		selectButton_->setEnabled(false);
		selectionLabel_->setText("No Plate Selected");
	}
	else{
		selectButton_->setEnabled(true);
		selectionLabel_->setText(QString("Select %1").arg(samplePlateBrowser_->data(current, Qt::DisplayRole).toString()));
	}
}

void AMSamplePlateBrowserView::onSamplePlatesListViewDoubleClicked(const QModelIndex &clicked){
	Q_UNUSED(clicked)
	onSelectButtonClicked();
}

void AMSamplePlateBrowserView::keyPressEvent(QKeyEvent *event){
	if(event->key() == Qt::Key_Escape){
		close();
		event->accept();
	}
	else
		QWidget::keyPressEvent(event);
}

AMSamplePlateCreationDialog::AMSamplePlateCreationDialog(QWidget *parent) :
	QDialog(parent)
{
	samplePlateNameLineEdit_ = new QLineEdit("New Sample Plate");
	QLabel *samplePlateNameLabel = new QLabel("Enter Sample Plate Name:");

	okButton_ = new QPushButton("Ok");
	cancelButton_ = new QPushButton("Cancel");

	QVBoxLayout *vl = new QVBoxLayout();
	vl->addWidget(samplePlateNameLabel);
	vl->addWidget(samplePlateNameLineEdit_);

	QHBoxLayout *hl = new QHBoxLayout();
	hl->addWidget(okButton_);
	hl->addWidget(cancelButton_);

	vl->addLayout(hl);

	setLayout(vl);

	okButton_->setEnabled(false);
	samplePlateNameLineEdit_->selectAll();
	samplePlateNameLineEdit_->setFocus();

	connect(okButton_, SIGNAL(clicked()), this, SLOT(accept()));
	connect(cancelButton_, SIGNAL(clicked()), this, SLOT(close()));
	connect(samplePlateNameLineEdit_, SIGNAL(textChanged(QString)), this, SLOT(onSamplePlateNameLineEditTextChanged(QString)));
}

QString AMSamplePlateCreationDialog::samplePlateName() const{
	return samplePlateNameLineEdit_->text();
}

void AMSamplePlateCreationDialog::onSamplePlateNameLineEditTextChanged(const QString &name){
	if(!name.isEmpty())
		okButton_->setEnabled(true);
	else
		okButton_->setEnabled(false);
}

void AMSamplePlateCreationDialog::keyPressEvent(QKeyEvent *event){
	if(event->key() == Qt::Key_Escape)
		close();
	QDialog::keyPressEvent(event);
}
