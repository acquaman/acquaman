#include "AMActionRunnerBottomBarCurrentView3.h"

#include "util/AMFontSizes.h"
#include "util/AMErrorMonitor.h"

#include <QMessageBox>
#include <QFrame>
#include <QHBoxLayout>
#include <QTimer>
#include <QMenu>

AMActionRunnerBottomBarCurrentView3::AMActionRunnerBottomBarCurrentView3(AMActionRunner3 *actionRunner, QWidget *parent)
	: QWidget(parent)
{
	actionRunner_ = actionRunner;
	whatIsRunning_ = "";
	rootLoopAction_ = 0;

	QHBoxLayout* hl = new QHBoxLayout;
	hl->setSpacing(0);
	hl->setContentsMargins(6, 2, 12, 1);

	descriptionLabel_ = new QLabel;
	descriptionLabel_->setObjectName("descriptionLabel");
	descriptionLabel_->setStyleSheet("#descriptionLabel { border: 1px black; border-radius: 10px; background: rgb(219,219,112); }");
	descriptionLabel_->setText("No actions running.");
	descriptionLabel_->setMinimumWidth(200);
	descriptionLabel_->setAlignment(Qt::AlignCenter);
	QFont font = descriptionLabel_->font();
	font.setBold(true);
	font.setPointSize(AM_FONT_REGULAR);
	descriptionLabel_->setFont(font);
	hl->addWidget(descriptionLabel_);
	hl->addSpacing(20);

	increaseIterations_ = new QToolButton;
	increaseIterations_->setIcon(QIcon(":/22x22/go-up-dark.png"));
	increaseIterations_->setToolTip("Increase the number of iterations of the loop.");
	increaseIterations_->setFixedSize(25, 20);
	increaseIterations_->setEnabled(false);
	increaseIterations_->setVisible(false);
	decreaseIterations_ = new QToolButton;
	decreaseIterations_->setIcon(QIcon(":/22x22/go-down-dark.png"));
	decreaseIterations_->setToolTip("Decrease the number of iterations of the loop.");
	decreaseIterations_->setFixedSize(25, 20);
	decreaseIterations_->setEnabled(false);
	decreaseIterations_->setVisible(false);
	QVBoxLayout *iterationLayout = new QVBoxLayout;
	iterationLayout->addWidget(increaseIterations_);
	iterationLayout->addWidget(decreaseIterations_);
	hl->addLayout(iterationLayout);
	hl->addSpacing(5);

	pauseButton_ = new QToolButton;
	pauseButton_->setIcon(QIcon(":/22x22/media-playback-pause.png"));
	pauseButton_->setEnabled(false);
	pauseButton_->setToolTip("Pause the current action.");
	hl->addWidget(pauseButton_);
	hl->addSpacing(5);
	skipButton_ = new QToolButton;
	skipButton_->setIcon(QIcon(":/media-seek-forward.png"));
	skipButton_->setEnabled(false);
	hl->addWidget(skipButton_);
	hl->addSpacing(5);
	cancelButton_ = new QToolButton;
	cancelButton_->setIcon(QIcon(":/22x22/list-remove-2.png"));
	cancelButton_->setEnabled(false);
	cancelButton_->setToolTip("Cancel the current action.");
	hl->addWidget(cancelButton_);
	hl->addSpacing(10);

	timeElapsedLabel_ = new QLabel("-:--");
	timeElapsedLabel_->setStyleSheet("border: 0px; color: white;\nfont: " AM_FONT_REGULAR_ "pt \"Lucida Grande\"");
	hl->addWidget(timeElapsedLabel_);
	hl->addSpacing(10);
	progressBar_ = new QProgressBar();
	progressBar_->setObjectName("progressBar");
	progressBar_->setStyleSheet("#progressBar { border: 1px solid black; color: white; border-radius: 7px; background: gray; } QProgressBar::chunk { background: rgb(60,119,197); border-radius: 7px; border: 0px; }");
	progressBar_->setAlignment(Qt::AlignCenter);
	progressBar_->setMinimumWidth(400);
	progressBar_->setMaximumWidth(600);
	progressBar_->setRange(0,1);
	progressBar_->setValue(0);
	hl->addWidget(progressBar_);
	hl->addSpacing(10);
	timeRemainingLabel_ = new QLabel("-:--");
	timeRemainingLabel_->setStyleSheet("border: 0px; color: white;\nfont: " AM_FONT_REGULAR_ "pt \"Lucida Grande\"");
	hl->addWidget(timeRemainingLabel_);
	hl->addSpacing(20);

	setLayout(hl);

	connect(actionRunner_, SIGNAL(currentActionChanged(AMAction3*)), this, SLOT(onCurrentActionChanged(AMAction3*)));
	connect(cancelButton_, SIGNAL(clicked()), actionRunner_, SLOT(cancelCurrentAction()));
	connect(pauseButton_, SIGNAL(clicked()), this, SLOT(onPauseButtonClicked()));
	connect(skipButton_, SIGNAL(clicked()), this, SLOT(onSkipButtonClicked()));
	connect(increaseIterations_, SIGNAL(clicked()), this, SLOT(onIncreaseLoopIterationsClicked()));
	connect(decreaseIterations_, SIGNAL(clicked()), this, SLOT(onDecreaseLoopIterationsClicked()));

	connect(actionRunner_, SIGNAL(currentActionStatusTextChanged(QString)), this, SLOT(onStatusTextChanged(QString)));
	connect(actionRunner_, SIGNAL(currentActionExpectedDurationChanged(double)), this, SLOT(onExpectedDurationChanged(double)));
	connect(actionRunner_, SIGNAL(currentActionProgressChanged(double,double)), this, SLOT(onProgressChanged(double,double)));
	connect(actionRunner_, SIGNAL(currentActionStateChanged(int,int)), this, SLOT(onStateChanged(int,int)));

	QTimer* timeUpdateTimer = new QTimer(this);
	connect(timeUpdateTimer, SIGNAL(timeout()), this, SLOT(onTimeUpdateTimer()));
	timeUpdateTimer->start(1000);
}

