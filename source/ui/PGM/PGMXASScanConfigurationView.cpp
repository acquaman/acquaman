#include "PGMXASScanConfigurationView.h"

#include "util/AMDateTimeUtils.h"
#include "beamline/PGM/PGMBeamline.h"
#include <QVBoxLayout>
#include <QGroupBox>
#include <QTabWidget>

PGMXASScanConfigurationView::PGMXASScanConfigurationView(PGMXASScanConfiguration *configuration, QWidget *parent)
	: AMScanConfigurationView(parent)
{
	configuration_ = configuration;

	// Scan Name

	QLabel *scanNameLabel = new QLabel("Scan Name: ");

	scanName_ = new QLineEdit();
	scanName_->setText(configuration_->name());
	scanName_->setAlignment(Qt::AlignCenter);

	QHBoxLayout *scanNameLayout = new QHBoxLayout;
	scanNameLayout->addWidget(scanNameLabel);
	scanNameLayout->addWidget(scanName_);

	// Regions view configuration

	regionsView_ = new AMStepScanAxisView("VLS-PGM Region Configuration", configuration_, 0, "Min", "Max");

	QVBoxLayout *regionsGroupLayout = new QVBoxLayout;
	regionsGroupLayout->addWidget(regionsView_);

	QGroupBox *regionsGroupBox = new QGroupBox("Regions");
	regionsGroupBox->setLayout(regionsGroupLayout);

	// Estimated scan time to display.

	QLabel *timeLabel = new QLabel("Estimated time per scan: ");
	estimatedTime_ = new QLabel;

	QHBoxLayout *timeLayout = new QHBoxLayout;
	timeLayout->addWidget(timeLabel);
	timeLayout->addWidget(estimatedTime_);

	QGroupBox *timeBox = new QGroupBox("Time");
	timeBox->setLayout(timeLayout);

	// Setup export option.
	exportSpectraCheckBox_ = new QCheckBox("Export Spectra");
	exportSpectraCheckBox_->setChecked(configuration_->autoExportEnabled());

	// Setup primary (scientific) detector options.
	scientificDetectorsView_ = new AMGenericStepScanConfigurationDetectorsView(0, PGMBeamline::pgm()->exposedScientificDetectors());

	QVBoxLayout *scientificDetectorsWidgetLayout = new QVBoxLayout();
	scientificDetectorsWidgetLayout->addWidget(scientificDetectorsView_);
	scientificDetectorsWidgetLayout->addStretch();

	QWidget *scientificDetectorsWidget = new QWidget();
	scientificDetectorsWidget->setLayout(scientificDetectorsWidgetLayout);

	// Setup options for all detectors.
	allDetectorsView_ = new AMGenericStepScanConfigurationDetectorsView(0, PGMBeamline::pgm()->exposedDetectors());

	QVBoxLayout *allDetectorsWidgetLayout = new QVBoxLayout();
	allDetectorsWidgetLayout->addWidget(allDetectorsView_);
	allDetectorsWidgetLayout->addStretch();

	QWidget *allDetectorsWidget = new QWidget();
	allDetectorsWidget->setLayout(allDetectorsWidgetLayout);

	// Setup the tabbed detectors view and add detectors to tabs.
	QTabWidget *detectorsView = new QTabWidget();
	detectorsView->addTab(scientificDetectorsWidget, "Scientific");
	detectorsView->addTab(allDetectorsWidget, "All");

	// Combine detectors and export widgets in single view.
	QVBoxLayout *sideVerticalLayout = new QVBoxLayout;
	sideVerticalLayout->addWidget(detectorsView);
	sideVerticalLayout->setAlignment(Qt::AlignTop);
	sideVerticalLayout->addWidget(exportSpectraCheckBox_);
	sideVerticalLayout->addStretch();

	QGroupBox *detectorBox = new QGroupBox("Detectors");
	detectorBox->setLayout(sideVerticalLayout);

	// Combine regions, scan name and time estimate.
	QVBoxLayout *regionsLayout = new QVBoxLayout;
	regionsLayout->addWidget(regionsGroupBox);
	regionsLayout->addStretch();
	regionsLayout->addLayout(scanNameLayout);
	regionsLayout->addWidget(timeBox);

	// Add layouts to main.
	QHBoxLayout *topHorizontalLayout = new QHBoxLayout;
	topHorizontalLayout->addLayout(regionsLayout);
	topHorizontalLayout->addWidget(detectorBox);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addStretch();
	mainLayout->addLayout(topHorizontalLayout);
	mainLayout->addStretch();

	QHBoxLayout *centeredLayout = new QHBoxLayout;
	centeredLayout->addStretch();
	centeredLayout->addLayout(mainLayout);
	centeredLayout->addStretch();

	setLayout(centeredLayout);

	// Setup connections
	connect(scanName_, SIGNAL(editingFinished()), this, SLOT(onScanNameEdited()));
	connect(configuration_, SIGNAL(nameChanged(QString)), scanName_, SLOT(setText(QString)));
	connect(exportSpectraCheckBox_, SIGNAL(clicked(bool)), this, SLOT(onExportSelectionChanged()) );
	connect(configuration_, SIGNAL(totalTimeChanged(double)), this, SLOT(onEstimatedTimeChanged(double)));
	onEstimatedTimeChanged(configuration_->totalTime());

}

void PGMXASScanConfigurationView::onScanNameEdited()
{
	configuration_->setName(scanName_->text());
	configuration_->setUserScanName(scanName_->text());
}

void PGMXASScanConfigurationView::onEstimatedTimeChanged(double newTime)
{
	estimatedTime_->setText("Estimated time per scan:\t" + AMDateTimeUtils::convertTimeToString(newTime));
}

void PGMXASScanConfigurationView::onExportSelectionChanged()
{
	configuration_->setAutoExportEnabled(exportSpectraCheckBox_->isChecked());
}
