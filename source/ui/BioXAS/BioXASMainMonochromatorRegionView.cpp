#include "BioXASMainMonochromatorRegionView.h"

BioXASMainMonochromatorRegionView::BioXASMainMonochromatorRegionView(BioXASMainMonochromator *mono, QWidget *parent) :
	QGroupBox(parent)
{
	// Initialize variables.

	mono_ = 0;

	// Create UI elements.

	QLabel *regionPrompt = new QLabel("Region:");
	regionText_ = new QLabel();

	QLabel *keyStatusPrompt = new QLabel("Key status:");
	keyStatusText_ = new QLabel();
	keyStatusLED_ = new QLabel();

	QLabel *brakeStatusPrompt = new QLabel("Brake status:");
	brakeStatusText_ = new QLabel();
	brakeStatusLED_ = new QLabel();
//	upperSlitEditor_ = 0;
//	lowerSlitEditor_ = 0;
//	braggEditor_ = 0;
//	crystalChangeEditor_ = 0;

	// Create and set layouts.

	QGridLayout *statusLayout = new QGridLayout();
	statusLayout->setMargin(0);
	statusLayout->addWidget(regionPrompt, 0, 0, 1, 1, Qt::AlignRight);
	statusLayout->addWidget(regionText_, 0, 1, 1, 1, Qt::AlignLeft);
	statusLayout->addWidget(keyStatusPrompt, 1, 0, 1, 1, Qt::AlignRight);
	statusLayout->addWidget(keyStatusText_, 1, 1, 1, 1, Qt::AlignLeft);
	statusLayout->addWidget(keyStatusLED_, 1, 2, 1, 1, Qt::AlignCenter);
	statusLayout->addWidget(brakeStatusPrompt, 2, 0, 1, 1, Qt::AlignRight);
	statusLayout->addWidget(brakeStatusText_, 2, 1, 1, 1, Qt::AlignLeft);
	statusLayout->addWidget(brakeStatusLED_, 2, 2, 1, 1, Qt::AlignCenter);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addLayout(statusLayout);
//	layout->addWidget(upperSlitEditor_);
//	layout->addWidget(lowerSlitEditor_);
//	layout->addWidget(braggEditor_);
//	layout->addWidget(crystalChangeEditor_);

	setLayout(layout);

	// Current settings.

	setTitle("Region");

	onRegionChanged();
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

		if (mono_) {
			disconnect( mono_, 0, this, 0 );
		}

		mono_ = newMono;

		if (mono_) {
			connect( mono_, SIGNAL(regionChanged(double)), this, SLOT(onRegionChanged()) );
			connect( mono_->keyStatus(), SIGNAL(valueChanged(double)), this, SLOT(onKeyStatusChanged()) );
			connect( mono_->brakeStatus(), SIGNAL(valueChanged(double)), this, SLOT(onBrakeStatusChanged()) );
		}

		onRegionChanged();
		onKeyStatusChanged();
		onBrakeStatusChanged();

		emit monoChanged(mono_);
	}
}

void BioXASMainMonochromatorRegionView::onRegionChanged()
{
	if (mono_ && mono_->regionControl()->isConnected()) {
		int currentRegion = mono_->regionControl()->value();

		if (currentRegion == BioXASMainMonochromatorRegionControl::Region::A)
			regionText_->setText("A");
		else if (currentRegion == BioXASMainMonochromatorRegionControl::Region::B)
			regionText_->setText("B");
		else
			regionText_->setText("None");

	} else {
		regionText_->setText("Not connected");
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
		keyStatusText_->setText("Not connected");
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
		brakeStatusText_->setText("Not connected");
		brakeStatusLED_->setPixmap(QPixmap(":/22x22/greenLEDOff.png"));
	}
}
