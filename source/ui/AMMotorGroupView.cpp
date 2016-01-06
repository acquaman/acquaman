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
#include "AMMotorGroupView.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>

////////////////////////////////////////////////////////////////////////////////
// Motor Group Object View
////////////////////////////////////////////////////////////////////////////////

AMMotorGroupObjectView::AMMotorGroupObjectView(AMMotorGroupObject *motorGroupObject,
											   QWidget *parent)
	:QWidget(parent)
{

	motorGroupObject_ = motorGroupObject;

	if(motorGroupObject_) {
		setupUi();
		setupData();
		setupConnections();

		setMotorValuesPrecision(3);
	}
}

void AMMotorGroupObjectView::setJogSize(double jogSize)
{
	jogSize_->setValue(jogSize);
}

void AMMotorGroupObjectView::setJogSingleStep(double jogSingleStep)
{
	jogSize_->setSingleStep(jogSingleStep);
}

void AMMotorGroupObjectView::setJogPrecision(int jogPrecision)
{
	jogSize_->setDecimals(jogPrecision);
}

void AMMotorGroupObjectView::setJogRange(double minJog, double maxJog)
{
	jogSize_->setRange(minJog, maxJog);
}

void AMMotorGroupObjectView::setMotorValuesPrecision(int motorValuesPrecision)
{
	horizontalTranslationValue_->setPrecision(motorValuesPrecision);
	horizontalTranslationValue_->setControlFormat('f', motorValuesPrecision);

	horizontalRotationValue_->setPrecision(motorValuesPrecision);
	horizontalRotationValue_->setControlFormat('f', motorValuesPrecision);

	verticalTranslationValue_->setPrecision(motorValuesPrecision);
	verticalTranslationValue_->setControlFormat('f', motorValuesPrecision);

	verticalRotationValue_->setPrecision(motorValuesPrecision);
	verticalRotationValue_->setControlFormat('f', motorValuesPrecision);

	normalTranslationValue_->setPrecision(motorValuesPrecision);
	normalTranslationValue_->setControlFormat('f', motorValuesPrecision);

	normalRotationValue_->setPrecision(motorValuesPrecision);
	normalRotationValue_->setControlFormat('f', motorValuesPrecision);
}

void AMMotorGroupObjectView::setMotorValuesRange(double minValue, double maxValue)
{
	horizontalTranslationValue_->setRange(minValue, maxValue);
	horizontalRotationValue_->setRange(minValue, maxValue);

	verticalTranslationValue_->setRange(minValue, maxValue);
	verticalRotationValue_->setRange(minValue, maxValue);

	normalTranslationValue_->setRange(minValue, maxValue);
	normalRotationValue_->setRange(minValue, maxValue);
}

void AMMotorGroupObjectView::onConnectionStateChanged(AMMotorGroupObject::MotionDirection direction,
													  AMMotorGroupAxis::MotionType motionType,
													  bool isConnected)
{

	switch (direction) {

	case AMMotorGroupObject::VerticalMotion:
		if(motionType == AMMotorGroupAxis::RotationalMotion) {
			verticalRotationIncrement_->setVisible(isConnected);
			verticalRotationDecrement_->setVisible(isConnected);
			verticalRotationValue_->setVisible(isConnected);
			verticalRotationLabel_->setVisible(isConnected);
		} else {
			verticalTranslationIncrement_->setVisible(isConnected);
			verticalTranslationDecrement_->setVisible(isConnected);
			verticalTranslationValue_->setVisible(isConnected);
			verticalTranslationLabel_->setVisible(isConnected);
		}
		break;
	case AMMotorGroupObject::HorizontalMotion:
		if(motionType == AMMotorGroupAxis::RotationalMotion) {
			horizontalRotationIncrement_->setVisible(isConnected);
			horizontalRotationDecrement_->setVisible(isConnected);
			horizontalRotationValue_->setVisible(isConnected);
			horizontalRotationLabel_->setVisible(isConnected);
		} else {
			horizontalTranslationIncrement_->setVisible(isConnected);
			horizontalTranslationDecrement_->setVisible(isConnected);
			horizontalTranslationValue_->setVisible(isConnected);
			horizontalTranslationLabel_->setVisible(isConnected);
		}
		break;
	case AMMotorGroupObject::NormalMotion:
		if(motionType == AMMotorGroupAxis::RotationalMotion) {
			normalRotationIncrement_->setVisible(isConnected);
			normalRotationDecrement_->setVisible(isConnected);
			normalRotationValue_->setVisible(isConnected);
			normalRotationLabel_->setVisible(isConnected);
		} else {
			normalTranslationIncrement_->setVisible(isConnected);
			normalTranslationDecrement_->setVisible(isConnected);
			normalTranslationValue_->setVisible(isConnected);
			normalTranslationLabel_->setVisible(isConnected);
		}
		break;
	}
}

