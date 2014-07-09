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


#include "AMActionRunnerBottomBarCurrentView3.h"

#include "util/AMFontSizes.h"
#include "util/AMErrorMonitor.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QTimer>
#include <QMenu>
#include <QMessageBox>

AMActionRunnerBottomBarCurrentView3::~AMActionRunnerBottomBarCurrentView3(){}

AMActionRunnerBottomBarCurrentView3::AMActionRunnerBottomBarCurrentView3(AMActionRunner3 *actionRunner, QWidget *parent)
	: AMActionRunnerCurrentViewBase(actionRunner, parent)
{
	rootLoopAction_ = 0;

	QHBoxLayout* hl = new QHBoxLayout;
	hl->setSpacing(0);
	hl->setContentsMargins(6, 2, 12, 1);

	// New parts.
	descriptionLabel_ = new QLabel;
	descriptionLabel_->setObjectName("descriptionLabel");
	descriptionLabel_->setStyleSheet("#descriptionLabel { border: 1px black; border-radius: 10px; background: rgb(219,219,112); }");
	descriptionLabel_->setText("No actions running.");
	descriptionLabel_->setMinimumWidth(350);
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

	// Customizing the elements from base view.
	pauseButton_->setIcon(QIcon(":/22x22/media-playback-pause.png"));
	pauseButton_->setEnabled(false);
	pauseButton_->setToolTip("Pause the current action.");
	hl->addWidget(pauseButton_);
	hl->addSpacing(5);

	skipButton_->setIcon(QIcon(":/media-seek-forward.png"));
	skipButton_->setEnabled(false);
	hl->addWidget(skipButton_);
	hl->addSpacing(5);

	cancelButton_->setIcon(QIcon(":/22x22/list-remove-2.png"));
	cancelButton_->setEnabled(false);
	cancelButton_->setToolTip("Cancel the current action.");
	hl->addWidget(cancelButton_);
	hl->addSpacing(10);

	timeElapsedLabel_->setStyleSheet("border: 0px; color: white;\nfont: " AM_FONT_REGULAR_ "pt \"Lucida Grande\"");
	hl->addWidget(timeElapsedLabel_);
	hl->addSpacing(10);

	progressBar_->setObjectName("progressBar");
	progressBar_->setStyleSheet("#progressBar { border: 1px solid black; color: white; border-radius: 7px; background: gray; } #progressBar::chunk { background: rgb(60,119,197); border-radius: 7px; border: 0px; }");
	progressBar_->setAlignment(Qt::AlignCenter);
	progressBar_->setMinimumWidth(400);
	progressBar_->setMaximumWidth(600);
	progressBar_->setRange(0,1);
	progressBar_->setValue(0);
	hl->addWidget(progressBar_);
	hl->addSpacing(10);

	timeRemainingLabel_->setStyleSheet("border: 0px; color: white;\nfont: " AM_FONT_REGULAR_ "pt \"Lucida Grande\"");
	hl->addWidget(timeRemainingLabel_);
	hl->addSpacing(20);

	setLayout(hl);

	connect(increaseIterations_, SIGNAL(clicked()), this, SLOT(onIncreaseLoopIterationsClicked()));
	connect(decreaseIterations_, SIGNAL(clicked()), this, SLOT(onDecreaseLoopIterationsClicked()));
}

void AMActionRunnerBottomBarCurrentView3::onCurrentActionChanged(AMAction3 *action)
{
	AMActionRunnerCurrentViewBase::onCurrentActionChanged(action);

	if (action && !action->parentAction() && qobject_cast<AMLoopAction3 *>(action)){

		rootLoopAction_ = qobject_cast<AMLoopAction3 *>(action);
		increaseIterations_->setEnabled(true);
		increaseIterations_->setVisible(true);
		decreaseIterations_->setEnabled(true);
		decreaseIterations_->setVisible(true);
		connect(rootLoopAction_, SIGNAL(currentIterationChanged(int)), this, SLOT(onLoopIterationUpdate(int)));
		connect(rootLoopAction_, SIGNAL(currentSubActionChanged(int)), this, SLOT(onRootLoopSubActionChanged(int)));
		connect(rootLoopAction_->info(), SIGNAL(infoChanged()), this, SLOT(onRootLoopActionLoopCountChanged()));
	}

	else{

		increaseIterations_->setEnabled(false);
		increaseIterations_->setVisible(false);
		decreaseIterations_->setEnabled(false);
		decreaseIterations_->setVisible(false);
	}

	if (!action){

		whatIsRunning_ = "No action running.";
		descriptionLabel_->setText(whatIsRunning_);
	}
}

