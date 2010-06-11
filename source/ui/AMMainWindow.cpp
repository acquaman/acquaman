#include "AMMainWindow.h"

/// Default constructor
AMMainWindow::AMMainWindow(QWidget *parent) : QWidget(parent) {

	sidebar_ = new AMSidebar();
	stackWidget_ = new QStackedWidget();

	// setup layouts
	vlayout_ = new QVBoxLayout(this);
	vlayout_->setContentsMargins(0, 0, 0, 0);
	vlayout_->setSpacing(0);
	QHBoxLayout* hl1 = new QHBoxLayout();
	hl1->setContentsMargins(0, 0, 0, 0);
	hl1->setSpacing(0);
	vlayout_->addLayout(hl1);


	hl1->addWidget(sidebar_);
	hl1->addWidget(stackWidget_);
	// connect click and double-click signals from the sidebar:
	connect(sidebar_, SIGNAL(linkClicked(QVariant)), this, SLOT(onSidebarLinkClicked(QVariant)));
	connect(sidebar_, SIGNAL(linkDoubleClicked(QVariant)), this, SLOT(onSidebarLinkDoubleClicked(QVariant)));

	// connect the stackWidget_'s currentWidgetChanged signal to adjust the highlights in the sidebar:
	connect(stackWidget_, SIGNAL(currentChanged(int)), this, SLOT(onFwdCurrentWidgetChanged(int)));
}

AMMainWindow::~AMMainWindow() {
	// delete all children, including the cut-loose windows
	QList<QWidget*> panes = pane2entry_.keys();
	foreach(QWidget* pane, panes) {
		deletePane(pane);
	}
}

/// Add a new \c pane to manage.  It will show up under category \c categoryName, at the given \c weight, with \c name and an icon from \c iconFileName.
void AMMainWindow::addPane(QWidget* pane, const QString& categoryName, const QString& title, const QString& iconFileName, double weight) {

	pane->setWindowTitle(title);
	pane->setWindowIcon(QIcon(iconFileName));

	stackWidget_->addWidget(pane);
	QWidget* selector = new AMSidebarDefaultSelector(title, iconFileName);
	pane2entry_.insert(pane, AMMainWindowEntry(pane, selector));
	sidebar_->addLink(categoryName, QVariant::fromValue(pane), selector, weight);

	// For cut-loose windows: we need to catch the close events -- must put back into main window instead of closing
	pane->installEventFilter(this);
}

/// Removes and deletes a pane widget (whether docked or undocked)
void AMMainWindow::deletePane(QWidget* pane) {

	if(!pane2entry_.contains(pane))
		return;

	// remove from sidebar:
	sidebar_->deleteLink(QVariant::fromValue(pane));
	// delete actual widget:
	delete pane;

	// remove from list:
	pane2entry_.remove(pane);
}

/// Removes a pane widget but does not delete it.  Ownership is now the responsibility of the caller. The pane becomes a top-level window.
void AMMainWindow::removePane(QWidget* pane) {
	if(!pane2entry_.contains(pane))
		return;

	sidebar_->deleteLink(QVariant::fromValue(pane));
	pane2entry_.remove(pane);

	QSize oldSize = pane->size();
	QPoint oldPos = pane->mapToGlobal(pane->geometry().topLeft());
	stackWidget_->removeWidget(pane);
	pane->setParent(0);
	pane->setGeometry(QRect(oldPos + QPoint(20, 20), oldSize));
	pane->show();
}

/// move a pane from inside the main window to a separate window.
void AMMainWindow::undock(QWidget* pane) {

	if(!pane2entry_.contains(pane))
		return;

	if(pane2entry_[pane].isCutLoose_)
		return;

	pane2entry_[pane].isCutLoose_ = true;

	QSize oldSize = pane->size();
	QPoint oldPos = pane->mapToGlobal(pane->geometry().topLeft());

	// the most intuitive thing for users would be for the main window to take them back to their last-visited pane, now that this one is gone.
	if(pane2entry_.contains(previousPane_) && !pane2entry_[previousPane_].isCutLoose_)
		stackWidget_->setCurrentWidget(previousPane_);

	stackWidget_->removeWidget(pane);
	pane->setParent(0);
	pane->setGeometry(QRect(oldPos + QPoint(20, 20), oldSize));
	pane->show();

}

/// return a \c pane that was undocked back to the main window.  Does not set this pane as the current widget.
void AMMainWindow::dock(QWidget* pane) {

	if(!pane2entry_.contains(pane))
		return;

	if(!pane2entry_[pane].isCutLoose_)
		return;

	stackWidget_->addWidget(pane);
	pane2entry_[pane].isCutLoose_ = false;
}

void AMMainWindow::onSidebarLinkClicked(const QVariant& linkContent) {

	QWidget* pane = linkContent.value<QWidget*>();
	if(!pane2entry_.contains(pane))
		return;

	// already done
	if(pane == stackWidget_->currentWidget())
		return;



	// If it's floating free, need to grab it back
	if(pane2entry_[pane].isCutLoose_) {
		dock(pane);
	}


	previousPane_ = stackWidget_->currentWidget();
	stackWidget_->setCurrentWidget(pane);

}

void AMMainWindow::onSidebarLinkDoubleClicked(const QVariant& linkContent) {
	QWidget* pane = linkContent.value<QWidget*>();

	undock(pane);
}

/// We intercept and forward the currentChanged(int) signal from the QStackedWidget, to keep the sidebar's highlighted link consistent with the current widget.
void AMMainWindow::onFwdCurrentWidgetChanged(int currentIndex) {

	stackWidget_->previousInFocusChain();

	if(currentIndex < 0)
		return;

	// slower: sidebar_->setHighlightedLink(QVariant::fromValue(stackWidget_->widget(currentIndex)));
	sidebar_->setHighlightedLink(pane2entry_[stackWidget_->widget(currentIndex)].sidebarWidget_);
}


/// if an undocked widget is about to be closed, we need to intercept that event and capture (dock) it back to the main window insead.
bool AMMainWindow::eventFilter(QObject* sourceObject, QEvent* event) {

	QWidget* pane = qobject_cast<QWidget*>(sourceObject);

	if(!pane2entry_.contains(pane))
		return QWidget::eventFilter(sourceObject, event);

	if(pane2entry_[pane].isCutLoose_ && event->type() == QEvent::Close) {
		dock(pane);
		return true;
	}

	return QWidget::eventFilter(sourceObject, event);
}
