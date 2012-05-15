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


#include "SGMSampleTransferView.h"

#include <QGridLayout>
#include <QStackedLayout>
#include <QLabel>
#include <QPushButton>

#include "ui/AMTopFrame.h"

#include "actions/AMBeamlineControlWaitAction.h"
#include "actions/AMBeamlineUserConfirmAction.h"

#include <QButtonGroup>

SGMSampleTransferView::SGMSampleTransferView(QWidget *parent) :
	QWidget(parent)
{
	topFrame_ = new AMTopFrame("SGM Sample Transfer Guidance");
	topFrame_->setIcon(QIcon(":/system-software-update.png"));

	vl_ = new QVBoxLayout();
	vl_->addWidget(topFrame_);
	vl_->setContentsMargins(0,0,0,0);
	vl_->setSpacing(1);
	setLayout(vl_);

	if(SGMBeamline::sgm()->isConnected())
		drawWidget();
	else
		connect(SGMBeamline::sgm(), SIGNAL(criticalControlsConnectionsChanged()), this, SLOT(delayedDrawWidget()));
}

void SGMSampleTransferView::delayedDrawWidget(){
	if(SGMBeamline::sgm()->isConnected()){
		qDebug() << "SGM is connected, drawing it";
		disconnect(SGMBeamline::sgm(), SIGNAL(connected(bool)), this, SLOT(delayedDrawWidget()));
		drawWidget();
	}
}

void SGMSampleTransferView::drawWidget(){
	mainLayout_ = new QStackedLayout();

	transferButtons_ = new QButtonGroup(this);
	QPushButton* tmpButton;
	tmpButton = new QPushButton("Transfer sample out of loadlock");
	transferButtons_->addButton(tmpButton, SGMBeamline::loadlockOut);
	tmpButton = new QPushButton("Transfer sample into loadlock");
	transferButtons_->addButton(tmpButton, SGMBeamline::loadlockIn);
	tmpButton = new QPushButton("Transfer sample out of chamber");
	transferButtons_->addButton(tmpButton, SGMBeamline::ChamberOut);
	tmpButton = new QPushButton("Transfer sample into chamber");
	transferButtons_->addButton(tmpButton, SGMBeamline::ChamberIn);

	transferBox_ = new SGMSampleTransferProceduresView("Transfer Procedures", transferButtons_);

	SGMSampleTransferPaneView* tmpPane;
	tmpPane = new SGMSampleTransferPaneView();
	transferPanes_.append(tmpPane);
	tmpPane = new SGMSampleTransferPaneView();
	transferPanes_.append(tmpPane);
	tmpPane = new SGMSampleTransferPaneView();
	transferPanes_.append(tmpPane);
	tmpPane = new SGMSampleTransferPaneView();
	transferPanes_.append(tmpPane);

	mainLayout_->addWidget(transferBox_);
	for(int x = 0; x < transferPanes_.count(); x++){
		mainLayout_->addWidget(transferPanes_.at(x));
		connect(transferPanes_.at(x), SIGNAL(completed()), this, SLOT(drawMain()));
	}
	connect(transferButtons_, SIGNAL(buttonClicked(int)), this, SLOT(drawPane(int)));

	vl_->addLayout(mainLayout_);
	mainLayout_->setContentsMargins(10, 0, 10, 0);
	mainLayout_->setSpacing(5);
	drawMain();
}

void SGMSampleTransferView::drawPane(int index){
	mainLayout_->setCurrentIndex(index);
	if(index != 0)
		transferPanes_.at(index-1)->startPane(SGMBeamline::sgm()->createTransferActions((SGMBeamline::sgmTransferType)index));
}

void SGMSampleTransferView::drawMain(){
	drawPane(0);
}

SGMSampleTransferProceduresView::SGMSampleTransferProceduresView(const QString &title, QButtonGroup *procedureButtons, QWidget *parent) :
		QGroupBox(title, parent)
{
	vl_ = new QVBoxLayout();
	mainLayout_ = new QGridLayout();
	procedureButtons_ = procedureButtons;
	for(int x = 0; x < procedureButtons_->buttons().count(); x++ )
		vl_->addWidget(procedureButtons_->button(x+1));
	mainLayout_->addLayout(vl_, 0, 0, 1, 1, Qt::AlignLeft|Qt::AlignTop);
	mainLayout_->setContentsMargins(10, 0, 10, 0);
	mainLayout_->setSpacing(5);

	setLayout(mainLayout_);
}

SGMSampleTransferPaneView::SGMSampleTransferPaneView(QWidget *parent) :
		QGroupBox("", parent)
{
	vl_ = new QVBoxLayout();
	listView_ = 0; //NULL
	completeLabel_ = new QLabel("");
	initialize();
	completeButton_ = new QPushButton("Return to Main Transfer Menu");
	connect(completeButton_, SIGNAL(clicked()), this, SIGNAL(completed()));

	vl_->addWidget(completeLabel_);
	vl_->addWidget(completeButton_);
	setLayout(vl_);
}

void SGMSampleTransferPaneView::startPane(AMBeamlineListAction *listAction){
	if(listView_){
		vl_->removeWidget(listView_);
		delete listView_;
	}
	listView_ = listAction->createView();
	vl_->insertWidget(0, listView_);
	connect(listAction, SIGNAL(succeeded()), this, SLOT(prepareCompletion()));
	initialize();
	listAction->start();
}

void SGMSampleTransferPaneView::initialize(){
	completeLabel_->setText("Procedure Incomplete");
}

void SGMSampleTransferPaneView::prepareCompletion(){
	qDebug() << "Heard list action completed";
	completeLabel_->setText("Procedure Finished");
}
