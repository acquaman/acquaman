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
	motorGroupLayout_->addLayout(absoluteValueLayout_);

	connect(rotateCW_, SIGNAL(clicked()), this, SLOT(onRotateCWClicked()));
	connect(rotateCCW_, SIGNAL(clicked()), this, SLOT(onRotateCCWClicked()));

	if(controlSetpoints_.size() > 3)
		connect(controlSetpoints_.at(3), SIGNAL(editingFinished()), this, SLOT(onFourthControlSetpoint()));

	for(int x = 0, size = controlSetpoints_.count(); x < size; x++){
//		disconnect(motorGroupObject_->controlAt(x), SIGNAL(valueChanged(double)), controlSetpoints_.at(x), SLOT(setValue(double)));
//		connect(motorGroupObject_->controlAt(x), SIGNAL(valueChanged(double)), this, SLOT(onMotorGroupObjectValueChanged(double)));
	}

	for(int x = 0, size = controlSetpointLayouts_.count(); x < size; x++){
		controlSetpointLayouts_.at(x)->itemAt(0)->widget()->hide();
		controlSetpointLayouts_.at(x)->itemAt(1)->widget()->hide();
	}

//	for(int x = absoluteValueLayout_->count()-2; x >= 0; x--)
//		absoluteValueLayout_->removeItem(absoluteValueLayout_->itemAt(x));

	AMExtendedControlEditor *oneControlEditor;
	for(int x = 0, size = motorGroupObject_->size(); x < size; x++){
		oneControlEditor = new AMExtendedControlEditor(motorGroupObject_->controlAt(x));
		oneControlEditor->setControlFormat('f', 2);
		controlEditors_.append(oneControlEditor);

		absoluteValueLayout_->addWidget(oneControlEditor);
	}
}


void SGMSampleManipulatorMotorGroupObjectView::onUpClicked(){

}

void SGMSampleManipulatorMotorGroupObjectView::onDownClicked(){

}

void SGMSampleManipulatorMotorGroupObjectView::onLeftClicked(){

}

void SGMSampleManipulatorMotorGroupObjectView::onRightClicked(){

}

void SGMSampleManipulatorMotorGroupObjectView::onInClicked(){

}

void SGMSampleManipulatorMotorGroupObjectView::onOutClicked(){

}

void SGMSampleManipulatorMotorGroupObjectView::onStopClicked(){

}

void SGMSampleManipulatorMotorGroupObjectView::onFirstControlSetpoint(){

}

void SGMSampleManipulatorMotorGroupObjectView::onSecondControlSetpoint(){

}

void SGMSampleManipulatorMotorGroupObjectView::onThirdControlSetpoint(){

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

#include <QDebug>
void SGMSampleManipulatorMotorGroupObjectView::onMotorGroupObjectValueChanged(double newValue){
	Q_UNUSED(newValue)
	QObject *sender = QObject::sender();
	AMControl *motorGroupControl = qobject_cast<AMControl*>(sender);
	if(motorGroupControl){
		for(int x = 0, size = motorGroupObject_->controls().count(); x < size; x++){
			if(motorGroupControl == motorGroupObject_->controls().at(x) && fabs(motorGroupControl->value() - motorGroupControl->setpoint()) > controlTolerane_ && fabs(controlSetpoints_.at(x)->value()-motorGroupControl->value()) > controlTolerane_){
			//if(motorGroupControl == motorGroupObject_->controls().at(x) && fabs(controlSetpoints_.at(x)->value()-motorGroupControl->value()) > controlTolerane_){
				qDebug() << "It exceeds the tolerane, so set it " << motorGroupControl->value() << motorGroupControl->setpoint() << fabs(motorGroupControl->value() - motorGroupControl->setpoint()) << " on " << motorGroupControl->name();
				controlSetpoints_.at(x)->setValue(motorGroupControl->value());
				return;
			}
		}
	}
}

SGMSampleManipulatorMotorGroupView::SGMSampleManipulatorMotorGroupView(AMMotorGroup *motorGroup, QWidget *parent) :
	AMMotorGroupView(motorGroup, parent)
{
}
