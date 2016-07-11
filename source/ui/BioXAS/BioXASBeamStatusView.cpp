#include "BioXASBeamStatusView.h"

BioXASBeamStatusView::BioXASBeamStatusView(BioXASBeamStatus *beamStatus, QWidget *parent) :
    QWidget(parent)
{
	beamStatus_ = 0;

	// Create and set main layouts.

	buttonsLayout_ = new QHBoxLayout();
	buttonsLayout_->setMargin(0);

	setLayout(buttonsLayout_);

	// Current settings.

	setBeamStatus(beamStatus);
}

BioXASBeamStatusView::~BioXASBeamStatusView()
{

}

void BioXASBeamStatusView::setBeamStatus(BioXASBeamStatus *newBeamStatus)
{
	if (beamStatus_ != newBeamStatus) {

		if (beamStatus_)
			disconnect( beamStatus_, 0, this, 0 );

		beamStatus_ = newBeamStatus;

		if (beamStatus_)
			connect( beamStatus_, SIGNAL(componentsChanged()), this, SLOT(updateBeamStatusView()) );

		emit beamStatusChanged(beamStatus_);
	}

	updateBeamStatusView();
}

void BioXASBeamStatusView::updateBeamStatusView()
{
	// Clear the beam status view.

	foreach (QAbstractButton *button, buttons_) {
		if (button) {
			buttons_.removeOne(button);
			buttonsLayout_->removeWidget(button);

			button->disconnect();
			button->deleteLater();
		}
	}

	// Create new buttons for each beam status component.

	if (beamStatus_) {
		foreach (AMControl *component, beamStatus_->components()) {

			if (component) {
				QAbstractButton *newButton = createButton(component);

				if (newButton) {
					buttons_ << newButton;
					buttonsLayout_->addWidget(newButton);
				}
			}
		}
	}
}

QAbstractButton* BioXASBeamStatusView::createButton(AMControl *control)
{
	AMControlToolButton *newButton = new AMControlToolButton(control);
	newButton->setObjectName(control ? control->name() : "");

	if (beamStatus_) {

		// Iterate through the list of beam status states associated with the given component.
		// Convert the beam status into a color state, and add it to the list of states
		// for the new button.

		QList<BioXASBeamStatusState> states = beamStatus_->componentBeamStatusStates(control);

		foreach (BioXASBeamStatusState state, states)
			newButton->addColorState(getColorState(state.beamStatusValue_), state.controlMinValue_, state.controlMaxValue_);
	}

	return newButton;
}

AMToolButton::ColorState BioXASBeamStatusView::getColorState(BioXASBeamStatus::Value beamStatusValue) const
{
	AMToolButton::ColorState result = AMToolButton::Neutral;

	if (beamStatusValue == BioXASBeamStatus::Off)
		result = AMToolButton::Bad;
	else if (beamStatusValue == BioXASBeamStatus::On)
		result = AMToolButton::Good;

	return result;
}
