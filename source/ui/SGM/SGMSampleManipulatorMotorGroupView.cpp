#include "SGMSampleManipulatorMotorGroupView.h"

#include <QGridLayout>

#include "ui/beamline/AMExtendedControlEditor.h"

SGMSampleManipulatorMotorGroupObjectView::SGMSampleManipulatorMotorGroupObjectView(AMMotorGroupObject *motorGroupObject, QWidget *parent) :
	AMMotorGroupObjectView(motorGroupObject, parent)
{
	controlTolerane_ = 0.02;

	rotateCW_ = new QToolButton();
	rotateCCW_ = new QToolButton();

	rotateCW_->setIcon(QIcon(":/ArrowCW.png"));
	rotateCCW_->setIcon(QIcon(":/ArrowCCW.png"));

	jog_->setValue(0.1);
	jog_->setSingleStep(0.05);

	arrowLayout_->removeWidget(status_);

	arrowLayout_->addWidget(rotateCW_, 0, 0);
	arrowLayout_->addWidget(rotateCCW_, 2, 2);

	motorGroupLayout_->removeItem(arrowLayout_);
	absoluteValueLayout_->removeItem(jogLayout_);
	motorGroupLayout_->removeItem(absoluteValueLayout_);

	QVBoxLayout *leftVL = new QVBoxLayout();
	leftVL->addLayout(arrowLayout_);
	QHBoxLayout *bottomHL = new QHBoxLayout();
	bottomHL->addLayout(jogLayout_);
	bottomHL->addWidget(status_);
	leftVL->addLayout(bottomHL);

	motorGroupLayout_->addLayout(leftVL);
	motorGroupLayout_->addStretch(10);
	motorGroupLayout_->addLayout(bottomHL);
	motorGroupLayout_->addLayout(absoluteValueLayout_);

	connect(rotateCW_, SIGNAL(clicked()), this, SLOT(onRotateCWClicked()));
	connect(rotateCCW_, SIGNAL(clicked()), this, SLOT(onRotateCCWClicked()));

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

SGMSampleManipulatorMotorGroupView::SGMSampleManipulatorMotorGroupView(AMMotorGroup *motorGroup, QWidget *parent) :
	AMMotorGroupView(motorGroup, parent)
{
}
