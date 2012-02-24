#include "REIXSControlMoveActionEditor.h"

#include <QComboBox>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QBoxLayout>
#include <QCheckBox>

// It's ok for this class to be coupled to REIXSBeamline, because it will only be created on applications where both the REIXSControlMoveActionInfo and REIXSControlMoveAction are registered.  That means we're coupled to the actual action anyway, which accesses the beamline.  On applications that should not access REIXSBeamline, this editor will not be registered.
#include "beamline/REIXS/REIXSBeamline.h"

REIXSControlMoveActionEditor::REIXSControlMoveActionEditor(REIXSControlMoveActionInfo *info, QWidget *parent)
	: QFrame(parent) {

	info_ = info;

	// Setup UI
	QHBoxLayout* hl = new QHBoxLayout(this);
	controlSelectorBox_ = new QComboBox();
	hl->addWidget(new QLabel("Move:"));
	hl->addWidget(controlSelectorBox_);
	toLabel_ = new QLabel(info_->isRelativeMove() ? "by:" : "to:");
	hl->addWidget(toLabel_);
	setpointBox_ = new QDoubleSpinBox();
	setpointBox_->setDecimals(3);	 /// \todo Should this update based on the control's precision?
	hl->addWidget(setpointBox_);
	unitsLabel_ = new QLabel("");
	hl->addWidget(unitsLabel_);
	relativeCheckBox_ = new QCheckBox("Relative Move");
    relativeCheckBox_->setChecked(info_->isRelativeMove());
	hl->addWidget(relativeCheckBox_);
	hl->addStretch(0);
	////////////////////


	populateControls();

	connect(controlSelectorBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onControlSelectorBoxCurrentIndexChanged(int)));
	connect(setpointBox_, SIGNAL(editingFinished()), this, SLOT(onSetpointBoxEditingFinished()));
	connect(relativeCheckBox_, SIGNAL(toggled(bool)), this, SLOT(onRelativeCheckBoxToggled(bool)));
}

void REIXSControlMoveActionEditor::onControlSelectorBoxCurrentIndexChanged(int index)
{
	// Possibility 1: Nothing selected.  (No valid control)
	if(index < 0) {
		setpointBox_->setEnabled(false);
		relativeCheckBox_->setEnabled(false);
		unitsLabel_->setText(QString());
		info_->setControlInfo(AMControlInfo());
	}
	else {
		// Get the control name:
		QString controlName = controlSelectorBox_->itemData(index).toString();
		// Access the real control from the beamline:
		AMControl* control = REIXSBeamline::bl()->allControlsSet()->controlNamed(controlName);
		if(!control) {
			setpointBox_->setEnabled(false);
			relativeCheckBox_->setEnabled(false);
			unitsLabel_->setText("[Invalid Control]");
			info_->setControlInfo(AMControlInfo());
		}
		else {
			setpointBox_->setEnabled(true);
			relativeCheckBox_->setEnabled(true);
			if(info_->isRelativeMove())
				setpointBox_->setRange(-999999, 999999);
			else
				setpointBox_->setRange(control->minimumValue(), control->maximumValue());
			unitsLabel_->setText(control->units());
			AMControlInfo newControlInfo = control->toInfo();	// need to get most of the AMControlInfo details from the beamline control...
			newControlInfo.setValue(setpointBox_->value());	// but grab the setpoint from the setpointBox_.
			info_->setControlInfo(newControlInfo);	// update the info to use this new setpoint.
		}
	}
}

void REIXSControlMoveActionEditor::onSetpointBoxEditingFinished()
{
	info_->setSetpoint(setpointBox_->value());
}

void REIXSControlMoveActionEditor::populateControls()
{
	// This will be used to find the index of the info's current control
	int currentIndex = -1;

	// clear any existing entries
	controlSelectorBox_->clear();

	// Iterate through all the controls in REIXSBeamline's allControlsSet():
	AMControlSet* allControls = REIXSBeamline::bl()->allControlsSet();
	for(int i=0, cc=allControls->count(); i<cc; i++) {
		AMControl* control = allControls->at(i);

		QString description = control->description();
		if(description.isEmpty())
			description = control->name();	// if there is no description, a name is better than nothing.

		// add to the combo box.
		controlSelectorBox_->addItem(description, control->name());	// store the actual control name in the UserRole.

		// Is this control the info's current control?
		if(control->name() == info_->controlInfo()->name())
			currentIndex = i;
	}

	controlSelectorBox_->blockSignals(true);
	controlSelectorBox_->setCurrentIndex(currentIndex);
	controlSelectorBox_->blockSignals(false);


	// update the units and setpointBox_ to reflect the current info_.
	if(currentIndex < 0) {
		setpointBox_->setEnabled(false);
		relativeCheckBox_->setEnabled(false);
		unitsLabel_->setText(QString());
	}
	else {
		setpointBox_->setEnabled(true);
		relativeCheckBox_->setEnabled(true);
		if(info_->isRelativeMove())// don't apply the control's min/max limits to a relative move. For ex: control with (min 5 max 200) could do a relative move from 70 of -10.
			setpointBox_->setRange(-999999, 999999);
		else
			setpointBox_->setRange(info_->controlInfo()->minimum(), info_->controlInfo()->maximum());
		setpointBox_->setValue(info_->controlInfo()->value());
		unitsLabel_->setText(info_->controlInfo()->units());
		relativeCheckBox_->setChecked(info_->isRelativeMove());
	}
}

void REIXSControlMoveActionEditor::onRelativeCheckBoxToggled(bool isRelative)
{
	info_->setIsRelativeMove(isRelative);

	// adjust range: if relative, should not constrain the spin box to the control's range.
	if(isRelative) {
		toLabel_->setText("by:");
		setpointBox_->setRange(-999999, 999999);
	}
	else {
		toLabel_->setText("to:");
		// get the min and max range for this control
		AMControl* control  = REIXSBeamline::bl()->allControlsSet()->controlNamed(controlSelectorBox_->itemData(controlSelectorBox_->currentIndex()).toString());
		if(control) {
			setpointBox_->setRange(control->minimumValue(), control->maximumValue());
		}
	}
}
