#include "VESPERSSingleElementVortexDetectorView.h"

VESPERSSingleElementVortexDetectorView::VESPERSSingleElementVortexDetectorView(VESPERSSingleElementVortexDetector *detector, QWidget *parent)
	: AMXRFDetailedDetectorView(detector, parent)
{
	singleElementVortexDetector_ = detector;
}

void VESPERSSingleElementVortexDetectorView::buildDetectorView()
{
	AMXRFDetailedDetectorView::buildDetectorView();

	peakingTimeSpinBox_ = new QDoubleSpinBox;
	peakingTimeSpinBox_->setMinimum(0);
	peakingTimeSpinBox_->setMaximum(100000);
	peakingTimeSpinBox_->setPrefix("Peaking Time: ");
	peakingTimeSpinBox_->setSuffix(QString(" %1s").arg(QString::fromUtf8("μ")));
	peakingTimeSpinBox_->setValue(singleElementVortexDetector_->peakingTime());
	peakingTimeSpinBox_->setAlignment(Qt::AlignCenter);
	peakingTimeSpinBox_->hide();

	connect(showEnergyRangeSpinBoxes_, SIGNAL(toggled(bool)), peakingTimeSpinBox_, SLOT(setVisible(bool)));
	connect(peakingTimeSpinBox_, SIGNAL(editingFinished()), this, SLOT(onPeakingTimeChanged()));
	connect(singleElementVortexDetector_, SIGNAL(peakingTimeChanged(double)), peakingTimeSpinBox_, SLOT(setValue(double)));

	energyRangeLayout_->insertWidget(energyRangeLayout_->count()-1, peakingTimeSpinBox_);
}

void VESPERSSingleElementVortexDetectorView::setMaximumEnergyImplementation(double energy)
{
	singleElementVortexDetector_->setMaximumEnergy(energy/1000.0);
}

void VESPERSSingleElementVortexDetectorView::onPeakingTimeChanged()
{
	singleElementVortexDetector_->setPeakingTime(peakingTimeSpinBox_->value());
}
