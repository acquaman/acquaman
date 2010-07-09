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

	if(item_->hasFeedback()){
//		connect(item_, SIGNAL);
	}
	else{

	}
	if(item_->previous())
		connect(item_->previous(), SIGNAL(succeeded()), item_, SLOT(start()));
	else
		item_->start();
}
