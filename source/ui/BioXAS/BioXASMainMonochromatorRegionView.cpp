#include "BioXASMainMonochromatorRegionView.h"

BioXASMainMonochromatorRegionView::BioXASMainMonochromatorRegionView(QWidget *parent) :
	QGroupBox(parent)
{
	// Initialize member variables.

	mono_ = BioXASMainBeamline::bioXAS()->mono();

	// Create UI elements.

	regionEditor_ = new AMExtendedControlEditor(mono_->regionControl());
	regionEditor_->setReadOnly(true);
	regionEditor_->setTitle("Region");

	upperSlitEditor_ = new AMExtendedControlEditor(mono_->upperSlitBladeMotor());
	upperSlitEditor_->setReadOnly(true);
	upperSlitEditor_->setTitle("Upper slit blade");

	lowerSlitEditor_ = new AMExtendedControlEditor(mono_->lowerSlitBladeMotor());
	lowerSlitEditor_->setReadOnly(true);
	lowerSlitEditor_->setTitle("Lower slit blade");

	braggEditor_ = new AMExtendedControlEditor(mono_->braggMotor());
	braggEditor_->setReadOnly(true);
	braggEditor_->setTitle("Bragg");

	crystalChangeCWEditor_ = new AMExtendedControlEditor(mono_->crystalChangeMotor()->cwLimitControl());
	crystalChangeCWEditor_->setReadOnly(true);
	crystalChangeCWEditor_->setTitle("Crystal change CW limit");

	crystalChangeCCWEditor_ = new AMExtendedControlEditor(mono_->crystalChangeMotor()->ccwLimitControl());
	crystalChangeCCWEditor_->setReadOnly(true);
	crystalChangeCCWEditor_->setTitle("Crystal change CCW limit");

	QLabel *keyStatusPrompt = new QLabel("Key status:");
	keyStatusText_ = new QLabel();
	keyStatusLED_ = new QLabel();

	QLabel *brakeStatusPrompt = new QLabel("Brake status:");
	brakeStatusText_ = new QLabel();
	brakeStatusLED_ = new QLabel();

	regionButton_ = new QPushButton("Switch regions");

	// Create and set layouts.

	QGridLayout *statusLayout = new QGridLayout();
	statusLayout->setMargin(0);
	statusLayout->addWidget(keyStatusPrompt, 0, 0, 1, 1, Qt::AlignRight);
	statusLayout->addWidget(keyStatusText_, 0, 1, 1, 1, Qt::AlignLeft);
	statusLayout->addWidget(keyStatusLED_, 0, 2, 1, 1, Qt::AlignCenter);
	statusLayout->addWidget(brakeStatusPrompt, 1, 0, 1, 1, Qt::AlignRight);
	statusLayout->addWidget(brakeStatusText_, 1, 1, 1, 1, Qt::AlignLeft);
	statusLayout->addWidget(brakeStatusLED_, 1, 2, 1, 1, Qt::AlignCenter);

	QHBoxLayout *buttonLayout = new QHBoxLayout();
	buttonLayout->addStretch();
	buttonLayout->addWidget(regionButton_);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->addWidget(regionEditor_);
	layout->addWidget(upperSlitEditor_);
	layout->addWidget(lowerSlitEditor_);
	layout->addWidget(braggEditor_);
	layout->addWidget(crystalChangeCWEditor_);
	layout->addWidget(crystalChangeCCWEditor_);
	layout->addLayout(statusLayout);
	layout->addLayout(buttonLayout);

	setLayout(layout);

	// Make connections.

	connect( mono_->regionControl(), SIGNAL(connected(bool)), this, SLOT(onRegionControlConnectedChanged()) );
	connect( mono_->keyStatus(), SIGNAL(valueChanged(double)), this, SLOT(onKeyStatusChanged()) );
	connect( mono_->brakeStatus(), SIGNAL(valueChanged(double)), this, SLOT(onBrakeStatusChanged()) );
	connect( regionButton_, SIGNAL(clicked()), this, SLOT(onRegionButtonClicked()) );

	// Current settings.

	setTitle("Region controls");
	setFlat(true);

	onRegionControlConnectedChanged();
	onKeyStatusChanged();
	onBrakeStatusChanged();
}

BioXASMainMonochromatorRegionView::~BioXASMainMonochromatorRegionView()
{

}

void BioXASMainMonochromatorRegionView::onRegionControlConnectedChanged()
{
	if (mono_->regionControl()->isConnected())
		regionButton_->setEnabled(true);
	else
		regionButton_->setEnabled(false);
}

void BioXASMainMonochromatorRegionView::onKeyStatusChanged()
{
	if (mono_->keyStatus()->isConnected()) {
		if (mono_->keyStatus()->value() == BioXASSSRLMonochromator::Key::Enabled) {
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
	if (mono_->brakeStatus()->isConnected()) {
		if (mono_->brakeStatus()->value() == BioXASSSRLMonochromator::Brake::Enabled) {
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

void BioXASMainMonochromatorRegionView::onRegionButtonClicked()
{
	if (mono_->regionControl()->isConnected()) {

		// Display the region control view.

		BioXASMainMonochromatorRegionControlView *regionControlView = new BioXASMainMonochromatorRegionControlView(mono_->regionControl(), this);
		regionControlView->setWindowFlags(Qt::Sheet);
		regionControlView->show();

		// Calculate desired region.

		int setpoint;
		int currentRegion = mono_->regionControl()->value();

		if (currentRegion == BioXASSSRLMonochromator::Region::A)
			setpoint = BioXASSSRLMonochromator::Region::B;
		else if (currentRegion == BioXASSSRLMonochromator::Region::B)
			setpoint = BioXASSSRLMonochromator::Region::A;
		else
			setpoint = BioXASSSRLMonochromator::Region::A;

		// Start the region change process.

		mono_->regionControl()->move(setpoint);
	}
}
