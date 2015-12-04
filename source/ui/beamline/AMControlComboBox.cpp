#include "AMControlComboBox.h"
#include "beamline/AMControl.h"
#include "beamline/AMPVControl.h"
#include "beamline/AMEnumeratedControl.h"
#include "ui/beamline/AMPVControlComboBox.h"
#include "ui/beamline/AMEnumeratedControlComboBox.h"

AMControlComboBox::AMControlComboBox(AMControl *control, QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables.

	control_ = 0;

	// Create UI elements.

	pvComboBox_ = new AMPVControlComboBox(0);

	pseudoMotorComboBox_ = new AMEnumeratedControlComboBox(0);

	// Create and set layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->addWidget(pvComboBox_);
	layout->addWidget(pseudoMotorComboBox_);

	setLayout(layout);

	// Current settings.

	setControl(control);
}

AMControlComboBox::~AMControlComboBox()
{

}

void AMControlComboBox::refresh()
{
	// Clear the view.

	pvComboBox_->setControl(0);
	pseudoMotorComboBox_->setControl(0);

	// Update view elements.
	// The elements that are shown depends on the kind of control given.

	AMReadOnlyPVControl *pvControl = qobject_cast<AMReadOnlyPVControl*>(control_);;
	AMEnumeratedControl *pseudoMotorControl = qobject_cast<AMEnumeratedControl*>(control_);

	if (pvControl) {

		pvComboBox_->setControl(pvControl);
		pvComboBox_->show();

		pseudoMotorComboBox_->setControl(0);
		pseudoMotorComboBox_->hide();

	} else if (pseudoMotorControl) {

		pvComboBox_->setControl(0);
		pvComboBox_->hide();

		pseudoMotorComboBox_->setControl(pseudoMotorControl);
		pseudoMotorComboBox_->show();

	} else {

		// By default, show the PV combo box (arbitrary decision), even
		// though it won't have a control set.

		pvComboBox_->setControl(0);
		pvComboBox_->show();

		pseudoMotorComboBox_->setControl(0);
		pseudoMotorComboBox_->hide();
	}
}

void AMControlComboBox::setControl(AMControl *newControl)
{
	if (control_ != newControl) {

		control_ = newControl;

		refresh();

		emit controlChanged(control_);
	}
}

