#include "AMSidebar.h"

AMSidebar::AMSidebar(QWidget* parent)
	: QTreeView(parent) {

	model_ = new QStandardItemModel(this);
	setModel(model_);

	setHeaderHidden(true);
	setRootIsDecorated(true);
	setAttribute(Qt::WA_MacShowFocusRect, false);
	setWordWrap(false);
	setIndentation(10);
	setAnimated(true);
	setEditTriggers(QAbstractItemView::SelectedClicked);

	doubleClickInProgress_ = false;

	connect(this, SIGNAL(clicked(QModelIndex)), this, SLOT(onItemClicked(QModelIndex)));
	connect(this, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onItemDoubleClicked(QModelIndex)));


	setMinimumWidth(200);

	setStyleSheet("AMSidebar { font: 500 10pt \"Lucida Grande\"; border-width: 1px;   border-style: solid;   border-color: rgb(221, 227, 234);  border-right-color: rgb(64, 64, 64); background-color: rgb(221, 227, 234); show-decoration-selected: 1; selection-background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(91, 146, 213, 255), stop:1 rgba(22, 84, 170, 255)); }"
				  " QTreeView::item { height: 30; } "
				  " /* QTreeView::item::hover { border-width: 1px; border-style: solid;	border-color: rgb(22, 84, 170); border-top-color: rgb(69, 128, 200); background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(91, 146, 213, 255), stop:1 rgba(22, 84, 170, 255)); } "
				  "QTreeView::item::selected { border: 1px solid rgb(115, 122, 153); border-top-color: rgb(131, 137, 167);  background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(170, 176, 197, 255), stop:1 rgba(115, 122, 153, 255)); } */");
}

/// add a category header with a certain \c weight
void AMSidebar::addHeading(const QString& categoryName, double weight) {
	if(headings_.contains(categoryName))
		return;
	AMSidebarHeading* h = new AMSidebarHeading(categoryName, weight);
	headings_.insert(categoryName, h);
	model_->invisibleRootItem()->appendRow(h);
	model_->invisibleRootItem()->sortChildren(0, Qt::AscendingOrder);
}

/// add a new link under category \c categoryName, with a payload \c linkContent.
/*! If no category with this name exists, a new category is created.
  */
QStandardItem* AMSidebar::addLink(const QString& categoryName, const QVariant& linkContent, const QString& text, const QIcon& icon, double weight) {

	AMSidebarHeading* h = heading(categoryName);
	AMSidebarItem* item = new AMSidebarItem(text, linkContent,icon, weight);
	h->appendRow(item);
	h->sortChildren(0, Qt::AscendingOrder);
	expand(h->index());

	return item;
}


/// add a new link (or group of links) under category \c categoryName, with a payload \c linkContent. The QStandardItem \c item is used to represent the top-level link.
/*!  To create nested links, you can give \c item a set of sub-items with QStandardItem::addRow() before calling this function.  Be sure to manually set each sub-item's AM::LinkRole to the desired destination first. */
void AMSidebar::addLink(const QString& categoryName, const QVariant& linkContent, QStandardItem* item, double weight) {

	AMSidebarHeading* h = heading(categoryName);
	item->setData(linkContent, AM::LinkRole);
	item->setData(weight, AM::WeightRole);
	h->appendRow(item);
	h->sortChildren(0, Qt::AscendingOrder);
}





/// Access the heading item for a given category
AMSidebarHeading* AMSidebar::heading(const QString& headingTitle) {
	if(!headings_.contains(headingTitle))
		addHeading(headingTitle);

	return headings_.value(headingTitle);
}



/*! \note The ordering of mouse events during a double-click goes like this:
  - [first release] onItemClicked()
  - [second press] onItemDoubleClicked()
  - [second release] onItemClicked() again. (for second click).
  Need to ignore the second release click if we just experienced a double-click.
  */
void AMSidebar::onItemClicked(const QModelIndex & index) {
	QVariant link = model_->itemFromIndex(index)->data(AM::LinkRole);

	if(doubleClickInProgress_) {
		doubleClickInProgress_ = false;
		return;
	}

	if(!link.isNull())
		emit linkClicked(link);
}

void AMSidebar::onItemDoubleClicked(const QModelIndex & index) {
	QVariant link = model_->itemFromIndex(index)->data(AM::LinkRole);
	doubleClickInProgress_ = true;
	if(!link.isNull())
		emit linkDoubleClicked(link);
}

/*
bool AMSidebar::eventFilter(QObject* sourceObject, QEvent* event) {

	QWidget* source = qobject_cast<QWidget*>(sourceObject);

	if(selector2link_.contains(source)) {

		switch(event->type()) {

		case QEvent::MouseButtonRelease:
			if(doubleClickInProgress_) {
				emit linkDoubleClicked(selector2link_[source].payload_);
			}
			else {
				emit linkClicked(selector2link_[source].payload_);
				setHighlightedLink(source);
			}

			break;


		case QEvent::MouseButtonDblClick:
			doubleClickInProgress_ = true;
			break;
		case QEvent::MouseButtonPress:
			doubleClickInProgress_ = false;
			break;


		default:
			break;
		}
	}
	return QFrame::eventFilter(source, event);
}*/
