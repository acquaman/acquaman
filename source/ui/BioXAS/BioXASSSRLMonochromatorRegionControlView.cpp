#include "BioXASSSRLMonochromatorRegionControlView.h"
#include "beamline/BioXAS/BioXASSSRLMonochromator.h"
#include "beamline/BioXAS/BioXASSSRLMonochromatorRegionControl.h"

BioXASSSRLMonochromatorRegionControlView::BioXASSSRLMonochromatorRegionControlView(BioXASSSRLMonochromatorRegionControl *regionControl, QWidget *parent) :
	QWidget(parent)
{
	// Initialize member variables.

	control_ = 0;

	// Create UI elements.

	QLabel *slitsStatusPrompt = new QLabel("Mask closed:");
	slitsStatusGreen_ = new AMControlValueGreenLEDView(0);
	slitsStatusGreen_->setGreenValue(AMNumber(BioXASSSRLMonochromator::Slits::Closed));
	slitsStatusRed_ = new AMControlValueRedLEDView(0);
	slitsStatusRed_->setRedValue(AMNumber(BioXASSSRLMonochromator::Slits::NotClosed));

	QLabel *paddleStatusPrompt = new QLabel("Paddle removed:");
	paddleStatusGreen_ = new AMControlValueGreenLEDView(0);
	paddleStatusGreen_->setGreenValue(AMNumber(BioXASSSRLMonochromator::Paddle::Out));
	paddleStatusRed_ = new AMControlValueRedLEDView(0);
	paddleStatusRed_->setRedValue(AMNumber(BioXASSSRLMonochromator::Paddle::NotOut));

	QLabel *keyStatusPrompt = new QLabel("Key enabled:");
	keyStatusGreen_ = new AMControlValueGreenLEDView(0);
	keyStatusGreen_->setGreenValue(AMNumber(BioXASSSRLMonochromator::Key::Enabled));
	keyStatusRed_ = new AMControlValueRedLEDView(0);
	keyStatusRed_->setRedValue(AMNumber(BioXASSSRLMonochromator::Key::Disabled));

	QLabel *brakeStatusPrompt = new QLabel("Brake enabled:");
	brakeStatusGreen_ = new AMControlValueGreenLEDView(0);
	brakeStatusGreen_->setGreenValue(AMNumber(BioXASSSRLMonochromator::Brake::Enabled));
	brakeStatusRed_ = new AMControlValueRedLEDView(0);
	brakeStatusRed_->setRedValue(AMNumber(BioXASSSRLMonochromator::Brake::Disabled));

	QLabel *braggInPositionPrompt = new QLabel("Bragg position status:");
	braggInPositionGreen_ = new AMControlValueGreenLEDView(0);
	braggInPositionGreen_->setGreenValue(AMNumber(BioXASSSRLMonochromator::Bragg::InPosition));
	braggInPositionRed_ = new AMControlValueRedLEDView(0);
	braggInPositionRed_->setRedValue(AMNumber(BioXASSSRLMonochromator::Bragg::NotInPosition));

	QLabel *regionAStatusPrompt = new QLabel("Region A status:");
	regionAStatusGreen_ = new AMControlValueGreenLEDView(0);
	regionAStatusGreen_->setGreenValue(AMNumber(BioXASSSRLMonochromator::Region::In));
	regionAStatusRed_ = new AMControlValueRedLEDView(0);
	regionAStatusRed_->setRedValue(AMNumber(BioXASSSRLMonochromator::Region::NotIn));

	QLabel *regionBStatusPrompt = new QLabel("Region B status:");
	regionBStatusGreen_ = new AMControlValueGreenLEDView(0);
	regionBStatusGreen_->setGreenValue(AMNumber(BioXASSSRLMonochromator::Region::In));
	regionBStatusRed_ = new AMControlValueRedLEDView(0);
	regionBStatusRed_->setRedValue(AMNumber(BioXASSSRLMonochromator::Region::NotIn));

	// Create and set layouts.

	QGridLayout *statusLayout = new QGridLayout();
	statusLayout->addWidget(slitsStatusPrompt, 0, 0, 1, 1, Qt::AlignRight);
	statusLayout->addWidget(slitsStatusGreen_, 0, 1, 1, 1, Qt::AlignCenter);
	statusLayout->addWidget(slitsStatusRed_, 0, 2, 1, 1, Qt::AlignCenter);

	statusLayout->addWidget(paddleStatusPrompt, 1, 0, 1, 1, Qt::AlignRight);
	statusLayout->addWidget(paddleStatusGreen_, 1, 1, 1, 1, Qt::AlignCenter);
	statusLayout->addWidget(paddleStatusRed_, 1, 2, 1, 1, Qt::AlignCenter);

	statusLayout->addWidget(keyStatusPrompt, 2, 0, 1, 1, Qt::AlignRight);
	statusLayout->addWidget(keyStatusGreen_, 2, 1, 1, 1, Qt::AlignCenter);
	statusLayout->addWidget(keyStatusRed_, 2, 2, 1, 1, Qt::AlignCenter);

	statusLayout->addWidget(brakeStatusPrompt, 3, 0, 1, 1, Qt::AlignRight);
	statusLayout->addWidget(brakeStatusGreen_, 3, 1, 1, 1, Qt::AlignCenter);
	statusLayout->addWidget(brakeStatusRed_, 3, 2, 1, 1, Qt::AlignCenter);

	statusLayout->addWidget(braggInPositionPrompt, 4, 0, 1, 1, Qt::AlignRight);
	statusLayout->addWidget(braggInPositionGreen_, 4, 1, 1, 1, Qt::AlignCenter);
	statusLayout->addWidget(braggInPositionRed_, 4, 2, 1, 1, Qt::AlignCenter);

	statusLayout->addWidget(regionAStatusPrompt, 5, 0, 1, 1, Qt::AlignRight);
	statusLayout->addWidget(regionAStatusGreen_, 5, 1, 1, 1, Qt::AlignCenter);
	statusLayout->addWidget(regionAStatusRed_, 5, 2, 1, 1, Qt::AlignCenter);

	statusLayout->addWidget(regionBStatusPrompt, 6, 0, 1, 1, Qt::AlignRight);
	statusLayout->addWidget(regionBStatusGreen_, 6, 1, 1, 1, Qt::AlignCenter);
	statusLayout->addWidget(regionBStatusRed_, 6, 2, 1, 1, Qt::AlignCenter);

	setLayout(statusLayout);

	// Current settings.

	setControl(regionControl);
}

