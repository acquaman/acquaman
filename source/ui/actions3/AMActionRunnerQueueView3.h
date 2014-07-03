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


#ifndef AMACTIONRUNNERQUEUEVIEW3_H
#define AMACTIONRUNNERQUEUEVIEW3_H

#include <QWidget>

class AMActionRunner3;
class QTreeView;
class QToolButton;
class QLabel;
class QPushButton;

#include <QStyledItemDelegate>
/// This delegate is used by the tree view in AMActionRunnerQueueView to show custom editor widgets depending on the action. The available editors depend on those that have been registered with AMActionRegistry.  You should never need to use this class directly.
class AMActionRunnerQueueItemDelegate3 : public QStyledItemDelegate {
	Q_OBJECT
public:

 	virtual ~AMActionRunnerQueueItemDelegate3();
	explicit AMActionRunnerQueueItemDelegate3(QObject* parent = 0) : QStyledItemDelegate(parent) {}

	virtual QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

	//    virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;

	virtual bool eventFilter(QObject *object, QEvent *event);

};

/// This UI class provides a view of the upcoming (queued) actions in an AMActionRunner. It is part of the overall AMWorkflowView.
class AMActionRunnerQueueView3 : public QWidget
{
	Q_OBJECT
public:
	/// Constructor. Should pass in the instance of AMActionRunner::s() for \c actionRunner.
 	virtual ~AMActionRunnerQueueView3();
	AMActionRunnerQueueView3(AMActionRunner3* actionRunner, QWidget *parent = 0);

	/// Returns true if the view is currently collapsed to show only the header bar, and false if it is fully shown.
	bool isCollapsed() const { return isCollapsed_; }

signals:
	/// This signal is emitted with \c true when the view widget is collapsed to show only the header bar, and \c false when it is restored.
	void collapsed(bool isCollapsed);

public slots:
	/// Collapses the view to show only the header bar.
	void collapse(bool isCollapsed);

protected slots:
	/// Called when the "Pause Queue" button is clicked or un-clicked
	void onPauseButtonClicked(bool isPaused);
	/// Called when actions are added or removed from the queue. We use this to update our subtitle text.
	void onQueueActionAddedOrRemoved();
	/// Called when the delete button is pushed to delete an action from the queue
	void onDeleteButtonClicked();
	/// Called when the duplicate button is pushed to duplicate one or more actions in the queue
	void onDuplicateButtonClicked();

	/// Called when the selected queue actions change. We use it to enable or disable the delete and duplicate buttons.
	void onSelectionChanged();
	/// Called when the queue notifies us that it's been paused or unpaused. We use it to set the state of the pause button
	void onQueuePaused(bool isPaused);

protected:
	/// Helper function to determine if all the selected indexes are at the same level of the hierarchy. We need this to be true in order to enable the 'duplicate actions' button.  Then a user can either duplicate top-level actions, or duplicate actions within an AMNestedAction, but there's no ambiguity as to what we would do for a combination of both.
	bool allSelectedIndexesAtSameLevel(const QModelIndexList& selectedIndexes);
	/// Helper function to determine everything about the state of the pause button.
	void pauseButtonConfiguration();

	AMActionRunner3* actionRunner_;
	QTreeView* treeView_;
	QLabel* headerTitle_, *headerSubTitle_;

	QToolButton* hideButton_;
	QPushButton* pauseButton_;

	QPushButton* deleteButton_, *duplicateButton_;

	bool isCollapsed_;
};


#endif // AMACTIONRUNNERQUEUEVIEW_H
