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


#include "AMBottomBar.h"

#include "util/AMFontSizes.h"

AMBottomBar::AMBottomBar(QWidget *parent) : QWidget(parent)
{
	setupUi(this);

	timeElapsedLabel->setStyleSheet("color: white; font: " AM_FONT_SMALL_ "pt \"Lucida Grande\";");
	timeRemainingLabel->setStyleSheet("color: white; font: " AM_FONT_SMALL_ "pt \"Lucida Grande\";");
	progressBar->setStyleSheet("QProgressBar { border: 1px solid rgb(116, 116, 116);border-radius: 3px;background-color: rgb(42, 42, 42);text-align: center;color: white; font: " AM_FONT_SMALL_ "pt \"Lucida Grande\";} QProgressBar::chunk {background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(136, 136, 136, 255), stop:0.494444 rgba(133, 133, 133, 255), stop:0.5 rgba(121, 121, 121, 255), stop:1 rgba(116, 116, 116, 255));}");

	addButton->setToolTip("Add Experiment Folder...");

	statusView_ = new AMStatusView();
	mainHLayout->addWidget(statusView_);

	progressBar->setMinimum(0);
	progressBar->setMaximum(100);
	progressBar->setValue(100);
	timeElapsedLabel->setText("0:00");
	timeRemainingLabel->setText("0:00");
	resumeScanButton->setVisible(false);	// hidden unless you want to show it.

	makeConnections();

}

AMBottomBar::~AMBottomBar()
{

}

void AMBottomBar::makeConnections()
{
	connect(pauseScanButton, SIGNAL(clicked()), this, SIGNAL(pauseScanIssued()));
	connect(stopScanButton, SIGNAL(clicked()), this, SIGNAL(stopScanIssued()));
	connect(resumeScanButton, SIGNAL(clicked()), this, SIGNAL(resumeScanIssued()));

	connect(restartScanButton, SIGNAL(clicked()), this, SIGNAL(restartScanIssued()));
	connect(adjustScanFinishButton, SIGNAL(clicked()), this, SIGNAL(adjustFinishPointsIssued()));
	connect(fullScreenButton, SIGNAL(clicked()), this, SIGNAL(fullScreenIssued()));

	connect(addButton, SIGNAL(clicked()), this, SIGNAL(addButtonClicked()));
}

void AMBottomBar::updateScanProgress(double secondsElapsed, double secondsTotal){
	progressBar->setMaximum((int)secondsTotal);
	progressBar->setValue((int)secondsElapsed);

	double secondsRemaining = secondsTotal - secondsElapsed;
	updateTimeElapsed(secondsElapsed);
	updateTimeRemaining(secondsRemaining);

	// Why this?
	if(secondsRemaining == 0)
		timeElapsedLabel->setText("0:00");


}

void AMBottomBar::updateTimeElapsed(double seconds)
{
	QTime elapsed = QTime(0,0,0,0).addMSecs(int(1000*seconds));
	QString eStr = (elapsed.hour() > 0) ? "h:mm:ss" : "m:ss" ;
	timeElapsedLabel->setText(elapsed.toString(eStr));
}

void AMBottomBar::updateTimeRemaining(double seconds)
{
	QTime remaining = QTime(0,0,0,0).addMSecs(int(1000*seconds));
	QString rStr = (remaining.hour() > 0) ? "h:mm:ss" : "m:ss" ;
	timeRemainingLabel->setText("-"+remaining.toString(rStr));
}

void AMBottomBar::updateProgress(double fractionElapsed, double total)
{
	progressBar->setMaximum(1000);
	progressBar->setValue(int(1000.0*fractionElapsed/total));
}

void AMBottomBar::setStatusText(const QString &text)
{
	progressBar->setTextVisible(true);
	progressBar->setFormat(text);
}

