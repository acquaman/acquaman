#include "BioXASZebraLogicBlockInputEnabledView.h"

BioXASZebraLogicBlockInputEnabledView::BioXASZebraLogicBlockInputEnabledView(BioXASZebraLogicBlockInput *control, QWidget *parent) :
    QWidget(parent)
{
	// Initialize class variables.

	control_ = 0;

	// Create UI elements.

	enabledBox_ = new QCheckBox();
	connect( enabledBox_, SIGNAL(toggled(bool)), this, SLOT(updateControl()) );

	// Create and set layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->addWidget(enabledBox_);

	setLayout(layout);

	// Current settings.

	setControl(control);

	refresh();
}

BioXASZebraLogicBlockInputEnabledView::~BioXASZebraLogicBlockInputEnabledView()
{

}

void BioXASZebraLogicBlockInputEnabledView::refresh()
{
	updateEnabledBox();
}

void BioXASZebraLogicBlockInputEnabledView::setControl(BioXASZebraLogicBlockInput *newControl)
{
	if (control_ != newControl) {

		if (control_)
			disconnect( control_, 0, this, 0 );

		control_ = newControl;

		if (control_) {
			connect( control_, SIGNAL(connected(bool)), this, SLOT(refresh()) );
			connect( control_, SIGNAL(enabledStateChanged(double)), this, SLOT(updateEnabledBox()) );
		}

		refresh();

		emit controlChanged(control_);
	}
}

void BioXASZebraLogicBlockInputEnabledView::updateEnabledBox()
{
	bool enabledChecked = false;

	if (control_)
		enabledChecked = control_->isEnabled();

	enabledBox_->blockSignals(true);
	enabledBox_->setChecked(enabledChecked);
	enabledBox_->blockSignals(false);
}

void BioXASZebraLogicBlockInputEnabledView::updateControl()
{
	if (control_)
		control_->setEnabled(enabledBox_->isChecked());
}
