#include "BioXASBeamStatusBar.h"
#include "beamline/BioXAS/BioXASBeamStatus.h"
#include "beamline/BioXAS/BioXASShutters.h"
#include "beamline/BioXAS/BioXASValves.h"
#include "beamline/BioXAS/BioXASM1MirrorMaskState.h"
#include "beamline/BioXAS/BioXASSSRLMonochromatorMaskState.h"
#include "ui/AMToolButton.h"
#include "ui/BioXAS/BioXASShuttersView.h"
#include "ui/BioXAS/BioXASControlEditor.h"
#include "ui/BioXAS/BioXASButtonEditorBar.h"

BioXASBeamStatusBar::BioXASBeamStatusBar(BioXASBeamStatus *beamStatus, QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables.

	beamStatus_ = 0;

	valueUpdateMapper_ = new QSignalMapper(this);
	connect( valueUpdateMapper_, SIGNAL(mapped(QObject*)), this, SLOT(updateButtonForComponent(QObject*)) );

	// Create UI elements.

	buttonBar_ = new BioXASButtonEditorBar();

	// Create and set layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->addWidget(buttonBar_);

	setLayout(layout);

	// Current settings.

	setBeamStatus(beamStatus);

	refresh();
}

BioXASBeamStatusBar::~BioXASBeamStatusBar()
{

}

void BioXASBeamStatusBar::refresh()
{
	// Clear the buttons and views.

	clearComponentViews();

	if (beamStatus_) {

		// Iterate through the beam status components, creating
		// views for each. Update the views while we're at it.

		foreach (AMControl *component, beamStatus_->components()) {
			addComponentView(component);
			updateButtonForComponent(component);
		}
	}
}

void BioXASBeamStatusBar::setBeamStatus(BioXASBeamStatus *newStatus)
{
	if (beamStatus_ != newStatus) {

		if (beamStatus_)
			disconnect( beamStatus_, 0, this, 0 );

		beamStatus_ = newStatus;

		if (beamStatus_)
			connect( beamStatus_, SIGNAL(componentsChanged()), this, SLOT(refresh()) );

		refresh();

		emit beamStatusChanged(beamStatus_);
	}
}

void BioXASBeamStatusBar::addComponentView(AMControl *newControl)
{
	if (newControl && !hasComponentView(newControl)) {

		// Create views for the component.

		QAbstractButton *button = createControlButton(newControl);
		QWidget *view = createControlView(newControl);

		// Add a mapping between the control and button, and
		// add the views to the button bar.

		controlButtonMap_.insert(newControl, button);
		buttonBar_->addButton(button, view);
	}
}

void BioXASBeamStatusBar::removeComponentView(AMControl *control)
{
	if (control && hasComponentView(control)) {
		QAbstractButton *button = controlButtonMap_.value(control);

		// Remove the mapping between control and button, and
		// remove views from the button bar.

		controlButtonMap_.remove(control);
		buttonBar_->removeButton(button);
	}
}

void BioXASBeamStatusBar::clearComponentViews()
{
	// Clear the mappings between controls and buttons, and
	// remove all views from the button bar.

	controlButtonMap_.clear();
	buttonBar_->clearButtons();
}

void BioXASBeamStatusBar::updateButtonForComponent(QObject *component)
{
	AMControl *control = qobject_cast<AMControl*>(component);

	if (beamStatus_ && control && hasComponentView(control)) {

		// Identify the button for the given control, and
		// update the color according to the control's value.

		AMToolButton *button = qobject_cast<AMToolButton*>(controlButtonMap_.value(control));

		if (button) {
			if (beamStatus_->componentInBeamOnState(control))
				button->setColorToGreen();
			else
				button->setColorToRed();
		}
	}
}

QAbstractButton* BioXASBeamStatusBar::createControlButton(AMControl *control) const
{
	QAbstractButton *button = 0;
	bool controlFound = false;

	BioXASShutters *shutters = qobject_cast<BioXASShutters*>(control);
	if (!controlFound && shutters) {
		button = new AMToolButton();
		button->setIcon(QIcon(":/shutterIcon2.png"));
		button->setToolTip("Shutters");

		controlFound = true;
	}

	BioXASValves *valves = qobject_cast<BioXASValves*>(control);
	if (!controlFound && valves) {
		button = new AMToolButton();
		button->setIcon(QIcon(":/valveIcon2.png"));
		button->setToolTip("Valves");

		controlFound = true;
	}

	BioXASM1MirrorMaskState *mirrorMask = qobject_cast<BioXASM1MirrorMaskState*>(control);
	if (!controlFound && mirrorMask) {
		button = new AMToolButton();
		button->setIcon(QIcon(":/mirror-icon1.png"));
		button->setToolTip("Mirror");

		controlFound = true;
	}

	BioXASSSRLMonochromatorMaskState *monoMask = qobject_cast<BioXASSSRLMonochromatorMaskState*>(control);
	if (!controlFound && monoMask) {
		button = new AMToolButton();
		button->setIcon(QIcon(":/mono-icon5.png"));
		button->setToolTip("Monochromator Mask");

		controlFound = true;
	}

	return button;
}

QWidget* BioXASBeamStatusBar::createControlView(AMControl *control) const
{
	QWidget *view = 0;
	bool controlFound = false;

	BioXASShutters *shutters = qobject_cast<BioXASShutters*>(control);
	if (!controlFound && shutters) {
		BioXASControlEditor *editor = new BioXASControlEditor(shutters);
		editor->setTitle("Shutters");

		view = editor;
		controlFound = true;
	}

	BioXASValves *valves = qobject_cast<BioXASValves*>(control);
	if (!controlFound && valves) {
		BioXASControlEditor *editor = new BioXASControlEditor(valves);
		editor->setTitle("Valves");

		view = editor;
		controlFound = true;
	}

	BioXASM1MirrorMaskState *mirrorMask = qobject_cast<BioXASM1MirrorMaskState*>(control);
	if (!controlFound && mirrorMask) {
		BioXASControlEditor *editor = new BioXASControlEditor(mirrorMask);
		editor->setTitle("Mirror mask");

		view = editor;
		controlFound = true;
	}

	BioXASSSRLMonochromatorMaskState *monoMask = qobject_cast<BioXASSSRLMonochromatorMaskState*>(control);
	if (!controlFound && monoMask) {
		BioXASControlEditor *editor = new BioXASControlEditor(monoMask);
		editor->setTitle("Mono mask");

		view = editor;
		controlFound = true;
	}

	return view;
}