void AMActionRunnerBottomBarCurrentView3::onCurrentActionChanged(AMAction3 *action)
{
	cancelButton_->setDisabled((action == 0));

	if (action)
		pauseButton_->setEnabled(action->canPause());

	if(action && action->state() == AMAction3::Paused) {
		pauseButton_->setIcon(QIcon(":/22x22/media-playback-start.png"));
		pauseButton_->setText("Resume");
	}
	else {
		pauseButton_->setIcon(QIcon(":/22x22/media-playback-pause.png"));
		pauseButton_->setText("Pause");
	}

	if (action){

		skipButton_->setEnabled(action->canSkip());

		if (action->canSkip() && action->skipOptions().size() == 1)
			skipButton_->setToolTip(action->skipOptions().first());
		else
			skipButton_->setToolTip("Finish: Click for options");
	}

	if (action && !action->parentAction() && qobject_cast<AMLoopAction3 *>(action)){

		rootLoopAction_ = qobject_cast<AMLoopAction3 *>(action);
		increaseIterations_->setEnabled(true);
		increaseIterations_->setVisible(true);
		decreaseIterations_->setEnabled(true);
		decreaseIterations_->setVisible(true);
		connect(rootLoopAction_, SIGNAL(currentIterationChanged(int)), this, SLOT(onLoopIterationUpdate(int)));
	}

	if (action){

		whatIsRunning_ = action->info()->name();

		if (rootLoopAction_)
			descriptionLabel_->setText(QString("%1\n[%2] %3 of %4").arg(whatIsRunning_).arg(action->statusText()).arg(rootLoopAction_->currentIteration()).arg(rootLoopAction_->loopCount()));
		else
			descriptionLabel_->setText(QString("%1\n[%2]").arg(whatIsRunning_).arg(action->statusText()));

		timeElapsedLabel_->setText("0:00");
		double expectedDuration = action->expectedDuration();
		timeRemainingLabel_->setText(expectedDuration > 0 ? formatSeconds(expectedDuration) : "?:??");
	}

	else{

		whatIsRunning_ = "No action running.";
		descriptionLabel_->setText(whatIsRunning_);
		timeElapsedLabel_->setText("-:--");
		timeRemainingLabel_->setText("-:--");
		progressBar_->setRange(0,1);
		progressBar_->setValue(0);
	}
}

void AMActionRunnerBottomBarCurrentView3::onPauseButtonClicked()
{
	AMAction3* currentAction = actionRunner_->currentAction();
	if(!currentAction)
		return;

	if(currentAction->state() == AMAction3::Paused) {
		currentAction->resume();
		return;
	}

	if(currentAction->state() == AMAction3::Running && currentAction->pause())
		;	// successfully paused; do nothing.
	else
		QMessageBox::warning(this, "This action can't be paused", QString("This '%1' action cannot be paused right now.\n\n(Some actions just can't be paused, and others can't be paused at certain points in time.)").arg(currentAction->info()->typeDescription()), QMessageBox::Ok);
}

void AMActionRunnerBottomBarCurrentView3::onSkipButtonClicked()
{
	AMAction3 *currentAction = actionRunner_->currentAction();

	if (!currentAction)
		return;

	if (currentAction->canSkip()){

		if (currentAction->skipOptions().size() == 1)
			currentAction->skip(currentAction->skipOptions().first());

		else{

			QMenu menu(this);

			for (int i = 0, size = currentAction->skipOptions().size(); i < size; i++)
				menu.addAction(currentAction->skipOptions().at(i));

			QAction *action = menu.exec(QCursor::pos());

			if (action)
				currentAction->skip(action->text());
		}
	}
}

