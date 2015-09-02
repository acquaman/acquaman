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


#include "SGMSampleManipulatorMotorGroupView.h"

#include <QGridLayout>
#include <QPushButton>
#include <QCheckBox>

#include "ui/beamline/AMExtendedControlEditor.h"

#include "beamline/SGM/SGMBeamline.h"

SGMSampleManipulatorMotorGroupObjectView::SGMSampleManipulatorMotorGroupObjectView(AMMotorGroupObject *motorGroupObject, QWidget *parent) :
	AMMotorGroupObjectView(motorGroupObject, parent)
{
	controlTolerane_ = 0.02;

	rotateCW_ = new QToolButton();
	rotateCCW_ = new QToolButton();

	setInFocusButton_ = new QPushButton("Set In Focus");
	moveInFocusFocusCheckBox_ = new QCheckBox("Move In Focus");
	moveInFocusFocusCheckBox_->setEnabled(false);

	rotateCW_->setIcon(QIcon(":/ArrowCW.png"));
	rotateCCW_->setIcon(QIcon(":/ArrowCCW.png"));

	jog_->setValue(0.1);
	jog_->setSingleStep(0.05);

	arrowLayout_->removeWidget(status_);

	arrowLayout_->addWidget(rotateCW_, 0, 0);
	arrowLayout_->addWidget(rotateCCW_, 2, 2);

	motorGroupLayout_->removeItem(arrowLayout_);
	arrowLayout_->setParent(0);
	absoluteValueLayout_->removeItem(jogLayout_);
	jogLayout_->setParent(0);
	motorGroupLayout_->removeItem(absoluteValueLayout_);
	absoluteValueLayout_->setParent(0);

	QVBoxLayout *leftVL = new QVBoxLayout();
	leftVL->addLayout(arrowLayout_);
	QHBoxLayout *bottomHL = new QHBoxLayout();
	bottomHL->addLayout(jogLayout_);
	bottomHL->addWidget(status_);
	leftVL->addLayout(bottomHL);

	QHBoxLayout *focusHL = new QHBoxLayout();
	focusHL->addWidget(setInFocusButton_);
	focusHL->addWidget(moveInFocusFocusCheckBox_);
	leftVL->addLayout(focusHL);

	motorGroupLayout_->addLayout(leftVL);
	motorGroupLayout_->addStretch(0);
	motorGroupLayout_->addLayout(absoluteValueLayout_);

	connect(rotateCW_, SIGNAL(clicked()), this, SLOT(onRotateCWClicked()));
	connect(rotateCCW_, SIGNAL(clicked()), this, SLOT(onRotateCCWClicked()));

	connect(setInFocusButton_, SIGNAL(clicked()), this, SLOT(onSetInFocusButtonClicked()));
	connect(moveInFocusFocusCheckBox_, SIGNAL(toggled(bool)), this, SLOT(onMoveInFocusCheckBoxToggled(bool)));
	connect(SGMBeamline::sgm(), SIGNAL(moveInFocusChanged(bool)), moveInFocusFocusCheckBox_, SLOT(setChecked(bool)));

	for(int x = 0, size = controlSetpointLayouts_.count(); x < size; x++){
		controlSetpointLayouts_.at(x)->itemAt(0)->widget()->hide();
		controlSetpointLayouts_.at(x)->itemAt(1)->widget()->hide();
	}

	AMExtendedControlEditor *oneControlEditor;
	for(int x = 0, size = motorGroupObject_->size(); x < size; x++){
		oneControlEditor = new AMExtendedControlEditor(motorGroupObject_->controlAt(x));
		oneControlEditor->setControlFormat('f', 2);
		controlEditors_.append(oneControlEditor);

		absoluteValueLayout_->addWidget(oneControlEditor);
	}
}

SGMSampleManipulatorMotorGroupObjectView::~SGMSampleManipulatorMotorGroupObjectView()
{
	while(controlEditors_.count() != 0)
		controlEditors_.removeAt(controlEditors_.count()-1);
}


void SGMSampleManipulatorMotorGroupObjectView::onUpClicked(){
	int index = motorGroupObject_->verticalIndex();
	motorGroupObject_->controlAt(index)->moveRelative(jog_->value(), AMControl::RelativeMoveFromSetpoint);
}

void SGMSampleManipulatorMotorGroupObjectView::onDownClicked(){
	int index = motorGroupObject_->verticalIndex();
	motorGroupObject_->controlAt(index)->moveRelative(-jog_->value(), AMControl::RelativeMoveFromSetpoint);
}

void SGMSampleManipulatorMotorGroupObjectView::onLeftClicked(){
	int index = motorGroupObject_->horizontalIndex();
	motorGroupObject_->controlAt(index)->moveRelative(-jog_->value(), AMControl::RelativeMoveFromSetpoint);
}

void SGMSampleManipulatorMotorGroupObjectView::onRightClicked(){
	int index = motorGroupObject_->horizontalIndex();
	motorGroupObject_->controlAt(index)->moveRelative(jog_->value(), AMControl::RelativeMoveFromSetpoint);
}

void SGMSampleManipulatorMotorGroupObjectView::onInClicked(){
	int index = motorGroupObject_->normalIndex();
	motorGroupObject_->controlAt(index)->moveRelative(-jog_->value(), AMControl::RelativeMoveFromSetpoint);
}

void SGMSampleManipulatorMotorGroupObjectView::onOutClicked(){
	int index = motorGroupObject_->normalIndex();
	motorGroupObject_->controlAt(index)->moveRelative(jog_->value(), AMControl::RelativeMoveFromSetpoint);
}

void SGMSampleManipulatorMotorGroupObjectView::onFirstControlSetpoint(){

}

void SGMSampleManipulatorMotorGroupObjectView::onSecondControlSetpoint(){

}

void SGMSampleManipulatorMotorGroupObjectView::onThirdControlSetpoint(){

}

void SGMSampleManipulatorMotorGroupObjectView::onRotateCWClicked(){
	motorGroupObject_->controlAt(3)->moveRelative(-jog_->value(), AMControl::RelativeMoveFromSetpoint);
}

void SGMSampleManipulatorMotorGroupObjectView::onRotateCCWClicked(){
	motorGroupObject_->controlAt(3)->moveRelative(jog_->value(), AMControl::RelativeMoveFromSetpoint);
}

void SGMSampleManipulatorMotorGroupObjectView::onMovingChanged(){
	bool isMoving = true;

	foreach (AMControl *control, motorGroupObject_->controls())
		isMoving &= control->isMoving();

	rotateCW_->setDisabled(isMoving);
	rotateCCW_->setDisabled(isMoving);

	AMMotorGroupObjectView::onMovingChanged();
}

void SGMSampleManipulatorMotorGroupObjectView::onSetInFocusButtonClicked(){
	SGMBeamline::sgm()->setInFocus();
	moveInFocusFocusCheckBox_->setEnabled(true);
}

void SGMSampleManipulatorMotorGroupObjectView::onMoveInFocusCheckBoxToggled(bool moveInFocus){
	SGMBeamline::sgm()->setMoveInFocus(moveInFocus);
}

SGMSampleManipulatorMotorGroupView::SGMSampleManipulatorMotorGroupView(AMMotorGroup *motorGroup, QWidget *parent) :
	AMMotorGroupView(motorGroup, parent)
{
}

SGMSampleManipulatorMotorGroupView::~SGMSampleManipulatorMotorGroupView()
{
}
