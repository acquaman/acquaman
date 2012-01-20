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


#include "AMBeamlineControlWaitAction.h"

#include <QSlider>
#include <QToolButton>
#include <QButtonGroup>
#include <QApplication>
#include <QFile>
#include <QVBoxLayout>
#include <QLabel>
#include <QStyle>

AMBeamlineControlWaitAction::AMBeamlineControlWaitAction(AMControl *control, AMBeamlineControlWaitAction::waitTargetType targetType, QObject *parent) :
		AMBeamlineActionItem(parent)
{
	control_ = 0; //NULL
	targetType_ = targetType;
	actionTolerance_ = 0;
	if(control){
		actionTolerance_ = control->tolerance();
		setControl(control);
	}
}

AMBeamlineActionItemView* AMBeamlineControlWaitAction::createView(int index){
	Q_UNUSED(index)
	return new AMBeamlineControlWaitDetailedActionView(this);
}

AMControl* AMBeamlineControlWaitAction::control(){
	return control_;
}

double AMBeamlineControlWaitAction::waitpoint() const{
	return waitpoint_;
}

double AMBeamlineControlWaitAction::outlier() const{
	return outlierpoint_;
}

double AMBeamlineControlWaitAction::actionTolerance() const{
	return actionTolerance_;
}

int AMBeamlineControlWaitAction::targetType() const{
	return targetType_;
}

double AMBeamlineControlWaitAction::holdTime() const{
	return holdTime_;
}

void AMBeamlineControlWaitAction::start(){
	if(isReady()){
		connect(control_, SIGNAL(valueChanged(double)), this, SLOT(onValueChanged(double)));
		connect(control_, SIGNAL(valueChanged(double)), this, SIGNAL(controlValueChanged(double)));
		startpoint_ = control_->value();
		outlierpoint_ = startpoint_;
		if( (targetType_ == AMBeamlineControlWaitAction::LessThanTarget) || (targetType_ == AMBeamlineControlWaitAction::GreaterThanTarget) )
			actionTolerance_ = 0;
		connect(&progressTimer_, SIGNAL(timeout()), this, SLOT(calculateProgress()));
		progressTimer_.start(500);
		setStarted(true);
		onValueChanged(control_->value());
	}
	else
		connect(this, SIGNAL(ready(bool)), this, SLOT(delayedStart(bool)));
}

void AMBeamlineControlWaitAction::cancel(){
	disconnect(control_, SIGNAL(valueChanged(double)), this, SLOT(onValueChanged(double)));
	disconnect(&progressTimer_, SIGNAL(timeout()), this, SLOT(calculateProgress()));
	progressTimer_.stop();
	setFailed(true); //NEED TO SEND FAILURE EXPLANATION
}

void AMBeamlineControlWaitAction::setControl(AMControl *control){
	if(control_)
		disconnect(control_, SIGNAL(connected(bool)), this, SLOT(onConnected(bool)));
	control_ = control;
	if(control_)
		connect(control_, SIGNAL(connected(bool)), this, SLOT(onConnected(bool)));
	checkReady();
}

bool AMBeamlineControlWaitAction::setWaitpoint(double waitpoint){
	if(!control_)
		return false;
	if(control_->isConnected() && control_->valueOutOfRange(waitpoint))
		return false;
	waitpoint_ = waitpoint;
	return true;
}

void AMBeamlineControlWaitAction::setActionTolerance(double actionTolerance){
	actionTolerance_ = actionTolerance;
}

void AMBeamlineControlWaitAction::setTargetType(AMBeamlineControlWaitAction::waitTargetType targetType){
	targetType_ = targetType;
}

bool AMBeamlineControlWaitAction::setHoldTime(double holdTime){
	if(holdTime < 0)
		return false;
	holdTime_ = holdTime;
	return true;
}

void AMBeamlineControlWaitAction::cleanup(){

}

void AMBeamlineControlWaitAction::delayedStart(bool ready){
	if(ready){
		disconnect(this, SIGNAL(ready(bool)), this, SLOT(delayedStart(bool)));
		start();
	}
}

