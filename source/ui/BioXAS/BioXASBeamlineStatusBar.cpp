#include "BioXASBeamlineStatusBar.h"
#include "beamline/BioXAS/BioXASBeamline.h"

BioXASBeamlineStatusBar::BioXASBeamlineStatusBar(QWidget *parent) :
    QWidget(parent)
{
	// Initialize class variables.

	selectedButton_ = 0;

	buttonsGroup_ = new QButtonGroup(this);
	buttonsGroup_->setExclusive(false);

	connect( buttonsGroup_, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(onButtonClicked(QAbstractButton*)) );

	// Create buttons box.

	buttonsLayout_ = new QHBoxLayout();
	buttonsLayout_->setMargin(0);

	QHBoxLayout *stretchLayout = new QHBoxLayout();
	stretchLayout->addStretch();

	QHBoxLayout *buttonsBoxLayout = new QHBoxLayout();
	buttonsBoxLayout->setMargin(0);
	buttonsBoxLayout->addLayout(buttonsLayout_);
	buttonsBoxLayout->addLayout(stretchLayout);

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

BioXASBeamlineStatusBar::~BioXASBeamlineStatusBar()
{

}

void BioXASBeamlineStatusBar::refresh()
{
	// Clear the view.

	hideViews();

	// Show the editor corresponding to the selected button.

	showViewForButton(selectedButton_);
}

void BioXASBeamlineStatusBar::addButton(QAbstractButton *newButton, QWidget *view)
{
	if (newButton && !buttonsList_.contains(newButton)) {

		// Add the new button to the buttons group, the buttons list, the buttons layout.

		buttonsGroup_->addButton(newButton, buttonsList_.size());
		buttonsList_.append(newButton);

		buttonsLayout_->addWidget(newButton);

		// Add the view to the button views list and the views layout.

		buttonViewsList_.append(view);

		if (view)
			buttonViewsLayout_->addWidget(view);
	}
}

void BioXASBeamlineStatusBar::removeButton(QAbstractButton *button)
{
	if (button && buttonsList_.contains(button)) {

		int buttonIndex = buttonsList_.indexOf(button);

		// Remove the corresponding view from the button views list and the views layout.

		if (buttonIndex >= 0 && buttonIndex < buttonViewsList_.size()) {
			QWidget *view = buttonViewsList_.at(buttonIndex);

			if (view)
				buttonViewsLayout_->removeWidget(view);

			buttonViewsList_.removeAt(buttonIndex);
		}

		// Remove the button from the button group, the button list, the button layout.

		if (buttonIndex >= 0 && buttonIndex < buttonsList_.size()) {
			buttonsGroup_->removeButton(button);
			buttonsList_.removeAt(buttonIndex);
			buttonsLayout_->removeWidget(button);
		}
	}
}

void BioXASBeamlineStatusBar::clearButtons()
{
	foreach (QAbstractButton *button, buttonsList_)
		removeButton(button);
}

void BioXASBeamlineStatusBar::setSelectedButton(QAbstractButton *newButton)
{
	if (selectedButton_ != newButton) {

		if (selectedButton_)
			selectedButton_->setChecked(false);

		selectedButton_ = newButton;

		if (selectedButton_)
			selectedButton_->setChecked(true);

		refresh();

		emit selectedButtonChanged();
	}
}

void BioXASBeamlineStatusBar::showViewForButton(QAbstractButton *button)
{
	// Show the editor corresponding to the given button.

	if (button) {
		int buttonIndex = buttonsList_.indexOf(button);

		if (buttonIndex >= 0 && buttonIndex < buttonViewsList_.size()) {
			QWidget *view = buttonViewsList_.at(buttonIndex);

			if (view) {
				view->show();
				buttonViewsBox_->show();
			}
		}
	}
}

void BioXASBeamlineStatusBar::hideViews()
{
	// Hide each button view.

	foreach (QWidget *view, buttonViewsList_) {
		if (view)
			view->hide();
	}

	// Hide the button view group.

	buttonViewsBox_->hide();
}

void BioXASBeamlineStatusBar::onButtonClicked(QAbstractButton *clickedButton)
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
