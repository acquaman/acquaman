#include "BottomBar.h"

BottomBar::BottomBar(QWidget *parent) : QWidget(parent)
{
	setupUi(this);


	statusView_ = new AMStatusView();
	mainHLayout->addWidget(statusView_);

	progressBar->setMinimum(0);
	progressBar->setMaximum(100);
	progressBar->setValue(100);
	timeElapsedLabel->setText("0:00");
	timeRemainingLabel->setText("0:00");

	makeConnections();

}

BottomBar::~BottomBar()
{

}

void BottomBar::makeConnections()
{
	connect(pauseScanButton, SIGNAL(clicked()), this, SIGNAL(pauseScanIssued()));
	connect(stopScanButton, SIGNAL(clicked()), this, SIGNAL(stopScanIssued()));
	connect(addButton, SIGNAL(clicked()), this, SIGNAL(addButtonClicked()));
}

void BottomBar::updateScanProgress(double secondsElapsed, double secondsTotal){
	progressBar->setMaximum((int)secondsTotal);
	progressBar->setValue((int)secondsElapsed);
	double secondsRemaining = secondsTotal - secondsElapsed;
	QTime elapsed = QTime(0,0,0,0).addMSecs((int)1000*secondsElapsed);
	QTime remaining = QTime(0,0,0,0).addMSecs((int)1000*secondsRemaining);
	QString eStr = (elapsed.hour() > 0) ? "h:mm:ss" : "m:ss" ;
	QString rStr = (remaining.hour() > 0) ? "h:mm:ss" : "m:ss" ;
	if(secondsRemaining == 0)
		timeElapsedLabel->setText("0:00");
	else
		timeElapsedLabel->setText(elapsed.toString(eStr));
	timeRemainingLabel->setText("-"+remaining.toString(rStr));
}
