#include "CLSPseudoMotorGroupView.h"

#include "beamline/CLS/CLSPseudoMotorGroup.h"

#include <QMessageBox>
#include <QMenu>

// CLSPseudoMotorGroupObjectView
////////////////////////////////////////////////////

 CLSPseudoMotorGroupObjectView::~CLSPseudoMotorGroupObjectView(){}
CLSPseudoMotorGroupObjectView::CLSPseudoMotorGroupObjectView(CLSPseudoMotorGroupObject *motorGroupObject, QWidget *parent)
	: AMMotorGroupObjectView(motorGroupObject, parent)
{

}

// CLSPseudoMotorGroupView
////////////////////////////////////////////////////

 CLSPseudoMotorGroupView::~CLSPseudoMotorGroupView(){}
CLSPseudoMotorGroupView::CLSPseudoMotorGroupView(CLSPseudoMotorGroup *motorGroup, QWidget *parent)
	: AMMotorGroupView(motorGroup, parent)
{
	initialized_ = false;
	connect(this, SIGNAL(currentMotorGroupObjectViewChanged(QString)), this, SLOT(onNewExclusiveViewMotorGroupViewChange(QString)));
	connect(this, SIGNAL(motorGroupVisibilityChanged(QString)), this, SLOT(onNewMultipleViewMotorGroupViewChange(QString)));
}

CLSPseudoMotorGroupView::CLSPseudoMotorGroupView(CLSPseudoMotorGroup *motorGroup, ViewMode viewMode, QWidget *parent)
	: AMMotorGroupView(motorGroup, viewMode, parent)
{
	initialized_ = false;
	connect(this, SIGNAL(currentMotorGroupObjectViewChanged(QString)), this, SLOT(onNewExclusiveViewMotorGroupViewChange(QString)));
	connect(this, SIGNAL(motorGroupVisibilityChanged(QString)), this, SLOT(onNewMultipleViewMotorGroupViewChange(QString)));
}

void CLSPseudoMotorGroupView::onCustomContextMenuRequested(const QPoint &pos)
{
	QMenu popup(this);
	QAction *temp;

	buildCLSPseudoMotorGroupMenuItems(&popup);
	popup.addSeparator();
	buildStandardMenuItems(&popup);

	temp = popup.exec(mapToGlobal(pos));

	if (temp){

		if (temp->text().contains("Reset"))
			resetCLSPseudoMotorGroupMenuItems(temp->text());

		else
			handleStandardMenuItems(temp->text());
	}
}

void CLSPseudoMotorGroupView::onNewExclusiveViewMotorGroupViewChange(const QString &name)
{
	Q_UNUSED(name);

	CLSPseudoMotorGroupObject *pseudoMotorObject = qobject_cast<CLSPseudoMotorGroupObject *>(currentMotorGroupObjectView()->motorGroupObject());

	if (pseudoMotorObject && initialized_)
		recommendResettingCLSPseudoMotorGroupObject(pseudoMotorObject);

	else
		initialized_ = true;
}

void CLSPseudoMotorGroupView::onNewMultipleViewMotorGroupViewChange(const QString &name)
{
	int newIndex = visibleMotorGroupObjectNames().indexOf(name);

	if (visibleMotorGroupObjectViews().at(newIndex)->isVisible()){

		CLSPseudoMotorGroupObject *pseudoMotorObject = qobject_cast<CLSPseudoMotorGroupObject *>(visibleMotorGroupObjectViews().at(newIndex)->motorGroupObject());

		if (pseudoMotorObject && initialized_)
			recommendResettingCLSPseudoMotorGroupObject(pseudoMotorObject);

		else
			initialized_ = true;
	}
}

void CLSPseudoMotorGroupView::recommendResettingCLSPseudoMotorGroupObject(CLSPseudoMotorGroupObject *motorObject)
{
	QMessageBox message(this);
	message.setWindowTitle("New Pseudo-Motor Selected");
	message.setText("The new motor group you've selected is not referenced to current position of the motors.  It is recommended to reset the motors, otherwise the result of the first move will be unexpected.");
	message.setIcon(QMessageBox::Warning);
	message.addButton("Reset Now", QMessageBox::AcceptRole);
	message.addButton("Later", QMessageBox::RejectRole);
	message.exec();

	if (message.clickedButton()->text() == "Reset Now")
		motorObject->resetPseudoMotor();
}

void CLSPseudoMotorGroupView::buildCLSPseudoMotorGroupMenuItems(QMenu *menu)
{
	if (viewMode_ == Exclusive){

		QAction *action = menu->addAction(QString("Reset %1").arg(currentMotorGroupObjectView()->motorGroupObject()->name()));
		action->setEnabled(qobject_cast<CLSPseudoMotorGroupObject *>(currentMotorGroupObjectView()->motorGroupObject()) != 0);
	}

	else if (viewMode_ == Multiple){

		QAction *action = menu->addAction("Reset All Selected Motors");
		bool containsPseudoMotors = false;

		foreach (AMMotorGroupObjectView *view, visibleMotorGroupObjectViews())
			if (qobject_cast<CLSPseudoMotorGroupObject *>(view->motorGroupObject()))
				containsPseudoMotors = true;

		action->setEnabled(containsPseudoMotors);
	}
}

void CLSPseudoMotorGroupView::resetCLSPseudoMotorGroupMenuItems(const QString &command)
{
	Q_UNUSED(command)

	if (viewMode_ == Exclusive && qobject_cast<CLSPseudoMotorGroupObject *>(currentMotorGroupObjectView()->motorGroupObject())){

		CLSPseudoMotorGroupObject *pseudoMotor = qobject_cast<CLSPseudoMotorGroupObject *>(currentMotorGroupObjectView()->motorGroupObject());
		pseudoMotor->resetPseudoMotor();
	}

	else if (viewMode_ == Multiple){

		foreach (AMMotorGroupObjectView *view, visibleMotorGroupObjectViews()){

			if (qobject_cast<CLSPseudoMotorGroupObject *>(view->motorGroupObject())){

				CLSPseudoMotorGroupObject *pseudoMotor = qobject_cast<CLSPseudoMotorGroupObject *>(currentMotorGroupObjectView()->motorGroupObject());
				pseudoMotor->resetPseudoMotor();
			}
		}
	}
}
