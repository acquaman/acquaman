#include "BioXASZebraLogicBlockInputCheckbox.h"

BioXASZebraLogicBlockInputCheckbox::BioXASZebraLogicBlockInputCheckbox(BioXASZebraLogicBlockInput *control, QWidget *parent) :
    QWidget(parent)
{
	// Initialize class variables.

	mode_ = Enabled;
	control_ = 0;

	// Create UI elements.

	checkbox_ = new QCheckBox();
	connect( checkbox_, SIGNAL(toggled(bool)), this, SLOT(updateControl()) );

	// Create and set layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->addWidget(checkbox_);

	setLayout(layout);

	// Current settings.

	setMode(mode);
	setControl(control);

	refresh();
}

BioXASZebraLogicBlockInputCheckbox::~BioXASZebraLogicBlockInputCheckbox()
{

}

void BioXASZebraLogicBlockInputCheckbox::refresh()
{
	updateBox();
}

void BioXASZebraLogicBlockInputCheckbox::setMode(Mode newMode)
{
	if (mode_ != newMode) {
		mode_ = newMode;
		updateBox();

		emit modeChanged(mode_);
	}
}

void BioXASZebraLogicBlockInputCheckbox::setControl(BioXASZebraLogicBlockInput *newControl)
{
	if (control_ != newControl) {

		if (control_)
			disconnect( control_, 0, this, 0 );

		control_ = newControl;

		if (control_) {
			connect( control_, SIGNAL(connected(bool)), this, SLOT(refresh()) );
			connect( control_, SIGNAL(enabledStateChanged(double)), this, SLOT(updateBox()) );
			connect( control_, SIGNAL(invertedStateChanged(double)), this, SLOT(updateBox()) );
		}

		refresh();

		emit controlChanged(control_);
	}
}

void BioXASZebraLogicBlockInputCheckbox::updateBox()
{
	bool boxChecked = false;

	if (control_) {
		if (mode_ == Enabled)
			boxChecked = control_->isEnabled();
		else if (mode_ == Inverted)
			boxChecked = control_->isInverted();
	}

	checkbox_->blockSignals(true);
	checkbox_->setChecked(boxChecked);
	checkbox_->blockSignals(false);
}

void BioXASZebraLogicBlockInputCheckbox::updateControl()
{
	if (control_) {
		bool checked = checkbox_->isChecked();

		if (mode_ == Enabled)
			control_->setEnabled(checked);
		else if (mode_ == Inverted)
			control_->setInverted(checked);
	}
}
