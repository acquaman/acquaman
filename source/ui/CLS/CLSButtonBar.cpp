#include "CLSButtonBar.h"
#include <QVariant>
#include <QDebug>
CLSButtonBar::CLSButtonBar(QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables.

	selectionMode_ = UnselectedUncheckedToSelectedUnchecked;
	reselectionMode_ = SelectedUncheckedToSelectedUnchecked;

	selectedButton_ = 0;

	buttonsGroup_ = new QButtonGroup(this);
	buttonsGroup_->setExclusive(false); // Exclusive must be turned off to have 'deselect when clicked' capability.

	connect( buttonsGroup_, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(setSelectedButton(QAbstractButton*)) );
	connect( buttonsGroup_, SIGNAL(buttonClicked(QAbstractButton*)), this, SIGNAL(buttonClicked(QAbstractButton*)) );

	// Create and set main layout.

	buttonsLayout_ = new QHBoxLayout();
	buttonsLayout_->setMargin(0);

	setLayout(buttonsLayout_);
}

CLSButtonBar::~CLSButtonBar()
{
	clearButtons();
}

void CLSButtonBar::addButton(QAbstractButton *newButton)
{
	if (newButton) {

		// Add the new button to the buttons group and the buttons layout.

		buttonsGroup_->addButton(newButton, buttonsGroup_->buttons().count());
		buttonsLayout_->addWidget(newButton);
	}
}

void CLSButtonBar::removeButton(QAbstractButton *button)
{
	if (button) {

		qDebug() << "\n\nRemoving button.";

		// Check if the button to remove is the selected button.
		// If so, set the selected button to 0.

		if (selectedButton_ == button)
			setSelectedButton(0);

		// Remove the button from the button group and the button layout.

		buttonsGroup_->removeButton(button);
		buttonsLayout_->removeWidget(button);
	}
}

void CLSButtonBar::removeButtons()
{
	for (int i = 0, count = buttonsGroup_->buttons().count(); i < count; i++) {
		QAbstractButton *button = buttonsGroup_->button(i);

		buttonsGroup_->removeButton(button);
		buttonsLayout_->removeWidget(button);
	}
}

void CLSButtonBar::clearButtons()
{
	for (int i = 0, count = buttonsGroup_->buttons().count(); i < count; i++) {
		QAbstractButton *button = buttonsGroup_->button(i);

		buttonsGroup_->removeButton(button);
		buttonsLayout_->removeWidget(button);

		button->disconnect();
		button->deleteLater();
	}
}
#include <QDebug>
void CLSButtonBar::setSelectedButton(QAbstractButton *button)
{
//	qDebug() << "\n\n";

//	// Uncheck any previous selection.

//	if (selectedButton_) {

//		qDebug() << "Unchecking previous selection.";

//		buttonsGroup_->blockSignals(true);
//		selectedButton_->setChecked(false);
//		buttonsGroup_->blockSignals(false);
//	}

//	// Update the selection and check state, according to whether a button is being
//	// selected or reselected, and the current selection/reselection modes.

//	if (selectedButton_ != button && selectionMode_ == UnselectedUncheckedToSelectedChecked) {

//		// Update selection.

//		selectedButton_ = button;

//		// Make sure new selection is checked.

//		if (selectedButton_) {
//			buttonsGroup_->blockSignals(true);
//			selectedButton_->setChecked(true);
//			buttonsGroup_->blockSignals(false);
//		}

//		qDebug() << "Selecting new checked button.";

//		emit selectedButtonChanged(selectedButton_);

//	} else if (selectedButton_ != button && selectionMode_ == UnselectedUncheckedToSelectedUnchecked) {

//		// Update selection.

//		selectedButton_ = button;

//		// Make sure new selection is unchecked.

//		if (selectedButton_) {
//			buttonsGroup_->blockSignals(true);
//			selectedButton_->setChecked(false);
//			buttonsGroup_->blockSignals(false);
//		}

//		qDebug() << "Reselecting unchecked button.";

//		emit selectedButtonChanged(selectedButton_);

//	} else if (selectedButton_ == button && reselectionMode_ == SelectedCheckedToUnselectedUnchecked) {

//		if (selectedButton_) {

//			qDebug() << "Deselecting button.";

//			// Update selection.

//			selectedButton_ = 0;
//			emit selectedButtonChanged(selectedButton_);
//		}

//	} else if (selectedButton_ == button && reselectionMode_ == SelectedUncheckedToSelectedUnchecked) {

//		// Nothing to do here.
//	}


//	///////

	if (selectedButton_ != button) {

		// In all cases where we are changing the selected button
		// (to either a new button or to 0), we want to uncheck the previous selection.

		if (selectedButton_) {
			buttonsGroup_->blockSignals(true);
			selectedButton_->setChecked(false);
			buttonsGroup_->blockSignals(false);
		}

		// Update the selected button.

		selectedButton_ = button;

		// If we are changing to a new selection, we may want the new selection
		// to be checked or unchecked, depending on the selection mode.

		if (selectedButton_) {

			buttonsGroup_->blockSignals(true);

			if (selectionMode_ == UnselectedUncheckedToSelectedChecked)
				selectedButton_->setChecked(true);

			else if (selectionMode_ == UnselectedUncheckedToSelectedUnchecked)
				selectedButton_->setChecked(false);

			buttonsGroup_->blockSignals(false);
		}

		qDebug() << "\n\nSelected button is valid:" << (selectedButton_ ? "Yes" : "No");
		qDebug() << "Selected button is checked:" << ((selectedButton_ && selectedButton_->isChecked()) ? "Yes" : "No");

		emit selectedButtonChanged(selectedButton_);

	} else if (selectedButton_) {

		// In these cases, we are selecting an already selected button.

		buttonsGroup_->blockSignals(true);

		if (reselectionMode_ == SelectedUncheckedToSelectedUnchecked) {
			selectedButton_->setChecked(false);

		} else if (reselectionMode_ == SelectedCheckedToUnselectedUnchecked) {

			// Uncheck previous selection.

			selectedButton_->setChecked(false);

			// Update the selected button.

			selectedButton_ = 0;
			emit selectedButtonChanged(selectedButton_);
		}

		buttonsGroup_->blockSignals(false);
	}
}

void CLSButtonBar::setSelectionMode(SelectionMode newMode)
{
	if (selectionMode_ != newMode) {
		selectionMode_ = newMode;
		emit selectionModeChanged(selectionMode_);
	}
}

void CLSButtonBar::setReselectionMode(ReselectionMode newMode)
{
	if (reselectionMode_ != newMode) {
		reselectionMode_ = newMode;
		emit reselectionModeChanged(reselectionMode_);
	}
}

void CLSButtonBar::onButtonClicked(QAbstractButton *clickedButton)
{
	// This method handles identifying the new button to select.
	// Whether that button is un/checked is handled in setSelectedButton.

//	if (selectedButton_ && selectedButton_ == clickedButton) {

//		if (reselectionMode_ == SelectedCheckedToUnselectedUnchecked)
//			setSelectedButton(0);

//	} else {

//		setSelectedButton(clickedButton);
//	}
}