void AMMotorGroupObjectView::onMotorError(AMMotorGroupObject::MotionDirection direction,
										  AMMotorGroupAxis::MotionType motionType,
										  int errorCode)
{
	// In the case of a limit failure, we disable the offending motor and show an
	// error message.
	if(AMControl::FailureExplanation(errorCode) == AMControl::LimitFailure) {

		// Disable motor controls
		switch (direction) {
		case AMMotorGroupObject::VerticalMotion:
			if(motionType == AMMotorGroupAxis::RotationalMotion) {
				verticalRotationIncrement_->setEnabled(false);
				verticalRotationDecrement_->setEnabled(false);
				verticalRotationValue_->setEnabled(false);
			} else {
				verticalTranslationIncrement_->setEnabled(false);
				verticalTranslationDecrement_->setEnabled(false);
				verticalTranslationValue_->setEnabled(false);
			}
			break;
		case AMMotorGroupObject::HorizontalMotion:
			if(motionType == AMMotorGroupAxis::RotationalMotion) {
				horizontalRotationIncrement_->setEnabled(false);
				horizontalRotationDecrement_->setEnabled(false);
				horizontalRotationValue_->setEnabled(false);
			} else {
				horizontalTranslationIncrement_->setEnabled(false);
				horizontalTranslationDecrement_->setEnabled(false);
				horizontalTranslationValue_->setEnabled(false);
			}
			break;
		case AMMotorGroupObject::NormalMotion:
			if(motionType == AMMotorGroupAxis::RotationalMotion) {
				normalRotationIncrement_->setEnabled(false);
				normalRotationDecrement_->setEnabled(false);
				normalRotationValue_->setEnabled(false);
			} else {
				normalTranslationIncrement_->setEnabled(false);
				normalTranslationDecrement_->setEnabled(false);
				normalTranslationValue_->setEnabled(false);
			}
			break;
		}

		// Show error message
		QString axisName;
		if(motionType == AMMotorGroupAxis::RotationalMotion) {
			axisName = motorGroupObject_->axis(direction)->rotationName();
		} else {
			axisName = motorGroupObject_->axis(direction)->translationName();
		}

		errorLabel_->setText(QString("Axis %1 reached limits, and has been disabled."));
		errorLabel_->setVisible(true);
		dismissError_->setVisible(true);

	}

	// See if we're a subclass that needs to do anything else.
	errorHandleImplementation(direction, motionType, errorCode);
}

void AMMotorGroupObjectView::onMovingStateChanged(AMMotorGroupObject::MotionDirection direction,
												  AMMotorGroupAxis::MotionType motionType,
												  bool isMoving)
{
	if(isMoving) {
		statusLabel_->setPixmap(QIcon(":/32x32/greenLEDOn.png").pixmap(25));
	} else {
		statusLabel_->setPixmap(QIcon(":/32x32/greenLEDOff.png").pixmap(25));
	}

	bool enabled = !isMoving;

	switch (direction) {
	case AMMotorGroupObject::VerticalMotion:
		if(motionType == AMMotorGroupAxis::RotationalMotion) {
			verticalRotationIncrement_->setEnabled(enabled);
			verticalRotationDecrement_->setEnabled(enabled);
			verticalRotationValue_->setEnabled(enabled);
		} else {
			verticalTranslationIncrement_->setEnabled(enabled);
			verticalTranslationDecrement_->setEnabled(enabled);
			verticalTranslationValue_->setEnabled(enabled);
		}
		break;
	case AMMotorGroupObject::HorizontalMotion:
		if(motionType == AMMotorGroupAxis::RotationalMotion) {
			horizontalRotationIncrement_->setEnabled(enabled);
			horizontalRotationDecrement_->setEnabled(enabled);
			horizontalRotationValue_->setEnabled(enabled);
		} else {
			horizontalTranslationIncrement_->setEnabled(enabled);
			horizontalTranslationDecrement_->setEnabled(enabled);
			horizontalTranslationValue_->setEnabled(enabled);
		}
		break;
	case AMMotorGroupObject::NormalMotion:
		if(motionType == AMMotorGroupAxis::RotationalMotion) {
			normalRotationIncrement_->setEnabled(enabled);
			normalRotationDecrement_->setEnabled(enabled);
			normalRotationValue_->setEnabled(enabled);
		} else {
			normalTranslationIncrement_->setEnabled(enabled);
			normalTranslationDecrement_->setEnabled(enabled);
			normalTranslationValue_->setEnabled(enabled);
		}
		break;
	}
}

