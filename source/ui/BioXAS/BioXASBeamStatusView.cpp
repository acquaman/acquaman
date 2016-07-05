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

	// Map the control's values to button color states.

	bool controlFound = false;



	return newButton;
}
