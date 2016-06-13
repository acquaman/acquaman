#include "BioXASGenericStepScanConfigurationView.h"

BioXASGenericStepScanConfigurationView::BioXASGenericStepScanConfigurationView(AMGenericStepScanConfiguration *configuration, AMControlSet *controls, AMDetectorSet *detectors, QWidget *parent) :
	AMScanConfigurationView(parent)
{
	// Initialize class variables.

	configuration_ = 0;
	controls_ = 0;
	detectors_ = 0;

	// Create UI elements.

	nameLineEdit_ = new QLineEdit();
	connect( nameLineEdit_, SIGNAL(textChanged(QString)), this, SLOT(onNameLineEditTextChanged()) );

	absoluteAxisView_ = new AMGenericStepScanAxisAbsoluteView(0, 0);

	QVBoxLayout *absoluteAxisWidgetLayout = new QVBoxLayout();
	absoluteAxisWidgetLayout->setMargin(0);
	absoluteAxisWidgetLayout->addWidget(absoluteAxisView_);
	absoluteAxisWidgetLayout->addStretch();

	QWidget *absoluteAxisWidget = new QWidget;
	absoluteAxisWidget->setLayout(absoluteAxisWidgetLayout);

	QTabWidget *positionTabWidget = new QTabWidget();
	positionTabWidget->addTab(absoluteAxisWidget, "Absolute positions");
	positionTabWidget->addTab(new QWidget(), "Relative positions");

	detectorsView_ = new AMGenericStepScanConfigurationDetectorsView(0, 0);

	QVBoxLayout *detectorsBoxLayout = new QVBoxLayout();
	detectorsBoxLayout->setMargin(0);
	detectorsBoxLayout->addWidget(detectorsView_);
	detectorsBoxLayout->addStretch();

	QGroupBox *detectorsBox = new QGroupBox("Detectors");
	detectorsBox->setLayout(detectorsBoxLayout);

	// Create and set main layouts.

	QVBoxLayout *leftLayout = new QVBoxLayout();
	leftLayout->addWidget(nameLineEdit_);
	leftLayout->addWidget(positionTabWidget);

	QVBoxLayout *rightLayout = new QVBoxLayout();
	rightLayout->addWidget(detectorsBox);

	QHBoxLayout *layout = new QHBoxLayout();
	layout->setMargin(0);
	layout->addLayout(leftLayout);
	layout->addLayout(rightLayout);

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

	updateAbsoluteScanAxisView();
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

void BioXASGenericStepScanConfigurationView::updateAbsoluteScanAxisView()
{
	absoluteAxisView_->setConfiguration(configuration_);
	absoluteAxisView_->setControls(controls_);
}

void BioXASGenericStepScanConfigurationView::updateDetectorsView()
{
	detectorsView_->setDetectors(detectors_);
}