void AMMotorGroupObjectView::onPositionUnitsChanged(AMMotorGroupObject::MotionDirection direction,
													AMMotorGroupAxis::MotionType motionType,
													const QString &positionUnits)
{
	switch (direction) {
	case AMMotorGroupObject::VerticalMotion:
		if(motionType == AMMotorGroupAxis::RotationalMotion) {
			verticalRotationValue_->setUnits(positionUnits);
		} else {
			verticalTranslationValue_->setUnits(positionUnits);
		}
		break;
	case AMMotorGroupObject::HorizontalMotion:
		if(motionType == AMMotorGroupAxis::RotationalMotion) {
			horizontalRotationValue_->setUnits(positionUnits);
		} else {
			horizontalTranslationValue_->setUnits(positionUnits);
		}
		break;
	case AMMotorGroupObject::NormalMotion:
		if(motionType == AMMotorGroupAxis::RotationalMotion) {
			normalRotationValue_->setUnits(positionUnits);
		} else {
			normalTranslationValue_->setUnits(positionUnits);
		}
		break;
	}
	refreshJogUnits();
}

void AMMotorGroupObjectView::onHorizontalTranslationIncrementClicked()
{
	if(motorGroupObject_->hasHorizontalAxis() &&
			motorGroupObject_->horizontalAxis()->canTranslate()) {


		double newSetpoint = motorGroupObject_->horizontalAxis()->currentTranslationPosition()
			+ jogSize_->value();

		motorGroupObject_->horizontalAxis()->setTranslatePosition(newSetpoint);
	}
}

void AMMotorGroupObjectView::onHorizontalTranslationDecrementClicked()
{
	if(motorGroupObject_->hasHorizontalAxis() &&
			motorGroupObject_->horizontalAxis()->canTranslate()) {

		double newSetpoint = motorGroupObject_->horizontalAxis()->currentTranslationPosition()
				- jogSize_->value();

		motorGroupObject_->horizontalAxis()->setTranslatePosition(newSetpoint);
	}
}

void AMMotorGroupObjectView::onHorizontalRotationIncrementClicked()
{
	if(motorGroupObject_->hasHorizontalAxis() &&
			motorGroupObject_->horizontalAxis()->canRotate()) {

		double newSetpoint = motorGroupObject_->horizontalAxis()->currentRotationPosition()
				+ jogSize_->value();


		motorGroupObject_->horizontalAxis()->setRotatePosition(newSetpoint);
	}
}

void AMMotorGroupObjectView::onHorizontalRotationDecrementClicked()
{
	if(motorGroupObject_->hasHorizontalAxis() &&
			motorGroupObject_->horizontalAxis()->canRotate()) {

		double newSetpoint = motorGroupObject_->horizontalAxis()->currentRotationPosition()
				- jogSize_->value();

		motorGroupObject_->horizontalAxis()->setRotatePosition(newSetpoint);
	}
}

void AMMotorGroupObjectView::onVerticalTranslationIncrementClicked()
{
	if(motorGroupObject_->hasVerticalAxis() &&
			motorGroupObject_->verticalAxis()->canTranslate()) {

		double newSetpoint = motorGroupObject_->verticalAxis()->currentTranslationPosition()
				+ jogSize_->value();

		motorGroupObject_->verticalAxis()->setTranslatePosition(newSetpoint);
	}
}

void AMMotorGroupObjectView::onVerticalTranslationDecrementClicked()
{
	if(motorGroupObject_->hasVerticalAxis() &&
			motorGroupObject_->verticalAxis()->canTranslate()) {

		double newSetpoint = motorGroupObject_->verticalAxis()->currentTranslationPosition()
				- jogSize_->value();

		motorGroupObject_->verticalAxis()->setTranslatePosition(newSetpoint);
	}
}

void AMMotorGroupObjectView::onVerticalRotationIncrementClicked()
{
	if(motorGroupObject_->hasVerticalAxis() &&
			motorGroupObject_->verticalAxis()->canRotate()) {

		double newSetpoint = motorGroupObject_->verticalAxis()->currentRotationPosition()
				+ jogSize_->value();

		motorGroupObject_->verticalAxis()->setRotatePosition(newSetpoint);
	}
}

void AMMotorGroupObjectView::onVerticalRotationDecrementClicked()
{
	if(motorGroupObject_->hasVerticalAxis() &&
			motorGroupObject_->verticalAxis()->canRotate()) {

		double newSetpoint = motorGroupObject_->verticalAxis()->currentRotationPosition()
				- jogSize_->value();

		motorGroupObject_->verticalAxis()->setRotatePosition(newSetpoint);
	}
}

void AMMotorGroupObjectView::onNormalTranslationIncrementClicked()
{
	if(motorGroupObject_->hasNormalAxis() &&
			motorGroupObject_->normalAxis()->canTranslate()) {

		double newSetpoint = motorGroupObject_->normalAxis()->currentTranslationPosition()
				+ jogSize_->value();

		motorGroupObject_->normalAxis()->setTranslatePosition(newSetpoint);
	}
}

