/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#ifndef AMMAINWINDOW_H
#define AMMAINWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QGroupBox>

#include <QStackedWidget>

#include <QTreeView>
#include "ui/AMWindowPaneModel.h"
#include "ui/AMWindowPaneProxyModel.h"

#include <QQueue>

#include "AMQEvents.h"

#include <QApplication>
#include "ui/AMTopFrame.h"
#include "ui/acquaman/AMScanConfigurationViewHolder3.h"
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

	/// Insert a custom widget into the vertical layout at \param index.
	void insertVerticalWidget(int index, QWidget *verticalWidget)
	{
		vlayout_->insertWidget(index, verticalWidget);
	}

	/// Add a custom widget below the sidebar and main area
	void addBottomWidget(QWidget* bottomWidget) {
		vlayout_->addWidget(bottomWidget);
	}
	/// Add a custom widget above the sidebar and main area
	void addTopWidget(QWidget* topWidget) {
		vlayout_->insertWidget(0, topWidget);
	}
	/// Add a custom widget beside the main window area on the right (ie: like the sidebar, but on the other side)
	void addRightWidget(QWidget* rightWidget) {
		hlayout_->addWidget(rightWidget);
	}

	/// Add a new \c pane to manage.  It will show up in the sidebar under category \c categoryName, with a \c title and an icon from \c iconFileName.  The \c resizeOnUndock is used to tell the manager to resize the pane to its sizeHint when it undocks. Returns a pointer to the newly-created window pane item in the model.
	/*! \note Don't use this to add the same pane more than once. If you want to add an alias entry to an existing pane, use the model() directly. You can call AMWindowPaneModel::initAliasItem() to setup an item suitable for adding to the model as an alias. */
	QStandardItem* addPane(QWidget* pane, const QString& categoryName, const QString& title, const QString& iconFileName, bool resizeOnUndock = false, bool visible = true);


	/// Remove and delete a pane widget (whether docked or undocked)
	void deletePane(QWidget* pane);

	/// Remove a pane widget but do not delete it.  Ownership is now the responsibility of the caller. The pane becomes a top-level window.
	void removePane(QWidget* pane);

	/// Shows a pane.
	void showPane(QWidget *pane);
	/// Hides a pane.
	void hidePane(QWidget *pane);


	/// Insert a new heading item at a given index.  This can be used in situations where you want a pane added using addPane() to appear at a given \c position (from top to bottom).  Call insertHeading() first, with the top-level position and name of the heading, and then call addPane() with the same heading name.
	/*! If \c position is less than 0 or > than the number of existing headings, the heading will be inserted at the bottom.  Returns the newly-created heading item.  */
	QStandardItem* insertHeading(const QString& title, int position);

	/// remove a heading with given title
	bool removeHeading(const QString& title);

	/// Full access to the window panes and their attributes can be retrieved through this model
	AMWindowPaneModel* windowPaneModel() { return model_; }

	/// Returns the pane that is currently selected in the sidebar of the main window. (Does not consider undocked windows.)  Returns 0 if none exist.
	QWidget* currentPane() const;

	/// Returns a widget pane that has been squished and fitted to be placed in a mainwindow
	static QWidget *buildMainWindowPane(const QString &name, const QString &iconPath, QWidget *appWidget);

	/// Returns a widget composed of the Top Frame and Configuration View
	static QWidget *buildMainWindowConfigurationPane(const QString &topFrameName, AMScanConfigurationView *configView, bool enableLoopAction = false, bool squeezeWidget = true);


