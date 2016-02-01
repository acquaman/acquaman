#include "BioXASSSRLMonochromatorEnergyCalibrationScanConfigurationView.h"

BioXASSSRLMonochromatorEnergyCalibrationScanConfigurationView::BioXASSSRLMonochromatorEnergyCalibrationScanConfigurationView(BioXASSSRLMonochromatorEnergyCalibrationScanConfiguration *config, QWidget *parent) :
	AMScanConfigurationView(parent)
{
	if (config) {
		configuration_ = config;
	} else {
		configuration_ = new BioXASSSRLMonochromatorEnergyCalibrationScanConfiguration();
	}

	scanView_ = new AMScanView();

	scanSetModel_ = scanView_->model();

	configView_ = new BioXASXASScanConfigurationView(configuration_);

	configViewHolder_ = new AMScanConfigurationViewHolder3(configView_, false);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(scanView_);
	layout->addWidget(configViewHolder_);

	setLayout(layout);
}

BioXASSSRLMonochromatorEnergyCalibrationScanConfigurationView::~BioXASSSRLMonochromatorEnergyCalibrationScanConfigurationView()
{

}