void AMMotorGroupObjectView::onNormalTranslationDecrementClicked()
{
	if(motorGroupObject_->hasNormalAxis() &&
			motorGroupObject_->normalAxis()->canTranslate()) {

		double newSetpoint = motorGroupObject_->normalAxis()->currentTranslationPosition()
				- jogSize_->value();

		motorGroupObject_->normalAxis()->setTranslatePosition(newSetpoint);
	}
}

void AMMotorGroupObjectView::onNormalRotationIncrementClicked()
{
	if(motorGroupObject_->hasNormalAxis() &&
			motorGroupObject_->normalAxis()->canRotate()) {

		double newSetpoint = motorGroupObject_->normalAxis()->currentRotationPosition()
				+ jogSize_->value();

		motorGroupObject_->normalAxis()->setRotatePosition(newSetpoint);
	}
}

void AMMotorGroupObjectView::onNormalRotationDecrementClicked()
{
	if(motorGroupObject_->hasNormalAxis() &&
			motorGroupObject_->normalAxis()->canRotate()) {

		double newSetpoint = motorGroupObject_->normalAxis()->currentRotationPosition()
				- jogSize_->value();

		motorGroupObject_->normalAxis()->setRotatePosition(newSetpoint);
	}
}

void AMMotorGroupObjectView::onStopClicked()
{
	motorGroupObject_->stopAll();
}

void AMMotorGroupObjectView::onErrorMessageDismissClicked()
{
	errorLabel_->setVisible(false);
	dismissError_->setVisible(false);
}

void AMMotorGroupObjectView::setupUi()
{
	QVBoxLayout* mainLayout = new QVBoxLayout();

	// Initialize Widgets

	// Error Box
	errorLabel_ = new QLabel();
	errorLabel_->setStyleSheet("QLabel { color: red; } ");
	errorLabel_->setVisible(false);

	dismissError_ = new QToolButton();
	dismissError_->setIcon(QIcon(":/Close.png"));
	errorLabel_->setVisible(false);

	horizontalTranslationIncrement_ = new QToolButton();
	horizontalTranslationIncrement_->setIcon(QIcon(":/go-next.png"));

	horizontalTranslationDecrement_ = new QToolButton();
	horizontalTranslationDecrement_->setIcon(QIcon(":/go-previous.png"));

	horizontalRotationIncrement_ = new QToolButton();
	horizontalRotationIncrement_->setIcon(QIcon(":/ArrowCW.png"));

	horizontalRotationDecrement_ = new QToolButton();
	horizontalRotationDecrement_->setIcon(QIcon(":/ArrowCCW.png"));


	verticalTranslationIncrement_ = new QToolButton();
	verticalTranslationIncrement_->setIcon(QIcon(":/go-up.png"));

	verticalTranslationDecrement_ = new QToolButton();
	verticalTranslationDecrement_->setIcon(QIcon(":/go-down.png"));

	verticalRotationIncrement_ = new QToolButton();
	verticalRotationIncrement_->setIcon(QIcon(":/ArrowCW.png"));

	verticalRotationDecrement_ = new QToolButton();
	verticalRotationDecrement_->setIcon(QIcon(":/ArrowCCW.png"));


	normalTranslationIncrement_ = new QToolButton();
	normalTranslationIncrement_->setIcon(QIcon(":/go-in.png"));

	normalTranslationDecrement_ = new QToolButton();
	normalTranslationDecrement_->setIcon(QIcon(":/go-out.png"));

	normalRotationIncrement_ = new QToolButton();
	normalRotationIncrement_->setIcon(QIcon(":/ArrowCW.png"));

	normalRotationDecrement_ = new QToolButton();
	normalRotationDecrement_->setIcon(QIcon(":/ArrowCCW.png"));

	stopAllButton_ = new QToolButton();
	stopAllButton_->setIcon(QIcon(":/stop.png"));

	horizontalTranslationValue_ = new AMExtendedControlEditor(0);
	horizontalTranslationLabel_ = new QLabel();

	horizontalRotationValue_ = new AMExtendedControlEditor(0);
	horizontalRotationLabel_ = new QLabel();


	verticalTranslationValue_ = new AMExtendedControlEditor(0);
	verticalTranslationLabel_ = new QLabel();

	verticalRotationValue_ = new AMExtendedControlEditor(0);
	verticalRotationLabel_ = new QLabel();

	normalTranslationValue_ = new AMExtendedControlEditor(0);
	normalTranslationLabel_ = new QLabel();

	normalRotationValue_ = new AMExtendedControlEditor(0);
	normalRotationLabel_ = new QLabel();

	jogSize_ = new QDoubleSpinBox();

	statusLabel_ = new QLabel();
	statusLabel_->setPixmap(QIcon(":/32x32/greenLEDOff.png").pixmap(25));


	// Do Layout
	QHBoxLayout* contentLayout = new QHBoxLayout();
	mainLayout->addLayout(contentLayout);

	QGridLayout* movementButtonLayout = new QGridLayout();

	contentLayout->addLayout(movementButtonLayout);

	movementButtonLayout->addWidget(statusLabel_, 1, 1);
	movementButtonLayout->addWidget(horizontalTranslationIncrement_, 2, 3);
	movementButtonLayout->addWidget(horizontalTranslationDecrement_, 2, 1);
	movementButtonLayout->addWidget(horizontalRotationIncrement_, 2, 4);
	movementButtonLayout->addWidget(horizontalRotationDecrement_, 2, 0);	

	movementButtonLayout->addWidget(verticalTranslationIncrement_, 1, 2);
	movementButtonLayout->addWidget(verticalTranslationDecrement_, 3, 2);
	movementButtonLayout->addWidget(verticalRotationIncrement_, 0, 2);
	movementButtonLayout->addWidget(verticalRotationDecrement_, 4, 2);

	movementButtonLayout->addWidget(normalTranslationIncrement_, 1, 3);
	movementButtonLayout->addWidget(normalTranslationDecrement_, 3, 1);
	movementButtonLayout->addWidget(normalRotationIncrement_, 0, 4);
	movementButtonLayout->addWidget(normalRotationDecrement_, 4, 0);

	movementButtonLayout->addWidget(stopAllButton_, 2,2);

	QGridLayout* valuesLayout = new QGridLayout();
	contentLayout->addLayout(valuesLayout);
	valuesLayout->addWidget(horizontalTranslationLabel_, 0, 0);
	valuesLayout->addWidget(horizontalTranslationValue_, 0, 1);

	valuesLayout->addWidget(horizontalRotationLabel_, 1, 0);
	valuesLayout->addWidget(horizontalRotationValue_, 1, 1);

	valuesLayout->addWidget(verticalTranslationLabel_, 2, 0);
	valuesLayout->addWidget(verticalTranslationValue_, 2, 1);

	valuesLayout->addWidget(verticalRotationLabel_, 3, 0);
	valuesLayout->addWidget(verticalRotationValue_, 3, 1);

	valuesLayout->addWidget(normalTranslationLabel_, 4, 0);
	valuesLayout->addWidget(normalTranslationValue_, 4, 1);

	valuesLayout->addWidget(normalRotationLabel_, 5, 0);
	valuesLayout->addWidget(normalRotationValue_, 5, 1);

	valuesLayout->addWidget(new QLabel("Jog: "), 7, 0);
	valuesLayout->addWidget(jogSize_, 7, 1);

	QHBoxLayout* errorLayout = new QHBoxLayout();
	errorLayout->addWidget(errorLabel_);
	errorLayout->addStretch();
	errorLayout->addWidget(dismissError_);

	mainLayout->addStretch();
	contentLayout->addStretch();
	setLayout(mainLayout);
}

