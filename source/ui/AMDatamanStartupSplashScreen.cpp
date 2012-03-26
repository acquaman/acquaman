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
