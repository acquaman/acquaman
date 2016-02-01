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


#include "AMControlMoveButton.h"

#include <QBoxLayout>

#include <QStringBuilder>

#include "AMQEvents.h"

#include "beamline/AMControl.h"
#include "ui/beamline/AMControlEditor.h"

 AMControlMoveButton::~AMControlMoveButton(){}
AMControlMoveButton::AMControlMoveButton(QWidget *parent, AMControl* control, QList<double> stepSizes,  bool directionReversed) :
    QToolButton(parent)
{
	directionReversed_ = directionReversed;

	control_ = 0;
	setControl(control);

	stepSizes_ = stepSizes;
	if(stepSizes_.isEmpty())
		stepSizes_ << 1.0;	// just to make sure we always have at least one, so that stepSizeIndex_ is valid.
	stepSizeIndex_ = 0;

	contextMenu_ = 0;
	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomContextMenuRequested(QPoint)));

	setText(QString::number(currentStepSize()) % (control_ ? control_->units() : QString()));
	connect(this, SIGNAL(clicked()), this, SLOT(onButtonClicked()));
}

void AMControlMoveButton::onControlDestroyed()
{
	setControl(0);
}

void AMControlMoveButton::setControl(AMControl *control)
{
	if(control == control_)
		return;

	// in the future: if we wanted to be slightly more efficient, could update the context menu to give it an editor for the new control instead of just deleting it and letting a new one be created.
	if(contextMenu_) {
		contextMenu_->deleteLater();
		contextMenu_ = 0;
	}

	if(control_) {
		disconnect(control_, 0, this, 0);
	}

	control_ = control;
	if(control_) {
		connect(control_, SIGNAL(destroyed()), this, SLOT(onControlDestroyed()));
		connect(control_, SIGNAL(connected(bool)), this, SLOT(setEnabled(bool)));
	}

	setText(QString::number(currentStepSize()) % (control_ ? control_->units() : QString()));
	if(control_ && toolTip().isEmpty())
		setToolTip(control_->description().isEmpty() ? control_->name() : control_->description());

	// enabled / disabled:
	if(control_ && control->isConnected())
		setEnabled(true);
	else
		setDisabled(true);
}

void AMControlMoveButton::onButtonClicked()
{
	/// \todo Support continuous jog mode instead of discrete moves
	if(control_) {
		control_->moveRelative(currentStepSize() * (directionReversed_ ? -1.0 : 1.0));
	}
}

bool AMControlMoveButton::setStepSizeIndex(int index)
{
	if(index<0 || index >= stepSizes_.count())
		return false;
	if(stepSizeIndex_ != index) {
		emit stepSizeIndexChanged(stepSizeIndex_ = index);
	}
	setText(QString::number(currentStepSize()) % (control_ ? control_->units() : QString()));
	return true;
}

bool AMControlMoveButton::setStepSizes(const QList<double> &stepSizes)
{
	if(stepSizes.isEmpty())
		return false;

	// in the future: if we wanted to be slightly more efficient, could update the context menu instead of letting a whole new one be created.
	if(contextMenu_) {
		contextMenu_->deleteLater();
		contextMenu_ = 0;
	}

	stepSizeIndex_ = 0;
	stepSizes_ = stepSizes;

	setText(QString::number(currentStepSize()) % (control_ ? control_->units() : QString()));

	return true;
}

void AMControlMoveButton::onCustomContextMenuRequested(const QPoint &point)
{
	if(!contextMenu_)
		contextMenu_ = new AMControlMoveButtonContextMenu(this);

	contextMenu_->move(mapToGlobal(point)-QPoint(6,6));
	contextMenu_->show();
}



 AMControlMoveButtonContextMenu::~AMControlMoveButtonContextMenu(){}
AMControlMoveButtonContextMenu::AMControlMoveButtonContextMenu(AMControlMoveButton *moveButton)
	: QDialog(moveButton, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint)
{
	moveButton_ = moveButton;
	stepSizeButtons_.setExclusive(true);
	QVBoxLayout* vl = new QVBoxLayout();
	vl->setContentsMargins(0,0,0,0);
	QFrame* contentsFrame = new QFrame();
	contentsFrame->setFrameStyle(QFrame::StyledPanel);
	QHBoxLayout* hl = new QHBoxLayout();

	hl->setSpacing(0);
	hl->setContentsMargins(4,4,4,4);
	int buttonId = 0;
	foreach(double stepSize, moveButton_->stepSizes()) {
		QToolButton* stepButton = new QToolButton();
		stepButton->setText(QString::number(stepSize));
		stepButton->setCheckable(true);
		stepSizeButtons_.addButton(stepButton, buttonId++);
		hl->addWidget(stepButton);
	}

	QAbstractButton* currentButton = stepSizeButtons_.button(moveButton_->stepSizeIndex());
	if(currentButton)
		currentButton->setChecked(true);

	connect(&stepSizeButtons_, SIGNAL(buttonClicked(int)), moveButton_, SLOT(setStepSizeIndex(int)));
	connect(moveButton_, SIGNAL(stepSizeIndexChanged(int)), this, SLOT(onStepSizeIndexChanged(int)));
	connect(&stepSizeButtons_, SIGNAL(buttonClicked(int)), this, SLOT(hide()));

	if(moveButton_->control()) {
		AMControlEditor* editor = new AMControlEditor(moveButton_->control());
		hl->addSpacing(10);
		hl->addWidget(editor);
	}

	contentsFrame->setLayout(hl);
	vl->addWidget(contentsFrame);
	setLayout(vl);
}

void AMControlMoveButtonContextMenu::onStepSizeIndexChanged(int newIndex)
{
	QAbstractButton* currentButton = stepSizeButtons_.button(newIndex);
	if(currentButton)
		currentButton->setChecked(true);
}


void AMControlMoveButtonContextMenu::leaveEvent(QEvent *e)
{
	hide();
	QDialog::leaveEvent(e);
}

void AMControlMoveButton::press(bool down)
{
	if(!isEnabled())
		return;

	// press
	if(down) {
		if(!isDown()) {
			setDown(true);
			emit pressed();
		}
	}
	// release, only if already pressed.
	else {
		if(isDown()) {
			setDown(false);
			emit released();
			emit clicked();
		}
	}
}