void AMMotorGroupObjectView::setupData()
{
	refreshJogUnits();
	jogSize_->setSingleStep(0.001);
	jogSize_->setMinimum(0.0000);
	jogSize_->setValue(0.050);
	jogSize_->setDecimals(3);
	jogSize_->setFixedWidth(110);

	bool canMoveGrouping = motorGroupObject_->hasHorizontalAxis() && motorGroupObject_->horizontalAxis()->rotationMotor();
	horizontalRotationIncrement_->setVisible(canMoveGrouping);
	horizontalRotationDecrement_->setVisible(canMoveGrouping);
	horizontalRotationValue_->setVisible(canMoveGrouping);
	horizontalRotationValue_->hideBorder();
	horizontalRotationLabel_->setVisible(canMoveGrouping);
	if(canMoveGrouping) {
		horizontalRotationValue_->setControl(motorGroupObject_->horizontalAxis()->rotationMotor());
		horizontalRotationValue_->setUnits(motorGroupObject_->horizontalAxis()->rotationPositionUnits());
		horizontalRotationLabel_->setText(motorGroupObject_->horizontalAxis()->rotationName());

	}

	canMoveGrouping = motorGroupObject_->hasHorizontalAxis() && motorGroupObject_->horizontalAxis()->translationMotor();
	horizontalTranslationIncrement_->setVisible(canMoveGrouping);
	horizontalTranslationDecrement_->setVisible(canMoveGrouping);
	horizontalTranslationValue_->setVisible(canMoveGrouping);
	horizontalTranslationValue_->hideBorder();
	horizontalTranslationLabel_->setVisible(canMoveGrouping);
	if(canMoveGrouping) {
		horizontalTranslationValue_->setControl(motorGroupObject_->horizontalAxis()->translationMotor());
		horizontalTranslationValue_->setUnits(motorGroupObject_->horizontalAxis()->translationPositionUnits());
		horizontalTranslationLabel_->setText(motorGroupObject_->horizontalAxis()->translationName());
	}

	canMoveGrouping = motorGroupObject_->hasVerticalAxis() && motorGroupObject_->verticalAxis()->rotationMotor();
	verticalRotationIncrement_->setVisible(canMoveGrouping);
	verticalRotationDecrement_->setVisible(canMoveGrouping);
	verticalRotationValue_->setVisible(canMoveGrouping);
	verticalRotationValue_->hideBorder();
	verticalRotationLabel_->setVisible(canMoveGrouping);
	if(canMoveGrouping) {
		verticalRotationValue_->setControl(motorGroupObject_->verticalAxis()->rotationMotor());
		verticalRotationValue_->setUnits(motorGroupObject_->verticalAxis()->rotationPositionUnits());
		verticalRotationLabel_->setText(motorGroupObject_->verticalAxis()->rotationName());
	}

	canMoveGrouping = motorGroupObject_->hasVerticalAxis() && motorGroupObject_->verticalAxis()->translationMotor();
	verticalTranslationIncrement_->setVisible(canMoveGrouping);
	verticalTranslationDecrement_->setVisible(canMoveGrouping);
	verticalTranslationValue_->setVisible(canMoveGrouping);
	verticalTranslationValue_->hideBorder();
	verticalTranslationLabel_->setVisible(canMoveGrouping);
	if(canMoveGrouping) {
		verticalTranslationValue_->setControl(motorGroupObject_->verticalAxis()->translationMotor());
		verticalTranslationValue_->setUnits(motorGroupObject_->verticalAxis()->translationPositionUnits());
		verticalTranslationLabel_->setText(motorGroupObject_->verticalAxis()->translationName());
	}

	canMoveGrouping = motorGroupObject_->hasNormalAxis() && motorGroupObject_->normalAxis()->rotationMotor();
	normalRotationIncrement_->setVisible(canMoveGrouping);
	normalRotationDecrement_->setVisible(canMoveGrouping);
	normalRotationValue_->setVisible(canMoveGrouping);
	normalRotationValue_->hideBorder();
	normalRotationLabel_->setVisible(canMoveGrouping);
	if(canMoveGrouping) {
		normalRotationValue_->setControl(motorGroupObject_->normalAxis()->rotationMotor());
		normalRotationValue_->setUnits(motorGroupObject_->normalAxis()->rotationPositionUnits());
		normalRotationLabel_->setText(motorGroupObject_->normalAxis()->rotationName());
	}

	canMoveGrouping = motorGroupObject_->hasNormalAxis() && motorGroupObject_->normalAxis()->translationMotor();
	normalTranslationIncrement_->setVisible(canMoveGrouping);
	normalTranslationDecrement_->setVisible(canMoveGrouping);
	normalTranslationValue_->setVisible(canMoveGrouping);
	normalTranslationValue_->hideBorder();
	normalTranslationLabel_->setVisible(canMoveGrouping);
	if(canMoveGrouping) {
		normalTranslationValue_->setControl(motorGroupObject_->normalAxis()->translationMotor());
		normalTranslationValue_->setUnits(motorGroupObject_->normalAxis()->translationPositionUnits());
		normalTranslationLabel_->setText(motorGroupObject_->normalAxis()->translationName());
	}
}


