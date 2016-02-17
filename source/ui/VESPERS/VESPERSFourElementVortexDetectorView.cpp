#include "VESPERSFourElementVortexDetectorView.h"

VESPERSFourElementVortexDetectorView::VESPERSFourElementVortexDetectorView(VESPERSFourElementVortexDetector *detector, QWidget *parent)
	: AMXRFDetailedDetectorView(detector, parent)
{
	fourElementVortexDetector_ = detector;
}

void VESPERSFourElementVortexDetectorView::buildDetectorView()
{
	AMXRFDetailedDetectorView::buildDetectorView();

	peakingTimeSpinBox_ = new QDoubleSpinBox;
	peakingTimeSpinBox_->setMinimum(0);
	peakingTimeSpinBox_->setMaximum(100000);
	peakingTimeSpinBox_->setPrefix("Peaking Time: ");
	peakingTimeSpinBox_->setSuffix(QString(" %1s").arg(QString::fromUtf8("μ")));
	peakingTimeSpinBox_->setValue(fourElementVortexDetector_->peakingTime());
	peakingTimeSpinBox_->setAlignment(Qt::AlignCenter);
	peakingTimeSpinBox_->hide();

	connect(showEnergyRangeSpinBoxes_, SIGNAL(toggled(bool)), peakingTimeSpinBox_, SLOT(setVisible(bool)));
	connect(peakingTimeSpinBox_, SIGNAL(editingFinished()), this, SLOT(onPeakingTimeChanged()));
	connect(fourElementVortexDetector_, SIGNAL(peakingTimeChanged(double)), peakingTimeSpinBox_, SLOT(setValue(double)));

	energyRangeLayout_->insertWidget(energyRangeLayout_->count()-1, peakingTimeSpinBox_);
}

void VESPERSFourElementVortexDetectorView::setMaximumEnergyImplementation(double energy)
{
	fourElementVortexDetector_->setMaximumEnergy(energy/1000.0);
}

void VESPERSFourElementVortexDetectorView::onPeakingTimeChanged()
{
	fourElementVortexDetector_->setPeakingTime(peakingTimeSpinBox_->value());
}
