#include "BioXASSSRLMonochromatorCalibrationView.h"
#include "beamline/BioXAS/BioXASSSRLMonochromator.h"

BioXASSSRLMonochromatorCalibrationView::BioXASSSRLMonochromatorCalibrationView(BioXASSSRLMonochromator *mono, QWidget *parent) :
    QWidget(parent)
{
	// Initialize class variables.

	mono_ = 0;

	// Create UI elements.

	calibrateEnergyButton_ = new QPushButton("Calibrate energy");
	connect( calibrateEnergyButton_, SIGNAL(clicked()), this, SLOT(onCalibrateEnergyButtonClicked()) );

	calibrateBraggButton_ = new QPushButton("Calibrate goniometer");
	connect( calibrateBraggButton_, SIGNAL(clicked()), this, SLOT(onCalibrateBraggButtonClicked()) );

	// Create and set main layout.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->setMargin(0);
	layout->addWidget(calibrateEnergyButton_);
	layout->addWidget(calibrateBraggButton_);

	setLayout(layout);

	// Current settings.

	setMono(mono);

	refresh();
}

BioXASSSRLMonochromatorCalibrationView::~BioXASSSRLMonochromatorCalibrationView()
{

}

void BioXASSSRLMonochromatorCalibrationView::refresh()
{
	// Update view elements.

	updateCalibrateEnergyButton();
	updateCalibrateBraggButton();
}

void BioXASSSRLMonochromatorCalibrationView::setMono(BioXASSSRLMonochromator *newControl)
{
	if (mono_ != newControl) {

		if (mono_)
			disconnect( mono_, 0, this, 0 );

		mono_ = newControl;

		if (mono_)
			connect( mono_, SIGNAL(connected(bool)), this, SLOT(refresh()) );

		refresh();

		emit monoChanged(mono_);
	}
}

void BioXASSSRLMonochromatorCalibrationView::updateCalibrateEnergyButton()
{
	bool enabled = false;

	if (mono_)
		enabled = true;

	calibrateEnergyButton_->setEnabled(enabled);
}

void BioXASSSRLMonochromatorCalibrationView::updateCalibrateBraggButton()
{
	bool enabled = false;

	if (mono_)
		enabled = true;

	calibrateBraggButton_->setEnabled(enabled);
}

void BioXASSSRLMonochromatorCalibrationView::onCalibrateEnergyButtonClicked()
{
	if (mono_) {
		AMControl *energyControl = mono_->energy();

		if (energyControl) {
			bool inputOK = false;
			double oldEnergy = energyControl->value();
			double newEnergy = QInputDialog::getDouble(this, "Energy Calibration", "Enter calibrated energy:", oldEnergy, BIOXASSSRLMONOCHROMATORENERGYVIEW_ENERGY_MIN, BIOXASSSRLMONOCHROMATORENERGYVIEW_ENERGY_MAX, 2, &inputOK);

			if (inputOK)
				energyControl->calibrate(oldEnergy, newEnergy);
		}
	}
}

void BioXASSSRLMonochromatorCalibrationView::onCalibrateBraggButtonClicked()
{
	if (mono_) {
		AMControl *braggControl = mono_->bragg();

		if (braggControl) {
			bool inputOK = false;
			double oldPosition = braggControl->value();
			double newPosition = QInputDialog::getDouble(this, "Goniometer Calibration", "Enter calibrated goniometer position:", oldPosition, BIOXASSSRLMONOCHROMATORENERGYVIEW_BRAGG_MIN, BIOXASSSRLMONOCHROMATORENERGYVIEW_BRAGG_MAX, 2, &inputOK);

			if (inputOK)
				braggControl->calibrate(oldPosition, newPosition);
		}
	}
}
