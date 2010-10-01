#ifndef AMMAINWINDOW_H
#define AMMAINWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>

#include <QStackedWidget>

#include <QTreeView>
#include "ui/AMWindowPaneModel.h"



/// This UI class manages a set of "window panes", which can either be docked and selected using an iTunes-style sidebar, or un-docked to float as independent windows. When an un-docked pane is clicked in the sidebar or closed, it is re-docked in the main window. Finally, a layout is available to add custom widgets above and below the main area.
/*! Internally, this class is implemented using an an AMWindowPaneModel. Both the sidebar, and the main window area, simply act as "views" on this model.  For full access to the underlying model, call windowPaneModel().*/
class AMMainWindow : public QWidget
{
Q_OBJECT
public:
	/// Default constructor
	explicit AMMainWindow(QWidget *parent = 0);

	/// AMMainWindow takes ownership for all of its pane widgets (whether docked or undocked), and deletes them in the destructor.
	virtual ~AMMainWindow();

	/// Add a custom widget below the sidebar and main area
	void addBottomWidget(QWidget* bottomWidget) {
		vlayout_->addWidget(bottomWidget);
	}
	/// Add a custom widget above the sidebar and main area
	void addTopWidget(QWidget* topWidget) {
		vlayout_->insertWidget(0, topWidget);
	}

	/// Add a new \c pane to manage.  It will show up in the sidebar under category \c categoryName, with a \c title and an icon from \c iconFileName.  Returns a pointer to the newly-created window pane item in the model.
	/*! \note Don't use this to add the same pane more than once. If you want to add an alias entry to an existing pane, use the model() directly. You can call AMWindowPaneModel::initAliasItem() to setup an item suitable for adding to the model as an alias. */
	QStandardItem* addPane(QWidget* pane, const QString& categoryName, const QString& title, const QString& iconFileName);


	/// Remove and delete a pane widget (whether docked or undocked)
	void deletePane(QWidget* pane);

	/// Remove a pane widget but do not delete it.  Ownership is now the responsibility of the caller. The pane becomes a top-level window.
	void removePane(QWidget* pane);


	/// Full access to the window panes and their attributes can be retrieved through this model
	AMWindowPaneModel* windowPaneModel() { return model_; }

	/// Access to the sidebar, which is a QTreeView containing links to the available window panes
	QTreeView* sidebar() const { return sidebar_; }


public slots:
	/// move a pane from inside the main window to a separate window.
	void undock(QWidget* pane) {
		model_->undock(model_->indexForPane(pane));
	}

	/// return a \c pane that was undocked back to the main window.  Does not set this pane as the current widget.
	void dock(QWidget* pane) {
		model_->dock(model_->indexForPane(pane));
	}

	/// Set this pane as the current widget. If the pane is currently undocked, bring it to the front.
	void goToPane(QWidget* pane);

signals:
	/// advertises when a new widget was selected as the current widget
	void currentPaneChanged(QWidget* pane);

	/// advertises when an "alias" item was selected as the current widget.  "Aliases" are items in the model that don't have their own unique window pane widgets, but instead link to another existing widget. When activated, they can deliver a message (in the form of a \c key and \c value pair) for that widget.
	void aliasItemActivated(QWidget* target, const QString& key, const QVariant& value);

	/// Emitted when an item in the window pane model has its "Close" button clicked. The model index \c index corresponds to the item in the model. What to do in this situation is up to you; AMMainWindow doesn't take any action here.
	void itemCloseButtonClicked(const QModelIndex& index);

protected slots:
	// Responding to the sidebar QTreeView selections
	//////////////////////////////////////////////////
	/// Called when a link in the sidebar is clicked.  This should set the linked pane as the current widget, re-capturing it if necessary.
	void onSidebarItemActivated(const QModelIndex& index, const QModelIndex& oldIndex);


	/// Called when a link in the sidebar is double-clicked. This should undock the linked pane and set it up as a new window.
	void onSidebarItemDoubleClicked(const QModelIndex& index);

	/// We intercept and forward the currentChanged(int) signal from the QStackedWidget, to keep the sidebar's highlighted link consistent with the current widget.
	void onFwdCurrentWidgetChanged(int currentIndex);

	// Responding to model events (widgets added, widgets removed, dock state changed...)
	//////////////////////////////////////////////////
	/// Catches when new items are added to the model, and processes the new widgets
	void onModelRowsInserted(const QModelIndex& parent, int start, int end);

	/// Catches when items are about to be removed from the model, and removes the corresponding widgets
	void onModelRowsAboutToBeRemoved(const QModelIndex& parent, int start, int end);

	/// Catches when the dock state of a widget changes in the model, and handles the actual docking/undocking of it
	void onDockStateChanged(QWidget* pane, bool isDocked);

	/// Catches when an item has been 'closed' by clicking on it's 'X' in the sidebar
	void onItemCloseButtonClicked(const QModelIndex& index);

protected:
	QStackedWidget* stackWidget_;

	QTreeView* sidebar_;

	QVBoxLayout* vlayout_;

	QWidget* previousPane_;

	AMWindowPaneModel* model_;


};

#endif // AMMAINWINDOW_H
