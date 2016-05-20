#include "IDEASKETEKDetailedDetectorView.h"

IDEASKETEKDetailedDetectorView::IDEASKETEKDetailedDetectorView(IDEASKETEKDetector *detector, QWidget *parent)
	: IDEASXRFDetailedDetectorView(detector, parent)
{
	ketekDetector_ = detector;
}

IDEASKETEKDetailedDetectorView::~IDEASKETEKDetailedDetectorView()
{

}

void IDEASKETEKDetailedDetectorView::buildDetectorView()
{
	IDEASXRFDetailedDetectorView::buildDetectorView();


	peakingTimeBox_ = new QComboBox();
	peakingTimeBox_->setObjectName(QString::fromUtf8("peakingTimeBox"));
	peakingTimeBox_->addItem("Setting Unknown");
	peakingTimeBox_->addItem("High Rate / Low Res");
	peakingTimeBox_->addItem("High Res / Low Rate");
	peakingTimeBox_->addItem("Ultra Res / Slow Rate");

	connect(ketekDetector_, SIGNAL(peakingTimeChanged(double)), this, SLOT(onPeakingTimeChanged()));
	connect(peakingTimeBox_, SIGNAL(currentIndexChanged(QString)), this, SLOT(onPeakingTimeBoxChanged(QString)));

	rightLayout_->addWidget(new QLabel("Detector Mode (Peaking Time):"));
	rightLayout_->addWidget(peakingTimeBox_);

}

void IDEASKETEKDetailedDetectorView::onPeakingTimeBoxChanged(const QString &arg1)
{
	if (arg1 == "High Rate / Low Res")
	{
		ketekDetector_->setPeakingTime(0.300);
		ketekDetector_->setPreampGain(1.2600);
	}
	else if (arg1 == "High Res / Low Rate")
	{
		ketekDetector_->setPeakingTime(2.00);
		ketekDetector_->setPreampGain(1.2375);
	}
	else if (arg1 == "Ultra Res / Slow Rate")
	{
		ketekDetector_->setPeakingTime(4.00);
		ketekDetector_->setPreampGain(1.2375);
	}

}

void IDEASKETEKDetailedDetectorView::onPeakingTimeChanged()
{

	// HACK ugly hard coded magic numbers...   Works for now.  -D.Muir
	peakingTimeBox_->blockSignals(true);

	if (ketekDetector_->peakingTime() == 0.3)
		peakingTimeBox_->setCurrentIndex(1);
	else if (ketekDetector_->peakingTime() == 2.0)
		peakingTimeBox_->setCurrentIndex(2);
	else if (ketekDetector_->peakingTime() == 4.0)
		peakingTimeBox_->setCurrentIndex(3);
	else
		peakingTimeBox_->setCurrentIndex(0);

	peakingTimeBox_->blockSignals(false);

}
