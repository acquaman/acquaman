#include "AMStartScreen.h"


AMStartScreen::AMStartScreen(QWidget *parent) :
	QSplashScreen(parent)
{
	//QPixmap pixmap(":/ ")
	QVBoxLayout *overallLayout = new QVBoxLayout(this);
	QGridLayout *startLayout = new QGridLayout(this);
	QLabel *selectRunLabel = new QLabel(tr("Please add a new run or set an existing one"),this);
	QPushButton *ok= new QPushButton("Start",this);

	startLayout->addWidget(selectRunLabel,1,1);
	startLayout->addWidget(ok,2,2);
	startLayout->addWidget(box,2,1);

	//overallLayout->addItem(pixmap);
	overallLayout->addLayout(startLayout);

	connect(ok,SIGNAL(clicked()),this,SLOT(storeCurrentRun()));
	connect(ok,SIGNAL(clicked()),this,SLOT(close()));
}

void AMStartScreen::storeCurrentRun(){
	AMUserSettings::userCurrentRun = box->currentRunId();
}

/*
 First check if it is user's first time. If it is, this should run after the first time screen has run.
 If not, it should run as usual.

 Display splashscreen: it should have: runSelector, option to start new scan, ....
 Make nice picture for it later...
*/
