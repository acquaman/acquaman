#include "BioXASXASScanConfigurationRegionsEditor.h"

#include "util/AMEnergyToKSpaceCalculator.h"
#include "util/AMDateTimeUtils.h"

BioXASXASScanConfigurationRegionsEditor::BioXASXASScanConfigurationRegionsEditor(BioXASXASScanConfiguration *configuration, QWidget *parent) :
	BioXASXASScanConfigurationView(parent)
{
	// Create UI elements.

	regionsView_ = 0;

	QLabel *estimatedTimePrompt = new QLabel("Estimated time: ");
	estimatedTimeLabel_ = new QLabel();

	xanesButton_ = new QPushButton("Auto Set XANES Regions");
	exafsButton_ = new QPushButton("Auto Set EXAFS Regions");

	// Create and set layouts.

	regionsViewLayout_ = new QHBoxLayout();

	QHBoxLayout *estimatedTimeLayout = new QHBoxLayout();
	estimatedTimeLayout->addWidget(estimatedTimePrompt);
	estimatedTimeLayout->addWidget(estimatedTimeLabel_);

	QHBoxLayout *buttonsLayout = new QHBoxLayout();
	buttonsLayout->addWidget(xanesButton_);
	buttonsLayout->addWidget(exafsButton_);

	QHBoxLayout *extrasLayout = new QHBoxLayout();
	extrasLayout->addLayout(estimatedTimeLayout);
	extrasLayout->addStretch();
	extrasLayout->addLayout(buttonsLayout);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->addLayout(regionsViewLayout_);
	layout->addLayout(extrasLayout);

	setLayout(layout);

	// Make connections.

	connect( xanesButton_, SIGNAL(clicked()), this, SLOT(onXANESButtonClicked()) );
	connect( exafsButton_, SIGNAL(clicked()), this, SLOT(onEXAFSButtonClicked()) );

	// Current settings.

	setConfiguration(configuration);
}

BioXASXASScanConfigurationRegionsEditor::~BioXASXASScanConfigurationRegionsEditor()
{

}

void BioXASXASScanConfigurationRegionsEditor::setConfiguration(BioXASXASScanConfiguration *newConfiguration)
{
	if (configuration_ != newConfiguration) {

		if (configuration_) {
			disconnect( configuration_, 0, this, 0 );

			foreach (AMScanAxis *axis, configuration_->scanAxes())
				disconnectScanAxis(axis);
		}

		configuration_ = newConfiguration;

		if (configuration_) {
			connect( configuration_, SIGNAL(totalTimeChanged(double)), this, SLOT(updateEstimatedTimeLabel()) );
			connect( configuration_, SIGNAL(scanAxisAdded(AMScanAxis*)), this, SLOT(connectScanAxis(AMScanAxis*)) );
			connect( configuration_, SIGNAL(scanAxisRemoved(AMScanAxis*)), this, SLOT(disconnectScanAxis(AMScanAxis*)) );
			connect( configuration_->dbObject(), SIGNAL(energyChanged(double)), this, SLOT(updateRegionsView()) );

			// Want to listen for any changes to existing scan axes, as region changes mean we must refresh this view.

			foreach (AMScanAxis *axis, configuration_->scanAxes())
				connectScanAxis(axis);
		}

		refresh();

		emit configurationChanged(configuration_);
	}
}

void BioXASXASScanConfigurationRegionsEditor::clear()
{
	// Clear the regions view.

	if (regionsView_) {
		regionsViewLayout_->removeWidget(regionsView_);
		regionsView_->deleteLater();
		regionsView_ = 0;
	}

	// Clear the estimated time.

	estimatedTimeLabel_->clear();
}

void BioXASXASScanConfigurationRegionsEditor::update()
{
	updateRegionsView();
	updateEstimatedTimeLabel();
	updateXANESButton();
	updateEXAFSButton();
}

void BioXASXASScanConfigurationRegionsEditor::refresh()
{
	// Clear view.

	clear();

	// Construct new view using configuration information.

	if (configuration_) {
		regionsView_ = createRegionsView(configuration_);

		if (regionsView_)
			regionsViewLayout_->addWidget(regionsView_);
	}

	// Update view.

	update();
}

void BioXASXASScanConfigurationRegionsEditor::updateRegionsView()
{
	if (configuration_ && regionsView_) {
		regionsView_->setEdgeEnergy(configuration_->energy());
	}
}

void BioXASXASScanConfigurationRegionsEditor::updateEstimatedTimeLabel()
{
	double newTime = 0;

	if (configuration_)
		newTime = configuration_->totalTime();

	if (estimatedTimeLabel_)
		estimatedTimeLabel_->setText(AMDateTimeUtils::convertTimeToString(newTime));
}

void BioXASXASScanConfigurationRegionsEditor::updateXANESButton()
{
	bool enabled = false;
	bool visible = false;

	if (configuration_) {
		enabled = true;

		if (configuration_->scanAxisAt(0))
			visible = true;
	}

	if (xanesButton_) {
		xanesButton_->setEnabled(enabled);
		xanesButton_->setVisible(visible);
	}
}

void BioXASXASScanConfigurationRegionsEditor::updateEXAFSButton()
{
	bool enabled = false;
	bool visible = false;

	if (configuration_) {
		enabled = true;

		if (configuration_->scanAxisAt(0))
			visible = true;
	}

	if (exafsButton_) {
		exafsButton_->setEnabled(enabled);
		exafsButton_->setVisible(visible);
	}
}

void BioXASXASScanConfigurationRegionsEditor::onXANESButtonClicked()
{
	if (configuration_)
		configuration_->setupDefaultXANESRegions();
}

void BioXASXASScanConfigurationRegionsEditor::onEXAFSButtonClicked()
{
	if (configuration_)
		configuration_->setupDefaultEXAFSRegions();
}

void BioXASXASScanConfigurationRegionsEditor::connectScanAxis(AMScanAxis *newAxis)
{
	if (newAxis) {
		connect( newAxis, SIGNAL(regionAdded(AMScanAxisRegion*)), this, SLOT(refresh()) );
		connect( newAxis, SIGNAL(regionOverwritten(AMScanAxisRegion*)), this, SLOT(refresh()) );
		connect( newAxis, SIGNAL(regionRemoved(AMScanAxisRegion*)), this, SLOT(refresh()) );
		connect( newAxis, SIGNAL(regionsChanged()), this, SLOT(refresh()) );
	}
}

void BioXASXASScanConfigurationRegionsEditor::disconnectScanAxis(AMScanAxis *axis)
{
	if (axis) {
		disconnect( axis, 0, this, 0 );
	}
}

AMEXAFSScanAxisView* BioXASXASScanConfigurationRegionsEditor::createRegionsView(BioXASXASScanConfiguration *configuration)
{
	AMEXAFSScanAxisView *view = 0;

	if (configuration) {
		QList<AMScanAxis*> axes = configuration->scanAxes();

		if (!axes.isEmpty())
			view = new AMEXAFSScanAxisView("Regions", configuration);
	}

	return view;
}
