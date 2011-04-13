#include "VESPERSXRFScanConfigurationView.h"
#include "beamline/VESPERS/VESPERSBeamline.h"
#include "ui/AMPeriodicTableView.h"
#include "acquaman/AMScanController.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolButton>

VESPERSXRFScanConfigurationView::VESPERSXRFScanConfigurationView(VESPERSXRFScanConfiguration *scanConfig, QWidget *parent)
	: AMScanConfigurationView(parent)
{
	configuration_ = scanConfig;
	view_ = new XRFDetailedDetectorView(scanConfig->detector());
	detector_ = scanConfig->detector();

	AMPeriodicTableView *table = new AMPeriodicTableView;
	table->setMaximumWidth(600);

	QToolButton *start = new QToolButton;
	start->setIcon(QIcon(":/play_button_green.png"));
	connect(start, SIGNAL(clicked()), this, SIGNAL(onStartClicked()));

	QToolButton *stop = new QToolButton;
	stop->setIcon(QIcon(":/red-stop-button.png"));
	connect(stop, SIGNAL(clicked()), this, SIGNAL(stopScan()));

	integrationTime_ = new QDoubleSpinBox;
	integrationTime_->setSuffix(" s");
	integrationTime_->setSingleStep(0.1);
	integrationTime_->setMaximum(1000.00);
	connect(integrationTime_, SIGNAL(editingFinished()), this, SLOT(onIntegrationTimeUpdate()));

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

	QVBoxLayout *configViewLayout = new QVBoxLayout;
	configViewLayout->addLayout(plotControlLayout);
	configViewLayout->addWidget(table);

	setLayout(configViewLayout);
}

void VESPERSXRFScanConfigurationView::onIntegrationTimeUpdate()
{
	detector_->setTime(integrationTime_->value());
}
