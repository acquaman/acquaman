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

AMDatamanStartupSplashScreen::AMDatamanStartupSplashScreen(QWidget *parent) :
	QWidget(parent)
{
	startupTextLabel_ = new QLabel("Starting up Acquaman");
	startupProgressBar_ = new QProgressBar();
	subStepTextLabel_ = new QLabel("");

	QVBoxLayout *mainVL = new QVBoxLayout();
	mainVL->addWidget(startupTextLabel_);
	mainVL->addWidget(startupProgressBar_);
	mainVL->addWidget(subStepTextLabel_);

	connect(AMErrorMon::mon(), SIGNAL(debug(AMErrorReport)), this, SLOT(onErrorMonDebug(AMErrorReport)));

	setLayout(mainVL);

	setMinimumSize(600, 100);

	currentStage_ = 0;
	setNumberOfStages(8);
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

#include <QDebug>
void AMDatamanStartupSplashScreen::onErrorMonDebug(AMErrorReport e){
	QString subStepText = e.description;
	QString removeString = "in \\[.*\\]: ";
	QRegExp rx(removeString);
	subStepText.remove(rx);
	subStepTextLabel_->setText(subStepText);
}

void AMDatamanStartupSplashScreen::setNumberOfStages(int numberOfStages){
	numberOfStages_ = numberOfStages;
	startupProgressBar_->setRange(0, numberOfStages_);
}
