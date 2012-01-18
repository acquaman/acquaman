#ifndef AMWORKFLOWVIEW_H
#define AMWORKFLOWVIEW_H

#include <QWidget>
#include "ui/actions2/AMActionRunnerQueueView.h"
#include "ui/actions2/AMActionRunnerCurrentView.h"
#include "ui/actions2/AMActionHistoryView.h"

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

protected:
	AMActionRunnerQueueView* queueView_;
	AMActionRunnerCurrentView* currentView_;
	AMActionHistoryView* historyView_;
};

#endif // AMWORKFLOWVIEW_H
