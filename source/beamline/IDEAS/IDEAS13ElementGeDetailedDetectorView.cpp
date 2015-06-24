#include "IDEAS13ElementGeDetailedDetectorView.h"

IDEAS13ElementGeDetailedDetectorView::IDEAS13ElementGeDetailedDetectorView(IDEAS13ElementGeDetector *detector, QWidget *parent)
	: IDEASXRFDetailedDetectorViewWithSave(detector, parent)
{
	ge13ElementDetector_ = detector;
}

IDEAS13ElementGeDetailedDetectorView::~IDEAS13ElementGeDetailedDetectorView()
{

}

void IDEAS13ElementGeDetailedDetectorView::buildDetectorView()
{
	/// Call inherited parent classes buildDetectorView
	IDEASXRFDetailedDetectorViewWithSave::buildDetectorView();

	peakingTimeBox_ = new QComboBox();
	peakingTimeBox_->setObjectName(QString::fromUtf8("peakingTimeBox"));
	peakingTimeBox_->addItem("Setting Unknown");
	peakingTimeBox_->addItem("High Rate / Low Res");
	peakingTimeBox_->addItem("High Res / Low Rate");
	peakingTimeBox_->addItem("Ultra Res / Slow Rate");


	connect(peakingTimeBox_, SIGNAL(currentIndexChanged(QString)), this, SLOT(onPeakingTimeBoxChanged(QString)));
	connect(ge13ElementDetector_, SIGNAL(peakingTimeChanged(double)), this, SLOT(onPeakingTimeChanged()));
	connect(ge13ElementDetector_, SIGNAL(connected(bool)), this, SLOT(onPeakingTimeChanged()));

	rightLayout_->addWidget(peakingTimeBox_);

}

void IDEAS13ElementGeDetailedDetectorView::onPeakingTimeBoxChanged(const QString &arg1)
{
	if (arg1 == "High Rate / Low Res")
	{
		ge13ElementDetector_->setPeakingTime(0.300);
		ge13ElementDetector_->setPreampGain(1.2600);
	}
	else if (arg1 == "High Res / Low Rate")
	{
		ge13ElementDetector_->setPeakingTime(0.200);
		ge13ElementDetector_->setPreampGain(1.2375);
	}
	else if (arg1 == "Ultra Res / Slow Rate")
	{
		ge13ElementDetector_->setPeakingTime(4.00);
		ge13ElementDetector_->setPreampGain(1.2375);
	}

}

void IDEAS13ElementGeDetailedDetectorView::onPeakingTimeChanged()
{

	// HACK ugly hard coded magic numbers...   Works for now.  -D.Muir
	peakingTimeBox_->blockSignals(true);

	if (ge13ElementDetector_->peakingTime() == 0.3)
	    peakingTimeBox_->setCurrentIndex(1);
	else if (ge13ElementDetector_->peakingTime() == 2.0)
	    peakingTimeBox_->setCurrentIndex(2);
	else if (ge13ElementDetector_->peakingTime() == 4.0)
	    peakingTimeBox_->setCurrentIndex(3);
	else
	    peakingTimeBox_->setCurrentIndex(0);

	peakingTimeBox_->blockSignals(false);

}
