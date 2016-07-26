#include "BioXASBeamStatusButtonBar.h"

BioXASBeamStatusButtonBar::BioXASBeamStatusButtonBar(BioXASBeamStatus *beamStatus, QWidget *parent) :
	QWidget(parent)
{
	beamStatus_ = 0;
	selectedComponent_ = 0;

	// Create button bar.

	buttonBar_ = new CLSButtonBar();

	connect( buttonBar_, SIGNAL(selectedButtonChanged(QAbstractButton*)), this, SLOT(updateSelectedComponent()) );
	connect( buttonBar_, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(onButtonBarButtonClicked(QAbstractButton*)) );

	// Create and set main layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->addWidget(buttonBar_);

	setLayout(layout);

	// Current settings.

	setBeamStatus(beamStatus);
}

BioXASBeamStatusButtonBar::~BioXASBeamStatusButtonBar()
{

}

void BioXASBeamStatusButtonBar::setBeamStatus(BioXASBeamStatus *newBeamStatus)
{
	if (beamStatus_ != newBeamStatus) {

		if (beamStatus_)
			disconnect( beamStatus_, 0, this, 0 );

		beamStatus_ = newBeamStatus;

		if (beamStatus_)
			connect( beamStatus_, SIGNAL(componentsChanged()), this, SLOT(updateButtonBar()) );

		emit beamStatusChanged(beamStatus_);
	}

	updateButtonBar();
}

void BioXASBeamStatusButtonBar::setSelectedComponent(AMControl *control)
{
	if (selectedComponent_ != control) {
		selectedComponent_ = control;
		emit selectedComponentChanged(selectedComponent_);
	}

	updateButtonBarSelectedButton();
}

void BioXASBeamStatusButtonBar::updateSelectedComponent()
{
	setSelectedComponent( getComponentForButton(buttonBar_->selectedButton()) );
}

void BioXASBeamStatusButtonBar::updateButtonBar()
{
	// Clear the button bar.

	buttonBar_->clearButtons();
	componentButtonMap_.clear();

	if (beamStatus_) {

		// Create new buttons for each beam status component.

		foreach (AMControl *component, beamStatus_->components()) {
			QAbstractButton *button = createButton(component);

			buttonBar_->addButton(button);
			componentButtonMap_.insert(component, button);
		}

		// Set the selected button.

		updateButtonBarSelectedButton();
	}
}

void BioXASBeamStatusButtonBar::updateButtonBarSelectedButton()
{
	buttonBar_->setSelectedButton( getButtonForComponent(selectedComponent_) );
}

void BioXASBeamStatusButtonBar::onButtonBarButtonClicked(QAbstractButton *clickedButton)
{
	emit componentClicked( getComponentForButton(clickedButton) );
}

QAbstractButton* BioXASBeamStatusButtonBar::createButton(AMControl *control)
{
	AMControlToolButton *newButton = new AMControlToolButton(control);
	newButton->setObjectName(control ? control->name() : "");

	if (beamStatus_) {

		// Iterate through the list of beam status states associated with the given component.
		// Convert the beam status into a color state, and add it to the list of states
		// for the new button.

		QList<BioXASBeamStatusState> states = beamStatus_->componentBeamStatusStates(control);

		foreach (BioXASBeamStatusState state, states)
			newButton->addColorState(getColorState(state.beamStatusValue()), state.controlMinValue(), state.controlMaxValue());
	}

	return newButton;
}

AMToolButton::ColorState BioXASBeamStatusButtonBar::getColorState(BioXASBeamStatus::Value beamStatusValue) const
{
	AMToolButton::ColorState result = AMToolButton::Neutral;

	if (beamStatusValue == BioXASBeamStatus::Off)
		result = AMToolButton::Bad;
	else if (beamStatusValue == BioXASBeamStatus::On)
		result = AMToolButton::Good;

	return result;
}

AMControl* BioXASBeamStatusButtonBar::getComponentForButton(QAbstractButton *button) const
{
	return componentButtonMap_.key(button, 0);
}

QAbstractButton* BioXASBeamStatusButtonBar::getButtonForComponent(AMControl *control) const
{
	return componentButtonMap_.value(control, 0);
}
