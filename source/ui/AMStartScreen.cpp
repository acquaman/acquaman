#include "AMStartScreen.h"

#include <QDebug>

AMStartScreen::AMStartScreen(QWidget *parent) :
	QWidget(parent)
{
	//QPixmap pixmap(":/ ")
	runSelector_ = new AMRunSelector(AMDatabase::userdb(),this);
	QVBoxLayout *overallLayout = new QVBoxLayout(this);
	QGridLayout *startLayout = new QGridLayout(this);
	QLabel *selectRunLabel = new QLabel(tr("Welcome to Acquaman!\n\nThis is your current run.\nIt will be used to organize your data for this visit to the facility.\n\nYou can change it, or create a new one."),this);
	QPushButton *ok= new QPushButton("Start",this);

	startLayout->addWidget(selectRunLabel,1,1);
	startLayout->addWidget(ok,2,2);
	startLayout->addWidget(runSelector_,2,1);

	//overallLayout->addItem(pixmap);
	overallLayout->addLayout(startLayout);

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
