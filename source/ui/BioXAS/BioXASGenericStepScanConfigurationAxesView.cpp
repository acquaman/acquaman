#include "BioXASGenericStepScanConfigurationAxesView.h"

BioXASGenericStepScanConfigurationAxesView::BioXASGenericStepScanConfigurationAxesView(AMGenericStepScanConfiguration *configuration, BioXASScanAxisRegionView::ViewMode viewMode, AMControlSet *controls, QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables.

	viewMode_ = BioXASScanAxisRegionView::Absolute;
	configuration_ = 0;
	controls_ = 0;

	// Create axis view layout.

	axisViewsLayout_ = new QVBoxLayout();

	// Create buttons.

	absoluteButton_ = new QRadioButton("Absolute");
	relativeButton_ = new QRadioButton("Relative");

	QButtonGroup *buttonGroup = new QButtonGroup(this);
	buttonGroup->addButton(absoluteButton_);
	buttonGroup->addButton(relativeButton_);

	connect( buttonGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(onViewModeButtonsClicked()) );

	QHBoxLayout *buttonsLayout = new QHBoxLayout();
	buttonsLayout->addStretch();
	buttonsLayout->addWidget(absoluteButton_);
	buttonsLayout->addWidget(relativeButton_);

	// Create and set main layout.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->addLayout(axisViewsLayout_);
	layout->addLayout(buttonsLayout);

	setLayout(layout);

	// Current settings.

	setViewMode(viewMode);
	setConfiguration(configuration);
	setControls(controls);
}

BioXASGenericStepScanConfigurationAxesView::~BioXASGenericStepScanConfigurationAxesView()
{

}

void BioXASGenericStepScanConfigurationAxesView::setViewMode(BioXASScanAxisRegionView::ViewMode newMode)
{
	if (viewMode_ != newMode) {
		viewMode_ = newMode;
		emit viewModeChanged(viewMode_);
	}

	updateAxisViews();
	updateAbsoluteButton();
	updateRelativeButton();
}

void BioXASGenericStepScanConfigurationAxesView::setConfiguration(AMGenericStepScanConfiguration *newConfiguration)
{
	if (configuration_ != newConfiguration) {

		if (configuration_)
			disconnect( configuration_, 0, this, 0 );

		configuration_ = newConfiguration;

		if (configuration_) {
			connect( configuration_, SIGNAL(scanAxisAdded(AMScanAxis*)), this, SLOT(updateAxisViews()) );
			connect( configuration_, SIGNAL(scanAxisRemoved(AMScanAxis*)), this, SLOT(updateAxisViews()) );
			connect( configuration_, SIGNAL(axisControlInfosChanged()), this, SLOT(updateAxisViews()) );
			connect( configuration_, SIGNAL(axisControlInfoAdded()), this, SLOT(updateAxisViews()) );
			connect( configuration_, SIGNAL(axisControlInfoRemoved()), this, SLOT(updateAxisViews()) );
			connect( configuration_, SIGNAL(axisControlInfoChanged()), this, SLOT(updateAxisViews()) );
		}

		emit configurationChanged(configuration_);
	}

	updateAxisViews();
}

void BioXASGenericStepScanConfigurationAxesView::setControls(AMControlSet *newControls)
{
	if (controls_ != newControls) {
		controls_ = newControls;
		emit controlsChanged(controls_);
	}

	updateAxisViews();
}

void BioXASGenericStepScanConfigurationAxesView::updateAxisViews()
{
	// Initially clear all axis views.

	foreach (QWidget *axisView, axisViews_) {

		axisViews_.removeOne(axisView);
		axisViewsLayout_->removeWidget(axisView);

		if (axisView) {
			axisView->disconnect();
			axisView->deleteLater();
		}
	}

	if (configuration_) {

		// Repopulate the axis views.

		int axisCount = configuration_->scanAxes().count();

		if (axisCount > 0) {

			// Create first axis view.

			QWidget *axisView = createAxisView(configuration_, 0, viewMode_, controls_);

			// Add axis view to the appropriate list/layout.

			axisViews_ << axisView;
			axisViewsLayout_->addWidget(axisView);

			// Continue iterating through the configuration's axes.
			// The enabled state of each subsequent axis view is a function
			// of the axis control of the preceeding axis.

			for (int i = 1; i < axisCount; i++) {

				if (configuration_->axisControlInfoAt(i-1).isValid()) {
					axisView = createAxisView(configuration_, i, viewMode_, controls_);
					axisView->setEnabled(true);
					axisViews_ << axisView;
					axisViewsLayout_->addWidget(axisView);

				} else {
					axisView = createAxisView(configuration_, i, viewMode_, 0);
					axisView->setEnabled(false);
					axisViews_ << axisView;
					axisViewsLayout_->addWidget(axisView);
				}
			}
		}
	}
}

void BioXASGenericStepScanConfigurationAxesView::updateAbsoluteButton()
{
	absoluteButton_->blockSignals(true);
	absoluteButton_->setChecked(viewMode_ == BioXASScanAxisRegionView::Absolute);
	absoluteButton_->blockSignals(false);
}

void BioXASGenericStepScanConfigurationAxesView::updateRelativeButton()
{
	relativeButton_->blockSignals(true);
	relativeButton_->setChecked(viewMode_ == BioXASScanAxisRegionView::Relative);
	relativeButton_->blockSignals(false);
}

void BioXASGenericStepScanConfigurationAxesView::onViewModeButtonsClicked()
{
	setViewMode( absoluteButton_->isChecked() ? BioXASScanAxisRegionView::Absolute : BioXASScanAxisRegionView::Relative );
}

QWidget* BioXASGenericStepScanConfigurationAxesView::createAxisView(AMGenericStepScanConfiguration *configuration, int axisNumber, BioXASScanAxisRegionView::ViewMode viewMode, AMControlSet *controls)
{
	QWidget *axisView = new BioXASGenericStepScanConfigurationAxisView(configuration, axisNumber, viewMode, controls);

	QVBoxLayout *axisBoxLayout = new QVBoxLayout();
	axisBoxLayout->setMargin(0);
	axisBoxLayout->addWidget(axisView);

	QGroupBox *axisBox = new QGroupBox(QString("Axis %1").arg(axisNumber+1));
	axisBox->setLayout(axisBoxLayout);
	axisBox->setFlat(true);

	return axisBox;
}
