#include "BioXASSSRLMonochromatorRegionControlView.h"
#include "beamline/BioXAS/BioXASSSRLMonochromator.h"
#include "beamline/BioXAS/BioXASSSRLMonochromatorRegionControl.h"

BioXASSSRLMonochromatorRegionControlView::BioXASSSRLMonochromatorRegionControlView(BioXASSSRLMonochromatorRegionControl *regionControl, QWidget *parent) :
	QGroupBox(parent)
{
	// Initialize member variables.

	regionControl_ = 0;

	// Create UI elements.

	QLabel *slitsStatusPrompt = new QLabel("Slits status:");
	slitsStatusGreen_ = new QLabel();
	slitsStatusRed_ = new QLabel();

	QLabel *paddleStatusPrompt = new QLabel ("Paddle status:");
	paddleStatusGreen_ = new QLabel();
	paddleStatusRed_ = new QLabel();

	QLabel *keyStatusPrompt = new QLabel("Key status:");
	keyStatusGreen_ = new QLabel();
	keyStatusRed_ = new QLabel();

	QLabel *brakeStatusPrompt = new QLabel("Brake status:");
	brakeStatusGreen_ = new QLabel();
	brakeStatusRed_ = new QLabel();

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

	setLayout(statusLayout);
	setFlat(true);

	// Initial settings.

	slitsStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
	slitsStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));

	paddleStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
	paddleStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));

	keyStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
	keyStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));

	brakeStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
	brakeStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));

	// Current settings.

	setRegionControl(regionControl);
}

BioXASSSRLMonochromatorRegionControlView::~BioXASSSRLMonochromatorRegionControlView()
{

}

void BioXASSSRLMonochromatorRegionControlView::setRegionControl(BioXASSSRLMonochromatorRegionControl *newControl)
{
	if (regionControl_ != newControl) {

		if (regionControl_)
			disconnect( regionControl_, 0, this, 0 );

		regionControl_ = newControl;

		if (regionControl_) {
			connect( regionControl_, SIGNAL(slitsStatusChanged(double)), this, SLOT(onSlitsStatusChanged()) );
			connect( regionControl_, SIGNAL(paddleStatusChanged(double)), this, SLOT(onPaddleStatusChanged()) );
			connect( regionControl_, SIGNAL(keyStatusChanged(double)), this, SLOT(onKeyStatusChanged()) );
			connect( regionControl_, SIGNAL(brakeStatusChanged(double)), this, SLOT(onBrakeStatusChanged()) );
		}
	}
}

void BioXASSSRLMonochromatorRegionControlView::onSlitsStatusChanged()
{
	if (regionControl_ && regionControl_->slitsStatusControl() && regionControl_->slitsStatusControl()->isConnected()) {
		if (regionControl_->slitsStatusControl()->value() == BioXASSSRLMonochromator::Slits::Closed) {
			slitsStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOn.png"));
			slitsStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));

		} else if (regionControl_->slitsStatusControl()->value() == BioXASSSRLMonochromator::Slits::NotClosed){
			slitsStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
			slitsStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOn.png"));

		} else {
			slitsStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
			slitsStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));
		}

	} else {
		slitsStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
		slitsStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));
	}
}

void BioXASSSRLMonochromatorRegionControlView::onPaddleStatusChanged()
{
	if (regionControl_ && regionControl_->paddleStatusControl()->isConnected()) {
		if (regionControl_->paddleStatusControl()->value() == BioXASSSRLMonochromator::Paddle::Out) {
			paddleStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOn.png"));
			paddleStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));

		} else if (regionControl_->paddleStatusControl()->value() == BioXASSSRLMonochromator::Paddle::NotOut){
			paddleStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
			paddleStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOn.png"));

		}  else {
			paddleStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
			paddleStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));
		}

	} else {
		paddleStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
		paddleStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));
	}
}

void BioXASSSRLMonochromatorRegionControlView::onKeyStatusChanged()
{
	if (regionControl_ && regionControl_->keyStatusControl()->isConnected()) {
		if (regionControl_->keyStatusControl()->value() == BioXASSSRLMonochromator::Key::Enabled) {
			keyStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOn.png"));
			keyStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));

		} else if (regionControl_->keyStatusControl()->value() == BioXASSSRLMonochromator::Key::Disabled){
			keyStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
			keyStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOn.png"));

		} else {
			keyStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
			keyStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));
		}

	} else {
		keyStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
		keyStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));
	}
}

void BioXASSSRLMonochromatorRegionControlView::onBrakeStatusChanged()
{
	if (regionControl_ && regionControl_->brakeStatusControl()->isConnected()) {
		if (regionControl_->brakeStatusControl()->value() == BioXASSSRLMonochromator::Brake::Enabled) {
			brakeStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOn.png"));
			brakeStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));

		} else if (regionControl_->brakeStatusControl()->value() == BioXASSSRLMonochromator::Brake::Disabled){
			brakeStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
			brakeStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOn.png"));

		} else {
			brakeStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
			brakeStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));
		}

	} else {
		brakeStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
		brakeStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));
	}
}
