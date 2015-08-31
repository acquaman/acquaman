#include "AMXYThetaControlMoveButton.h"
#include "beamline/AMControl.h"
#include "actions3/AMActionSupport.h"
#include "actions3/AMListAction3.h"


#include "AMQEvents.h"


#include <QBoxLayout>
#include <QStringBuilder>
#include <QString>
#include <QFrame>

AMXYThetaControlMoveButton::AMXYThetaControlMoveButton(QWidget *parent,
													   AMControl* controlX,
													   AMControl* controlY,
													   AMControl* controlTheta,
													   QList<double> stepSizes,
													   bool directionReversed,
													   bool direction2Reversed,
													   double angleOffset) : QToolButton(parent)
{
	directionXReversed_ = directionReversed;
	directionYReversed_ = direction2Reversed;

	angleOffset_ = angleOffset;

	controlX_ = 0;
	setControlX(controlX);

	controlY_ = 0;
	setControlY(controlY);

	controlTheta_ = 0;
	setControlTheta(controlTheta);

	checkStatus();

	setToolTip("XYTheta Move");

	stepSizes_ = stepSizes;
	if(stepSizes_.isEmpty())
		stepSizes_ << 1.0;	// just to make sure we always have at least one, so that stepSizeIndex_ is valid.
	stepSizeIndex_ = 0;

	contextMenu_ = 0;
	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomContextMenuRequested(QPoint)));

	setText(QString::number(currentStepSize()) % (controlX_ ? controlX_->units() : QString()));
	connect(this, SIGNAL(clicked()), this, SLOT(onButtonClicked()));

}




void AMXYThetaControlMoveButton::setControlX(AMControl *control)
{
	if(control == controlX_)
		return;

	if(contextMenu_) {
		contextMenu_->deleteLater();
		contextMenu_ = 0;
	}

	if(controlX_) {
		disconnect(controlX_, 0, this, 0);
	}

	controlX_ = control;
	if(controlX_) {
		connect(controlX_, SIGNAL(destroyed()), this, SLOT(onControlXDestroyed()));
		connect(controlX_, SIGNAL(connected(bool)), this, SLOT(checkStatus()));
	}

	// enabled / disabled:
	checkStatus();
}

void AMXYThetaControlMoveButton::setControlY(AMControl *control2)
{
	if(control2 == controlY_)
		return;

	if(contextMenu_) {
		contextMenu_->deleteLater();
		contextMenu_ = 0;
	}

	if(controlY_) {
		disconnect(controlY_, 0, this, 0);
	}

	controlY_ = control2;
	if(controlY_) {
		connect(controlY_, SIGNAL(destroyed()), this, SLOT(onControlYDestroyed()));
		connect(controlY_, SIGNAL(connected(bool)), this, SLOT(checkStatus()));
	}

	// enabled / disabled:
	checkStatus();
}

void AMXYThetaControlMoveButton::setControlTheta(AMControl *controlTheta)
{
	if(controlTheta == controlTheta_)
		return;

	if(contextMenu_) {
		contextMenu_->deleteLater();
		contextMenu_ = 0;
	}

	if(controlTheta_) {
		disconnect(controlTheta_, 0, this, 0);
	}

	controlTheta_ = controlTheta;
	if(controlTheta_) {
		connect(controlTheta_, SIGNAL(destroyed()), this, SLOT(onControlThetaDestroyed()));
		connect(controlTheta_, SIGNAL(connected(bool)), this, SLOT(checkStatus()));
	}

	// enabled / disabled:
	checkStatus();
}

void AMXYThetaControlMoveButton::onButtonClicked()
{
	if(controlX_ && controlY_ && controlTheta_) {
		controlX_->moveRelative(currentStepSize() * (directionXReversed_ ? -1.0 : 1.0) * sin((controlTheta_->value() + angleOffset_)/57.297));
		controlY_->moveRelative(currentStepSize() * (directionYReversed_ ? -1.0 : 1.0) * cos((controlTheta_->value() + angleOffset_)/57.297));
	}
}


void AMXYThetaControlMoveButton::onControlXDestroyed()
{
	setControlX(0);
}
void AMXYThetaControlMoveButton::onControlYDestroyed()
{
	setControlY(0);
}
void AMXYThetaControlMoveButton::onControlThetaDestroyed()
{
	setControlTheta(0);
}

void AMXYThetaControlMoveButton::checkStatus()
{
	if(controlX_ && controlX_->isConnected() && controlY_ && controlY_->isConnected() && controlTheta_ && controlTheta_->isConnected())
		setEnabled(true);
	else
		setDisabled(true);

}

bool AMXYThetaControlMoveButton::setStepSizeIndex(int index)
{
	if(index<0 || index >= stepSizes_.count())
		return false;
	if(stepSizeIndex_ != index) {
		emit stepSizeIndexChanged(stepSizeIndex_ = index);
	}
	setText(QString::number(currentStepSize()) % (controlX_ ? controlX_->units() : QString()));
	return true;
}

bool AMXYThetaControlMoveButton::setStepSizes(const QList<double> &stepSizes)
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

	setText(QString::number(currentStepSize()) % (controlX_ ? controlX_->units() : QString()));

	return true;
}


void AMXYThetaControlMoveButton::onCustomContextMenuRequested(const QPoint &point)
{
	if(!contextMenu_)
		contextMenu_ = new AMXYThetaControlMoveButtonContextMenu(this);

	contextMenu_->move(mapToGlobal(point)-QPoint(6,6));
	contextMenu_->show();
}

void AMXYThetaControlMoveButton::press(bool down)
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



AMXYThetaControlMoveButtonContextMenu::~AMXYThetaControlMoveButtonContextMenu(){}
AMXYThetaControlMoveButtonContextMenu::AMXYThetaControlMoveButtonContextMenu(AMXYThetaControlMoveButton *moveButton)
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

void AMXYThetaControlMoveButtonContextMenu::onStepSizeIndexChanged(int newIndex)
{
   QAbstractButton* currentButton = stepSizeButtons_.button(newIndex);
   if(currentButton)
	   currentButton->setChecked(true);
}


void AMXYThetaControlMoveButtonContextMenu::leaveEvent(QEvent *e)
{
   hide();
   QDialog::leaveEvent(e);
}


