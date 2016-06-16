#include "BioXASGenericStepScanConfigurationView.h"

BioXASGenericStepScanConfigurationView::BioXASGenericStepScanConfigurationView(AMGenericStepScanConfiguration *configuration, AMControlSet *controls, AMDetectorSet *detectors, QWidget *parent) :
	AMScanConfigurationView(parent)
{
	// Initialize class variables.

	configuration_ = 0;
	controls_ = 0;
	detectors_ = 0;

	// Create name editor.

	nameLineEdit_ = new QLineEdit();
	connect( nameLineEdit_, SIGNAL(textChanged(QString)), this, SLOT(onNameLineEditTextChanged()) );

	QVBoxLayout *nameBoxLayout = new QVBoxLayout();
	nameBoxLayout->addWidget(nameLineEdit_);

	QGroupBox *nameBox = new QGroupBox("Name");
	nameBox->setLayout(nameBoxLayout);

	// Create axes editor.

	axesView_ = new BioXASGenericStepScanConfigurationAxesView(0);

	QVBoxLayout *axesBoxLayout = new QVBoxLayout();
	axesBoxLayout->addWidget(axesView_);
	axesBoxLayout->addStretch();

	QGroupBox *axesBox = new QGroupBox("Positions");
	axesBox->setLayout(axesBoxLayout);

	// Create main scan editor.

	QVBoxLayout *scanBoxLayout = new QVBoxLayout();
	scanBoxLayout->addWidget(nameBox);
	scanBoxLayout->addWidget(axesBox);

	QGroupBox *scanBox = new QGroupBox("Scan");
	scanBox->setLayout(scanBoxLayout);

	// Create main detectors editor.

	detectorsView_ = new AMGenericStepScanConfigurationDetectorsView(0, 0);

	QVBoxLayout *detectorsBoxLayout = new QVBoxLayout();
	detectorsBoxLayout->addWidget(detectorsView_);
	detectorsBoxLayout->addStretch();

	QGroupBox *detectorsBox = new QGroupBox("Detectors");
	detectorsBox->setLayout(detectorsBoxLayout);
	detectorsBox->setMinimumWidth(BIOXASGENERICSTEPSCANCONFIGURATIONVIEW_DETECTOR_VIEW_MIN_WIDTH);

	// Create and set main layouts.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->setMargin(0);
	layout->addWidget(scanBox);
	layout->addWidget(detectorsBox);

	setLayout(layout);

	// Current settings.

	setConfiguration(configuration);
	setControls(controls);
	setDetectors(detectors);
}

BioXASGenericStepScanConfigurationView::~BioXASGenericStepScanConfigurationView()
{

}

void BioXASGenericStepScanConfigurationView::setConfiguration(AMGenericStepScanConfiguration *newConfiguration)
{
	if (configuration_ != newConfiguration) {

		if (configuration_)
			disconnect( configuration_, 0, this, 0 );

		configuration_ = newConfiguration;

		if (configuration_) {
			connect( configuration_, SIGNAL(nameChanged(QString)), this, SLOT(updateNameLineEdit()) );
		}

		emit configurationChanged(configuration_);
	}

	updateNameLineEdit();
}

void BioXASGenericStepScanConfigurationView::setControls(AMControlSet *newControls)
{
	if (controls_ != newControls) {
		controls_ = newControls;
		emit controlsChanged(controls_);
	}

	updateAxesView();
}

void BioXASGenericStepScanConfigurationView::setDetectors(AMDetectorSet *newDetectors)
{
	if (detectors_ != newDetectors) {
		detectors_ = newDetectors;
		emit detectorsChanged(detectors_);
	}

	updateDetectorsView();
}

void BioXASGenericStepScanConfigurationView::updateNameLineEdit()
{
	nameLineEdit_->clear();
	nameLineEdit_->setEnabled(false);

	if (configuration_) {
		nameLineEdit_->setEnabled(true);
		nameLineEdit_->setAlignment(Qt::AlignCenter);
		nameLineEdit_->setText(configuration_->name());
	}
}

void BioXASGenericStepScanConfigurationView::onNameLineEditTextChanged()
{
	if (configuration_) {
		configuration_->setName(nameLineEdit_->text());
		configuration_->setUserScanName(nameLineEdit_->text());
	}
}

void BioXASGenericStepScanConfigurationView::updateAxesView()
{
	axesView_->setConfiguration(configuration_);
	axesView_->setControls(controls_);
}

void BioXASGenericStepScanConfigurationView::updateDetectorsView()
{
	detectorsView_->setDetectors(detectors_);
}

