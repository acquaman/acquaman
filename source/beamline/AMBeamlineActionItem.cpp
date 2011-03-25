/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

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


#include "AMBeamlineActionItem.h"

AMBeamlineActionItemStateFlag::AMBeamlineActionItemStateFlag(bool initialState, QObject *parent) :
		QObject(parent)
{
	state_ = initialState;
}

bool AMBeamlineActionItemStateFlag::state() const{
	return state_;
}

void AMBeamlineActionItemStateFlag::setState(bool state){
	if(state_ != state){
		state_ = state;
		emit stateChanged(state_);
	}
}

AMBeamlineActionItem::AMBeamlineActionItem(QObject *parent) :
	QObject(parent)
{
	previous_ = NULL;
	next_ = NULL;
	type_ = "actionItem";
	reinitialized_.setState(false);
	connect(&ready_, SIGNAL(stateChanged(bool)), this, SLOT(dirtyInitialized()));
	connect(&started_, SIGNAL(stateChanged(bool)), this, SLOT(dirtyInitialized()));
	connect(&succeeded_, SIGNAL(stateChanged(bool)), this, SLOT(dirtyInitialized()));
	connect(&failed_, SIGNAL(stateChanged(bool)), this, SLOT(dirtyInitialized()));
	connect(&finished_, SIGNAL(stateChanged(bool)), this, SLOT(dirtyInitialized()));
	initialize();
}

AMBeamlineActionItem::AMBeamlineActionItem(bool delayInitialize, QObject *parent){
	previous_ = NULL;
	next_ = NULL;
	type_ = "actionItem";
	reinitialized_.setState(false);
	connect(&ready_, SIGNAL(stateChanged(bool)), this, SLOT(dirtyInitialized()));
	connect(&started_, SIGNAL(stateChanged(bool)), this, SLOT(dirtyInitialized()));
	connect(&succeeded_, SIGNAL(stateChanged(bool)), this, SLOT(dirtyInitialized()));
	connect(&failed_, SIGNAL(stateChanged(bool)), this, SLOT(dirtyInitialized()));
	connect(&finished_, SIGNAL(stateChanged(bool)), this, SLOT(dirtyInitialized()));
	if(!delayInitialize)
		initialize();
}

bool AMBeamlineActionItem::isInitialized() const{
	return initialized_.state();
}

bool AMBeamlineActionItem::isReinitialized() const{
	return reinitialized_.state();
}

bool AMBeamlineActionItem::isReady() const{
	return ready_.state();
}

bool AMBeamlineActionItem::hasStarted() const{
	return started_.state();
}

bool AMBeamlineActionItem::isRunning() const{
	return started_.state() && !finished_.state();
}

bool AMBeamlineActionItem::hasSucceeded() const{
	return succeeded_.state();
}

bool AMBeamlineActionItem::hasFailed() const{
	return failed_.state();
}

bool AMBeamlineActionItem::hasFinished() const{
	return finished_.state();
}

AMBeamlineActionItem* AMBeamlineActionItem::previous() const {
	return previous_;
}

AMBeamlineActionItem* AMBeamlineActionItem::next() const {
	return next_;
}

QString AMBeamlineActionItem::type() const {
	return type_;
}

void AMBeamlineActionItem::reset(bool delayInitialize){
	reinitialized_.setState(true);
	if(!delayInitialize)
		initialize();
}

bool AMBeamlineActionItem::setPrevious(AMBeamlineActionItem *previous){
	previous_ = previous;
	return true;
}

bool AMBeamlineActionItem::setNext(AMBeamlineActionItem *next){
	next_ = next;
	return true;
}

void AMBeamlineActionItem::setReady(bool isReady){
	if(ready_.state() != isReady){
		ready_.setState(isReady);
		emit ready(ready_.state());
	}
}

void AMBeamlineActionItem::setStarted(bool isStarted){
	if(started_.state() != isStarted){
		started_.setState(isStarted);
		if(started_.state())
			emit started();
	}
}

void AMBeamlineActionItem::setSucceeded(bool isSucceeded){
	if(succeeded_.state() != isSucceeded){
		succeeded_.setState(isSucceeded);
		if(succeeded_.state()){
			emit progress(1.0, 1.0);
			emit succeeded();
			setFinished(true);
		}
	}
}

void AMBeamlineActionItem::setFailed(bool isFailed, int explanation){
	if(failed_.state() != isFailed){
		failed_.setState(isFailed);
		if(failed_.state()){
			emit failed(explanation);
			setFinished(true);
		}
	}
}

void AMBeamlineActionItem::setFinished(bool isFinished){
	if(finished_.state() != isFinished){
		finished_.setState(isFinished);
		if(finished_.state()){
			emit finished();
		}
	}
}

void AMBeamlineActionItem::initialize(){
	//qDebug() << "AMBeamlineActionItem::initialize()";
	ready_.setState(false);
	started_.setState(false);
	succeeded_.setState(false);
	failed_.setState(false);
	finished_.setState(false);
	initialized_.setState(true);
	emit initialized();
}

void AMBeamlineActionItem::dirtyInitialized(){
	if(initialized_.state() != false){
		initialized_.setState(false);
	}
}

