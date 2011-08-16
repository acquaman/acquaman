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

#include <QObject>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDebug>
#include <QMouseEvent>
#include <QMenu>
#include <QAction>
#include <QStyle>

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
	description_ = "Generic Action";
	message_ = "";
	helpImages_.clear();
	reinitialized_.setState(false);
	connect(&ready_, SIGNAL(stateChanged(bool)), this, SLOT(dirtyInitialized()));
	connect(&started_, SIGNAL(stateChanged(bool)), this, SLOT(dirtyInitialized()));
	connect(&succeeded_, SIGNAL(stateChanged(bool)), this, SLOT(dirtyInitialized()));
	connect(&failed_, SIGNAL(stateChanged(bool)), this, SLOT(dirtyInitialized()));
	connect(&finished_, SIGNAL(stateChanged(bool)), this, SLOT(dirtyInitialized()));
	initialize();
}

AMBeamlineActionItem::AMBeamlineActionItem(bool delayInitialize, QObject *parent)
    : QObject(parent)
{
	previous_ = NULL;
	next_ = NULL;
	description_ = "Generic Action";
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
	//return started_.state() && !finished_.state();
	return started_.state() && !(finished_.state() || succeeded_.state() || failed_.state());
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

AMBeamlineActionItem* AMBeamlineActionItem::createCopy() const{
	return 0; //NULL
}

QString AMBeamlineActionItem::description() const{
	return description_;
}

QString AMBeamlineActionItem::message() const{
	return message_;
}

bool AMBeamlineActionItem::hasHelp() const{
	return !helpImages_.isEmpty();
}

AMOrderedSet<QString, QPixmap> AMBeamlineActionItem::helpImages() const{
	return helpImages_;
}

void AMBeamlineActionItem::reset(bool delayInitialize){
	reinitialized_.setState(true);
	if(!delayInitialize)
		initialize();
}

bool AMBeamlineActionItem::setPrevious(AMBeamlineActionItem *previous){
	if(previous_ != previous){
		previous_ = previous;
		emit previousChanged();
	}
	return true;
}

bool AMBeamlineActionItem::setNext(AMBeamlineActionItem *next){
	if(next_ != next){
		next_ = next;
		emit nextChanged();
	}
	return true;
}

void AMBeamlineActionItem::setDescription(const QString &description){
	bool changed = false;
	if(description_ != description)
		changed = true;
	description_ = description;
	if(changed)
		emit descriptionChanged(description_);
}

void AMBeamlineActionItem::setMessage(const QString &message){
	message_ = message;
}

void AMBeamlineActionItem::setHelp(const AMOrderedSet<QString, QPixmap> &helpImages){
	helpImages_ = helpImages;
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

AMBeamlineActionItemView::AMBeamlineActionItemView(AMBeamlineActionItem *action, int index, QWidget *parent) :
		QFrame(parent)
{
	//action_ = action;
	action_ = 0;//NULL
	oldHeight_ = 0;
	setAction(action);
	index_ = index;
	inFocus_ = false;
	movable_ = false;
	optionsMenu_ = 0; //NULL
	setLineWidth(2);
	setMidLineWidth(0);
	setFrameStyle(QFrame::StyledPanel);
	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);

	setAutoFillBackground(true);
	QPalette newPalette(palette());
	newPalette.setColor(QPalette::Window, QColor(255, 255, 255));
	setPalette(newPalette);
}

AMBeamlineActionItem* AMBeamlineActionItemView::action(){
	return action_;
}

bool AMBeamlineActionItemView::movable() const{
	return movable_;
}

void AMBeamlineActionItemView::setIndex(int index){
	index_ = index;
}

void AMBeamlineActionItemView::setAction(AMBeamlineActionItem *action){
	if(action_){
		disconnect(action_, SIGNAL(descriptionChanged(QString)), this, SIGNAL(descriptionChanged(QString)));
		disconnect(action_, SIGNAL(started()), this, SLOT(updateLook()));
		disconnect(action_, SIGNAL(failed(int)), this, SLOT(updateLook()));
		disconnect(action_, SIGNAL(succeeded()), this, SLOT(updateLook()));
		disconnect(action_, SIGNAL(previousChanged()), this, SLOT(onPreviousNextChanged()));
		disconnect(action_, SIGNAL(nextChanged()), this, SLOT(onPreviousNextChanged()));
	}
	action_ = action;
	if(action_){
		connect(action_, SIGNAL(descriptionChanged(QString)), this, SIGNAL(descriptionChanged(QString)));
		connect(action_, SIGNAL(started()), this, SLOT(updateLook()));
		connect(action_, SIGNAL(failed(int)), this, SLOT(updateLook()));
		connect(action_, SIGNAL(succeeded()), this, SLOT(updateLook()));
		connect(action_, SIGNAL(previousChanged()), this, SLOT(onPreviousNextChanged()));
		connect(action_, SIGNAL(nextChanged()), this, SLOT(onPreviousNextChanged()));
	}
	onPreviousNextChanged();
}

void AMBeamlineActionItemView::defocusItem(){
	inFocus_ = false;
	updateLook();
}

void AMBeamlineActionItemView::setMovable(bool movable){
	movable_ = movable;
}

void AMBeamlineActionItemView::onCreateCopyClicked(){
	AMBeamlineActionItem *actionCopy = action_->createCopy();
	if(actionCopy){
		qDebug() << "ActionView has a copy to send out";
		emit copyRequested(actionCopy);
	}
}

void AMBeamlineActionItemView::mousePressEvent(QMouseEvent *event){
	if (event->button() == Qt::LeftButton) {
		event->ignore();
		return;
	}
	/*if(event->button() == Qt::LeftButton){
		if(inFocus_)
			defocusItem();
		else{
			inFocus_ = true;
			updateLook();
			emit focusRequested(action_);
		}
	}*/
	else if(event->button() == Qt::RightButton){
		if(optionsMenu_)
			delete optionsMenu_;
		optionsMenu_ = new QMenu(this);
		QAction *tmpAction;
		tmpAction = optionsMenu_->addAction("Copy this Action");
		tmpAction->setData(0);
		connect(tmpAction, SIGNAL(triggered()), this, SLOT(onCreateCopyClicked()));
		optionsMenu_->popup(QCursor::pos());
		optionsMenu_->show();
	}
	else{
		event->ignore();
		return;
	}
}

void AMBeamlineActionItemView::paintEvent(QPaintEvent *event){
	QFrame::paintEvent(event);
	if(oldHeight_ != height()){
		oldHeight_ = height();
		//qDebug() << "New height is " << height();
		emit heightChanged(height());
	}
}

void AMBeamlineActionItemView::updateLook(){
	QPalette newPalette(palette());
	if(action_->isRunning() || inFocus_)
		setFrameStyle(QFrame::Box);
	else
		setFrameStyle(QFrame::StyledPanel);

	if(action_->isRunning())
		newPalette.setColor(QPalette::Window, QColor(194, 230, 208) );
	else if(action_->hasFailed())
		newPalette.setColor(QPalette::Window, QColor(255, 170, 170) );
	else if(action_->hasSucceeded())
		newPalette.setColor(QPalette::Window, QColor(200, 200, 200) );
	else
		newPalette.setColor(QPalette::Window, QColor(255, 255, 255) );

	if(inFocus_)
		newPalette.setColor(QPalette::WindowText, QColor(0, 255, 0));
	else
		newPalette.setColor(QPalette::WindowText, QColor(QPalette::WindowText));

	setPalette(newPalette);
}

void AMBeamlineActionItemView::onPreviousNextChanged(){

}

void AMBeamlineActionItemView::onMoveUpButtonClicked(){

}

void AMBeamlineActionItemView::onMoveDownButtonClicked(){

}


AMImageListView::AMImageListView(const AMOrderedSet<QString, QPixmap> &images, QWidget *parent) :
		QWidget(parent)
{
	QGridLayout *gl = new QGridLayout();
	int columnCount = 0;
	int scaledHeight = 0;
	if(images.count() <= 4){
		columnCount = 2;
		scaledHeight = 300;
	}
	else if(images.count() <= 9){
		columnCount = 3;
		scaledHeight = 200;
	}
	else{
		columnCount = 4;
		scaledHeight = 100;
	}
	int currentRow = 0;
	int currentColumn = 0;
	QLabel *tmpLabel;
	for(int x = 0; x < images.count(); x++){
		tmpLabel = new QLabel();
		tmpLabel->setPixmap(images.at(x).scaledToHeight(scaledHeight, Qt::SmoothTransformation));
		gl->addWidget(tmpLabel, currentRow, currentColumn);
		currentColumn++;
		if(currentColumn == columnCount){
			currentColumn = 0;
			currentRow++;
		}
	}
	setLayout(gl);
	gl->setContentsMargins(2, 2, 2, 2);
}
