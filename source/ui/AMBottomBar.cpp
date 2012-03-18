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


#include "AMBottomBar.h"

#include "util/AMFontSizes.h"

AMBottomBar::AMBottomBar(QWidget *parent) : QWidget(parent)
{
	setupUi(this);
	timeElapsedLabel->setStyleSheet("color: white; font: " AM_FONT_SMALL_ "pt \"Lucida Grande\";");
	timeRemainingLabel->setStyleSheet("color: white; font: " AM_FONT_SMALL_ "pt \"Lucida Grande\";");
	addButton->setToolTip("Add Experiment Folder...");

	statusView_ = new AMStatusView();
	mainHLayout->addWidget(statusView_);

	progressBar->setMinimum(0);
	progressBar->setMaximum(100);
	progressBar->setValue(100);
	timeElapsedLabel->setText("0:00");
	timeRemainingLabel->setText("0:00");

	makeConnections();

}

AMBottomBar::~AMBottomBar()
{

}

void AMBottomBar::makeConnections()
{
	connect(pauseScanButton, SIGNAL(clicked()), this, SIGNAL(pauseScanIssued()));
	connect(stopScanButton, SIGNAL(clicked()), this, SIGNAL(stopScanIssued()));
	connect(addButton, SIGNAL(clicked()), this, SIGNAL(addButtonClicked()));
}

void AMBottomBar::updateScanProgress(double secondsElapsed, double secondsTotal){
	progressBar->setMaximum((int)secondsTotal);
	progressBar->setValue((int)secondsElapsed);
	double secondsRemaining = secondsTotal - secondsElapsed;
	QTime elapsed = QTime(0,0,0,0).addMSecs(int(1000*secondsElapsed));
	QTime remaining = QTime(0,0,0,0).addMSecs(int(1000*secondsRemaining));
	QString eStr = (elapsed.hour() > 0) ? "h:mm:ss" : "m:ss" ;
	QString rStr = (remaining.hour() > 0) ? "h:mm:ss" : "m:ss" ;
	if(secondsRemaining == 0)
		timeElapsedLabel->setText("0:00");
	else
		timeElapsedLabel->setText(elapsed.toString(eStr));
	timeRemainingLabel->setText("-"+remaining.toString(rStr));
}
