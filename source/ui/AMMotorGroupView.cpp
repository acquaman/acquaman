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
	}
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
	// Add error message to console log.
	QString axisName;
	if(motionType == AMMotorGroupAxis::RotationalMotion) {
		axisName = motorGroupObject_->axis(direction)->rotationName();
	} else {
		axisName = motorGroupObject_->axis(direction)->translationName();
	}

	logConsole_->append(QString("\n%1: %2")
						.arg(axisName)
						.arg(AMControl::failureExplanation(errorCode)));

	// In the case of a limit failure, we disable the offending motor.
	if(AMControl::FailureExplanation(errorCode) == AMControl::LimitFailure) {
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
	}

	// See if we're a subclass that needs to do anything else.
	errorHandleImplementation(direction, motionType, errorCode);
}

void AMMotorGroupObjectView::onMovingStateChanged(AMMotorGroupObject::MotionDirection direction,
												  AMMotorGroupAxis::MotionType motionType,
												  bool isMoving)
{
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
			verticalRotationValue_->setSuffix(positionUnits);
		} else {
			verticalTranslationValue_->setSuffix(positionUnits);
		}
		break;
	case AMMotorGroupObject::HorizontalMotion:
		if(motionType == AMMotorGroupAxis::RotationalMotion) {
			horizontalRotationValue_->setSuffix(positionUnits);
		} else {
			horizontalTranslationValue_->setSuffix(positionUnits);
		}
		break;
	case AMMotorGroupObject::NormalMotion:
		if(motionType == AMMotorGroupAxis::RotationalMotion) {
			normalRotationValue_->setSuffix(positionUnits);
		} else {
			normalTranslationValue_->setSuffix(positionUnits);
		}
		break;
	}
}

void AMMotorGroupObjectView::onPositionValueChanged(AMMotorGroupObject::MotionDirection direction,
													AMMotorGroupAxis::MotionType motionType,
													double positionValue)
{
		switch (direction) {
		case AMMotorGroupObject::VerticalMotion:
			if(motionType == AMMotorGroupAxis::RotationalMotion) {
				verticalRotationValue_->setValue(positionValue);
			} else {
				verticalTranslationValue_->setValue(positionValue);
			}
			break;
		case AMMotorGroupObject::HorizontalMotion:
			if(motionType == AMMotorGroupAxis::RotationalMotion) {
				horizontalRotationValue_->setValue(positionValue);
			} else {
				horizontalTranslationValue_->setValue(positionValue);
			}
			break;
		case AMMotorGroupObject::NormalMotion:
			if(motionType == AMMotorGroupAxis::RotationalMotion) {
				normalRotationValue_->setValue(positionValue);
			} else {
				normalTranslationValue_->setValue(positionValue);
			}
			break;
		}
}

void AMMotorGroupObjectView::onShowLogButtonClicked()
{
	logConsole_->setVisible(!logConsole_->isVisible());
}

void AMMotorGroupObjectView::setupUi()
{
	QVBoxLayout* mainLayout = new QVBoxLayout();

	// Title Label
	QFont titleFont;
	titleFont.setPointSize(18);
	titleFont.setBold(true);

	titleLabel_ = new QLabel();
	titleLabel_->setFont(titleFont);

	mainLayout->addWidget(titleLabel_);

	// Error Box
	logConsole_ = new QTextEdit();
	logConsole_->setReadOnly(true);
	logConsole_->setVisible(false);

	showLogButton_ = new QToolButton();
	showLogButton_->setIcon(QIcon(":/accessories-text-editor.png"));
	showLogButton_->setToolTip("Show/Hide Log");

	// Initialize Widgets
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

	horizontalTranslationValue_ = new QDoubleSpinBox();
	horizontalTranslationLabel_ = new QLabel();
	horizontalRotationValue_ = new QDoubleSpinBox();
	horizontalRotationLabel_ = new QLabel();

	verticalTranslationValue_ = new QDoubleSpinBox();
	verticalTranslationLabel_ = new QLabel();
	verticalRotationValue_ = new QDoubleSpinBox();
	verticalRotationLabel_ = new QLabel();

	normalTranslationValue_ = new QDoubleSpinBox();
	normalTranslationLabel_ = new QLabel();
	normalRotationValue_ = new QDoubleSpinBox();
	normalRotationLabel_ = new QLabel();

	// Do Layout
	QHBoxLayout* contentLayout = new QHBoxLayout();
	mainLayout->addLayout(contentLayout);

	QGridLayout* movementButtonLayout = new QGridLayout();

	contentLayout->addLayout(movementButtonLayout);

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

	mainLayout->addWidget(showLogButton_);
	mainLayout->addWidget(logConsole_);

	mainLayout->addStretch();
	contentLayout->addStretch();
	setLayout(mainLayout);
}

