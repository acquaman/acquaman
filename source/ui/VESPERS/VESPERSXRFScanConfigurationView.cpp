#include "VESPERSXRFScanConfigurationView.h"

#include <QVBoxLayout>
#include <QPushButton>

VESPERSXRFScanConfigurationView::VESPERSXRFScanConfigurationView(VESPERSXRFScanConfiguration *scanConfig, QWidget *parent)
	: AMScanConfigurationView(parent)
{
	configuration_ = scanConfig;

	integrationTime_ = new QDoubleSpinBox;
	integrationTime_->setSuffix(" s");
	integrationTime_->setSingleStep(0.1);
	integrationTime_->setMaximum(1000.00);
	integrationTime_->setAlignment(Qt::AlignCenter);
	connect(integrationTime_, SIGNAL(editingFinished()), this, SLOT(onIntegrationTimeUpdate()));
	connect(configuration_, SIGNAL(integrationTimeChanged(double)), integrationTime_, SLOT(setValue(double)));

	QPushButton *customizeSettings = new QPushButton(QIcon(":/hammer.png"), "Settings");
	customizeSettings->setCheckable(true);
	connect(customizeSettings, SIGNAL(toggled(bool)), this, SLOT(onAdvancedSettingsChanged(bool)));

	minEnergy_ = new QDoubleSpinBox;
	minEnergy_->hide();
	minEnergy_->setSuffix(" keV");
	minEnergy_->setSingleStep(0.01);
	minEnergy_->setMinimum(0.0);
	minEnergy_->setMaximum(30.00);
	minEnergy_->setAlignment(Qt::AlignCenter);
	connect(minEnergy_, SIGNAL(editingFinished()), this, SLOT(onMinimumEnergyUpdate()));
	connect(configuration_, SIGNAL(minimumEnergyChanged(double)), this, SLOT(onMinimumEnergyChanged(double)));

	maxEnergy_ = new QDoubleSpinBox;
	maxEnergy_->hide();
	maxEnergy_->setSuffix(" keV");
	maxEnergy_->setSingleStep(0.01);
	maxEnergy_->setMaximum(30.00);
	maxEnergy_->setAlignment(Qt::AlignCenter);
	connect(maxEnergy_, SIGNAL(editingFinished()), this, SLOT(onMaximumEnergyUpdate()));
	connect(configuration_, SIGNAL(maximumEnergyChanged(double)), this, SLOT(onMaximumEnergyChanged(double)));

	peakingTime_ = new QDoubleSpinBox;
	peakingTime_->hide();
	peakingTime_->setSuffix(QString::fromUtf8(" μs"));
	peakingTime_->setSingleStep(0.01);
	peakingTime_->setMaximum(100);
	peakingTime_->setAlignment(Qt::AlignCenter);
	connect(peakingTime_, SIGNAL(editingFinished()), this, SLOT(onPeakingTimeUpdate()));
	connect(configuration_, SIGNAL(peakingTimeChanged(double)), peakingTime_, SLOT(setValue(double)));

	QFont font(this->font());
	font.setBold(true);

	QLabel *timeLabel = new QLabel("Real Time");
	timeLabel->setFont(font);
	minEnergyLabel_ = new QLabel("Min. Energy");
	minEnergyLabel_->hide();
	minEnergyLabel_->setFont(font);
	maxEnergyLabel_ = new QLabel("Max. Energy");
	maxEnergyLabel_->hide();
	maxEnergyLabel_->setFont(font);
	peakingTimeLabel_ = new QLabel("Peaking Time");
	peakingTimeLabel_->hide();
	peakingTimeLabel_->setFont(font);

	QVBoxLayout *controlLayout = new QVBoxLayout;
	controlLayout->addWidget(timeLabel);
	controlLayout->addWidget(integrationTime_);
	controlLayout->addWidget(customizeSettings);
	controlLayout->addWidget(minEnergyLabel_);
	controlLayout->addWidget(minEnergy_);
	controlLayout->addWidget(maxEnergyLabel_);
	controlLayout->addWidget(maxEnergy_);
	controlLayout->addWidget(peakingTimeLabel_);
	controlLayout->addWidget(peakingTime_);

	setLayout(controlLayout);
}

VESPERSXRFScanConfigurationView::~VESPERSXRFScanConfigurationView()
{

}

void VESPERSXRFScanConfigurationView::onAdvancedSettingsChanged(bool advanced)
{
	minEnergyLabel_->setVisible(advanced);
	minEnergy_->setVisible(advanced);
	maxEnergyLabel_->setVisible(advanced);
	maxEnergy_->setVisible(advanced);
	peakingTimeLabel_->setVisible(advanced);
	peakingTime_->setVisible(advanced);
}
