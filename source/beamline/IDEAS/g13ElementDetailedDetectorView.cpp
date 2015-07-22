#include "g13ElementDetailedDetectorView.h"

g13ElementDetailedDetectorView::g13ElementDetailedDetectorView(IDEAS13ElementGeDetector *detector, QWidget *parent)
	: IDEASXRFDetailedDetectorViewWithSave(detector, parent)
{
	g13ElementDetector_ = detector;
}

g13ElementDetailedDetectorView::~g13ElementDetailedDetectorView()
{

}

void g13ElementDetailedDetectorView::buildDetectorView()
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
	connect(g13ElementDetector_, SIGNAL(peakingTimeChanged(double)), this, SLOT(onPeakingTimeChanged()));
	connect(g13ElementDetector_, SIGNAL(connected(bool)), this, SLOT(onPeakingTimeChanged()));

	rightLayout_->addWidget(peakingTimeBox_);

}

void g13ElementDetailedDetectorView::onPeakingTimeBoxChanged(const QString &arg1)
{
	if (arg1 == "High Rate / Low Res")
	{
		g13ElementDetector_->setPeakingTime(0.300);
		g13ElementDetector_->setPreampGain(1.2600);
	}
	else if (arg1 == "High Res / Low Rate")
	{
		g13ElementDetector_->setPeakingTime(0.200);
		g13ElementDetector_->setPreampGain(1.2375);
	}
	else if (arg1 == "Ultra Res / Slow Rate")
	{
		g13ElementDetector_->setPeakingTime(4.00);
		g13ElementDetector_->setPreampGain(1.2375);
	}

}

void g13ElementDetailedDetectorView::onPeakingTimeChanged()
{

	// HACK ugly hard coded magic numbers...   Works for now.  -D.Muir
	peakingTimeBox_->blockSignals(true);

	if (g13ElementDetector_->peakingTime() == 0.3)
	    peakingTimeBox_->setCurrentIndex(1);
	else if (g13ElementDetector_->peakingTime() == 2.0)
	    peakingTimeBox_->setCurrentIndex(2);
	else if (g13ElementDetector_->peakingTime() == 4.0)
	    peakingTimeBox_->setCurrentIndex(3);
	else
	    peakingTimeBox_->setCurrentIndex(0);

	peakingTimeBox_->blockSignals(false);

}
