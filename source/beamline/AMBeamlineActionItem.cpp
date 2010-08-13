#include "AMBeamlineActionItem.h"

AMBeamlineActionItem::AMBeamlineActionItem(QString message, QObject *parent) :
	QObject(parent)
{
	message_ = message;
	previous_ = NULL;
	next_ = NULL;
	running_ = false;
	hasFeedback_ = false;
	needsInput_ = false;
	type_ = "actionItem";
}

bool AMBeamlineActionItem::setPrevious(AMBeamlineActionItem *previous){
	previous_ = previous;
	return true;
	/*
	if(previous_ == previous)
		return;
	if(previous_)
		disconnect(previous_, SIGNAL(succeeded()), this, SLOT(start()));
	if(previous && previous->next())
		disconnect(previous, SIGNAL(succeeded()), previous->next(), SLOT(start()));
	previous_ = previous;
	if(previous_)
		previous_->setNext(this);
	*/
}

bool AMBeamlineActionItem::setNext(AMBeamlineActionItem *next){
	next_ = next;
	return true;
	/*
	if(next_ == next)
		return;
//	if(next && next->previous())
//		disconnect(next->previous(), SIGNAL(succeeded()), next, SLOT(start()));
	next_ = next;
	if(next_){
		next_->setPrevious(this);
//		connect(this, SIGNAL(succeeded()), next_, SLOT(start()));
	}
	*/
}

AMBeamlineActionView::AMBeamlineActionView(AMBeamlineActionItem *action, int index, QWidget *parent) :
		QFrame(parent)
{
	action_ = action;
	index_ = index;
	inFocus_ = false;
	setLineWidth(1);
	setFrameStyle(QFrame::StyledPanel);
}

AMBeamlineActionItem* AMBeamlineActionView::action(){
	return action_;
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
	qDebug() << "Detected mouse click";
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

AMBeamlineActionHiddenView::AMBeamlineActionHiddenView(int count, QWidget *parent) :
		AMBeamlineActionView(NULL, 0, parent)
{
	count_ = count;

	infoLabel_ = new QLabel("");
	expandButton_ = new QPushButton("");
	onInfoChanged();

	hl_ = new QHBoxLayout();
	hl_->addWidget(infoLabel_);
	hl_->addWidget(expandButton_, 0, Qt::AlignRight);
	setLayout(hl_);
}

void AMBeamlineActionHiddenView::onInfoChanged(){
	QString infoStr, buttonStr;
	infoStr.setNum(count_);
	buttonStr.setNum(count_);
	infoStr.prepend("<");
	infoStr.append(" Hidden Items>");
	infoLabel_->setText(infoStr);
	buttonStr.prepend("Expand ");
	buttonStr.append(" Items");
	expandButton_->setText(buttonStr);
}

void AMBeamlineActionHiddenView::onStopCancelButtonClicked(){

}

void AMBeamlineActionHiddenView::onPlayPauseButtonClicked(){

}

void AMBeamlineActionHiddenView::onHideButtonClicked(){

}

AMBeamlineActionItemView::AMBeamlineActionItemView(AMBeamlineActionItem *item, QWidget *parent) :
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
