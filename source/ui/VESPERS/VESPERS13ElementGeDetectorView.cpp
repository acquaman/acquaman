#include "VESPERS13ElementGeDetectorView.h"

VESPERS13ElementGeDetectorView::VESPERS13ElementGeDetectorView(VESPERS13ElementGeDetector *detector, QWidget *parent)
	: VESPERSXRFDetailedDetectorView(detector, parent)
{
	ge13ElementDetector_ = detector;
}

void VESPERS13ElementGeDetectorView::buildDetectorView()
{
	VESPERSXRFDetailedDetectorView::buildDetectorView();

	peakingTimeSpinBox_ = new QDoubleSpinBox;
	peakingTimeSpinBox_->setMinimum(0);
	peakingTimeSpinBox_->setMaximum(100000);
	peakingTimeSpinBox_->setPrefix("Peaking Time: ");
	peakingTimeSpinBox_->setSuffix(QString(" %1s").arg(QString::fromUtf8("μ")));
	peakingTimeSpinBox_->setValue(ge13ElementDetector_->peakingTime());
	peakingTimeSpinBox_->setAlignment(Qt::AlignCenter);
	peakingTimeSpinBox_->hide();

	connect(showEnergyRangeSpinBoxes_, SIGNAL(toggled(bool)), peakingTimeSpinBox_, SLOT(setVisible(bool)));
	connect(peakingTimeSpinBox_, SIGNAL(editingFinished()), this, SLOT(onPeakingTimeChanged()));
	connect(ge13ElementDetector_, SIGNAL(peakingTimeChanged(double)), peakingTimeSpinBox_, SLOT(setValue(double)));

	energyRangeLayout_->insertWidget(energyRangeLayout_->count()-1, peakingTimeSpinBox_);
}

void VESPERS13ElementGeDetectorView::onPeakingTimeChanged()
{
	ge13ElementDetector_->setPeakingTime(peakingTimeSpinBox_->value());
}
