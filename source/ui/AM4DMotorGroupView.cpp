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


#include "AM4DMotorGroupView.h"

#include <QGridLayout>
#include <QPushButton>
#include <QCheckBox>

#include "beamline/AMBeamline.h"
#include "ui/beamline/AMExtendedControlEditor.h"


AM4DMotorGroupObjectView::AM4DMotorGroupObjectView(AMMotorGroupObject *motorGroupObject, QWidget *parent) :
	AMMotorGroupObjectView(motorGroupObject, parent)
{
	jog_->setValue(0.1);
	jog_->setSingleStep(0.05);

	// create new components of AM4DMotorGroupObjectView
	rotateCW_ = new QToolButton();
	rotateCCW_ = new QToolButton();
	rotateCW_->setIcon(QIcon(":/ArrowCW.png"));
	rotateCCW_->setIcon(QIcon(":/ArrowCCW.png"));

	setInFocusButton_ = new QPushButton("Set In Focus");
	moveInFocusFocusCheckBox_ = new QCheckBox("Move In Focus");
	moveInFocusFocusCheckBox_->setEnabled(false);

	//modification to the original AMMotorGroupObjectView
	// arrowLayout - remove the status button and add the two rotate buttons
	arrowLayout_->removeWidget(status_);
	arrowLayout_->addWidget(rotateCW_, 0, 0);
	arrowLayout_->addWidget(rotateCCW_, 2, 2);

	//motorGrouplayout - remove the arrowLayout and the absoluteValueLayout, which will be reorganized
	motorGroupLayout_->removeItem(arrowLayout_);
	arrowLayout_->setParent(0);

	// === new Layout: ===
	// left side (vertically): arrowLayout, status button, move in focus layout
	QVBoxLayout *leftVL = new QVBoxLayout();
	leftVL->addLayout(arrowLayout_);
	leftVL->addWidget(status_, 0, Qt::AlignHCenter);

	QHBoxLayout *focusHL = new QHBoxLayout();
	focusHL->addWidget(setInFocusButton_);
	focusHL->addWidget(moveInFocusFocusCheckBox_);
	leftVL->addLayout(focusHL);

	motorGroupLayout_->insertLayout(0, leftVL);
	motorGroupLayout_->insertStretch(1, 0);

	connect(rotateCW_, SIGNAL(clicked()), this, SLOT(onRotateCWClicked()));
	connect(rotateCCW_, SIGNAL(clicked()), this, SLOT(onRotateCCWClicked()));

	connect(setInFocusButton_, SIGNAL(clicked()), this, SLOT(onSetInFocusButtonClicked()));
	connect(moveInFocusFocusCheckBox_, SIGNAL(toggled(bool)), this, SLOT(onMoveInFocusCheckBoxToggled(bool)));

	if (controlSetpoints_.size() > 3)
		connect(controlSetpoints_.at(3), SIGNAL(editingFinished()), this, SLOT(onFourthControlSetpoint()));

	enableMoveInFocus(false);
}

AM4DMotorGroupObjectView::~AM4DMotorGroupObjectView()
{
}

void AM4DMotorGroupObjectView::enableMoveInFocus(bool enable)
{
	// was enabled, disconnect
	if (moveInFocusEnabled_) {
//  leave the codes here first. Currently this signal is for SGM beamline only, need to be generalized
//		disconnect(AMBeamline::bl(), SIGNAL(moveInFocusChanged(bool)), moveInFocusFocusCheckBox_, SLOT(setChecked(bool)));
	}

	moveInFocusEnabled_ = enable;
	if (moveInFocusEnabled_) {
//		connect(AMBeamline::bl(), SIGNAL(moveInFocusChanged(bool)), moveInFocusFocusCheckBox_, SLOT(setChecked(bool)));
	}

	setInFocusButton_->setVisible(moveInFocusEnabled_);
	moveInFocusFocusCheckBox_->setVisible(moveInFocusEnabled_);
}

void AM4DMotorGroupObjectView::onFourthControlSetpoint(){
	motorGroupObject_->controlAt(3)->move(controlSetpoints_.at(3)->value());
}

void AM4DMotorGroupObjectView::onRotateCWClicked(){
	motorGroupObject_->controlAt(3)->moveRelative(-jog_->value(), AMControl::RelativeMoveFromSetpoint);
}

void AM4DMotorGroupObjectView::onRotateCCWClicked(){
	motorGroupObject_->controlAt(3)->moveRelative(jog_->value(), AMControl::RelativeMoveFromSetpoint);
}

void AM4DMotorGroupObjectView::onMovingChanged(){
	bool isMoving = motorGroupObject_->isMotorMoving();

	rotateCW_->setDisabled(isMoving);
	rotateCCW_->setDisabled(isMoving);

	AMMotorGroupObjectView::onMovingChanged();
}

void AM4DMotorGroupObjectView::onSetInFocusButtonClicked(){
//	AMBeamline::bl()->setInFocus();
	moveInFocusFocusCheckBox_->setEnabled(true);
}

void AM4DMotorGroupObjectView::onMoveInFocusCheckBoxToggled(bool moveInFocus){
	Q_UNUSED(moveInFocus)

//	AMBeamline::bl()->setMoveInFocus(moveInFocus);
}