void AMActionRunnerBottomBarCurrentView3::onStatusTextChanged(const QString &newStatus)
{
	if (rootLoopAction_)
		descriptionLabel_->setText(QString("%1\n[%2] %3 of %4").arg(whatIsRunning_).arg(newStatus).arg(rootLoopAction_->currentIteration()).arg(rootLoopAction_->loopCount()));
	else
		descriptionLabel_->setText(QString("%1\n[%2]").arg(whatIsRunning_).arg(newStatus));
}

void AMActionRunnerBottomBarCurrentView3::onExpectedDurationChanged(double totalSeconds)
{
	double elapsed = actionRunner_->currentAction()->elapsedTime();
	if(totalSeconds > 0)
		timeRemainingLabel_->setText(formatSeconds(totalSeconds-elapsed));
	else
		timeRemainingLabel_->setText("?:??");
}

void AMActionRunnerBottomBarCurrentView3::onProgressChanged(double numerator, double denominator)
{
	progressBar_->setRange(0, int(denominator*100));
	progressBar_->setValue(int(numerator*100));
}

void AMActionRunnerBottomBarCurrentView3::onStateChanged(int state, int previousState)
{
	if(state == AMAction3::Paused) {
		pauseButton_->setText("Resume");
		pauseButton_->setIcon(QIcon(":/22x22/media-playback-start.png"));
	}

	if(previousState == AMAction3::Resuming) {
		pauseButton_->setText("Pause");
		pauseButton_->setIcon(QIcon(":/22x22/media-playback-pause.png"));
	}

	if (rootLoopAction_ && rootLoopAction_->inFinalState()){

		disconnect(rootLoopAction_, SIGNAL(currentIterationChanged(int)), this, SLOT(onLoopIterationUpdate(int)));
		rootLoopAction_ = 0;
	}

	// Can pause or resume from only these states:
	if (actionRunner_->currentAction())
		pauseButton_->setEnabled(actionRunner_->currentAction()->canPause() && (state == AMAction3::Running || state == AMAction3::Paused));

	else
		pauseButton_->setEnabled(state == AMAction3::Running || state == AMAction3::Paused);
}

void AMActionRunnerBottomBarCurrentView3::onTimeUpdateTimer()
{
	AMAction3* currentAction = actionRunner_->currentAction();
	if(currentAction) {
		double elapsed = currentAction->runningTime();
		double expectedDuration = currentAction->expectedDuration();
		timeRemainingLabel_->setText(expectedDuration > 0 ? formatSeconds(expectedDuration-elapsed) : "?:??");
		timeElapsedLabel_->setText(formatSeconds(elapsed));
	}
}

QString AMActionRunnerBottomBarCurrentView3::formatSeconds(double seconds)
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

void AMActionRunnerBottomBarCurrentView3::onLoopIterationUpdate(int iteration)
{
	descriptionLabel_->setText(QString("%1\n[%2] %3 of %4").arg(whatIsRunning_).arg(actionRunner_->currentAction()->statusText()).arg(iteration).arg(rootLoopAction_->loopCount()));

	if (rootLoopAction_->currentIteration() == rootLoopAction_->loopCount())
		decreaseIterations_->setEnabled(false);
}

void AMActionRunnerBottomBarCurrentView3::onIncreaseLoopIterationsClicked()
{
	// No checks are done here because you should only be able to get here if there is already a valid root loop action.
	rootLoopAction_->setLoopCount(rootLoopAction_->loopCount()+1);
	descriptionLabel_->setText(QString("%1\n[%2] %3 of %4").arg(whatIsRunning_).arg(actionRunner_->currentAction()->statusText()).arg(rootLoopAction_->currentIteration()).arg(rootLoopAction_->loopCount()));
}

void AMActionRunnerBottomBarCurrentView3::onDecreaseLoopIterationsClicked()
{
	// No checks are done here because you should only be able to get here if there is already a valid root loop action.
	// Also, you should only be able to get here if decreasing the loop iterations is valid.  So extra checks are not necessary.
	rootLoopAction_->setLoopCount(rootLoopAction_->loopCount()-1);
	descriptionLabel_->setText(QString("%1\n[%2] %3 of %4").arg(whatIsRunning_).arg(actionRunner_->currentAction()->statusText()).arg(rootLoopAction_->currentIteration()).arg(rootLoopAction_->loopCount()));

	if (rootLoopAction_->currentIteration() == rootLoopAction_->loopCount())
		decreaseIterations_->setEnabled(false);
}
