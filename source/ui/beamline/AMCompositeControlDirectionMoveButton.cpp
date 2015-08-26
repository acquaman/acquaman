#include "AMCompositeControlDirectionMoveButton.h"
#include "beamline/AMControl.h"
#include "actions3/AMActionSupport.h"
#include "actions3/AMListAction3.h"


#include "AMQEvents.h"


#include <QBoxLayout>
#include <QStringBuilder>
#include <QString>
#include <QFrame>

AMCompositeControlDirectionMoveButton::AMCompositeControlDirectionMoveButton(QWidget *parent,
																			  AMControl* control, AMControl* control2, AMControl* controlR,
																			  QList<double> stepSizes,
																			  bool directionReversed, bool direction2Reversed ) :
	AMControlMoveButton(parent, control, stepSizes, directionReversed)
{
	direction2Reversed_ = direction2Reversed;

	control2_ = 0;
	setControl2(control2);

	controlR_ = 0;
	setControlR(controlR);

	checkStatus();

	setToolTip("Composite Angle Move");

}




void AMCompositeControlDirectionMoveButton::setControl(AMControl *control)
{
	if(control == control_)
		return;

	if(directionMoveContextMenu_) {
		directionMoveContextMenu_->deleteLater();
		directionMoveContextMenu_ = 0;
	}

	if(control_) {
		disconnect(control_, 0, this, 0);
	}

	control_ = control;
	if(control_) {
		connect(control_, SIGNAL(destroyed()), this, SLOT(onControlDestroyed()));
		connect(control_, SIGNAL(connected(bool)), this, SLOT(checkStatus()));
	}

	setText(QString::number(currentStepSize()) % (control_ ? control_->units() : QString()));

	// enabled / disabled:
	checkStatus();
}

void AMCompositeControlDirectionMoveButton::setControl2(AMControl *control2)
{
	if(control2 == control2_)
		return;

	if(directionMoveContextMenu_) {
		directionMoveContextMenu_->deleteLater();
		directionMoveContextMenu_ = 0;
	}

	if(control2_) {
		disconnect(control2_, 0, this, 0);
	}

	control2_ = control2;
	if(control_) {
		connect(control2_, SIGNAL(destroyed()), this, SLOT(onControl2Destroyed()));
		connect(control2_, SIGNAL(connected(bool)), this, SLOT(checkStatus()));
	}

	setText(QString::number(currentStepSize()) % (control_ ? control_->units() : QString()));

	// enabled / disabled:
	checkStatus();
}

void AMCompositeControlDirectionMoveButton::setControlR(AMControl *controlR)
{
	if(controlR == controlR_)
		return;

	if(directionMoveContextMenu_) {
		directionMoveContextMenu_->deleteLater();
		directionMoveContextMenu_ = 0;
	}

	if(controlR_) {
		disconnect(controlR_, 0, this, 0);
	}

	controlR_ = controlR;
	if(control_) {
		connect(controlR_, SIGNAL(destroyed()), this, SLOT(onControlRDestroyed()));
		connect(controlR_, SIGNAL(connected(bool)), this, SLOT(checkStatus()));
	}

	setText(QString::number(currentStepSize()) % (control_ ? control_->units() : QString()));

	// enabled / disabled:
	checkStatus();
}

void AMCompositeControlDirectionMoveButton::onButtonClicked()
{
	if(control_ && control2_ && controlR_) {
		control_->moveRelative(currentStepSize() * (directionReversed_ ? -1.0 : 1.0) * sin(controlR_->value()/57.297));
		control2_->moveRelative(currentStepSize() * (direction2Reversed_ ? -1.0 : 1.0) * cos(controlR_->value()/57.297));
	}

//This seems that it should be better, but it not really any more "parallel", and it through "not within tolerance" errors when it is within tollerance.
//	if(control_ && control2_ && controlR_) {
//		AMListAction3 *compositeMove  = new AMListAction3(new AMListActionInfo3("Sample Move",
//																					   "Sample Move in Plate Plane"),
//																 AMListAction3::Parallel);

//		compositeMove->addSubAction(AMActionSupport::buildControlMoveAction(control_, currentStepSize() * (directionReversed_ ? -1.0 : 1.0) * sin(controlR_->value()/57.297),true));
//		compositeMove->addSubAction(AMActionSupport::buildControlMoveAction(control2_, currentStepSize() * (directionReversed_ ? -1.0 : 1.0) * cos(controlR_->value()/57.297),true));

//		compositeMove->start();
//	}
}

void AMCompositeControlDirectionMoveButton::onControl2Destroyed()
{
	setControl2(0);
}
void AMCompositeControlDirectionMoveButton::onControlRDestroyed()
{
	setControlR(0);
}

void AMCompositeControlDirectionMoveButton::checkStatus()
{
	if(control_ && control_->isConnected() && control2_ && control2_->isConnected() && controlR_ && controlR_->isConnected())
		setEnabled(true);
	else
		setDisabled(true);

}

void AMCompositeControlDirectionMoveButton::onCustomContextMenuRequested(const QPoint &point)
{
	if(!directionMoveContextMenu_)
		directionMoveContextMenu_ = new AMCompositeControlMoveButtonContextMenu(this);

	directionMoveContextMenu_->move(mapToGlobal(point)-QPoint(6,6));
	directionMoveContextMenu_->show();
}





AMCompositeControlMoveButtonContextMenu::~AMCompositeControlMoveButtonContextMenu(){}
AMCompositeControlMoveButtonContextMenu::AMCompositeControlMoveButtonContextMenu(AMControlMoveButton *moveButton)
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

   contentsFrame->setLayout(hl);
   vl->addWidget(contentsFrame);
   setLayout(vl);
}

void AMCompositeControlMoveButtonContextMenu::onStepSizeIndexChanged(int newIndex)
{
   QAbstractButton* currentButton = stepSizeButtons_.button(newIndex);
   if(currentButton)
	   currentButton->setChecked(true);
}


void AMCompositeControlMoveButtonContextMenu::leaveEvent(QEvent *e)
{
   hide();
   QDialog::leaveEvent(e);
}

