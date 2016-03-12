#include "BioXASControlButtonBar.h"
#include "ui/BioXAS/BioXASControlButton.h"

BioXASControlButtonBar::BioXASControlButtonBar(QWidget *parent) :
	BioXASButtonBar(parent)
{
	selectedControl_ = 0;

	connect( this, SIGNAL(selectedButtonChanged(QAbstractButton*)), this, SLOT(updateSelectedControl()) );
}

BioXASControlButtonBar::~BioXASControlButtonBar()
{

}

void BioXASControlButtonBar::addControl(AMControl *control, double greenValue)
{
	QAbstractButton *button = createButton(control, greenValue);

	if (button) {
		controlButtonMap_.insert(control, button);
		addButton(button);
	}
}

void BioXASControlButtonBar::removeControl(AMControl *control)
{
	QAbstractButton *button = controlButtonMap_.value(control, 0);

	if (button) {
		controlButtonMap_.remove(control);
		removeButton(button);
	}
}

void BioXASControlButtonBar::clearControls()
{
	QList<AMControl*> controls = controlButtonMap_.keys();

	foreach (AMControl *control, controls)
		removeControl(control);
}

QAbstractButton* BioXASControlButtonBar::createButton(AMControl *control, double greenValue)
{
	BioXASControlButton *button = new BioXASControlButton(control);
	button->setGreenValue(greenValue);

	return button;
}

void BioXASControlButtonBar::setSelectedControl(AMControl *control)
{
	if (selectedControl_ != control) {
		selectedControl_ = control;
		emit selectedControlChanged(selectedControl_);
	}
}

void BioXASControlButtonBar::updateSelectedControl()
{
	AMControl *selectedControl = controlButtonMap_.key(selectedButton(), 0);
	setSelectedControl(selectedControl);
}