AMBeamlineActionView::AMBeamlineActionView(AMBeamlineActionItem *action, int index, QWidget *parent) :
		QFrame(parent)
{
	action_ = action;
	index_ = index;
	inFocus_ = false;
	viewType_ = "actionView";
	setLineWidth(1);
	setFrameStyle(QFrame::StyledPanel);
	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
}

AMBeamlineActionItem* AMBeamlineActionView::action(){
	return action_;
}

QString AMBeamlineActionView::viewType() const{
	return viewType_;
}

void AMBeamlineActionView::setIndex(int index){
	index_ = index;
}

void AMBeamlineActionView::setAction(AMBeamlineActionItem *action){
	action_ = action;
}

void AMBeamlineActionView::defocusItem(){
	inFocus_ = false;
	updateLook();
}

void AMBeamlineActionView::mousePressEvent(QMouseEvent *event){
	if (event->button() != Qt::LeftButton) {
		event->ignore();
		return;
	}
	if(inFocus_)
		defocusItem();
	else{
		inFocus_ = true;
		updateLook();
		emit focusRequested(action_);
	}
}

void AMBeamlineActionView::updateLook(){
	if(inFocus_)
		setFrameStyle(QFrame::Box);
	if(inFocus_)
		setStyleSheet("AMBeamlineScanActionView { background : rgb(194, 210, 215) }");
	else{
		setStyleSheet("AMBeamlineScanActionView { background : rgb(230, 222, 214) }");
		setFrameStyle(QFrame::StyledPanel);
	}
}

/*****************************************************************************
*
*
*   COMPATABILITY CLASSES
*
*
*
******************************************************************************/

AM1BeamlineActionItem::AM1BeamlineActionItem(QString message, QObject *parent) :
	QObject(parent)
{
	message_ = message;
	previous_ = NULL;
	next_ = NULL;
	started_ = false;
	succeeded_ = false;
	failed_ = false;
	running_ = false;
	hasFeedback_ = false;
	needsInput_ = false;
	type_ = "actionItem";
}

bool AM1BeamlineActionItem::setPrevious(AM1BeamlineActionItem *previous){
	previous_ = previous;
	return true;
}

bool AM1BeamlineActionItem::setNext(AM1BeamlineActionItem *next){
	next_ = next;
	return true;
}


AMBeamlineActionItemView::AMBeamlineActionItemView(AM1BeamlineActionItem *item, QWidget *parent) :
		QWidget(parent)
{
	item_ = item;
	hl_ = new QHBoxLayout();
	message_ = new QLabel(item_->message(), this);
	message_->setWordWrap(true);
	light_ = new QPushButton("", this);
	proceed_ = new QPushButton("Proceed", this);
	hl_->addWidget(message_);
	hl_->addWidget(light_);
	hl_->addWidget(proceed_);
	initializeView();
	setLayout(hl_);

	connect(item_, SIGNAL(started()), this, SLOT(onStart()));
	connect(proceed_, SIGNAL(clicked()), item_, SIGNAL(succeeded()));
	if(item_->previous())
		connect(item_->previous(), SIGNAL(succeeded()), item_, SLOT(start()));

	if(!item_->hasFeedback())
		connect(proceed_, SIGNAL(clicked(bool)), this, SLOT(onReady(bool)));
	else
		connect(item_, SIGNAL(ready(bool)), this, SLOT(onReady(bool)));
	QString masterStyle = "QPushButton { max-width: 72px }";
	setStyleSheet(masterStyle);
}

void AMBeamlineActionItemView::initializeView(){
	QString lightDisableStyle = "QPushButton { background: lightgray; border: 1px solid lightgray; }";
	message_->setEnabled(false);
	light_->setEnabled(false);
	proceed_->setEnabled(false);
	light_->setStyleSheet(lightDisableStyle);
}

void AMBeamlineActionItemView::onStart(){
	QString lightStopStyle = "QPushButton { background: red; border: 1px solid red; }";
	QString lightYieldStyle = "QPushButton { background: yellow; border: 1px solid yellow; }";
	message_->setEnabled(true);
	if(!item_->hasFeedback()){
		light_->setStyleSheet(lightYieldStyle);
		proceed_->setEnabled(true);
	}
	else
		light_->setStyleSheet(lightStopStyle);
}

void AMBeamlineActionItemView::onReady(bool ready){
	QString lightStopStyle = "QPushButton { background: red; border: 1px solid red; }";
	QString lightGoStyle = "QPushButton { background: green; border: 1px solid green; }";
	QString lightGoYieldStyle = "QPushButton { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0.4 yellow, stop:1 green); border: 1px qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0.4 yellow, stop:1 green); }";
	if(item_->hasFeedback()){
		if(ready)
			light_->setStyleSheet(lightGoStyle);
		else
			light_->setStyleSheet(lightStopStyle);
		proceed_->setEnabled(ready);
	}
	else{
		light_->setStyleSheet(lightGoYieldStyle);
	}
}

void AMBeamlineActionItemView::fixMessageSize(int width){
	message_->setMinimumWidth(width);
}

void AMBeamlineActionItemView::fixLightSize(int width){
	light_->setMinimumWidth(width);
}

void AMBeamlineActionItemView::fixProceedSize(int width){
	proceed_->setMinimumWidth(width);
}