void AMBeamlineControlWaitAction::onValueChanged(double newValue){
	bool startTimer = false;
	bool stopTimer = false;
	bool timerRunning = holdTimeTimer_.isActive();

	switch(targetType_){
	case AMBeamlineControlWaitAction::LessThanTarget :
		if( !timerRunning && (newValue < waitpoint_) )
			startTimer = true;
		else if( timerRunning && (newValue >= waitpoint_) )
			stopTimer = true;
		break;
	case AMBeamlineControlWaitAction::GreaterThanTarget :
		if( !timerRunning && (newValue > waitpoint_) )
			startTimer = true;
		else if( timerRunning && (newValue <= waitpoint_) )
			stopTimer = true;
		break;
	case AMBeamlineControlWaitAction::EqualToTarget :
		if( !timerRunning && (fabs(newValue-waitpoint_) < actionTolerance_) )
			startTimer = true;
		else if( timerRunning && (fabs(newValue-waitpoint_) >= actionTolerance_) )
			stopTimer = true;
		break;
	case AMBeamlineControlWaitAction::NotEqualToTarget :
		if( !timerRunning && (fabs(newValue-waitpoint_) > actionTolerance_) )
			startTimer = true;
		else if( timerRunning && (fabs(newValue-waitpoint_) <= actionTolerance_) )
			stopTimer = true;
		break;
	}
	if(startTimer){
		connect(&holdTimeTimer_, SIGNAL(timeout()), this, SLOT(onHoldTimeReached()));
		holdTimeTimer_.start(int(holdTime_));
	}
	else if(stopTimer){
		disconnect(&holdTimeTimer_, SIGNAL(timeout()), this, SLOT(onHoldTimeReached()));
		holdTimeTimer_.stop();
	}
}

void AMBeamlineControlWaitAction::onConnected(bool connected){
	Q_UNUSED(connected)
	if(!hasStarted()){
		if(control_->valueOutOfRange(waitpoint_))
			waitpoint_ = 0;
		checkReady();
	}
}

void AMBeamlineControlWaitAction::checkReady(){
	if(!control_ || !control_->isConnected())
		setReady(false);
	else
		setReady(true);
}

void AMBeamlineControlWaitAction::calculateProgress(){
	double elapsed;
	if(targetType_ != AMBeamlineControlWaitAction::NotEqualToTarget){
		if( fabs(control_->value()-waitpoint_) > fabs(outlierpoint_-waitpoint_) ){
			outlierpoint_ = control_->value();
			emit outlierValueChanged(outlierpoint_);
		}
		elapsed = 1.0 - (fabs(control_->value()-waitpoint_)-actionTolerance_)/(fabs(outlierpoint_-waitpoint_)-actionTolerance_);
	}
	else
		elapsed = fabs(control_->value()-waitpoint_)/actionTolerance_;
	//qDebug() << "Progress is " << elapsed/1.0;
	emit progress(elapsed, 1.0);
}

void AMBeamlineControlWaitAction::onHoldTimeReached(){
	disconnect(&holdTimeTimer_, SIGNAL(timeout()), this, SLOT(onHoldTimeReached()));
	holdTimeTimer_.stop();
	progressTimer_.stop();
	emit progress(1.0, 1.0);
	setSucceeded(true);
}

AMBeamlineControlWaitDetailedActionView::AMBeamlineControlWaitDetailedActionView(AMBeamlineControlWaitAction *waitAction, int index, QWidget *parent) :
		AMBeamlineActionItemView(waitAction, index, parent)
{
	messageLabel_ = 0; //NULL
	progressSlider_ = 0; //NULL
	waitAction_ = 0; //NULL
	setAction(waitAction);

	setMinimumHeight(NATURAL_ACTION_VIEW_HEIGHT);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

	messageLabel_ = new QLabel();
	messageLabel_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Maximum);

	if(waitAction_ && waitAction_->targetType() == AMBeamlineControlWaitAction::GreaterThanTarget)
		progressSlider_ = new AMDoubleSlider(AMDoubleSlider::logrithmic, AMDoubleSlider::destinationRight);
	else if(waitAction_ && waitAction_->targetType() == AMBeamlineControlWaitAction::LessThanTarget)
		progressSlider_ = new AMDoubleSlider(AMDoubleSlider::logrithmic, AMDoubleSlider::destinationLeft);
	else
		progressSlider_ = new AMDoubleSlider();

	finishedState_ = new QToolButton();
	finishedState_->setEnabled(false);
	finishedState_->setIcon(QIcon(":/greenCheck.png"));

	helpButton_ = new QToolButton();
	helpButton_->setIcon(style()->standardIcon(QStyle::SP_MessageBoxQuestion));
	connect(helpButton_, SIGNAL(clicked()), this, SLOT(onHelpButtonClicked()));

	mainHL_ = new QHBoxLayout();
	mainHL_->addWidget(messageLabel_);
	mainHL_->addWidget(progressSlider_);

	setLayout(mainHL_);
	QMargins mainHLMargins = mainHL_->contentsMargins();
	mainHLMargins.setTop(1);
	mainHLMargins.setBottom(1);
	mainHL_->setContentsMargins(mainHLMargins);

	onInfoChanged();
}

