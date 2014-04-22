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


#include "AMDatamanStartupSplashScreen.h"

#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QTimer>

 AMDatamanStartupSplashScreen::~AMDatamanStartupSplashScreen(){}
AMDatamanStartupSplashScreen::AMDatamanStartupSplashScreen(QWidget *parent) :
	QWidget(parent)
{
	startupTextLabel_ = new QLabel("Starting up Acquaman");
	startupProgressBar_ = new QProgressBar();
	subStepTextLabel_ = new QLabel("");
	waitingProgressBar_ = new QProgressBar();
	waitingProgressBar_->setMinimum(0);
	waitingProgressBar_->setMaximum(0);
	waitingProgressBar_->setMaximumWidth(200);

	mainVL_ = new QVBoxLayout();
	mainVL_->addWidget(startupTextLabel_);
	mainVL_->addWidget(startupProgressBar_);

	subTextHL_ = new QHBoxLayout();
	subTextHL_->addWidget(subStepTextLabel_);
	mainVL_->addLayout(subTextHL_);

	connect(AMErrorMon::mon(), SIGNAL(debug(AMErrorReport)), this, SLOT(onErrorMonDebug(AMErrorReport)));

	setLayout(mainVL_);

	setMinimumSize(600, 100);

	currentStage_ = 0;
	specialMode_ = AMDatamanStartupSplashScreen::noMode;
	setNumberOfStages(8);
}

int AMDatamanStartupSplashScreen::numberOfStages() const{
	return numberOfStages_;
}

void AMDatamanStartupSplashScreen::onErrorMonStartupCode(AMErrorReport e)
{
	startupProgressBar_->setValue(currentStage_);
	QString stageDescription = e.description;
	stageDescription = stageDescription.remove(0, stageDescription.indexOf("Acquaman Startup"));
	startupTextLabel_->setText(stageDescription);
	currentStage_++;
}

void AMDatamanStartupSplashScreen::onErrorMonStartupFinished(AMErrorReport e){
	Q_UNUSED(e)
	startupProgressBar_->setValue(currentStage_);
	startupTextLabel_->setText("Acquaman Startup Finished");
	QTimer::singleShot(1000, this, SLOT(hide()));
}

void AMDatamanStartupSplashScreen::onErrorMonDebug(AMErrorReport e){
	QString subStepText = e.description;
	QString removeString = "in \\[.*\\]: ";
	QRegExp rx(removeString);
	subStepText.remove(rx);
	subStepTextLabel_->setText(subStepText);
	resize(sizeHint());
}

void AMDatamanStartupSplashScreen::onErrorMonChangeMode(AMErrorReport e){
	QString modeText = e.description;
	QString removeString = "in \\[.*\\]: ";
	QRegExp rx(removeString);
	modeText.remove(rx);
	if(modeText == "Waiting" || modeText == "waiting"){
		specialMode_ = AMDatamanStartupSplashScreen::waitingMode;
		subTextHL_->addWidget(waitingProgressBar_);
	}
	else{
		specialMode_ = AMDatamanStartupSplashScreen::noMode;
		subTextHL_->removeWidget(waitingProgressBar_);
	}
}

void AMDatamanStartupSplashScreen::setNumberOfStages(int numberOfStages){
	numberOfStages_ = numberOfStages;
	startupProgressBar_->setRange(0, numberOfStages_);
}
