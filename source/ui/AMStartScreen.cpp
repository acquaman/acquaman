/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "AMStartScreen.h"

#include <QDebug>

AMStartScreen::AMStartScreen(QWidget *parent) :
	QWidget(parent)
{
	//QPixmap pixmap(":/ ")
	runSelector_ = new AMRunSelector(AMDatabase::userdb(),this);
	//QVBoxLayout *overallLayout = new QVBoxLayout(this);
	//QGridLayout *startLayout = new QGridLayout(this);
	QVBoxLayout *overallLayout = new QVBoxLayout();
	QGridLayout *startLayout = new QGridLayout();
	QLabel *selectRunLabel = new QLabel(tr("Welcome to Acquaman!\n\nThis is your current run.\nIt will be used to organize your data for this visit to the facility.\n\nYou can change it, or create a new one."),this);
	QPushButton *ok= new QPushButton("Start",this);

	startLayout->addWidget(selectRunLabel,1,1);
	startLayout->addWidget(ok,2,2);
	startLayout->addWidget(runSelector_,2,1);

	//overallLayout->addItem(pixmap);
	overallLayout->addLayout(startLayout);
	setLayout(overallLayout);

	setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
	setAttribute(Qt::WA_DeleteOnClose, true);

	connect(ok,SIGNAL(clicked()),this,SLOT(close()));

	// Useability tweak: If there are no valid runs (ie: the current run selected is invalid), we can start creating one
	if(runSelector_->currentRunId() < 1)
		runSelector_->showAddRunDialog();

}

#include <QMessageBox>
#include "dataman/AMUser.h"

bool AMStartScreen::storeCurrentRun(){
	if(runSelector_->currentRunId() > 0) {
		AMUser::user()->setCurrentRunId( runSelector_->currentRunId() );
		return true;
	}
	else {
		QMessageBox::information(this, "Please select a run", "You must select or create a valid run. ");
		return false;
	}
}


#include <QCloseEvent>
#include <QApplication>

void AMStartScreen::closeEvent(QCloseEvent *e) {

	if(QApplication::closingDown() || storeCurrentRun())
		e->accept();
	else
		e->ignore();
}
