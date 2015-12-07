#include "AMPVControlComboBox.h"
#include "beamline/AMPVControl.h"

AMPVControlComboBox::AMPVControlComboBox(AMReadOnlyPVControl *control, QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables.

	control_ = 0;

	// Create UI elements.

	comboBox_ = new QComboBox(this);

	// Create and set layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->addWidget(comboBox_);

	setLayout(layout);

	// Current settings.

	setControl(control);
}

AMPVControlComboBox::~AMPVControlComboBox()
{

}

void AMPVControlComboBox::refresh()
{
	// Clears the view.

	clearBox();

	// Updates the view--adds value options and sets current value.

	if (control_)
		comboBox_->addItems(control_->moveEnumNames());

	updateBoxValue();
}

void AMPVControlComboBox::setControl(AMReadOnlyPVControl *newControl)
{
	if (control_ != newControl) {

		if (control_)
			disconnect( control_, 0, this, 0 );

		control_ = newControl;

		if (control_) {
			connect( control_, SIGNAL(enumChanged()), this, SLOT(refresh()) );
			connect( control_, SIGNAL(valueChanged(double)), this, SLOT(updateBoxValue()) );
		}

		refresh();

		emit controlChanged(control_);
	}
}

void AMPVControlComboBox::clearBox()
{
	comboBox_->clear();
}

void AMPVControlComboBox::updateBoxValue()
{
	int newIndex = 0;

	if (control_)
		newIndex = int(control_->value());

	comboBox_->setCurrentIndex(newIndex);
}

