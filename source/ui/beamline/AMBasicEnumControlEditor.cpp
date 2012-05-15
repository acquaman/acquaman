/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "AMBasicEnumControlEditor.h"

#include <QMenu>
#include <QApplication>
#include "beamline/AMControl.h"

AMBasicEnumControlEditor::AMBasicEnumControlEditor(AMControl *control, QWidget *parent) :
	QToolButton(parent), movingIcon_(":/16x16/view-refresh.png")
{
	setIcon(QIcon());
	setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

	control_ = control;
	menu_ = new QMenu(this);
	setMenu(menu_);
	setPopupMode(QToolButton::InstantPopup);
	connect(menu_, SIGNAL(triggered(QAction*)), this, SLOT(onMenuActionTriggered(QAction*)));

	connect(control_, SIGNAL(connected(bool)), this, SLOT(onControlConnected(bool)));
	connect(control_, SIGNAL(enumChanges(QStringList)), this, SLOT(onEnumChanges(QStringList)));
	connect(control_, SIGNAL(valueChanged(double)), this, SLOT(onControlValueChanged(double)));

	// initialize:
	onControlConnected(control_->isConnected());
	onEnumChanges(control_->enumNames());

}

void AMBasicEnumControlEditor::onControlConnected(bool connected)
{
	setEnabled(connected);

	if(connected) {
		onControlValueChanged(control_->value());
	}
	else {
		setText("[Not Connected]");
	}
}

void AMBasicEnumControlEditor::onControlValueChanged(double value)
{
	if(control_->isEnum()) {
		setText(control_->enumNameAt(value));
	}
	else {
		setText(QString("%1 %2").arg(value).arg(control_->units()));
	}
}

void AMBasicEnumControlEditor::onEnumChanges(const QStringList enumStates)
{
	// first, we might have different enum values now... Update the text:
	onControlValueChanged(control_->value());

	// Setup the actions for the drop-down menu
	menu_->clear();
	for(int i=0,cc=enumStates.count(); i<cc; i++) {
		QAction* action = new QAction(enumStates.at(i), this);
		action->setData(i);
		menu_->addAction(action);
	}
}

void AMBasicEnumControlEditor::onMenuActionTriggered(QAction *action)
{
	onNewEnumValueChosen(action->data().toInt());
}

void AMBasicEnumControlEditor::onNewEnumValueChosen(int value)
{
	if(control_->canMove()) {
		control_->move(value);
	}
	else {
		QApplication::beep();
	}
}

void AMBasicEnumControlEditor::onControlMovingChanged(bool isMoving)
{
	setIcon(isMoving ? movingIcon_ : QIcon());
}
