#include "BioXASSSRLMonochromatorRegionView.h"

BioXASSSRLMonochromatorRegionView::BioXASSSRLMonochromatorRegionView(BioXASSSRLMonochromator *mono, QWidget *parent) :
	QGroupBox(parent)
{
	// Initialize member variables.

	mono_ = 0;

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

	setMono(mono);
}

BioXASSSRLMonochromatorRegionView::~BioXASSSRLMonochromatorRegionView()
{

}

void BioXASSSRLMonochromatorRegionView::setMono(BioXASSSRLMonochromator *newMono)
{
	if (mono_ != newMono) {

		if (mono_) {
			disconnect( mono_->slitsStatusControl(), 0, this, 0 );
			disconnect( mono_->paddleStatusControl(), 0, this, 0 );
			disconnect( mono_->keyStatusControl(), 0, this, 0 );
			disconnect( mono_->brakeStatusControl(), 0, this, 0 );
		}

		mono_ = newMono;

		if (mono_) {
			connect( mono_->slitsStatusControl(), SIGNAL(valueChanged(double)), this, SLOT(onSlitsStatusChanged()) );
			connect( mono_->paddleStatusControl(), SIGNAL(valueChanged(double)), this, SLOT(onPaddleStatusChanged()) );
			connect( mono_->keyStatusControl(), SIGNAL(valueChanged(double)), this, SLOT(onKeyStatusChanged()) );
			connect( mono_->brakeStatusControl(), SIGNAL(valueChanged(double)), this, SLOT(onBrakeStatusChanged()) );
		}

		onSlitsStatusChanged();
		onPaddleStatusChanged();
		onKeyStatusChanged();
		onBrakeStatusChanged();

		emit monoChanged(mono_);
	}
}

void BioXASSSRLMonochromatorRegionView::onSlitsStatusChanged()
{
	if (mono_ && mono_->slitsStatusControl()->isConnected()) {
		if (mono_->slitsStatusControl()->value() == BioXASSSRLMonochromator::Slits::Closed) {
			slitsStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOn.png"));
			slitsStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));

		} else if (mono_->slitsStatusControl()->value() == BioXASSSRLMonochromator::Slits::NotClosed){
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

void BioXASSSRLMonochromatorRegionView::onPaddleStatusChanged()
{
	if (mono_ && mono_->paddleStatusControl()->isConnected()) {
		if (mono_->paddleStatusControl()->value() == BioXASSSRLMonochromator::Paddle::Out) {
			paddleStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOn.png"));
			paddleStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));

		} else if (mono_->paddleStatusControl()->value() == BioXASSSRLMonochromator::Paddle::NotOut){
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

void BioXASSSRLMonochromatorRegionView::onKeyStatusChanged()
{
	if (mono_ && mono_->keyStatusControl()->isConnected()) {
		if (mono_->keyStatusControl()->value() == BioXASSSRLMonochromator::Key::Enabled) {
			keyStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOn.png"));
			keyStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));

		} else if (mono_->keyStatusControl()->value() == BioXASSSRLMonochromator::Key::Disabled){
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

void BioXASSSRLMonochromatorRegionView::onBrakeStatusChanged()
{
	if (mono_ && mono_->brakeStatusControl()->isConnected()) {
		if (mono_->brakeStatusControl()->value() == BioXASSSRLMonochromator::Brake::Enabled) {
			brakeStatusGreen_->setPixmap(QPixmap(":/22x22/greenLEDOn.png"));
			brakeStatusRed_->setPixmap(QPixmap(":/22x22/redLEDOff.png"));

		} else if (mono_->brakeStatusControl()->value() == BioXASSSRLMonochromator::Brake::Disabled){
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
