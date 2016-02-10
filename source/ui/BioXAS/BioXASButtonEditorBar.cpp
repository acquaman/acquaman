#include "BioXASButtonEditorBar.h"
#include "beamline/BioXAS/BioXASBeamline.h"

BioXASButtonEditorBar::BioXASButtonEditorBar(QWidget *parent) :
    QWidget(parent)
{
	// Initialize class variables.

	selectedButton_ = 0;

	buttonsGroup_ = new QButtonGroup(this);
	buttonsGroup_->setExclusive(false); // Exclusive must be turned off to have 'deselect when clicked' capability.

	connect( buttonsGroup_, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(onButtonClicked(QAbstractButton*)) );

	// Create buttons box.

	buttonsLayout_ = new QHBoxLayout();
	buttonsLayout_->setMargin(0);

	QHBoxLayout *buttonsBoxLayout = new QHBoxLayout();
	buttonsBoxLayout->setMargin(0);
	buttonsBoxLayout->addStretch();
	buttonsBoxLayout->addLayout(buttonsLayout_);
	buttonsBoxLayout->addStretch();

	QWidget *buttonsBox = new QWidget();
	buttonsBox->setLayout(buttonsBoxLayout);

	// Create views box.

	buttonViewsLayout_ = new QVBoxLayout();

	buttonViewsBox_ = new QGroupBox();
	buttonViewsBox_->setFlat(true);
	buttonViewsBox_->setLayout(buttonViewsLayout_);

	// Create and set main layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->addWidget(buttonsBox);
	layout->addWidget(buttonViewsBox_);

	setLayout(layout);

	// Current settings.

	refresh();
}

BioXASButtonEditorBar::~BioXASButtonEditorBar()
{

}

void BioXASButtonEditorBar::refresh()
{
	// Clear the view.

	hideViews();

	// Show the editor corresponding to the selected button.

	showViewForButton(selectedButton_);
}

void BioXASButtonEditorBar::addButton(QAbstractButton *newButton, QWidget *view)
{
	if (newButton) {

		// Add the new button to the buttons group and the buttons layout.

		buttonsGroup_->addButton(newButton, buttonViewMap_.keys().count());
		buttonsLayout_->addWidget(newButton);

		// Add mapping between the button and the corresponding view.

		if (view) {
			buttonViewMap_.insert(newButton, view);
			buttonViewsLayout_->addWidget(view);
		}
	}
}

void BioXASButtonEditorBar::removeButton(QAbstractButton *button)
{
	if (button) {

		// Remove the button from the button group and the button layout.

		buttonsGroup_->removeButton(button);
		buttonsLayout_->removeWidget(button);

		// Remove the corresponding view from the views layout.

		QWidget *view = buttonViewMap_.value(button, 0);

		if (view)
			buttonViewsLayout_->removeWidget(view);

		// Remove the button/view mapping.

		buttonViewMap_.remove(button);
	}
}

void BioXASButtonEditorBar::clearButtons()
{
	foreach (QAbstractButton *button, buttonViewMap_.keys())
		removeButton(button);
}

void BioXASButtonEditorBar::setSelectedButton(QAbstractButton *newButton)
{
	if (selectedButton_ != newButton) {

		if (selectedButton_) {
			selectedButton_->blockSignals(true);
			selectedButton_->setChecked(false);
			selectedButton_->blockSignals(false);
		}

		selectedButton_ = newButton;

		if (selectedButton_) {
			selectedButton_->blockSignals(true);
			selectedButton_->setChecked(true);
			selectedButton_->blockSignals(false);
		}

		refresh();

		emit selectedButtonChanged();
	}
}

void BioXASButtonEditorBar::showViewForButton(QAbstractButton *button)
{
	// Show the view corresponding to the given button.

	QWidget *view = buttonViewMap_.value(button, 0);

	if (view) {
		view->show();
		buttonViewsBox_->show();
	}
}

void BioXASButtonEditorBar::hideViews()
{
	// Hide each button view.

	foreach (QWidget *view, buttonViewMap_.values()) {
		if (view)
			view->hide();
	}

	// Hide the button view group.

	buttonViewsBox_->hide();
}

void BioXASButtonEditorBar::onButtonClicked(QAbstractButton *clickedButton)
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
