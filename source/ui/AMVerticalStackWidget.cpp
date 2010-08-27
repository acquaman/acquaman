#include "AMVerticalStackWidget.h"

#include "ui/AMHeaderButton.h"

Q_DECLARE_METATYPE(QToolButton*);

AMVerticalStackWidget::AMVerticalStackWidget(QWidget *parent) :
		QFrame(parent)
{
	vl_ = new QVBoxLayout();
	vl_->setSpacing(0);
	vl_->setContentsMargins(0,0,0,0);
	vl_->addStretch();
	setLayout(vl_);

	setFrameShape(QFrame::StyledPanel);
	setFrameShadow(QFrame::Sunken);
	setStyleSheet("AMVerticalStackWidget {background:white; }");


}




/// Return the index of a widget in the stack. (-1 if not found)
int AMVerticalStackWidget::indexOf(QWidget* widget) const {
	for(int i=0; i<count(); i++)
		if( model_.data(model_.index(i,0), AM::PointerRole).value<QWidget*>() == widget)
			return i;
	return -1;
}
/// Return the widget at a specific index, or 0 for invalid indexes
QWidget* AMVerticalStackWidget::widget(int index) const {
	if(index<0 || index>=count())
		return 0;
	return model_.data(model_.index(index,0), AM::PointerRole).value<QWidget*>();
}

/// Insert a widget at a specific index in the stack. Inserting at \c index = -1 is equivalent to appending to the end. The AMVerticalStackWidget takes ownership of the widget.
void AMVerticalStackWidget::insertItem(int index, QWidget* widget, const QString& text, bool collapsable) {
	if(index < 0 || index > count())
		index = count();

	QStandardItem* item = new QStandardItem(text);
	item->setData(qVariantFromValue(widget), AM::PointerRole);
	item->setData(false, Qt::CheckStateRole);
	item->setFlags( collapsable ? (Qt::ItemIsEnabled | Qt::ItemIsUserCheckable) : Qt::ItemIsEnabled);

	QToolButton* header = new AMHeaderButton();
	header->setText(text);
	item->setData(qVariantFromValue(header), AM::WidgetRole);
	connect(header, SIGNAL(clicked()), this, SLOT(onHeaderButtonClicked()));

	vl_->insertWidget(2*index, header);
	vl_->insertWidget(2*index+1, widget);


	model_.insertRow(index, item);
}


/// Remove a widget and return it.
QWidget* AMVerticalStackWidget::takeItem(int index) {
	if(index<0 || index>=count())
		return 0;

	QStandardItem* item = model_.takeRow(index).at(0);
	QWidget* w = item->data(AM::PointerRole).value<QWidget*>();
	QToolButton* b = item->data(AM::WidgetRole).value<QToolButton*>();

	vl_->takeAt(2*index);
	vl_->takeAt(2*index + 1);

	delete b;
	delete item;
	return w;
}

/// Is this item collapsable?
bool AMVerticalStackWidget::itemCollapsable(int index) const {
	if(index < 0 || index >= count())
		return false;

	QStandardItem* item = model_.item(index,0);
	return (item->flags() & Qt::ItemIsUserCheckable);
}
/// Is this item collapsed?
bool AMVerticalStackWidget::itemIsCollapsed(int index) const {
	if(index < 0 || index >= count())
		return false;

	return model_.data(model_.index(index,0), Qt::CheckStateRole).toBool();
}

/// Set the heading text for a widget
void AMVerticalStackWidget::setItemText(int index, const QString& text) {
	if(index < 0 || index >= count())
		return;

	QStandardItem* item = model_.item(index, 0);
	item->setData(text, Qt::DisplayRole);
	QToolButton* tb = item->data(AM::WidgetRole).value<QToolButton*>();
	if(tb)
		tb->setText(text);
}


/// Expand a given widget
void AMVerticalStackWidget::expandItem(int index) {
	if(index < 0 || index >= count())
		return;

	QWidget* w = model_.data(model_.index(index, 0), AM::PointerRole).value<QWidget*>();
	if(w)
		w->show();
	QToolButton* h = model_.data(model_.index(index,0), AM::WidgetRole).value<QToolButton*>();
	if(h)
		h->setIcon(QIcon(":/22x22/arrow-white-down.png"));
//		h->setArrowType(Qt::DownArrow);
	model_.setData(model_.index(index,0), false, Qt::CheckStateRole);
}

/// Collapse a given widget
void AMVerticalStackWidget::collapseItem(int index) {
	if(index < 0 || index >= count())
		return;

	QWidget* w = model_.data(model_.index(index, 0), AM::PointerRole).value<QWidget*>();
	if(w)
		w->hide();
	QToolButton* h = model_.data(model_.index(index,0), AM::WidgetRole).value<QToolButton*>();
	if(h)
		h->setIcon(QIcon(":/22x22/arrow-white-right.png"));
//		h->setArrowType(Qt::RightArrow);
	model_.setData(model_.index(index,0), true, Qt::CheckStateRole);
}


/// \todo Optimize this so we don't have to linear search through the whole model to find out which header was clicked
void AMVerticalStackWidget::onHeaderButtonClicked() {
	QObject* s = sender();
	for(int i=0; i<count(); i++) {
		if(model_.data(model_.index(i,0), AM::WidgetRole).value<QToolButton*>() == s) {
			if(itemCollapsable(i))
				itemIsCollapsed(i) ? expandItem(i) : collapseItem(i);
			return;
		}
	}
}

QSize AMVerticalStackWidget::sizeHint() const {
	QSize rv = QFrame::sizeHint();

	int width = rv.width();
	for(int i=0; i<count(); i++) {
		width = qMax(width, model_.data(model_.index(i,0), AM::PointerRole).value<QWidget*>()->sizeHint().width());
	}

	rv.setWidth(width+2);
	return rv;
}
