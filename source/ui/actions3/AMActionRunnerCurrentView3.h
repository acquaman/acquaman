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


#ifndef AMACTIONRUNNERCURRENTVIEW3_H
#define AMACTIONRUNNERCURRENTVIEW3_H

#include <QWidget>
#include <QStandardItem>

class AMActionRunner3;
class AMAction3;
class QTreeView;
class QToolButton;
class QLabel;
class QPushButton;
class QStandardItemModel;
class QProgressBar;

#define AMACTIONRUNNERCURRENTMODEL3_REQUESTED_CHILD_INDEX_WITH_INVALID_PARENT_ACTION 270901
#define AMACTIONRUNNERCURRENTMODEL3_SUBACTION_NOT_FOUND_AT_CHILD_INDEX_ROW 270902
#define AMACTIONRUNNERCURRENTMODEL3_NO_ACTION_AT_INDEX 270903
#define AMACTIONRUNNERCURRENTMODEL3_ACTION_NOT_FOUND_AS_CURRENT_ACTION 270904
#define AMACTIONRUNNERCURRENTMODEL3_ACTION_NOT_FOUND_IN_LIST_ACTION 270905


/// This QAbstractItemModel wraps the current action of AMActionRunner, and is used by AMActionRunnerCurrentView. It is hierarchical to display nested actions as they run.  You should never need to use this class directly.
class AMActionRunnerCurrentModel3 : public QAbstractItemModel {
	Q_OBJECT
public:
	/// Constructor.
 	virtual ~AMActionRunnerCurrentModel3();
	AMActionRunnerCurrentModel3(AMActionRunner3* actionRunner, QObject* parent = 0);

	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;

	QModelIndex parent(const QModelIndex &child) const;

	int rowCount(const QModelIndex &parent = QModelIndex()) const;

	int columnCount(const QModelIndex &parent = QModelIndex()) const;

	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;

	bool hasChildren(const QModelIndex &parent = QModelIndex()) const;

	AMAction3* actionAtIndex(const QModelIndex& index) const;
	// requires a linear search of the \c action's parentAction(), or of the actionRunner queue if the action is at the top level.
	QModelIndex indexForAction(AMAction3* action) const;

protected slots:
	void onCurrentActionChanged(AMAction3* currentAction);

protected:
	AMActionRunner3* actionRunner_;
	AMAction3* currentAction_;
};

#include <QStyledItemDelegate>
/// This delegate is used by the tree view in AMActionRunnerCurrentView to show custom editor widgets depending on the action. The available editors depend on those that have been registered with AMActionRegistry.  You should never need to use this class directly.
class AMActionRunnerCurrentItemDelegate3 : public QStyledItemDelegate {
	Q_OBJECT
public:

 	virtual ~AMActionRunnerCurrentItemDelegate3();
	explicit AMActionRunnerCurrentItemDelegate3(QObject* parent = 0) : QStyledItemDelegate(parent) {}

	virtual QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

	//    virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;

	virtual bool eventFilter(QObject *object, QEvent *event);

};

// Previously:
//
///// This subclass of QStandardItem is used inside AMActionRunnerCurrentView's QTreeView to display actions.  You should never need to use this class directly.
//class AMActionQueueModelItem : public QStandardItem {
//public:
//	/// Used to identify a specific type of item subclass
//	enum Type { AMActionQueueModelItemType = QStandardItem::UserType + 1049 };
//	/// Construct a model item for the given \c action.
//	AMActionQueueModelItem(AMAction* action);
//	/// The two things that separate us from QStandardItems: we have an associated action, and we return data based on that action
//	AMAction* action() { return action_; }
//	/// Action-specific data: we return the action's info's shortDescription() for Qt::DisplayRole, and cache and return pixmaps based on the info's iconFileName() for Qt::DecorationRole
//	virtual QVariant data(int role) const;
//	/// Used to identify a specific type of item subclass
//	virtual int type() const { return AMActionQueueModelItemType; }
//protected:
//	/// Pointer to our associated action
//	AMAction* action_;
//};

/// This class provides a view of the currently-running action in the AMActionRunner. It is part of the overall AMWorkflowView.
class AMActionRunnerCurrentView3 : public QWidget
{
	Q_OBJECT
public:
	/// Constructor. Should pass in AMActionRunner::s() for \c actionRunner
 	virtual ~AMActionRunnerCurrentView3();
	AMActionRunnerCurrentView3(AMActionRunner3* actionRunner, QWidget *parent = 0);

	/// Sets whether the cancel prompt should be shown.
	void setCancelPromptVisibility(bool showPrompt) { showCancelPrompt_ = showPrompt; }
	/// Returns whether the cancel prompt will be shown.
	bool showCancelPrompt() const { return showCancelPrompt_; }

signals:

public slots:

protected slots:
	/// When the current action in the AMActionRunner changes
	void onCurrentActionChanged(AMAction3*);

	/// When the pause button is clicked for the current action
	void onPauseButtonClicked();
	/// When the skip button is pressed, it handles telling the action to skip.
	void onSkipButtonClicked();
	/// Handles when the cancel button is clicked.  Prompts the user if this was actually what they wanted to do.
	void onCancelButtonClicked();

	// Signals from the current action, forwarded through the Action Runner
	/// When the action's status text changes
	void onStatusTextChanged(const QString& newStatus);
	/// When the action's expected duration changes
	void onExpectedDurationChanged(double totalSeconds);
	/// When the action's progress changes
	void onProgressChanged(double numerator, double denominator);
	/// When the action's state changes. We use this to change the pause button to "Resume" if the action is paused.
	void onStateChanged(int state, int previousState);

	/// Called on a one-second timer to update the time elapsed, time remaining
	void onTimeUpdateTimer();

protected:

	/// Helper function to format an amount of seconds into an hour-minute-seconds string.
	QString formatSeconds(double seconds);

	QTreeView *currentActionView_;
	//	QStandardItemModel* currentActionModel_;

	QPushButton *cancelButton_, *pauseButton_;
	QPushButton *skipButton_;
	QProgressBar *progressBar_;
	QLabel *timeElapsedLabel_, *timeRemainingLabel_, *headerTitle_, *headerSubTitle_;

	AMActionRunner3 *actionRunner_;

	/// This string holds a small message that states what is currently running.  This is used when someone might be interested on what is "hanging up" the current action, or if they are just curious where they are.
	QString whatIsRunning_;
	/// The flag for determinining whether the cancel prompt should be displayed.
	bool showCancelPrompt_;
};

#endif // AMACTIONRUNNERCURRENTVIEW3_H
