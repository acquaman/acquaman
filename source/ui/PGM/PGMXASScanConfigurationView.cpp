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

	scanName_ = new QLineEdit();
	scanName_->setText(configuration_->name());

	regionsView_ = new AMStepScanAxisView("VLS-PGM Region Configuration", configuration_);
	regionsView_->setStartLabel("Min");
	regionsView_->setEndLabel("Max");

	QGroupBox *regionsGroupBox = new QGroupBox("Regions");
	regionsGroupBox->setLayout(regionsView_->layout());

	// Setup export option.
	exportSpectraCheckBox_ = new QCheckBox("Export Spectra");
	exportSpectraCheckBox_->setChecked(configuration_->autoExportEnabled());

	// Setup primary (scientific) detector options.
	scientificDetectorsView_ = new AMGenericStepScanConfigurationDetectorsView(configuration_, PGMBeamline::pgm()->exposedScientificDetectors());

	QVBoxLayout *scientificDetectorsWidgetLayout = new QVBoxLayout();
	scientificDetectorsWidgetLayout->addWidget(scientificDetectorsView_);
	scientificDetectorsWidgetLayout->addStretch();

	QWidget *scientificDetectorsWidget = new QWidget();
	scientificDetectorsWidget->setLayout(scientificDetectorsWidgetLayout);

	// Setup options for all detectors.
	allDetectorsView_ = new AMGenericStepScanConfigurationDetectorsView(configuration_, PGMBeamline::pgm()->exposedDetectors());

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

	// Add layouts to main.
	QHBoxLayout *topHorizontalLayout = new QHBoxLayout;
	topHorizontalLayout->addWidget(regionsGroupBox);
	topHorizontalLayout->addWidget(detectorBox);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addStretch();
	mainLayout->addLayout(topHorizontalLayout);
	mainLayout->addWidget(scanName_);
	mainLayout->addStretch();

	setLayout(mainLayout);

	// Setup connections
	connect(scanName_, SIGNAL(editingFinished()), this, SLOT(onScanNameEdited()));
	connect(configuration_, SIGNAL(nameChanged(QString)), scanName_, SLOT(setText(QString)));
	connect( exportSpectraCheckBox_, SIGNAL(clicked(bool)), this, SLOT(updateConfigurationExportSpectraPreference()) );

}

void PGMXASScanConfigurationView::onScanNameEdited()
{
	configuration_->setName(scanName_->text());
	configuration_->setUserScanName(scanName_->text());
}

void PGMXASScanConfigurationView::onExportSelectionChanged(QAbstractButton *button)
{
	configuration_->setAutoExportEnabled(button->isChecked());
}
