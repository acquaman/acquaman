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
	connect(integrationTime_, SIGNAL(editingFinished()), this, SLOT(onIntegrationTimeUpdate()));
	connect(detector_->integrationTimeControl(), SIGNAL(valueChanged(double)), integrationTime_, SLOT(setValue(double)));

	QHBoxLayout *startAndStopLayout = new QHBoxLayout;
	startAndStopLayout->addWidget(start);
	startAndStopLayout->addWidget(stop);

	QVBoxLayout *controlLayout = new QVBoxLayout;
	controlLayout->addLayout(startAndStopLayout);
	controlLayout->addWidget(integrationTime_);
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

void VESPERSXRFScanConfigurationView::onStopClicked()
{
	VESPERSXRFScanController *current = qobject_cast<VESPERSXRFScanController *>(AMScanControllerSupervisor::scanControllerSupervisor()->currentScanController());

	if (current)
		current->finish();
}