public slots:
	/// move a pane from inside the main window to a separate window.
	void undock(QWidget* pane) {
		model_->undock(model_->indexForPane(pane));
	}

	/// return a \c pane that was undocked back to the main window.  Does not set this pane as the current widget.
	void dock(QWidget* pane) {
		model_->dock(model_->indexForPane(pane));
	}

	/// Set this pane as the current widget. If the pane is currently undocked, bring it to the front.  If it is docked, make it the currently-selected in the sidebar of the mainwindow.
	/*! \param pane Pointer to the widget that should be made current
	  */
	void setCurrentPane(QWidget* pane);
	/// Set this pane as the current widget. If the pane is currently undocked, bring it to the front.  If it is docked, make it the currently-selected in the sidebar of the mainwindow.
	/*! \param index The index of the pane's entry in our AMWindowPaneModel model.
   */
	void setCurrentIndex(const QModelIndex& index);

	/// Collapses the heading with the given name.
	void collapseHeading(const QString &name);
	/// Expands the heading with the given name.
	void expandHeading(const QString &name);
	/// Expands all headings.
	void expandAllHeadings();

	/// Collapses the heading at the given index.
	void collapseHeadingIndex(const QModelIndex &index);
	/// Expands the heading at the given index.
	void expandHeadingIndex(const QModelIndex &index);

signals:
	/// advertises when a new widget was selected as the current widget. \note This is only emitted for widgets that are docked within the main window itself. It is not emitted when undocked widgets become current or raised by the user.
	void currentPaneChanged(QWidget* pane);

	/// advertises when an "alias" item was selected as the current widget.  "Aliases" are items in the model that don't have their own unique window pane widgets, but instead link to another existing widget. When activated, they can deliver a message (in the form of a \c key and \c value pair) for that widget.
	void aliasItemActivated(QWidget* target, const QString& key, const QVariant& value);

	/// Emitted when an item in the window pane model has its "Close" button clicked. The model index \c index corresponds to the item in the model. What to do in this situation is up to you. AMMainWindow doesn't take any action here.
	void itemCloseButtonClicked(const QModelIndex& index);

	/// Emitted when can item in the window pane model is right clicked
	void itemRightClicked(const QModelIndex &index, const QPoint &globalPosition);

protected slots:
	// Responding to the sidebar QTreeView selections
	//////////////////////////////////////////////////
	/// Called when a link in the sidebar is clicked.  This should set the linked pane as the current widget, re-docking it if necessary.
	void onSidebarItemSelectionChanged();


	/// Called when a link in the sidebar is double-clicked. This should undock the linked pane and set it up as a new window.
	void onSidebarItemDoubleClicked(const QModelIndex& index);


	// Responding to model events (widgets added, widgets removed, dock state changed...)
	//////////////////////////////////////////////////
	/// Catches when new items are added to the model, and processes the new widgets
	void onModelRowsInserted(const QModelIndex& parent, int start, int end);

	/// Catches when items are about to be removed from the model, and removes the corresponding widgets
	void onModelRowsAboutToBeRemoved(const QModelIndex& parent, int start, int end);

	/// Catches when the dock state of a widget changes in the model, and handles the actual docking/undocking of it. Also handles resizing if necessary.
	void onDockStateChanged(QWidget* pane, bool isDocked, bool shouldResize);

	/// Catches when the visibility of a pane widget changes in the model, and handles changing the current widget if appropriate.
	void onVisibilityChanged(QWidget *pane, bool isVisible);

	/// Catches when an item has been 'closed' by clicking on it's 'X' in the sidebar
	void onItemCloseButtonClicked(const QModelIndex& index);

	/// Catches when an item has been 'right clicked'
	void onItemRightClickDetected(const QModelIndex& index, const QPoint &globalPosition);

protected:
	QStackedWidget* stackWidget_;

	QTreeView* sidebar_;

	QVBoxLayout* vlayout_;
	QHBoxLayout* hlayout_;

	// addition for usability: when undocking or removing the current widget, we can use this to go back to the user's previous one. (Includes only docked selections)
	QQueue<QPersistentModelIndex> previousSelections_;

	/// The window pane model.
	AMWindowPaneModel* model_;
	/// The window pane proxy model. Allows for panes to be filtered.
	AMWindowPaneProxyModel *proxyModel_;


	void removeFromPreviousSelectionsQueue(const QModelIndex& removed);
	void addToPreviousSelectionsQueue(const QModelIndex& current);
	/// returns previous index that should be switched to when removing or undocking \c current.
	QModelIndex getPreviousSelection(const QModelIndex& current);
};

#endif // AMMAINWINDOW_H
