#include "BioXASGenericStepScanConfigurationAxesView.h"

BioXASGenericStepScanConfigurationAxesView::BioXASGenericStepScanConfigurationAxesView(AMGenericStepScanConfiguration *configuration, BioXASScanAxisRegionView::ViewMode viewMode, AMControlSet *controls, QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables.

	viewMode_ = BioXASScanAxisRegionView::Absolute;
	configuration_ = 0;
	controls_ = 0;

	// Create axis views.

	axisView0_ = new BioXASGenericStepScanConfigurationAxisView(0, 0, viewMode_, 0);

	QVBoxLayout *axisBox0Layout = new QVBoxLayout();
	axisBox0Layout->setMargin(0);
	axisBox0Layout->addWidget(axisView0_);

	QGroupBox *axisBox0 = new QGroupBox(QString("Axis 1"));
	axisBox0->setLayout(axisBox0Layout);
	axisBox0->setFlat(true);

	axisView1_ = new BioXASGenericStepScanConfigurationAxisView(0, 1, viewMode_, 0);

	QVBoxLayout *axisBox1Layout = new QVBoxLayout();
	axisBox1Layout->setMargin(0);
	axisBox1Layout->addWidget(axisView1_);

	QGroupBox *axisBox1 = new QGroupBox(QString("Axis 2"));
	axisBox1->setLayout(axisBox1Layout);
	axisBox1->setFlat(true);

	QVBoxLayout *axesViewLayout = new QVBoxLayout();
	axesViewLayout->addWidget(axisBox0);
	axesViewLayout->addWidget(axisBox1);

	QWidget *axesView = new QWidget();
	axesView->setLayout(axesViewLayout);

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
	layout->addWidget(axesView);
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

	// Update the first axis view.

	axisView0_->setViewMode(viewMode_);
	axisView0_->setConfiguration(configuration_);
	axisView0_->setControls(controls_);

	// Update the second axis view.
	// Note that the enabled state of the second axis view
	// depends on the axis control of the preceeding axis.

	axisView1_->setViewMode(viewMode_);
	axisView1_->setConfiguration(configuration_);
	axisView1_->setControls(controls_);
	axisView1_->setEnabled( configuration_ && configuration_->axisControlInfos().count() >= 1 && configuration_->axisControlInfoAt(0).isValid() );
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