BioXASSSRLMonochromatorRegionControlView::~BioXASSSRLMonochromatorRegionControlView()
{

}

void BioXASSSRLMonochromatorRegionControlView::clear()
{
	slitsStatusGreen_->setControl(0);
	slitsStatusRed_->setControl(0);
	paddleStatusGreen_->setControl(0);
	paddleStatusRed_->setControl(0);
	keyStatusGreen_->setControl(0);
	keyStatusRed_->setControl(0);
	brakeStatusGreen_->setControl(0);
	brakeStatusRed_->setControl(0);
	braggInPositionGreen_->setControl(0);
	braggInPositionRed_->setControl(0);
	regionAStatusGreen_->setControl(0);
	regionAStatusRed_->setControl(0);
	regionBStatusGreen_->setControl(0);
	regionBStatusRed_->setControl(0);
}

void BioXASSSRLMonochromatorRegionControlView::update()
{
	if (control_) {
		slitsStatusGreen_->setControl(control_->slitsStatusControl());
		slitsStatusRed_->setControl(control_->slitsStatusControl());
		paddleStatusGreen_->setControl(control_->paddleStatusControl());
		paddleStatusRed_->setControl(control_->paddleStatusControl());
		keyStatusGreen_->setControl(control_->keyStatusControl());
		keyStatusRed_->setControl(control_->keyStatusControl());
		brakeStatusGreen_->setControl(control_->brakeStatusControl());
		brakeStatusRed_->setControl(control_->brakeStatusControl());
		braggInPositionGreen_->setControl(control_->braggAtCrystalChangePositionStatusControl());
		braggInPositionRed_->setControl(control_->braggAtCrystalChangePositionStatusControl());
		regionAStatusGreen_->setControl(control_->regionAStatusControl());
		regionAStatusRed_->setControl(control_->regionAStatusControl());
		regionBStatusGreen_->setControl(control_->regionBStatusControl());
		regionBStatusRed_->setControl(control_->regionBStatusControl());
	}
}

void BioXASSSRLMonochromatorRegionControlView::refresh()
{
	// Clear the view.

	clear();

	// Update the view.

	update();
}

void BioXASSSRLMonochromatorRegionControlView::setControl(BioXASSSRLMonochromatorRegionControl *newControl)
{
	if (control_ != newControl) {

		control_ = newControl;

		refresh();

		emit controlChanged(control_);
	}
}
