#include "VESPERSTimedLineScanConfigurationView.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>

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

	contentsLayout_->addWidget(timeBox, 3, 3, 2, 1);
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
	dwellTime_->setMaximum(timePerAcquisitionBox_->value());
}

void VESPERSTimedLineScanConfigurationView::onIterationsChanged()
{
	timedLineConfiguration()->setIterations(iterationsBox_->value());
}
