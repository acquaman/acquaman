#include "CLSControlButtonBar.h"

#include "ui/CLS/CLSControlButton.h"

CLSControlButtonBar::CLSControlButtonBar(CLSButtonBar::SelectionMode selectionMode, QWidget *parent) :
	CLSButtonBar(selectionMode, parent)
{
	selectedControl_ = 0;

	connect( this, SIGNAL(selectedButtonChanged(QAbstractButton*)), this, SLOT(updateSelectedControl()) );
}

CLSControlButtonBar::~CLSControlButtonBar()
{
	clearControls();
}

void CLSControlButtonBar::addControl(AMControl *control, double greenValue)
{
	QAbstractButton *button = createButton(control, greenValue);

	if (button) {
		controlButtonMap_.insert(control, button);
		addButton(button);
	}
}

void CLSControlButtonBar::removeControl(AMControl *control)
{
	QAbstractButton *button = controlButtonMap_.value(control, 0);

	if (button) {
		controlButtonMap_.remove(control);
		removeButton(button);
	}
}

void CLSControlButtonBar::clearControls()
{
	QList<AMControl*> controls = controlButtonMap_.keys();

	foreach (AMControl *control, controls)
		removeControl(control);
}

QAbstractButton* CLSControlButtonBar::createButton(AMControl *control, double greenValue)
{
	CLSControlButton *button = new CLSControlButton(control);
	button->setGreenValue(greenValue);
	return button;
}

void CLSControlButtonBar::setSelectedControl(AMControl *control)
{
	if (selectedControl_ != control && controlButtonMap_.contains(control)) {
		selectedControl_ = control;
		emit selectedControlChanged(selectedControl_);
	}
}

void CLSControlButtonBar::updateSelectedControl()
{
	AMControl *selectedControl = controlButtonMap_.key(selectedButton(), 0);
	setSelectedControl(selectedControl);
}

void CLSControlButtonBar::onButtonClicked(QAbstractButton *clickedButton)
{
	CLSButtonBar::onButtonClicked(clickedButton);

	if (clickedButton && controlButtonMap_.values().contains(clickedButton))
		emit controlClicked(controlButtonMap_.key(clickedButton, 0));
}
