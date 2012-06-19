/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef AMWORKFLOWVIEW_H
#define AMWORKFLOWVIEW_H

#include <QWidget>
#include "ui/actions2/AMActionRunnerQueueView.h"
#include "ui/actions2/AMActionRunnerCurrentView.h"
#include "ui/actions2/AMActionHistoryView.h"
#include "ui/actions2/AMAddActionDialog.h"

class QSpacerItem;
class QPushButton;

/// This widget provides an overall view of a user's workflow: an AMActionRunnerQueueView to manage the upcoming actions, an AMActionRunnerCurrentView to manage the current action, and an AMActionHistoryView for the completed actions. Using this widget, users can duplicate or delete upcoming actions, pause or resume the workflow queue, observe and cancel the current action, and review/re-queue past actions.
class AMWorkflowView : public QWidget
{
    Q_OBJECT
public:
	/// Constructor
    explicit AMWorkflowView(QWidget *parent = 0);

	/// Access the queue view for upcoming actions
	AMActionRunnerQueueView* queueView() { return queueView_; }

	/// Access the current view for the current action
	AMActionRunnerCurrentView* currentView() { return currentView_; }

	/// Access the history view of completed actions
	AMActionHistoryView* historyView() { return historyView_; }

signals:

public slots:

protected slots:
	/// When one of the historyView() or queueView() is collapsed (or expanded) we check to ensure that if they are both collapsed, we should add a spacer to our layout to keep it from breaking (since there won't be any expanding widgets in the layout.)
	void onViewCollapsed();

	/// When the "Add Action" button is clicked, we open up an AMAddActionDialog (if we don't have one yet).
	void onAddActionButtonClicked();

protected:
	AMActionRunnerQueueView* queueView_;
	AMActionRunnerCurrentView* currentView_;
	AMActionHistoryView* historyView_;

	QSpacerItem* layoutSpacer_;

	QPushButton* addActionButton_;
	AMAddActionDialog* addActionDialog_;
};

#endif // AMWORKFLOWVIEW_H
