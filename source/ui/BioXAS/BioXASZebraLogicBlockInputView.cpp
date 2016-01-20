#include "BioXASZebraLogicBlockInputView.h"

BioXASZebraLogicBlockInputView::BioXASZebraLogicBlockInputView(BioXASZebraLogicBlockInput *control, QWidget *parent) :
    QWidget(parent)
{
	// Initialize class variables.

	control_ = 0;

	// Create UI elements.

	enabledBox_ = new QCheckBox(this);
	connect( enabledBox_, SIGNAL(toggled(bool)), this, SLOT(updateControl()) );

	inputView_ = new BioXASZebraInputView(0);

	invertedBox_ = new QCheckBox(this);
	connect( invertedBox_, SIGNAL(toggled(bool)), this, SLOT(updateControl()) );

	// Create and set layouts.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->setMargin(0);
	layout->addWidget(enabledBox_);
	layout->addWidget(inputView_);
	layout->addWidget(invertedBox_);

	setLayout(layout);

	// Current settings.

	setControl(control);

	refresh();
}

BioXASZebraLogicBlockInputView::~BioXASZebraLogicBlockInputView()
{

}

void BioXASZebraLogicBlockInputView::refresh()
{
	updateEnabledBox();
	updateInputView();
	updateInvertedBox();
}

void BioXASZebraLogicBlockInputView::setControl(BioXASZebraLogicBlockInput *newControl)
{
	if (control_ != newControl) {

		if (control_) {
			disconnect( control_, 0, this, 0 );
		}

		control_ = newControl;

		if (control_) {
			connect( control_, SIGNAL(enabledStatusChanged(bool)), this, SLOT(updateEnabledBox()) );
			connect( control_, SIGNAL(invertedStatusChanged(bool)), this, SLOT(updateInvertedBox()) );
		}

		refresh();

		emit controlChanged(control_);
	}
}

void BioXASZebraLogicBlockInputView::updateEnabledBox()
{
	bool enabled = false;

	if (control_)
		enabled = control_->enabledStatus();

	enabledBox_->blockSignals(true);
	enabledBox_->setChecked(enabled);
	enabledBox_->blockSignals(false);
}

void BioXASZebraLogicBlockInputView::updateInputView()
{
	inputView_->setControl(control_);
}

void BioXASZebraLogicBlockInputView::updateInvertedBox()
{
	bool inverted = false;

	if (control_)
		inverted = control_->invertedStatus();

	invertedBox_->blockSignals(true);
	invertedBox_->setChecked(inverted);
	invertedBox_->blockSignals(false);
}

void BioXASZebraLogicBlockInputView::updateControl()
{
	if (control_) {
		control_->setEnabled(enabledBox_->isChecked());
		control_->setInverted(invertedBox_->isChecked());
	}
}
