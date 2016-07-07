#include "AMActionRunnerCurrentViewBase.h"

#include "util/AMFontSizes.h"
#include "util/AMErrorMonitor.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QTimer>
#include <QMenu>
#include <QMessageBox>

AMActionRunnerCurrentViewBase::AMActionRunnerCurrentViewBase(AMActionRunner3 *actionRunner, QWidget *parent)
	: QWidget(parent)
{
	actionRunner_ = actionRunner;
	whatIsRunning_ = "";
	showCancelPrompt_ = true;

	pauseButton_ = new QToolButton;
	skipButton_ = new QToolButton;
	cancelButton_ = new QToolButton;
	timeElapsedLabel_ = new QLabel("-:--");
	timeRemainingLabel_ = new QLabel("-:--");
	progressBar_ = new QProgressBar();

	connect(cancelButton_, SIGNAL(clicked()), this, SLOT(onCancelButtonClicked()));
	connect(pauseButton_, SIGNAL(clicked()), this, SLOT(onPauseButtonClicked()));
	connect(skipButton_, SIGNAL(clicked()), this, SLOT(onSkipButtonClicked()));

	connect(actionRunner_, SIGNAL(actionRunnerPausableChanged(bool)), this, SLOT(onActionRunnerPausableChanged(bool)));
	connect(actionRunner_, SIGNAL(currentActionChanged(AMAction3*)), this, SLOT(onCurrentActionChanged(AMAction3*)));
	connect(actionRunner_, SIGNAL(currentActionStatusTextChanged(QString)), this, SLOT(onStatusTextChanged(QString)));
	connect(actionRunner_, SIGNAL(currentActionExpectedDurationChanged(double)), this, SLOT(onExpectedDurationChanged(double)));
	connect(actionRunner_, SIGNAL(currentActionProgressChanged(double,double)), this, SLOT(onProgressChanged(double,double)));
	connect(actionRunner_, SIGNAL(currentActionStateChanged(int,int)), this, SLOT(onStateChanged(int,int)));

	QTimer* timeUpdateTimer = new QTimer(this);
	connect(timeUpdateTimer, SIGNAL(timeout()), this, SLOT(onTimeUpdateTimer()));
	timeUpdateTimer->start(1000);
}

void AMActionRunnerCurrentViewBase::onCurrentActionChanged(AMAction3 *action)
{
	cancelButton_->setDisabled((action == 0));
	skipButton_->setDisabled(true);

	if(action && action->state() == AMAction3::Paused) {
		pauseButton_->setIcon(QIcon(":/22x22/media-playback-start.png"));
		pauseButton_->setText("Resume");
		pauseButton_->setToolTip("Resume the current action");
	}
	else {
		pauseButton_->setIcon(QIcon(":/22x22/media-playback-pause.png"));
		pauseButton_->setText("Pause");
		pauseButton_->setToolTip("Pause the current action");
	}

	if (action){

		skipButton_->setEnabled(action->canSkip());

		if (action->canSkip() && action->skipOptions().size() == 1)
			skipButton_->setToolTip(action->skipOptions().first());
		else
			skipButton_->setToolTip("Finish: Click for options");
	}

	if (action)
		updateDescriptionViews(action);

	else {

		whatIsRunning_ = "No action running.";
		timeElapsedLabel_->setText("-:--");
		timeRemainingLabel_->setText("-:--");
		progressBar_->setRange(0,1);
		progressBar_->setValue(0);
	}
}

void AMActionRunnerCurrentViewBase::onPauseButtonClicked()
{
	if (!actionRunner_->isActionRunnerPausable()) {
		return;
	}

	AMAction3* currentAction = actionRunner_->currentAction();
	if(!currentAction)
		return;

	if(currentAction->state() == AMAction3::Paused) {
		currentAction->resume();
		return;
	}

	if(currentAction->state() == AMAction3::Running && currentAction->pause())
		;	// successfully paused, do nothing.
	else
		QMessageBox::warning(this, "This action can't be paused", QString("This '%1' action cannot be paused right now.\n\n(Some actions just can't be paused, and others can't be paused at certain points in time.)").arg(currentAction->info()->typeDescription()), QMessageBox::Ok);
}

void AMActionRunnerCurrentViewBase::onSkipButtonClicked()
{
	AMAction3 *currentAction = actionRunner_->currentAction();

	if (!currentAction)
		return;

	if (currentAction->canSkip()){

		if (currentAction->skipOptions().size() == 1 && !currentAction->hasChildren())
			currentAction->skip(currentAction->skipOptions().first());

		else if (!currentAction->hasChildren()){

			QMenu menu(this);

			for (int i = 0, size = currentAction->skipOptions().size(); i < size; i++)
				menu.addAction(currentAction->skipOptions().at(i));

			QAction *action = menu.exec(QCursor::pos());

			if (action)
				currentAction->skip(action->text());
		}

		else{

			QMenu menu(this);

			for (int i = 0, size = currentAction->skipOptions().size(); i < size; i++)
				menu.addAction(currentAction->skipOptions().at(i));

			AMListAction3 *listAction = qobject_cast<AMListAction3 *>(currentAction);

			if (listAction && listAction->currentSubAction()->canSkip())
				for (int i = 0, size = listAction->currentSubAction()->skipOptions().size(); i < size; i++)
					menu.addAction(listAction->currentSubAction()->skipOptions().at(i));

			QAction *action = menu.exec(QCursor::pos());

			if (action){

				if (currentAction->skipOptions().contains(action->text()))
					currentAction->skip(action->text());

				else
					listAction->currentSubAction()->skip(action->text());
			}
		}
	}
}