void AMMotorGroupObjectView::setupConnections()
{
	connect(motorGroupObject_, SIGNAL(connectedStateChanged(AMMotorGroupObject::MotionDirection,AMMotorGroupAxis::MotionType,bool)),
			this, SLOT(onConnectionStateChanged(AMMotorGroupObject::MotionDirection,AMMotorGroupAxis::MotionType,bool)));

	connect(motorGroupObject_, SIGNAL(motorError(AMMotorGroupObject::MotionDirection,AMMotorGroupAxis::MotionType,int)),
			this, SLOT(onMotorError(AMMotorGroupObject::MotionDirection,AMMotorGroupAxis::MotionType,int)));

	connect(motorGroupObject_, SIGNAL(movingStateChanged(AMMotorGroupObject::MotionDirection,AMMotorGroupAxis::MotionType,bool)),
			this, SLOT(onMovingStateChanged(AMMotorGroupObject::MotionDirection,AMMotorGroupAxis::MotionType,bool)));

	connect(motorGroupObject_, SIGNAL(positionUnitsChanged(AMMotorGroupObject::MotionDirection,AMMotorGroupAxis::MotionType,QString)),
			this, SLOT(onPositionUnitsChanged(AMMotorGroupObject::MotionDirection,AMMotorGroupAxis::MotionType,QString)));

	connect(horizontalTranslationIncrement_, SIGNAL(clicked()),
			this, SLOT(onHorizontalTranslationIncrementClicked()));

	connect(horizontalTranslationDecrement_, SIGNAL(clicked()),
			this, SLOT(onHorizontalTranslationDecrementClicked()));

	connect(horizontalRotationIncrement_, SIGNAL(clicked()),
			this, SLOT(onHorizontalRotationIncrementClicked()));

	connect(horizontalRotationDecrement_, SIGNAL(clicked()),
			this, SLOT(onHorizontalRotationDecrementClicked()));

	connect(verticalTranslationIncrement_, SIGNAL(clicked()),
			this, SLOT(onVerticalTranslationIncrementClicked()));

	connect(verticalTranslationDecrement_, SIGNAL(clicked()),
			this, SLOT(onVerticalTranslationDecrementClicked()));

	connect(verticalRotationIncrement_, SIGNAL(clicked()),
			this, SLOT(onVerticalRotationIncrementClicked()));

	connect(verticalRotationDecrement_, SIGNAL(clicked()),
			this, SLOT(onVerticalRotationDecrementClicked()));

	connect(normalTranslationIncrement_, SIGNAL(clicked()),
			this, SLOT(onNormalTranslationIncrementClicked()));

	connect(normalTranslationDecrement_, SIGNAL(clicked()),
			this, SLOT(onNormalTranslationDecrementClicked()));

	connect(normalRotationIncrement_, SIGNAL(clicked()),
			this, SLOT(onNormalRotationIncrementClicked()));

	connect(normalRotationDecrement_, SIGNAL(clicked()),
			this, SLOT(onNormalRotationDecrementClicked()));

	connect(stopAllButton_, SIGNAL(clicked()),
			this, SLOT(onStopClicked()));

}

