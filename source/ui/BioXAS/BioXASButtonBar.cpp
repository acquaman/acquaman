#include "BioXASButtonBar.h"

BioXASButtonBar::BioXASButtonBar(QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables.

	selectedButton_ = 0;

	buttonsGroup_ = new QButtonGroup(this);
	buttonsGroup_->setExclusive(false); // Exclusive must be turned off to have 'deselect when clicked' capability.

	connect( buttonsGroup_, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(onButtonClicked(QAbstractButton*)) );

	// Create and set main layout.

	buttonsLayout_ = new QHBoxLayout();
	buttonsLayout_->setMargin(0);

	setLayout(buttonsLayout_);
}

BioXASButtonBar::~BioXASButtonBar()
{

}

void BioXASButtonBar::addButton(QAbstractButton *newButton)
{
	if (newButton) {

		// Add the new button to the buttons group and the buttons layout.

		buttonsGroup_->addButton(newButton, buttonsGroup_->buttons().count());
		buttonsLayout_->addWidget(newButton);
	}
}

void BioXASButtonBar::removeButton(QAbstractButton *button)
{
	if (button) {

		// Check if the button to remove is the selected button.
		// If so, set the selected button to 0.

		if (selectedButton_ == button)
			setSelectedButton(0);

		// Remove the button from the button group and the button layout.

		buttonsGroup_->removeButton(button);
		buttonsLayout_->removeWidget(button);
	}
}

void BioXASButtonBar::clearButtons()
{
	for (int i = 0, count = buttonsGroup_->buttons().count(); i < count; i++) {
		QAbstractButton *button = buttonsGroup_->button(i);

		buttonsGroup_->removeButton(button);
		buttonsLayout_->removeWidget(button);
	}
}

void BioXASButtonBar::setSelectedButton(QAbstractButton *button)
{
	if (selectedButton_ != button) {

		if (selectedButton_) {
			selectedButton_->blockSignals(true);
			selectedButton_->setChecked(false);
			selectedButton_->blockSignals(false);
		}

		selectedButton_ = button;

		if (selectedButton_) {
			selectedButton_->blockSignals(true);
			selectedButton_->setChecked(true);
			selectedButton_->blockSignals(false);
		}

		emit selectedButtonChanged(selectedButton_);
	}
}

void BioXASButtonBar::onButtonClicked(QAbstractButton *clickedButton)
{
	// If the group's selection is identical to the current
	// selection, the selected button should be unchecked.
	// Otherwise, the clicked button should become the new
	// selection.

	if (selectedButton_ == clickedButton)
		setSelectedButton(0);
	else
		setSelectedButton(clickedButton);
}
