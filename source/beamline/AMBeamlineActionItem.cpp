#include "AMBeamlineActionItem.h"

AMBeamlineActionItem::AMBeamlineActionItem(QObject *parent) :
	QObject(parent)
{
	previous_ = NULL;
	next_ = NULL;
}

AMBeamlineActionItemView::AMBeamlineActionItemView(AMBeamlineActionItem *item, QWidget *parent) :
		QWidget(parent)
{
	item_ = item;
	hl_ = new QHBoxLayout();
	message_ = new QLabel(item_->message(), this);
	message_->setEnabled(false);
	message_->setWordWrap(true);
	light_ = new QPushButton("", this);
	light_->setEnabled(false);
//	light_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
	proceed_ = new QPushButton("Proceed", this);
	proceed_->setEnabled(false);
//	proceed_->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
	hl_->addWidget(message_);
	hl_->addWidget(light_);
	hl_->addWidget(proceed_);
	setLayout(hl_);

	connect(item_, SIGNAL(started()), this, SLOT(onStart()));
	connect(proceed_, SIGNAL(clicked()), item_, SIGNAL(succeeded()));
	if(item_->previous()){
		connect(item_->previous(), SIGNAL(succeeded()), item_, SLOT(start()));
		qDebug() << "Has previous, no need to start";
	}
	else{
		qDebug() << "No previous, starting now";
		item_->start();
	}
	QString masterStyle = "QPushButton { max-width: 72px }";
	setStyleSheet(masterStyle);
}

void AMBeamlineActionItemView::onStart(){
	QString lightStopStyle = "QPushButton { background: red; border: 1px solid red; }";
	QString lightYieldStyle = "QPushButton { background: yellow; border: 1px solid yellow; }";
	QString lightGoStyle = "QPushButton { background: green; border: 1px solid green; }";
	message_->setEnabled(true);
	if(!item_->hasFeedback()){
		light_->setStyleSheet(lightYieldStyle);
		proceed_->setEnabled(true);
	}
	else{
		light_->setStyleSheet(lightStopStyle);
		connect(item_, SIGNAL(ready(bool)), this, SLOT(onReady(bool)));
	}
}

void AMBeamlineActionItemView::onReady(bool ready){
	QString lightStopStyle = "QPushButton { background: red; border: 1px solid red; }";
	QString lightGoStyle = "QPushButton { background: green; border: 1px solid green; }";
	if(ready)
		light_->setStyleSheet(lightGoStyle);
	else
		light_->setStyleSheet(lightStopStyle);
	proceed_->setEnabled(ready);
}
