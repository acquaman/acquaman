#include "AMActionRunnerBotomBarCurrentView3.h"

#include "util/AMFontSizes.h"
#include "util/AMErrorMonitor.h"

#include <QFrame>
#include <QHBoxLayout>

AMActionRunnerBotomBarCurrentView3::AMActionRunnerBotomBarCurrentView3(AMActionRunner3 *actionRunner, QWidget *parent)
	: QWidget(parent)
{
	actionRunner_ = actionRunner;
	whatIsRunning_ = "";

	// setup UI
	QFrame* topFrame = new QFrame();
	topFrame->setObjectName("topFrame");
	topFrame->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	topFrame->setStyleSheet("QFrame#topFrame {\nbackground-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(89, 89, 89, 255), stop:0.494444 rgba(89, 89, 89, 255), stop:0.5 rgba(58, 58, 58, 255), stop:1 rgba(58, 58, 58, 255));\nborder-bottom: 1px solid black;\n}");
	QHBoxLayout* hl = new QHBoxLayout(topFrame);
	hl->setSpacing(0);
	hl->setContentsMargins(6, 2, 12, 1);

	timeElapsedLabel_ = new QLabel("0:00");
	timeElapsedLabel_->setStyleSheet("color: white;\nfont: " AM_FONT_LARGE_ "pt \"Lucida Grande\"");
	hl->addWidget(timeElapsedLabel_);
	hl->addSpacing(10);
	progressBar_ = new QProgressBar();
	progressBar_->setMaximumWidth(600);
	hl->addWidget(progressBar_, 1);
	hl->addSpacing(10);
	timeRemainingLabel_ = new QLabel("0:00");
	timeRemainingLabel_->setStyleSheet("color: white;\nfont: " AM_FONT_LARGE_ "pt \"Lucida Grande\"");
	hl->addWidget(timeRemainingLabel_);
	hl->addSpacing(20);

//	pauseButton_ = new QToolButton(QIcon(":/22x22/media-playback-pause.png"));
//	pauseButton_->setEnabled(false);
//	hl->addWidget(pauseButton_);
//	skipButton_ = new QToolButton(QIcon(":/media-seek-forward.png"));
//	skipButton_->setEnabled(false);
//	skipButton_->setVisible(false);
//	hl->addWidget(skipButton_);
//	cancelButton_ = new QToolButton(QIcon(":/22x22/list-remove-2.png"));
//	cancelButton_->setEnabled(false);
//	hl->addWidget(cancelButton_);
}

void AMActionRunnerBotomBarCurrentView3::onCurrentActionChanged(AMAction3 *action)
{

}

void AMActionRunnerBotomBarCurrentView3::onPauseButtonClicked()
{

}

void AMActionRunnerBotomBarCurrentView3::onSkipButtonClicked()
{

}

void AMActionRunnerBotomBarCurrentView3::onStatusTextChanged(const QString &newStatus)
{

}

void AMActionRunnerBotomBarCurrentView3::onExpectedDurationChanged(double totalSeconds)
{

}

void AMActionRunnerBotomBarCurrentView3::onProgressChanged(double numerator, double denominator)
{

}

void AMActionRunnerBotomBarCurrentView3::onStateChanged(int state, int previousState)
{

}

void AMActionRunnerBotomBarCurrentView3::onTimeUpdateTimer()
{

}
