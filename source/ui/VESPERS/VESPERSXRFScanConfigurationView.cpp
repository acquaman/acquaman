#include "VESPERSXRFScanConfigurationView.h"
#include "beamline/VESPERS/VESPERSBeamline.h"
#include "acquaman/VESPERS/VESPERSXRFScanController.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolButton>

VESPERSXRFScanConfigurationView::VESPERSXRFScanConfigurationView(VESPERSXRFScanConfiguration *scanConfig, QWidget *parent)
	: AMScanConfigurationView(parent)
{
	configuration_ = scanConfig;
	view_ = new XRFDetailedDetectorView(scanConfig->detector());
	detector_ = scanConfig->detector();

	connect(detector_, SIGNAL(detectorConnected(bool)), this, SLOT(setEnabled(bool)));

	QToolButton *start = new QToolButton;
	start->setIcon(QIcon(":/play_button_green.png"));
	connect(start, SIGNAL(clicked()), this, SLOT(onStartClicked()));

	QToolButton *stop = new QToolButton;
	stop->setIcon(QIcon(":/red-stop-button.png"));
	connect(stop, SIGNAL(clicked()), this, SLOT(onStopClicked()));

	integrationTime_ = new QDoubleSpinBox;
	integrationTime_->setSuffix(" s");
	integrationTime_->setSingleStep(0.1);
	integrationTime_->setMaximum(1000.00);
	integrationTime_->setAlignment(Qt::AlignCenter);
	connect(integrationTime_, SIGNAL(editingFinished()), this, SLOT(onIntegrationTimeUpdate()));
	connect(detector_->integrationTimeControl(), SIGNAL(valueChanged(double)), integrationTime_, SLOT(setValue(double)));

	maxEnergy_ = new QDoubleSpinBox;
	maxEnergy_->setSuffix(" keV");
	maxEnergy_->setSingleStep(0.01);
	maxEnergy_->setMaximum(30.00);
	maxEnergy_->setAlignment(Qt::AlignCenter);
	connect(maxEnergy_, SIGNAL(editingFinished()), this, SLOT(onMaximumEnergyUpdate()));
	connect(detector_->maximumEnergyControl(), SIGNAL(valueChanged(double)), maxEnergy_, SLOT(setValue(double)));

	peakingTime_ = new QDoubleSpinBox;
	peakingTime_->setSuffix(QString(" %1s").arg(QString::fromUtf8("μ")));
	peakingTime_->setSingleStep(0.01);
	peakingTime_->setMaximum(100);
	peakingTime_->setMinimum(1.0);
	peakingTime_->setAlignment(Qt::AlignCenter);
	connect(peakingTime_, SIGNAL(editingFinished()), this, SLOT(onPeakingTimeUpdate()));
	connect(detector_->peakingTimeControl(), SIGNAL(valueChanged(double)), this, SLOT(onPeakingTimeUpdate()));

	QFont font(this->font());
	font.setBold(true);

	QLabel *startLabel = new QLabel("Start & Stop");
	startLabel->setFont(font);
	QLabel *timeLabel = new QLabel("Real Time");
	timeLabel->setFont(font);
	QLabel *maxEnergyLabel = new QLabel("Max. Energy");
	maxEnergyLabel->setFont(font);
	QLabel *peakingTimeLabel = new QLabel("Peaking Time");
	peakingTimeLabel->setFont(font);

	QHBoxLayout *startAndStopLayout = new QHBoxLayout;
	startAndStopLayout->addWidget(start);
	startAndStopLayout->addWidget(stop);

	QVBoxLayout *controlLayout = new QVBoxLayout;
	controlLayout->addSpacing(20);
	controlLayout->addWidget(startLabel);
	controlLayout->addLayout(startAndStopLayout);
	controlLayout->addWidget(timeLabel);
	controlLayout->addWidget(integrationTime_);
	controlLayout->addWidget(maxEnergyLabel);
	controlLayout->addWidget(maxEnergy_);
	controlLayout->addWidget(peakingTimeLabel);
	controlLayout->addWidget(peakingTime_);
	controlLayout->addStretch();

	QHBoxLayout *plotControlLayout = new QHBoxLayout;
	plotControlLayout->addWidget(view_);
	plotControlLayout->addLayout(controlLayout);

	setLayout(plotControlLayout);
}

void VESPERSXRFScanConfigurationView::onIntegrationTimeUpdate()
{
	detector_->setTime(integrationTime_->value());
}

void VESPERSXRFScanConfigurationView::onMaximumEnergyUpdate()
{
	detector_->setMaximumEnergyControl(maxEnergy_->value());
}

void VESPERSXRFScanConfigurationView::onPeakingTimeUpdate()
{
	detector_->setPeakingTimeControl(peakingTime_->value());
}

void VESPERSXRFScanConfigurationView::onStopClicked()
{
	VESPERSXRFScanController *current = qobject_cast<VESPERSXRFScanController *>(AMScanControllerSupervisor::scanControllerSupervisor()->currentScanController());

	if (current)
		current->finish();
}