void AMActionRunnerBottomBarCurrentView3::updateDescriptionViews(AMAction3 *action)
{
	whatIsRunning_ = action->info()->name();

	if (rootLoopAction_)
		descriptionLabel_->setText(QString("%1\n[%2] %3 of %4").arg(whatIsRunning_).arg(action->statusText()).arg(rootLoopAction_->currentIteration()+1).arg(rootLoopAction_->loopCount()));
	else
		descriptionLabel_->setText(QString("%1\n[%2]").arg(whatIsRunning_).arg(action->statusText()));

	timeElapsedLabel_->setText("0:00");
	double expectedDuration = action->expectedDuration();
	timeRemainingLabel_->setText(expectedDuration > 0 ? formatSeconds(expectedDuration) : "N/A");
}

void AMActionRunnerBottomBarCurrentView3::onStateChanged(int state, int previousState)
{
	AMActionRunnerCurrentViewBase::onStateChanged(state, previousState);

	if (rootLoopAction_ && rootLoopAction_->inFinalState()){

		disconnect(rootLoopAction_, SIGNAL(currentIterationChanged(int)), this, SLOT(onLoopIterationUpdate(int)));
		disconnect(rootLoopAction_, SIGNAL(currentSubActionChanged(int)), this, SLOT(onRootLoopSubActionChanged(int)));
		disconnect(rootLoopAction_->info(), SIGNAL(infoChanged()), this, SLOT(onRootLoopActionLoopCountChanged()));
		rootLoopAction_ = 0;
	}
}

void AMActionRunnerBottomBarCurrentView3::onStatusTextChanged(const QString &newStatus)
{
	Q_UNUSED(newStatus);

	AMAction3 *action = AMActionRunner3::workflow()->currentAction();

	if (rootLoopAction_)
		descriptionLabel_->setText(QString("%1\n[%2] %3 of %4").arg(whatIsRunning_).arg(rootLoopAction_->currentSubAction()->stateDescription(action->state())).arg(rootLoopAction_->currentIteration()+1).arg(rootLoopAction_->loopCount()));

	else
		descriptionLabel_->setText(QString("%1\n[%2]").arg(whatIsRunning_).arg(action->stateDescription(action->state())));
}

void AMActionRunnerBottomBarCurrentView3::onLoopIterationUpdate(int iteration)
{
	descriptionLabel_->setText(QString("%1\n[%2] %3 of %4").arg(whatIsRunning_).arg(actionRunner_->currentAction()->stateDescription(actionRunner_->currentAction()->state())).arg(iteration+1).arg(rootLoopAction_->loopCount()));

	if (rootLoopAction_->currentIteration()+1 == rootLoopAction_->loopCount())
		decreaseIterations_->setEnabled(false);
}

void AMActionRunnerBottomBarCurrentView3::onIncreaseLoopIterationsClicked()
{
	// No checks are done here because you should only be able to get here if there is already a valid root loop action.
	rootLoopAction_->setLoopCount(rootLoopAction_->loopCount()+1);
	descriptionLabel_->setText(QString("%1\n[%2] %3 of %4").arg(whatIsRunning_).arg(rootLoopAction_->currentSubAction()->stateDescription(rootLoopAction_->currentSubAction()->state())).arg(rootLoopAction_->currentIteration()+1).arg(rootLoopAction_->loopCount()));
	decreaseIterations_->setEnabled(true);
}

void AMActionRunnerBottomBarCurrentView3::onDecreaseLoopIterationsClicked()
{
	// No checks are done here because you should only be able to get here if there is already a valid root loop action.
	// Also, you should only be able to get here if decreasing the loop iterations is valid.  So extra checks are not necessary.
	rootLoopAction_->setLoopCount(rootLoopAction_->loopCount()-1);
	descriptionLabel_->setText(QString("%1\n[%2] %3 of %4").arg(whatIsRunning_).arg(rootLoopAction_->currentSubAction()->stateDescription(rootLoopAction_->currentSubAction()->state())).arg(rootLoopAction_->currentIteration()+1).arg(rootLoopAction_->loopCount()));

	if (rootLoopAction_->currentIteration()+1 == rootLoopAction_->loopCount())
		decreaseIterations_->setEnabled(false);
}

void AMActionRunnerBottomBarCurrentView3::onRootLoopSubActionChanged(int index)
{
	if (index < rootLoopAction_->subActionCount()){

		whatIsRunning_ = rootLoopAction_->subActionAt(index)->info()->name();
		descriptionLabel_->setText(QString("%1\n[%2] %3 of %4").arg(whatIsRunning_).arg(rootLoopAction_->subActionAt(index)->stateDescription(rootLoopAction_->subActionAt(index)->state())).arg(rootLoopAction_->currentIteration()+1).arg(rootLoopAction_->loopCount()));
	}
}

void AMActionRunnerBottomBarCurrentView3::onRootLoopActionLoopCountChanged()
{
	descriptionLabel_->setText(QString("%1\n[%2] %3 of %4").arg(whatIsRunning_).arg(rootLoopAction_->currentSubAction()->stateDescription(rootLoopAction_->currentSubAction()->state())).arg(rootLoopAction_->currentIteration()+1).arg(rootLoopAction_->loopCount()));

	if (rootLoopAction_->currentIteration()+1 == rootLoopAction_->loopCount())
		decreaseIterations_->setEnabled(false);
}
