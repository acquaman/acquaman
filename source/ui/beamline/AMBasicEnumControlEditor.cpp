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