void AMMotorGroupObjectView::refreshJogUnits()
{
	QString jogUnits;
	QHash<QString, bool> units;
	if(motorGroupObject_->hasHorizontalAxis()) {
		if(motorGroupObject_->horizontalAxis()->canRotate()) {
			units.insert(motorGroupObject_->horizontalAxis()->rotationPositionUnits(), false);
		}

		if(motorGroupObject_->horizontalAxis()->canTranslate()) {
			units.insert(motorGroupObject_->horizontalAxis()->translationPositionUnits(), false);
		}
	}

	if(motorGroupObject_->hasVerticalAxis()) {
		if(motorGroupObject_->verticalAxis()->canRotate()) {
			units.insert(motorGroupObject_->verticalAxis()->rotationPositionUnits(), false);
		}

		if(motorGroupObject_->verticalAxis()->canTranslate()) {
			units.insert(motorGroupObject_->verticalAxis()->translationPositionUnits(), false);
		}
	}

	if(motorGroupObject_->hasNormalAxis()) {
		if(motorGroupObject_->normalAxis()->canRotate()) {
			units.insert(motorGroupObject_->normalAxis()->rotationPositionUnits(), false);
		}

		if(motorGroupObject_->normalAxis()->canTranslate()) {
			units.insert(motorGroupObject_->normalAxis()->translationPositionUnits(), false);
		}
	}

	foreach (QString currentUnits, units.keys()) {
		if(jogUnits.isEmpty()) {
			jogUnits = currentUnits;
		} else {
				jogUnits.append(QString("/%1").arg(currentUnits));
		}
	}

	jogSize_->setSuffix(QString(" %1").arg(jogUnits));
}

void AMMotorGroupObjectView::errorHandleImplementation(AMMotorGroupObject::MotionDirection,
													   AMMotorGroupAxis::MotionType,
													   int)
{
	// Do nothing here. If subclasses want to do some custom things for the
	// error case, they can do so in their own override of this function.
}

////////////////////////////////////////////////////////////////////////////////
// Motor Group View
////////////////////////////////////////////////////////////////////////////////

AMMotorGroupView::AMMotorGroupView(AMMotorGroup* motorGroup,
								   ViewMode viewMode,
								   QWidget* parent)
	: QWidget(parent)
{
	motorGroup_ = motorGroup;
	viewMode_ = viewMode;

	if(motorGroup_) {
		if(viewMode == NormalView) {
			setupNormalUi();
		} else {
			setupCompactUi();
		}
	}
}

AMMotorGroupObject * AMMotorGroupView::selectedGroupObject() const
{
	if(currentSelectedGroupObjectName_.isEmpty()) {
		return 0;
	}

	return motorGroup_->motorGroupObject(currentSelectedGroupObjectName_);
}

void AMMotorGroupView::setSelectedGroupObject(const QString &groupObjectName)
{
	if(viewMode_ == NormalView) {

		int tabIndexOfGroupObject = motorGroupTabMap_.value(groupObjectName, -1);
		if(tabIndexOfGroupObject != -1) {
			groupObjectTabs_->setCurrentIndex(tabIndexOfGroupObject);
		}		

	} else {

		bool objectFound = false;
		for(int iItem = 0, itemCount = groupObjectSelector_->count();
			iItem < itemCount && !objectFound;
			++iItem) {

			if(groupObjectSelector_->itemText(iItem) == groupObjectName) {

				objectFound = true;
				if(groupObjectSelector_->currentIndex() != iItem) {
					groupObjectSelector_->setCurrentIndex(iItem);
				}
			}
		}
	}
}

