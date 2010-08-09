#include "AMBeamlineActionItem.h"

AMBeamlineActionItem::AMBeamlineActionItem(QString message, QObject *parent) :
	QObject(parent)
{
	message_ = message;
	previous_ = NULL;
	next_ = NULL;
	hasFeedback_ = false;
	needsInput_ = false;
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
