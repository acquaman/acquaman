#include "VESPERSTimedLineScanConfigurationView.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <math.h>

VESPERSTimedLineScanConfigurationView::VESPERSTimedLineScanConfigurationView(VESPERSTimedLineScanConfiguration *configuration, QWidget *parent)
	: VESPERSSpatialLineScanConfigurationView(configuration, parent)
{
	timePerAcquisitionBox_ = createDwellTimeSpinBox(timedLineConfiguration()->timePerAcquisition());
	connect(timePerAcquisitionBox_, SIGNAL(editingFinished()), this, SLOT(onTimePerAcquisitionChanged()));
	connect(timedLineConfiguration(), SIGNAL(timePerAcquisitionChanged(double)), timePerAcquisitionBox_, SLOT(setValue(double)));

	iterationsBox_ = new QSpinBox;
	iterationsBox_->setRange(0, 1000000);
	iterationsBox_->setPrefix("#: ");
	iterationsBox_->setAlignment(Qt::AlignCenter);
	iterationsBox_->setValue(timedLineConfiguration()->iterations());
	connect(iterationsBox_, SIGNAL(valueChanged(int)), this, SLOT(onIterationsChanged()));
	connect(timedLineConfiguration(), SIGNAL(iterationsChanged(int)), iterationsBox_, SLOT(setValue(int)));

	helpMessage_ = new QLabel("The Time Per Acquisition must be longer than the time it takes to do the individual line scan!");
	helpMessage_->hide();

	QHBoxLayout *timePerAcquisitionLayout = new QHBoxLayout;
	timePerAcquisitionLayout->addWidget(new QLabel("Time Per Acquisition;"));
	timePerAcquisitionLayout->addWidget(timePerAcquisitionBox_);

	QHBoxLayout *iterationsLayout = new QHBoxLayout;
	iterationsLayout->addWidget(new QLabel("# of Iterations:"));
	iterationsLayout->addWidget(iterationsBox_);

	QVBoxLayout *timeLayout = new QVBoxLayout;
	timeLayout->addLayout(timePerAcquisitionLayout);
	timeLayout->addLayout(iterationsLayout);

	QGroupBox *timeBox = new QGroupBox("Time Attributes");
	timeBox->setLayout(timeLayout);

	contentsLayout_->addWidget(timeBox, 4, 3, 2, 1);
	contentsLayout_->addWidget(helpMessage_, 6, 0, 1, 5);

	connect(configuration_, SIGNAL(timeChanged(double)), this, SLOT(onTimesChanged()));
	connect(configuration_, SIGNAL(totalTimeChanged(double)), this, SLOT(onTimesChanged()));
}

VESPERSTimedLineScanConfigurationView::~VESPERSTimedLineScanConfigurationView()
{

}

VESPERSTimedLineScanConfiguration * VESPERSTimedLineScanConfigurationView::timedLineConfiguration() const
{
	return qobject_cast<VESPERSTimedLineScanConfiguration *>(configuration_);
}

void VESPERSTimedLineScanConfigurationView::onTimePerAcquisitionChanged()
{
	timedLineConfiguration()->setTimePerAcquisition(timePerAcquisitionBox_->value());
}

void VESPERSTimedLineScanConfigurationView::onIterationsChanged()
{
	timedLineConfiguration()->setIterations(iterationsBox_->value());
}

void VESPERSTimedLineScanConfigurationView::onTimesChanged()
{
	QPalette good(this->palette());
	QPalette bad(good);
	bad.setColor(QPalette::Base, Qt::red);

	double size = fabs(configuration_->end()-configuration_->start());
	int points = int(fabs(size/configuration_->step()));
	if ((size - (points + 0.01)*configuration_->step()) < 0)
		points += 1;
	else
		points += 2;

	AMScanAxisRegion *region = configuration_->scanAxisAt(0)->regionAt(0);

	if (points*(double(region->regionTime())+configuration_->timeOffset()) < timedLineConfiguration()->timePerAcquisition()){

		timePerAcquisitionBox_->setPalette(good);
		helpMessage_->hide();
	}

	else{

		timePerAcquisitionBox_->setPalette(bad);
		helpMessage_->show();
	}
}
