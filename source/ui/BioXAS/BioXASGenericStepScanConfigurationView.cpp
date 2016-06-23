#include "BioXASGenericStepScanConfigurationView.h"
#include "util/AMDateTimeUtils.h"

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

	// Create axes editor.

	axesView_ = new BioXASGenericStepScanConfigurationAxesView(0);

	QVBoxLayout *axesBoxLayout = new QVBoxLayout();
	axesBoxLayout->addWidget(axesView_);
	axesBoxLayout->addStretch();

	QGroupBox *axesBox = new QGroupBox("Positions");
	axesBox->setLayout(axesBoxLayout);

	// Create meta-data view.

	estimatedTimeLabel_ = new QLabel();

	dimensionsLabel_ = new QLabel();

	pointsCountLabel_ = new QLabel();

	QGridLayout *metaDataBoxLayout = new QGridLayout();
	metaDataBoxLayout->addWidget(new QLabel("Estimated time: "), 0, 0, 1, 1, Qt::AlignRight);
	metaDataBoxLayout->addWidget(estimatedTimeLabel_, 0, 1);
	metaDataBoxLayout->addWidget(new QLabel("Dimensions: "), 1, 0, 1, 1, Qt::AlignRight);
	metaDataBoxLayout->addWidget(dimensionsLabel_, 1, 1);
	metaDataBoxLayout->addWidget(new QLabel("Points: "), 2, 0, 1, 1, Qt::AlignRight);
	metaDataBoxLayout->addWidget(pointsCountLabel_, 2, 1);

	QGroupBox *metaDataBox = new QGroupBox("Meta-data");
	metaDataBox->setLayout(metaDataBoxLayout);

	// Create main scan editor.

	QVBoxLayout *scanBoxLayout = new QVBoxLayout();
	scanBoxLayout->addWidget(nameLineEdit_);
	scanBoxLayout->addWidget(axesBox);
	scanBoxLayout->addWidget(metaDataBox);

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
			connect( configuration_, SIGNAL(expectedDurationChanged(double)), this, SLOT(updateEstimatedTimeLabel()) );
			connect( configuration_, SIGNAL(configurationChanged()), this, SLOT(updateDimensionsLabel()) );
			connect( configuration_, SIGNAL(configurationChanged()), this, SLOT(updatePointsCountLabel()) );
		}

		emit configurationChanged(configuration_);
	}

	updateNameLineEdit();
	updateEstimatedTimeLabel();
	updateDimensionsLabel();
	updatePointsCountLabel();
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

void BioXASGenericStepScanConfigurationView::updateEstimatedTimeLabel()
{
	QString newText = "";

	if (configuration_)
		newText = AMDateTimeUtils::convertTimeToString(configuration_->totalTime());

	estimatedTimeLabel_->setText(newText);
}

void BioXASGenericStepScanConfigurationView::updateDimensionsLabel()
{
	QString newText = "";

	if (configuration_) {

		if (configuration_->scanAxes().size() == 1) {

			double size = fabs(double(configuration_->scanAxisAt(0)->regionAt(0)->regionEnd()) - double(configuration_->scanAxisAt(0)->regionAt(0)->regionStart()));
			newText = QString("%1").arg(QString::number(size, 'f', 1));

		} else if (configuration_->scanAxes().size() == 2) {

			double hSize = fabs(double(configuration_->scanAxisAt(0)->regionAt(0)->regionEnd()) - double(configuration_->scanAxisAt(0)->regionAt(0)->regionStart()));
			double vSize = fabs(double(configuration_->scanAxisAt(1)->regionAt(0)->regionEnd()) - double(configuration_->scanAxisAt(1)->regionAt(0)->regionStart()));

			newText = QString("%1 x %2").arg(QString::number(hSize, 'f', 1)).arg(QString::number(vSize, 'f', 1));
		}
	}

	dimensionsLabel_->setText(newText);
}

void BioXASGenericStepScanConfigurationView::updatePointsCountLabel()
{
	QString newText = "";

	if (configuration_) {

		if (configuration_->scanAxes().size() == 1)
			newText = QString("%1").arg(configuration_->scanAxisAt(0)->numberOfPoints());

		else if (configuration_->scanAxes().size() == 2)
			newText = QString("%1").arg(configuration_->scanAxisAt(0)->numberOfPoints() * configuration_->scanAxisAt(1)->numberOfPoints());
	}

	pointsCountLabel_->setText(newText);
}

void BioXASGenericStepScanConfigurationView::updateDetectorsView()
{
	detectorsView_->setDetectors(detectors_);
}