void AMMotorGroupObjectView::setupData()
{
	titleLabel_->setText(motorGroupObject_->name());

	bool canMoveGrouping = motorGroupObject_->hasHorizontalAxis() && motorGroupObject_->horizontalAxis()->canRotate();
	horizontalRotationIncrement_->setVisible(canMoveGrouping);
	horizontalRotationDecrement_->setVisible(canMoveGrouping);
	horizontalRotationValue_->setVisible(canMoveGrouping);
	horizontalRotationLabel_->setVisible(canMoveGrouping);
	if(canMoveGrouping) {
		double rotationValue =
				motorGroupObject_->horizontalAxis()->currentRotationPosition();
		QString suffix =
				motorGroupObject_->horizontalAxis()->rotationPositionUnits();
		QString name =
				motorGroupObject_->horizontalAxis()->rotationName();

		horizontalRotationValue_->setValue(rotationValue);
		horizontalRotationValue_->setSuffix(suffix);
		horizontalRotationLabel_->setText(name);
	}

	canMoveGrouping = motorGroupObject_->hasHorizontalAxis() && motorGroupObject_->horizontalAxis()->canTranslate();
	horizontalTranslationIncrement_->setVisible(canMoveGrouping);
	horizontalTranslationDecrement_->setVisible(canMoveGrouping);
	horizontalTranslationValue_->setVisible(canMoveGrouping);
	horizontalTranslationLabel_->setVisible(canMoveGrouping);
	if(canMoveGrouping) {
		double rotationValue =
				motorGroupObject_->horizontalAxis()->currentTranslationPosition();
		QString suffix =
				motorGroupObject_->horizontalAxis()->translationPositionUnits();
		QString name =
				motorGroupObject_->horizontalAxis()->translationName();

		horizontalTranslationValue_->setValue(rotationValue);
		horizontalTranslationValue_->setSuffix(suffix);
		horizontalTranslationLabel_->setText(name);
	}

	canMoveGrouping = motorGroupObject_->hasVerticalAxis() && motorGroupObject_->verticalAxis()->canRotate();
	verticalRotationIncrement_->setVisible(canMoveGrouping);
	verticalRotationDecrement_->setVisible(canMoveGrouping);
	verticalRotationValue_->setVisible(canMoveGrouping);
	verticalRotationLabel_->setVisible(canMoveGrouping);
	if(canMoveGrouping) {
		double rotationValue =
				motorGroupObject_->verticalAxis()->currentRotationPosition();
		QString suffix =
				motorGroupObject_->verticalAxis()->rotationPositionUnits();
		QString name =
				motorGroupObject_->verticalAxis()->rotationName();

		verticalRotationValue_->setValue(rotationValue);
		verticalRotationValue_->setSuffix(suffix);
		verticalRotationLabel_->setText(name);
	}

	canMoveGrouping = motorGroupObject_->hasVerticalAxis() && motorGroupObject_->verticalAxis()->canTranslate();
	verticalTranslationIncrement_->setVisible(canMoveGrouping);
	verticalTranslationDecrement_->setVisible(canMoveGrouping);
	verticalTranslationValue_->setVisible(canMoveGrouping);
	verticalTranslationLabel_->setVisible(canMoveGrouping);
	if(canMoveGrouping) {
		double rotationValue =
				motorGroupObject_->verticalAxis()->currentTranslationPosition();
		QString suffix =
				motorGroupObject_->verticalAxis()->translationPositionUnits();
		QString name =
				motorGroupObject_->verticalAxis()->translationName();

		verticalTranslationValue_->setValue(rotationValue);
		verticalTranslationValue_->setSuffix(suffix);
		verticalTranslationLabel_->setText(name);
	}

	canMoveGrouping = motorGroupObject_->hasNormalAxis() && motorGroupObject_->normalAxis()->canRotate();
	normalRotationIncrement_->setVisible(canMoveGrouping);
	normalRotationDecrement_->setVisible(canMoveGrouping);
	normalRotationValue_->setVisible(canMoveGrouping);
	normalRotationLabel_->setVisible(canMoveGrouping);
	if(canMoveGrouping) {
		double rotationValue =
				motorGroupObject_->normalAxis()->currentRotationPosition();
		QString suffix =
				motorGroupObject_->normalAxis()->rotationPositionUnits();
		QString name =
				motorGroupObject_->normalAxis()->rotationName();

		normalRotationValue_->setValue(rotationValue);
		normalRotationValue_->setSuffix(suffix);
		normalRotationLabel_->setText(name);
	}

	canMoveGrouping = motorGroupObject_->hasNormalAxis() && motorGroupObject_->normalAxis()->canTranslate();
	normalTranslationIncrement_->setVisible(canMoveGrouping);
	normalTranslationDecrement_->setVisible(canMoveGrouping);
	normalTranslationValue_->setVisible(canMoveGrouping);
	normalTranslationLabel_->setVisible(canMoveGrouping);
	if(canMoveGrouping) {
		double rotationValue =
				motorGroupObject_->normalAxis()->currentTranslationPosition();
		QString suffix =
				motorGroupObject_->normalAxis()->translationPositionUnits();
		QString name =
				motorGroupObject_->normalAxis()->translationName();

		normalTranslationValue_->setValue(rotationValue);
		normalTranslationValue_->setSuffix(suffix);
		normalTranslationLabel_->setText(name);
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

	connect(motorGroupObject_, SIGNAL(positionValueChanged(AMMotorGroupObject::MotionDirection,AMMotorGroupAxis::MotionType,double)),
			this, SLOT(onPositionValueChanged(AMMotorGroupObject::MotionDirection,AMMotorGroupAxis::MotionType,double)));

	connect(showLogButton_, SIGNAL(clicked()),
			this, SLOT(onShowLogButtonClicked()));

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
								   QWidget* parent)
	: QWidget(parent)
{


	motorGroup_ = motorGroup;

	groupMotionStatusMapper_ = new QSignalMapper(this);

	groupObjectTabs_ = new QTabWidget();
	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(groupObjectTabs_);

	foreach(AMMotorGroupObject* currentObject, motorGroup->motorGroupObjects()) {
		AMMotorGroupObjectView* currentObjectView = new AMMotorGroupObjectView(currentObject);

		QIcon tabIcon = QIcon(":/OFF.png");
		int tabIndex =
				groupObjectTabs_->addTab(currentObjectView, tabIcon, currentObject->name());

		motorGroupTabMap_.insert(currentObject->name(), tabIndex);

		connect(currentObject, SIGNAL(motorError(AMMotorGroupObject::MotionDirection,AMMotorGroupAxis::MotionType,int)),
				groupMotionStatusMapper_, SLOT(map()));

		groupMotionStatusMapper_->setMapping(currentObject, currentObject->name());
	}

	connect(groupMotionStatusMapper_, SIGNAL(mapped(const QString&)),
			this, SLOT(onGroupObjectMotionStatusAltered(const QString&)));

	setLayout(layout);
}

void AMMotorGroupView::onGroupObjectMotionStatusAltered(const QString& groupObjectName)
{
	AMMotorGroupObject* motorGroupObject = motorGroup_->motorGroupObject(groupObjectName);
	QIcon statusIcon;
	if(motorGroupObject->isMoving()) {
		statusIcon = QIcon(":/ON.png");
	} else {
		statusIcon = QIcon(":/OFF.png");
	}

	int tabIndex = motorGroupTabMap_.value(groupObjectName, -1);

	if(tabIndex >= 0 ) {
		groupObjectTabs_->setTabIcon(tabIndex, statusIcon);
	}
}