void AMBeamlineControlWaitDetailedActionView::setIndex(int index){
	index_ = index;
	onInfoChanged();
}

void AMBeamlineControlWaitDetailedActionView::setAction(AMBeamlineActionItem *action){
	AMBeamlineControlWaitAction *waitAction = qobject_cast<AMBeamlineControlWaitAction*>(action);
	if(waitAction_){
		disconnect(waitAction_, SIGNAL(controlValueChanged(double)), this, SLOT(onInfoChanged()));
		disconnect(waitAction_->control(), SIGNAL(connected(bool)), this, SLOT(onInfoChanged()));
		disconnect(waitAction_, SIGNAL(outlierValueChanged(double)), this, SLOT(onInfoChanged()));
		disconnect(waitAction_, SIGNAL(started()), this, SLOT(onActionStarted()));
		disconnect(waitAction_, SIGNAL(succeeded()), this, SLOT(onActionSucceeded()));
		disconnect(waitAction_, SIGNAL(failed(int)), this, SLOT(onActionFailed(int)));
	}
	waitAction_ = waitAction;
	if(waitAction_){
		connect(waitAction_, SIGNAL(controlValueChanged(double)), this, SLOT(onInfoChanged()));
		connect(waitAction_->control(), SIGNAL(connected(bool)), this, SLOT(onInfoChanged()));
		connect(waitAction_, SIGNAL(outlierValueChanged(double)), this, SLOT(onInfoChanged()));
		connect(waitAction_, SIGNAL(started()), this, SLOT(onActionStarted()));
		connect(waitAction_, SIGNAL(succeeded()), this, SLOT(onActionSucceeded()));
		connect(waitAction_, SIGNAL(failed(int)), this, SLOT(onActionFailed(int)));
	}
	onInfoChanged();
}

void AMBeamlineControlWaitDetailedActionView::onInfoChanged(){
	if(waitAction_ && messageLabel_ && progressSlider_){
		messageLabel_->setText(waitAction_->message());
		double outlier;
		if(waitAction_->isRunning())
			outlier = waitAction_->outlier();
		else
			outlier = waitAction_->control()->value();
		if(outlier > waitAction_->waitpoint()){
			progressSlider_->setMaximum(outlier);
			progressSlider_->setMinimum(waitAction_->waitpoint());
		}
		else{
			progressSlider_->setMaximum(waitAction_->waitpoint());
			progressSlider_->setMinimum(outlier);
		}
		if(waitAction_->isRunning())
			progressSlider_->setCurrentValue(waitAction_->control()->value());
		if(waitAction_->hasHelp() && (mainHL_->indexOf(helpButton_) == -1) )
			mainHL_->addWidget(helpButton_);
	}
}

void AMBeamlineControlWaitDetailedActionView::onPlayPauseButtonClicked(){
	//UNUSED
}

void AMBeamlineControlWaitDetailedActionView::onStopCancelButtonClicked(){
	//UNUSED
}

void AMBeamlineControlWaitDetailedActionView::onActionStarted(){
	emit actionStarted(waitAction_);
}

void AMBeamlineControlWaitDetailedActionView::onActionSucceeded(){
	mainHL_->addWidget(finishedState_);
	emit actionSucceeded(waitAction_);
}

void AMBeamlineControlWaitDetailedActionView::onActionFailed(int explanation){
	Q_UNUSED(explanation)
}

void AMBeamlineControlWaitDetailedActionView::onHelpButtonClicked(){
	AMImageListView *helpImagesView = new AMImageListView(waitAction_->helpImages());
	helpImagesView->show();
}

