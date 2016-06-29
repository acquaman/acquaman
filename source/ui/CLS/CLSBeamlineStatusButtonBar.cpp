#include "CLSBeamlineStatusButtonBar.h"

#include "beamline/CLS/CLSBeamlineStatus.h"

CLSBeamlineStatusButtonBar::CLSBeamlineStatusButtonBar(CLSBeamlineStatus *beamlineStatus, QWidget *parent) :
	CLSControlButtonBar(parent)
{
	// Initialize class variables.

	beamlineStatus_ = 0;

	// Current settings.
	setBeamlineStatusComponent(beamlineStatus);
}

CLSBeamlineStatusButtonBar::~CLSBeamlineStatusButtonBar()
{

}

void CLSBeamlineStatusButtonBar::refresh()
{
	// Clear the controls.
	clearControls();

	// Add control for each component in the beam status.
	if (beamlineStatus_) {
		addControl(beamlineStatus_, CLSBeamlineStatus::On);
		foreach (AMControl *control, beamlineStatus_->components())
			addControl(control, beamlineStatus_->componentBeamOnValue(control));
	}
}

void CLSBeamlineStatusButtonBar::setBeamlineStatusComponent(CLSBeamlineStatus *newControl)
{
	if (beamlineStatus_ != newControl) {

		if (beamlineStatus_)
			disconnect( beamlineStatus_, 0, this, 0 );

		beamlineStatus_ = newControl;

		if (beamlineStatus_)
			connect( beamlineStatus_, SIGNAL(componentsChanged()), this, SLOT(refresh()) );

		refresh();

		emit beamlineStatusComponentChanged(beamlineStatus_);
	}
}

QAbstractButton* CLSBeamlineStatusButtonBar::createButton(AMControl *control, double greenValue)
{
	QAbstractButton *button = CLSControlButtonBar::createButton(control, greenValue);

	if (button) {
		QIcon buttonIcon;
		QString toolTip = "";
		if (beamlineStatus_ == control) {
			buttonIcon = QIcon(":/beamStatus.png");
			toolTip = "Beam Status";
		} else if (beamlineStatus_->isShutterControl(control)) {
			buttonIcon = QIcon(":/shutterIcon2.png");
			toolTip = "Shutters";
		} else if (beamlineStatus_->isValveControl(control)) {
			buttonIcon = QIcon(":/valveIcon2.png");
			toolTip = "Valves";
		} else if (beamlineStatus_->isMirrorMaskControl(control)) {
			buttonIcon = QIcon(":/mirror-icon1.png");
			toolTip = "Mirror Mask";
		} else if (beamlineStatus_->isMonoMaskControl(control)) {
			buttonIcon =QIcon(":/mono-icon5.png");
			toolTip = "Monochromator Mask";
		}

		if (toolTip.length() > 0) {
			button->setIcon(buttonIcon);
			button->setToolTip(toolTip);
		} else {
			// we don't want to display this button
			button->deleteLater();
			button = 0;
		}
	}

	return button;
}
