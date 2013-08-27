#include "SGMSampleManipulatorMotorGroupView.h"

#include <QGridLayout>

SGMSampleManipulatorMotorGroupObjectView::SGMSampleManipulatorMotorGroupObjectView(AMMotorGroupObject *motorGroupObject, QWidget *parent) :
	AMMotorGroupObjectView(motorGroupObject, parent)
{
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
	motorGroupLayout_->addLayout(absoluteValueLayout_);

	connect(rotateCW_, SIGNAL(clicked()), this, SLOT(onRotateCWClicked()));
	connect(rotateCCW_, SIGNAL(clicked()), this, SLOT(onRotateCCWClicked()));

	if(controlSetpoints_.size() > 3)
		connect(controlSetpoints_.at(3), SIGNAL(editingFinished()), this, SLOT(onFourthControlSetpoint()));
}

void SGMSampleManipulatorMotorGroupObjectView::onRotateCWClicked(){
	motorGroupObject_->controlAt(3)->move(controlSetpoints_.at(3)->value() + jog_->value());
}

void SGMSampleManipulatorMotorGroupObjectView::onRotateCCWClicked(){
	motorGroupObject_->controlAt(3)->move(controlSetpoints_.at(3)->value() - jog_->value());
}

void SGMSampleManipulatorMotorGroupObjectView::onFourthControlSetpoint(){
	motorGroupObject_->controlAt(3)->move(controlSetpoints_.at(3)->value());
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
