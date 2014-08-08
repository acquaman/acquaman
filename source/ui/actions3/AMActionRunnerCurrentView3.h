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

#include "ui/actions3/AMActionRunnerCurrentViewBase.h"

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
	AMActionRunnerCurrentModel3(AMActionRunner3* actionRunner, QObject* parent = 0);
	virtual ~AMActionRunnerCurrentModel3();

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
class AMActionRunnerCurrentItemDelegate3 : public QStyledItemDelegate
{
	Q_OBJECT

public:
	/// Constructor.  Builds a custom delegate.
	explicit AMActionRunnerCurrentItemDelegate3(QObject* parent = 0) : QStyledItemDelegate(parent) {}
	/// Destructor.
	virtual ~AMActionRunnerCurrentItemDelegate3();
	/// Returns a QWidget pointer for when you need to customize the action.
	virtual QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	/// An event filter to handle and ignore specific events.
	virtual bool eventFilter(QObject *object, QEvent *event);

};

/// This class provides a view of the currently-running action in the AMActionRunner. It is part of the overall AMWorkflowView.
class AMActionRunnerCurrentView3 : public AMActionRunnerCurrentViewBase
{
	Q_OBJECT

public:
	/// Constructor. Should pass in AMActionRunner::s() for \c actionRunner
	AMActionRunnerCurrentView3(AMActionRunner3* actionRunner, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMActionRunnerCurrentView3();

signals:

public slots:

protected slots:
	/// When the current action in the AMActionRunner changes
	void onCurrentActionChanged(AMAction3 *action);
	/// When the action's status text changes
	void onStatusTextChanged(const QString& newStatus);
	/// Handles updating whatever description labels child views require.
	virtual void updateDescriptionViews(AMAction3 *action);

protected:
	/// The tree view.
	QTreeView *currentActionView_;

	/// The header label.
	QLabel *headerTitle_;
	/// The header sub label.
	QLabel *headerSubTitle_;
};

#endif // AMACTIONRUNNERCURRENTVIEW3_H
