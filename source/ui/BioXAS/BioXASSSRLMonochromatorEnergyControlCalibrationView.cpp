#include "BioXASSSRLMonochromatorEnergyControlCalibrationView.h"

BioXASSSRLMonochromatorEnergyControlCalibrationView::BioXASSSRLMonochromatorEnergyControlCalibrationView(BioXASSSRLMonochromatorEnergyControlCalibrationController *controller, QWidget *parent) :
    QWidget(parent)
{
	// Create UI elements.

//	plotWidget_ = new MPlotWidget();

	scanView_ = new AMScanView();

	energy_ = new QDoubleSpinBox();
	energy_->setSuffix("eV");
	energy_->setMinimum(0);
	energy_->setMaximum(1000000);
	energy_->setValue(0);
	energy_->setEnabled(false);

	applyButton_ = new QPushButton("Apply");
	applyButton_->setEnabled(false);

	cancelButton_ = new QPushButton("Cancel");

	// Create and set layouts.

	QHBoxLayout *energyLayout = new QHBoxLayout();
	energyLayout->addStretch();
	energyLayout->addWidget(energy_);

	QHBoxLayout *buttonLayout = new QHBoxLayout();
	buttonLayout->addStretch();
	buttonLayout->addWidget(applyButton_);
	buttonLayout->addWidget(cancelButton_);

	QVBoxLayout *layout = new QVBoxLayout();
//	layout->addWidget(plotWidget_);
	layout->addWidget(scanView_);
	layout->addLayout(energyLayout);
	layout->addLayout(buttonLayout);

	setLayout(layout);

	// Current settings.

	setWindowTitle("Energy calibration");
	setCalibrationController(controller);

	// Make connections.

	connect( energy_, SIGNAL(valueChanged(double)), this, SLOT(onEnergyChanged()) );
	connect( applyButton_, SIGNAL(clicked()), this, SLOT(onApplyButtonClicked()) );
	connect( cancelButton_, SIGNAL(clicked()), this, SLOT(onCancelButtonClicked()) );
}

BioXASSSRLMonochromatorEnergyControlCalibrationView::~BioXASSSRLMonochromatorEnergyControlCalibrationView()
{

}

void BioXASSSRLMonochromatorEnergyControlCalibrationView::setCalibrationController(BioXASSSRLMonochromatorEnergyControlCalibrationController *newController)
{
	if (controller_ != newController) {

		if (controller_) {
//			plotWidget_->setPlot(0);
			scanView_->
			energy_->setEnabled(false);
			applyButton_->setEnabled(false);
		}

		controller_ = newController;

		if (controller_) {
			controller_->setParent(this);

			plotWidget_->setPlot(controller_->plot());
			energy_->setEnabled(true);
			energy_->setValue(controller_->energy());
			applyButton_->setEnabled(true);
		}

		emit calibrationControllerChanged(controller_);
	}
}

void BioXASSSRLMonochromatorEnergyControlCalibrationView::onEnergyChanged()
{
	if (controller_) {
		controller_->setEnergy(energy_->value());
	}
}

void BioXASSSRLMonochromatorEnergyControlCalibrationView::onApplyButtonClicked()
{
	if (controller_) {
		controller_->applyCalibrationSettings();
	}
}

void BioXASSSRLMonochromatorEnergyControlCalibrationView::onCancelButtonClicked()
{
	close();
}


