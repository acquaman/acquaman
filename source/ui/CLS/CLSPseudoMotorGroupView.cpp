#include "CLSPseudoMotorGroupView.h"

#include "beamline/CLS/CLSPseudoMotorGroup.h"

#include <QMenu>

// CLSPseudoMotorGroupObjectView
////////////////////////////////////////////////////

CLSPseudoMotorGroupObjectView::CLSPseudoMotorGroupObjectView(CLSPseudoMotorGroupObject *motorGroupObject, QWidget *parent)
	: AMMotorGroupObjectView(motorGroupObject, parent)
{

}

// CLSPseudoMotorGroupView
////////////////////////////////////////////////////

CLSPseudoMotorGroupView::CLSPseudoMotorGroupView(CLSPseudoMotorGroup *motorGroup, QWidget *parent)
	: AMMotorGroupView(motorGroup, parent)
{
}

CLSPseudoMotorGroupView::CLSPseudoMotorGroupView(CLSPseudoMotorGroup *motorGroup, ViewMode viewMode, QWidget *parent)
	: AMMotorGroupView(motorGroup, viewMode, parent)
{

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
			handleCLSPseudoMotorGroupMenuItems(temp->text());

		else
			handleStandardMenuItems(temp->text());
	}
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

void CLSPseudoMotorGroupView::handleCLSPseudoMotorGroupMenuItems(const QString &command)
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