AMDoubleSlider::AMDoubleSlider(AMDoubleSlider::scalingModeType scalingMode, AMDoubleSlider::destinationType destinationMode, QWidget *parent):
		QWidget(parent)
{
	scalingMode_ = scalingMode;
	destinationMode_ = destinationMode;
	slider_ = new QSlider(Qt::Horizontal);
	slider_->setMinimum(0);
	slider_->setMaximum(120);
	slider_->setEnabled(false);

	QFile file;
	if(destinationMode_ == AMDoubleSlider::destinationRight)
		file.setFileName(":sliderWaitGreaterThan.qss");
	else
		file.setFileName(":sliderWaitLessThan.qss");
	file.open(QFile::ReadOnly);
	QString sliderStyleSheet = QLatin1String(file.readAll());
	slider_->setStyleSheet(sliderStyleSheet);

	valLabel_ = new QLabel("");
	minLabel_ = new QLabel("");
	maxLabel_ = new QLabel("");

	linearButton_ = new QToolButton();
	linearButton_->setText("lin");
	linearButton_->setCheckable(true);
	linearButton_->setContentsMargins(1, 1, 1, 1);
	logrithmicButton_ = new QToolButton();
	logrithmicButton_->setText("log");
	logrithmicButton_->setCheckable(true);
	logrithmicButton_->setContentsMargins(1, 1, 1, 1);
	scalingGroup_ = new QButtonGroup();
	scalingGroup_->addButton(linearButton_, AMDoubleSlider::linear);
	scalingGroup_->addButton(logrithmicButton_, AMDoubleSlider::logrithmic);
	if(scalingMode_ == AMDoubleSlider::linear)
		linearButton_->setChecked(true);
	else if(scalingMode_ == AMDoubleSlider::logrithmic)
		logrithmicButton_->setChecked(true);
	connect(scalingGroup_, SIGNAL(buttonClicked(int)), this, SLOT(onScalingButtonClicked(int)));

	QHBoxLayout *hl= new QHBoxLayout();
	hl->addWidget(minLabel_);
	hl->addStretch(10);
	hl->addWidget(linearButton_);
	hl->addWidget(valLabel_);
	hl->addWidget(logrithmicButton_);
	hl->addStretch(10);
	hl->addWidget(maxLabel_);
	hl->setContentsMargins(1, 1, 1, 1);
	hl->setMargin(2);

	vl_ = new QVBoxLayout();
	vl_->addWidget(slider_);
	vl_->addSpacing(0);
	vl_->addLayout(hl);
	setLayout(vl_);
}

double AMDoubleSlider::currentValue() const{
	return curValue_;
}

double AMDoubleSlider::minimum() const{
	return minValue_;
}

double AMDoubleSlider::maximum() const{
	return maxValue_;
}

AMDoubleSlider::scalingModeType AMDoubleSlider::scalingMode() const{
	return scalingMode_;
}

void AMDoubleSlider::setCurrentValue(double value){
	if(value < minValue_)
		curValue_ = minValue_;
	else if(value > maxValue_)
		curValue_ = maxValue_;
	else
		curValue_ = value;

	double percent;
	if(scalingMode_ == AMDoubleSlider::logrithmic)
		percent = 100.0*(log10(curValue_/minValue_))/(log10(maxValue_/minValue_));
	else//presume linear
		percent = 100.0*(curValue_-minValue_)/(maxValue_-minValue_);

	valLabel_->setText(QString("%1").arg(curValue_));
	slider_->setValue((int)percent+10);
}

void AMDoubleSlider::setMinimum(double minimum){
	minValue_ = minimum;
	minLabel_->setText(QString("%1").arg(minValue_));
}

void AMDoubleSlider::setMaximum(double maximum){
	maxValue_ = maximum;
	maxLabel_->setText(QString("%1").arg(maxValue_));
}

void AMDoubleSlider::setScalingMode(int scalingMode){
	if(trySetScalingMode(scalingMode))
		scalingGroup_->button(scalingMode)->setChecked(true);
}

bool AMDoubleSlider::trySetScalingMode(int scalingMode){
	if(scalingMode < AMDoubleSlider::linear || scalingMode > AMDoubleSlider::logrithmic)
		return false;
	if(scalingMode_ == scalingMode)
		return false;
	scalingMode_ = (AMDoubleSlider::scalingModeType)scalingMode;
	setCurrentValue(curValue_);
	return true;
}

void AMDoubleSlider::onScalingButtonClicked(int scalingMode){
	trySetScalingMode(scalingMode);
}
