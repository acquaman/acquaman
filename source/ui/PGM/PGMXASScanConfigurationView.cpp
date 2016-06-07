#include "PGMXASScanConfigurationView.h"

#include "util/AMDateTimeUtils.h"
#include <QVBoxLayout>
#include <QGroupBox>

PGMXASScanConfigurationView::PGMXASScanConfigurationView(PGMXASScanConfiguration *configuration, AMDetectorSet *detectors, QWidget *parent)
	: AMScanConfigurationView(parent)
{
	configuration_ = configuration;

	scanName_ = new QLineEdit();
	scanName_->setText(configuration_->name());

	connect(scanName_, SIGNAL(editingFinished()), this, SLOT(onScanNameEdited()));
	connect(configuration_, SIGNAL(nameChanged(QString)), scanName_, SLOT(setText(QString)));

	regionsView_ = new AMStepScanAxisView("VLS-PGM Region Configuration", configuration_);

        QGroupBox *regionsGroupBox = new QGroupBox("Regions");
        regionsGroupBox->setLayout(regionsView_->layout());

        // Detector selection.
        QGroupBox *detectorGroupBox = new QGroupBox("Detectors");
        detectorGroup_ = new QButtonGroup;
        detectorGroup_->setExclusive(false);
        detectorLayout_ = new QVBoxLayout;

        connect(detectorGroup_, SIGNAL(buttonClick(QAbstractButton*)), this, SLOT(onDetectorSelectionChanged(QAbstractButton*)));

        detectorGroupBox->setLayout(detectorLayout_);

        setDetectors(detectors);

        // Export option
        exportSpectraCheckBox_ = new QCheckBox("Export Spectra");
        connect( exportSpectraCheckBox_, SIGNAL(clicked(bool)), this, SLOT(updateConfigurationExportSpectraPreference()) );


        QVBoxLayout *scanRegionsVerticalLayout = new QVBoxLayout;
        scanRegionsVerticalLayout->addWidget( regionsGroupBox );
        scanRegionsVerticalLayout->addWidget(scanName_);

        QVBoxLayout *sideVerticalLayout = new QVBoxLayout;
        sideVerticalLayout->addWidget(detectorGroupBox);
        sideVerticalLayout->setAlignment(Qt::AlignTop);
        sideVerticalLayout->addWidget(exportSpectraCheckBox_);

        QHBoxLayout *mainLayout = new QHBoxLayout;
        mainLayout->addLayout(scanRegionsVerticalLayout);
        mainLayout->addLayout(sideVerticalLayout);




        setLayout(mainLayout);
}

void PGMXASScanConfigurationView::setDetectors(AMDetectorSet *newDetectors)
{
    if (detectors_ != newDetectors) {

            // Clear previously displayed detectors.

            for (int buttonIndex = 0, buttonCount = detectorGroup_->buttons().count(); buttonIndex < buttonCount; buttonIndex++) {
                    QAbstractButton *button = detectorGroup_->button(buttonIndex);
                    detectorLayout_->removeWidget(button);
                    detectorGroup_->removeButton(button);
                    button->deleteLater();
            }

            detectorButtonMap_.clear();

            // Set new detectors.

            detectors_ = newDetectors;

            // Add new detectors.

            for (int detectorIndex = 0, detectorCount = detectors_->count(); detectorIndex < detectorCount; detectorIndex++) {
                    AMDetector *detector = detectors_->at(detectorIndex);

                    if (detector) {
                            QCheckBox *checkBox = new QCheckBox(detector->name());
                            detectorGroup_->addButton(checkBox);
                            detectorLayout_->addWidget(checkBox);

                            detectorButtonMap_.insert(detector, checkBox);

                            // Update current selection.

                            if (configuration_ && configuration_->detectorConfigurations().contains(detector->name())) {
                                    checkBox->blockSignals(true);
                                    checkBox->setChecked(true);
                                    checkBox->blockSignals(false);
                            }
                    }
            }
    }
}

void PGMXASScanConfigurationView::onScanNameEdited()
{
	configuration_->setName(scanName_->text());
        configuration_->setUserScanName(scanName_->text());
}

void PGMXASScanConfigurationView::onDetectorSelectionChanged(QAbstractButton *button)
{

}

void PGMXASScanConfigurationView::onExportSelectionChanged(QAbstractButton *button)
{

}
