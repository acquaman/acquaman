#include "CLSAmptekSDD123DetectorNewView.h"

#include <QHBoxLayout>
#include "beamline/CLS/CLSAmptekSDD123DetectorNew.h"

 CLSAmptekSDD123DetectorNewView::~CLSAmptekSDD123DetectorNewView(){}
CLSAmptekSDD123DetectorNewView::CLSAmptekSDD123DetectorNewView(CLSAmptekSDD123DetectorNew *detector, QWidget *parent) :
	AMDetectorGeneralDetailedView(detector, parent)
{
	detectorTemperatureDSBox_ = new QDoubleSpinBox();
	detectorTemperatureDSBox_->setMinimum(0);
	detectorTemperatureDSBox_->setMaximum(1000);
	if(detector_->isConnected())
		detectorTemperatureDSBox_->setValue(detector->detectorTemperature());
	detectorTemperatureDSBox_->setEnabled(false);
	mainHL_->addWidget(detectorTemperatureDSBox_);

	connect(detector, SIGNAL(detectorTemperatureChanged(double)), detectorTemperatureDSBox_, SLOT(setValue(double)));
}
