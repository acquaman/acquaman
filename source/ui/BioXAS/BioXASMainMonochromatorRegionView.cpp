#include "BioXASMainMonochromatorRegionView.h"

BioXASMainMonochromatorRegionView::BioXASMainMonochromatorRegionView(BioXASMainMonochromator *mono, QWidget *parent) :
	QWidget(parent)
{
	// Initialize variables.

	mono_ = 0;

	// Create UI elements.

	QLabel *keyStatusPrompt = new QLabel("Key status:");
	keyStatusText_ = new QLabel();
	keyStatusLED_ = new QLabel();

	QLabel *brakeStatusPrompt = new QLabel("Brake status:");
	brakeStatusText_ = new QLabel();
	brakeStatusLED_ = new QLabel();
//	upperSlitEditor_ = new BioXASCLSMAXvMotorControlEditor(0);
//	lowerSlitEditor_ = new BioXASCLSMAXvMotorControlEditor(0);
//	braggEditor_ = new BioXASCLSMAXvMotorControlEditor(0);
//	crystalChangeEditor_ = new BioXASCLSMAXvMotorControlEditor(0);

	// Create and set layouts.

	QGridLayout *statusLayout = new QGridLayout();
	statusLayout->setMargin(0);
	statusLayout->addWidget(keyStatusPrompt, 0, 0);
	statusLayout->addWidget(keyStatusText_, 0, 1);
	statusLayout->addWidget(keyStatusLED_, 0, 2);
	statusLayout->addWidget(brakeStatusPrompt, 1, 0);
	statusLayout->addWidget(brakeStatusText_, 1, 1);
	statusLayout->addWidget(brakeStatusLED_, 1, 2);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addLayout(statusLayout);
//	layout->addWidget(upperSlitEditor_);
//	layout->addWidget(lowerSlitEditor_);
//	layout->addWidget(braggEditor_);
//	layout->addWidget(crystalChangeEditor_);

	setLayout(layout);

	// Current settings.

	onKeyStatusChanged();
	onBrakeStatusChanged();

	setMono(mono);
}

BioXASMainMonochromatorRegionView::~BioXASMainMonochromatorRegionView()
{

}

void BioXASMainMonochromatorRegionView::setMono(BioXASMainMonochromator *newMono)
{
	if (mono_ != newMono) {

		if (mono_)
			disconnect( mono_, 0, this, 0 );

		mono_ = newMono;

		if (mono_) {
			connect( mono_->keyStatus(), SIGNAL(valueChanged(double)), this, SLOT(onKeyStatusChanged()) );
			connect( mono_->brakeStatus(), SIGNAL(valueChanged(double)), this, SLOT(onBrakeStatusChanged()) );
		}

		onKeyStatusChanged();
		onBrakeStatusChanged();

		emit monoChanged(mono_);
	}
}

void BioXASMainMonochromatorRegionView::onKeyStatusChanged()
{
	if (mono_ && mono_->keyStatus()->isConnected()) {
		if (mono_->keyStatus()->value() == BioXASMainMonochromator::Key::Enabled) {
			keyStatusText_->setText("Enabled");
			keyStatusLED_->setPixmap(QPixmap(":/22x22/greenLEDOn.png"));

		} else {
			keyStatusText_->setText("Disabled");
			keyStatusLED_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
		}

	} else {
		keyStatusText_->setText("Disconnected");
		keyStatusLED_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
	}
}

void BioXASMainMonochromatorRegionView::onBrakeStatusChanged()
{
	if (mono_ && mono_->brakeStatus()->isConnected()) {
		if (mono_->brakeStatus()->value() == BioXASMainMonochromator::Brake::Enabled) {
			brakeStatusText_->setText("Enabled");
			brakeStatusLED_->setPixmap(QPixmap(":/22x22/greenLEDOn.png"));

		} else {
			brakeStatusText_->setText("Disabled");
			brakeStatusLED_->setPixmap(QPixmap(":/22x22/redLEDOn.png"));
		}

	} else {
		brakeStatusText_->setText("Disconnected");
		brakeStatusLED_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
	}
}
