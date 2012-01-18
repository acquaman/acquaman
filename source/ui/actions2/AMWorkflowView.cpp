#include "AMWorkflowView.h"
#include "actions2/AMActionRunner.h"

#include <QBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QPixmap>

AMWorkflowView::AMWorkflowView(QWidget *parent) :
    QWidget(parent)
{
	QHBoxLayout* hl = new QHBoxLayout();
	hl->setContentsMargins(12,12,12,12);
	hl->setSpacing(12);

	QLabel* titleIcon = new QLabel();
	titleIcon->setMaximumSize(36,36);
	titleIcon->setPixmap(QPixmap(":/user-away.png"));
	titleIcon->setScaledContents(true);
	hl->addWidget(titleIcon);
	QLabel* titleLabel = new QLabel("Workflow");
	titleLabel->setStyleSheet("font: 20pt \"Lucida Grande\";\ncolor: rgb(79, 79, 79);");
	hl->addWidget(titleLabel);
	hl->addStretch(1);

	currentView_ = new AMActionRunnerCurrentView(AMActionRunner::s());
	queueView_ = new AMActionRunnerQueueView(AMActionRunner::s());
	historyView_ = new AMActionHistoryView(AMActionRunner::s());

	QVBoxLayout* vl = new QVBoxLayout(this);
	vl->setContentsMargins(0,0,0,0);
	vl->setSpacing(0);

	vl->addLayout(hl);
	vl->addWidget(historyView_);
	vl->addSpacing(12);
	vl->addWidget(currentView_);
	vl->addSpacing(12);
	vl->addWidget(queueView_);
}


#include <QCloseEvent>
#include <QDebug>
#include <QMessageBox>
#include "actions2/AMAction.h"

void AMWorkflowView::closeEvent(QCloseEvent *e)
{
	AMActionRunner::s()->setQueuePaused(true);

	qDebug() << "CLOSE EVENT";

	if(AMActionRunner::s()->actionRunning()) {
		e->setAccepted(false);
		qDebug() << "Attempting to block close";

		if(QMessageBox::question(this,
							  "An action is still running. Are you sure you want to quit?",
							  QString("There is an action (%1) still running.  Do you want to stop it?").arg(AMActionRunner::s()->currentAction()->info()->shortDescription()),
							  QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes) {
			AMAction* action = AMActionRunner::s()->currentAction();
			if(action)
				action->cancel();
			// problem: we don't know how long this will take to cancel, so we can't continue with the quit process. They'll just have to try quitting again, and the action will probably be done by then.
		}
	}

	QWidget::closeEvent(e);
}