void AMMotorGroupView::hideMotorGroupSelection()
{
	if (viewMode_ == CompactView) {
		groupObjectSelector_->setVisible(false);
	}
}

void AMMotorGroupView::onGroupObjectMotionStatusAltered(const QString& groupObjectName)
{
	AMMotorGroupObject* motorGroupObject = motorGroup_->motorGroupObject(groupObjectName);
	QIcon statusIcon;
	if(motorGroupObject->isMoving()) {
		statusIcon = QIcon(":/32x32/greenLEDOn.png").pixmap(25);
	} else {
		statusIcon = QIcon(":/32x32/greenLEDOff.png").pixmap(25);
	}

	int tabIndex = motorGroupTabMap_.value(groupObjectName, -1);

	if(tabIndex >= 0 ) {
		groupObjectTabs_->setTabIcon(tabIndex, statusIcon);
	}
}

void AMMotorGroupView::onGroupComboBoxIndexChanged(int index)
{
	groupObjectStack_->setCurrentIndex(index);

	QString nameOfSelectedGroup =
			groupObjectSelector_->itemText(index);

	currentSelectedGroupObjectName_ = nameOfSelectedGroup;
	emit currentMotorGroupObjectViewChanged(nameOfSelectedGroup);
}

void AMMotorGroupView::onGroupTabViewIndexChanged(int index)
{
	QString nameOfSelectedGroup =
			motorGroupTabMap_.key(index);

	currentSelectedGroupObjectName_ = nameOfSelectedGroup;
	emit currentMotorGroupObjectViewChanged(nameOfSelectedGroup);
}

void AMMotorGroupView::setupCompactUi()
{
	// Zero widgets we're not going to use:
	groupObjectTabs_ = 0;
	groupMotionStatusMapper_ = 0;

	// Set up the stuff we are going to use.
	QVBoxLayout* mainLayout = new QVBoxLayout();

	groupObjectSelector_ = new QComboBox();
	groupObjectStack_ = new QStackedWidget();

	foreach(AMMotorGroupObject* groupObject, motorGroup_->motorGroupObjects()) {

		AMMotorGroupObjectView* currentObjectView = new AMMotorGroupObjectView(groupObject);

		groupObjectSelector_->addItem(groupObject->name());
		groupObjectStack_->addWidget(currentObjectView);
	}

	connect(groupObjectSelector_, SIGNAL(currentIndexChanged(int)),
			this, SLOT(onGroupComboBoxIndexChanged(int)));


	if(groupObjectSelector_->count() > 0) {
		currentSelectedGroupObjectName_ = groupObjectSelector_->itemText(0);
	}

	mainLayout->addWidget(groupObjectSelector_);
	mainLayout->addWidget(groupObjectStack_);

	setLayout(mainLayout);
}

void AMMotorGroupView::setupNormalUi()
{
	// Zero widgets we're not going to use:
	groupObjectSelector_ = 0;
	groupObjectStack_ = 0;

	// Set up the stuff we are going to use.
	groupMotionStatusMapper_ = new QSignalMapper(this);

	groupObjectTabs_ = new QTabWidget();
	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(groupObjectTabs_);

	foreach(AMMotorGroupObject* currentObject, motorGroup_->motorGroupObjects()) {

		AMMotorGroupObjectView* currentObjectView = new AMMotorGroupObjectView(currentObject);

		QIcon tabIcon = QIcon(":/32x32/greenLEDOff.png").pixmap(25);
		int tabIndex =
				groupObjectTabs_->addTab(currentObjectView, tabIcon, currentObject->name());

		motorGroupTabMap_.insert(currentObject->name(), tabIndex);

		connect(currentObject, SIGNAL(movingStateChanged(AMMotorGroupObject::MotionDirection,AMMotorGroupAxis::MotionType,bool)),
				groupMotionStatusMapper_, SLOT(map()));

		groupMotionStatusMapper_->setMapping(currentObject, currentObject->name());
	}

	if(!motorGroupTabMap_.isEmpty()) {
		currentSelectedGroupObjectName_ = motorGroupTabMap_.key(0);
	}

	connect(groupMotionStatusMapper_, SIGNAL(mapped(const QString&)),
			this, SLOT(onGroupObjectMotionStatusAltered(const QString&)));

	connect(groupObjectTabs_, SIGNAL(currentChanged(int)),
			this, SLOT(onGroupTabViewIndexChanged(int)));

	setLayout(layout);
}



