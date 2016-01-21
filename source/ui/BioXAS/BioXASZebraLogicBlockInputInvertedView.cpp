#include "BioXASZebraLogicBlockInputInvertedView.h"

BioXASZebraLogicBlockInputInvertedView::BioXASZebraLogicBlockInputInvertedView(BioXASZebraLogicBlockInput *control, QWidget *parent) :
    QWidget(parent)
{
	// Initialize class variables.

	control_ = 0;

	// Create UI elements.

	invertedBox_ = new QCheckBox();
	connect( invertedBox_, SIGNAL(toggled(bool)), this, SLOT(updateControl()) );

	// Create and set layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->addWidget(invertedBox_);

	setLayout(layout);

	// Current settings.

	setControl(control);

	refresh();
}

BioXASZebraLogicBlockInputInvertedView::~BioXASZebraLogicBlockInputInvertedView()
{

}

void BioXASZebraLogicBlockInputInvertedView::refresh()
{
	updateInvertedBox();
}

void BioXASZebraLogicBlockInputInvertedView::setControl(BioXASZebraLogicBlockInput *newControl)
{
	if (control_ != newControl) {

		if (control_)
			disconnect( control_, 0, this, 0 );

		control_ = newControl;

		if (control_) {
			connect( control_, SIGNAL(connected(bool)), this, SLOT(refresh()) );
			connect( control_, SIGNAL(invertedStatusChanged(bool)), this, SLOT(updateInvertedBox()) );
		}

		refresh();

		emit controlChanged(control_);
	}
}

void BioXASZebraLogicBlockInputInvertedView::updateInvertedBox()
{
	bool invertedChecked = false;

	if (control_)
		invertedChecked = control_->invertedStatus();

	invertedBox_->blockSignals(true);
	invertedBox_->setChecked(invertedChecked);
	invertedBox_->blockSignals(false);
}

void BioXASZebraLogicBlockInputInvertedView::updateControl()
{
	if (control_)
		control_->setInverted(invertedBox_->isChecked());
}