void AMActionRunnerCurrentViewBase::onCancelButtonClicked()
{
	AMScanAction *scanAction = qobject_cast<AMScanAction *>(actionRunner_->currentAction());
	AMListAction3 *listAction = qobject_cast<AMListAction3 *>(actionRunner_->currentAction());
	bool scanActionRunning = (scanAction != 0) || (listAction != 0 && (qobject_cast<AMScanAction *>(listAction->currentSubAction()) != 0));

	if (scanActionRunning && showCancelPrompt_){

		AMCancelActionPrompt cancelPrompt;
		cancelPrompt.setWindowTitle("Cancel Scan");
		cancelPrompt.setText("Are you sure you wish to cancel this scan?  If you wish to have your data auto-exported: do not cancel and use <img src=:/media-seek-forward.png width=25 height=25> skip instead.  Be warned that cancelling the scan stops the workflow as well.");

		cancelPrompt.exec();

		showCancelPrompt_ = cancelPrompt.shouldWarn();

		if (cancelPrompt.result() == QDialog::Accepted)
			actionRunner_->cancelCurrentAction();
	}

	else {

		// if no prompt, then just cancel.
		actionRunner_->cancelCurrentAction();
	}
}

void AMActionRunnerCurrentViewBase::onActionRunnerPausableChanged(bool pausable)
{
	pauseButton_->setEnabled(pausable);
	pauseButton_->repaint(); // force repaint;
}

void AMActionRunnerCurrentViewBase::onExpectedDurationChanged(double totalSeconds)
{
	AMAction3 *currentAction = actionRunner_->currentAction();
	double elapsed = 0;

	if (qobject_cast<AMListAction3 *>(currentAction) && ((AMListAction3 *)currentAction)->currentSubAction())
		elapsed = ((AMListAction3 *)currentAction)->currentSubAction()->runningTime();
	else
		elapsed = currentAction->runningTime();

	if(totalSeconds > 0)
		timeRemainingLabel_->setText(formatSeconds(totalSeconds-elapsed));
	else
		timeRemainingLabel_->setText("N/A");
}

void AMActionRunnerCurrentViewBase::onProgressChanged(double numerator, double denominator)
{
	progressBar_->setRange(0, int(denominator*100));
	progressBar_->setValue(int(numerator*100));
}

void AMActionRunnerCurrentViewBase::onStateChanged(int state, int previousState)
{
	if(state == AMAction3::Paused) {
		pauseButton_->setText("Resume");
		pauseButton_->setIcon(QIcon(":/22x22/media-playback-start.png"));
		pauseButton_->setToolTip("Resume the current action.");
	}

	if(previousState == AMAction3::Resuming) {
		pauseButton_->setText("Pause");
		pauseButton_->setIcon(QIcon(":/22x22/media-playback-pause.png"));
		pauseButton_->setToolTip("Pause the current action.");
	}

	skipButton_->setEnabled(state == AMAction3::Running);

	AMAction3 *action = actionRunner_->currentAction();

	if (action){

		if (action->canSkip() && state == AMAction3::Paused)
			skipButton_->setToolTip("Can not skip while paused");

		else if (action->canSkip() && action->skipOptions().size() == 1)
			skipButton_->setToolTip(action->skipOptions().first());

		else
			skipButton_->setToolTip("Finish: Click for options");
	}
}

void AMActionRunnerCurrentViewBase::onTimeUpdateTimer()
{
	AMAction3* currentAction = actionRunner_->currentAction();
	if(currentAction) {

		double elapsed = 0;

		if (qobject_cast<AMListAction3 *>(currentAction) && ((AMListAction3 *)currentAction)->currentSubAction())
			elapsed = ((AMListAction3 *)currentAction)->currentSubAction()->runningTime();
		else
			elapsed = currentAction->runningTime();
		double expectedDuration = currentAction->expectedDuration();
		timeRemainingLabel_->setText(expectedDuration > 0 ? formatSeconds(expectedDuration-elapsed) : "N/A");
		timeElapsedLabel_->setText(formatSeconds(elapsed));
	}
}

QString AMActionRunnerCurrentViewBase::formatSeconds(double seconds)
{
	if(seconds < 0)
		return QString("?:??");

	QString rv;

	if(seconds > 60*60*24) {
		QDateTime now = QDateTime::currentDateTime();
		QDateTime later = now.addSecs(qRound(seconds));

		rv.append(QString("%1 days, ").arg(now.daysTo(later)));
	}

	QTime t(0,0,0);
	t = t.addSecs(qRound(seconds));
	if(seconds > 60*60)
		rv.append(t.toString("h:m:ss"));
	else
		rv.append(t.toString("m:ss"));

	return rv;
}
