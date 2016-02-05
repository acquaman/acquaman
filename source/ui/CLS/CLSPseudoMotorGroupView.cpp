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


#include "CLSPseudoMotorGroupView.h"
#include <QMessageBox>
#include <QAction>
#include <QMenu>

CLSPseudoMotorGroupView::CLSPseudoMotorGroupView(CLSPseudoMotorGroup *motorGroup,
												 AMMotorGroupView::ViewMode viewMode,
												 QWidget *parent)
	: AMMotorGroupView(motorGroup, viewMode, parent)
{
	isInitialized_ = false;

	connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomContextMenuRequested(QPoint)));
	setContextMenuPolicy(Qt::CustomContextMenu);

	connect(this, SIGNAL(currentMotorGroupObjectViewChanged(QString)),
			this, SLOT(onGroupObjectViewChanged(QString)));
}

void CLSPseudoMotorGroupView::onCustomContextMenuRequested(const QPoint &pos)
{
	QMenu popup(this);
	QAction *temp;

	buildCLSPseudoMotorGroupMenuItems(&popup);

	temp = popup.exec(mapToGlobal(pos));

	if (temp){

		if (temp->text().contains("Reset"))
			resetCLSPseudoMotorGroupMenuItem();
	}
}

void CLSPseudoMotorGroupView::onGroupObjectViewChanged(const QString &groupObjectName)
{
	CLSPseudoMotorGroupObject* currentMotorGroupObject =
			qobject_cast<CLSPseudoMotorGroupObject*>(motorGroup_->motorGroupObject(groupObjectName));

	if(currentMotorGroupObject && isInitialized_) {
		recommendResettingCLSPseudoMotorGroupObject(currentMotorGroupObject);
	} else {
		isInitialized_ = true;
	}
}

void CLSPseudoMotorGroupView::buildCLSPseudoMotorGroupMenuItems(QMenu *menu)
{
	if(!selectedGroupObject()) {
		return;
	}

	QAction *action = menu->addAction(QString("Reset %1")
									  .arg(selectedGroupObject()->name()));

	action->setEnabled(qobject_cast<CLSPseudoMotorGroupObject *>(selectedGroupObject()) != 0);
}

void CLSPseudoMotorGroupView::resetCLSPseudoMotorGroupMenuItem()
{
	CLSPseudoMotorGroupObject *pseudoMotor = qobject_cast<CLSPseudoMotorGroupObject *>(selectedGroupObject());

	if(pseudoMotor) {
		pseudoMotor->resetPseudoMotor();
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
