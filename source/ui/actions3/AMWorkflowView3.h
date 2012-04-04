#ifndef AMWORKFLOWVIEW3_H
#define AMWORKFLOWVIEW3_H

#include <QWidget>
#include "ui/actions3/AMActionRunnerQueueView3.h"
#include "ui/actions3/AMActionRunnerCurrentView3.h"
#include "ui/actions3/AMActionHistoryView3.h"
#include "ui/actions3/AMAddActionDialog3.h"

class QSpacerItem;
class QPushButton;

/// This widget provides an overall view of a user's workflow: an AMActionRunnerQueueView to manage the upcoming actions, an AMActionRunnerCurrentView to manage the current action, and an AMActionHistoryView for the completed actions. Using this widget, users can duplicate or delete upcoming actions, pause or resume the workflow queue, observe and cancel the current action, and review/re-queue past actions.
class AMWorkflowView3 : public QWidget
{
    Q_OBJECT
public:
	/// Constructor
    explicit AMWorkflowView3(QWidget *parent = 0);

	/// Access the queue view for upcoming actions
    AMActionRunnerQueueView3* queueView() { return queueView_; }

	/// Access the current view for the current action
    AMActionRunnerCurrentView3* currentView() { return currentView_; }

	/// Access the history view of completed actions
    AMActionHistoryView3* historyView() { return historyView_; }

signals:

public slots:

protected slots:
	/// When one of the historyView() or queueView() is collapsed (or expanded) we check to ensure that if they are both collapsed, we should add a spacer to our layout to keep it from breaking (since there won't be any expanding widgets in the layout.)
	void onViewCollapsed();

	/// When the "Add Action" button is clicked, we open up an AMAddActionDialog (if we don't have one yet).
	void onAddActionButtonClicked();

protected:
    AMActionRunnerQueueView3* queueView_;
    AMActionRunnerCurrentView3* currentView_;
    AMActionHistoryView3* historyView_;

	QSpacerItem* layoutSpacer_;

	QPushButton* addActionButton_;
    AMAddActionDialog3* addActionDialog_;
};

#endif // AMWORKFLOWVIEW3_H
