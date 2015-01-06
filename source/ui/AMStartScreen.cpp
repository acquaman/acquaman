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


#include "AMStartScreen.h"

#include <QBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include "ui/dataman/AMRunSelector.h"
#include "dataman/database/AMDatabase.h"

 AMStartScreen::~AMStartScreen(){}
AMStartScreen::AMStartScreen(bool mustAccept, QWidget *parent) :
	QDialog(parent)
{
	mustAccept_ = mustAccept;
	runSelector_ = new AMRunSelector(AMDatabase::database("user"),this);

	QVBoxLayout *overallLayout = new QVBoxLayout();
	QGridLayout *startLayout = new QGridLayout();
	QLabel *selectRunLabel = new QLabel(tr("Welcome to Acquaman!\n\nThis is your current run.\nIt will be used to organize your data for this visit to the facility.\n\nYou can change it, or create a new one."));
	QPushButton *ok= new QPushButton("Select");

	startLayout->addWidget(selectRunLabel,1,1);
	startLayout->addWidget(ok,2,2);
	startLayout->addWidget(runSelector_,2,1);
	overallLayout->addLayout(startLayout);
	setLayout(overallLayout);

	setAttribute(Qt::WA_DeleteOnClose, true);

	connect(ok,SIGNAL(clicked()),this,SLOT(accept()));
}

#include <QMessageBox>
#include "dataman/AMUser.h"
#include <QApplication>

void AMStartScreen::accept()
{
	if(runSelector_->currentRunId() > 0) {
		AMUser::user()->setCurrentRunId( runSelector_->currentRunId() );
		QDialog::accept();
	}
	else {
		runSelector_->activateWindow();
		runSelector_->raise();
		QMessageBox::information(this, "Please select a run", "You must select or create a valid run.");
	}

	if(QApplication::closingDown())
		QDialog::accept();

	// otherwise, don't do anything... Should not accept. This will leave the dialog open.
}

void AMStartScreen::reject()
{
	if(!mustAccept_ || QApplication::closingDown())
		QDialog::reject();
	else {
		runSelector_->activateWindow();
		runSelector_->raise();
		QMessageBox::information(this, "Please select a run", "You must select or create a valid run.");
	}
}
