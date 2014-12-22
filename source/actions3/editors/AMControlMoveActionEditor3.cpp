/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").
Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "AMControlMoveActionEditor3.h"

#include <QComboBox>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QBoxLayout>
#include <QCheckBox>

#include "beamline/AMBeamline.h"

AMControlMoveActionEditor3::AMControlMoveActionEditor3(AMControlMoveActionInfo3 *info, QWidget *parent)
	 : QFrame(parent) {

	 info_ = info;

	 // Setup UI
	 QHBoxLayout* hl = new QHBoxLayout();
	 controlSelectorBox_ = new QComboBox();
	 hl->addWidget(new QLabel("Move:"));
	 hl->addWidget(controlSelectorBox_);
	 toLabel_ = new QLabel(info_->isRelativeMove() ? "by:" : "to:");
	 hl->addWidget(toLabel_);
	 setpointBox_ = new QDoubleSpinBox();
	 setpointBox_->setDecimals(3);
	 hl->addWidget(setpointBox_);
	 unitsLabel_ = new QLabel("");
	 hl->addWidget(unitsLabel_);
	 relativeCheckBox_ = new QCheckBox("Relative Move");
	 relativeCheckBox_->setChecked(info_->isRelativeMove());
	 hl->addWidget(relativeCheckBox_);
	 hl->addStretch(0);

	 setLayout(hl);
	 ////////////////////


	 populateControls();

	 connect(controlSelectorBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onControlSelectorBoxCurrentIndexChanged(int)));
	 connect(setpointBox_, SIGNAL(editingFinished()), this, SLOT(onSetpointBoxEditingFinished()));
	 connect(relativeCheckBox_, SIGNAL(toggled(bool)), this, SLOT(onRelativeCheckBoxToggled(bool)));
 }

AMControlMoveActionEditor3::~AMControlMoveActionEditor3(){}

void AMControlMoveActionEditor3::onControlSelectorBoxCurrentIndexChanged(int index)
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
		AMControl* control = AMBeamline::bl()->exposedControlByName(controlName);

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

void AMControlMoveActionEditor3::onSetpointBoxEditingFinished()
{
	info_->setSetpoint(setpointBox_->value());
}

void AMControlMoveActionEditor3::populateControls()
{
	// This will be used to find the index of the info's current control
	int currentIndex = -1;

	// clear any existing entries
	controlSelectorBox_->clear();

	// Iterate through all the controls in the exposed AMBeamline controls.
	AMControlSet *allControls = AMBeamline::bl()->exposedControls();
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

void AMControlMoveActionEditor3::onRelativeCheckBoxToggled(bool isRelative)
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
		AMControl* control  = AMBeamline::bl()->exposedControlByName(controlSelectorBox_->itemData(controlSelectorBox_->currentIndex()).toString());
		if(control) {
			setpointBox_->setRange(control->minimumValue(), control->maximumValue());
		}
	}
}
