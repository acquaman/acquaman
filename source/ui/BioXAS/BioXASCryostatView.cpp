#include "BioXASCryostatView.h"
#include "beamline/BioXAS/BioXASCryostat.h"
#include "ui/CLS/CLSControlEditor.h"

BioXASCryostatView::BioXASCryostatView(BioXASCryostat *cryostat, QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables.

	cryostat_ = 0;

	// Create UI elements.

	pressureEditor_ = new CLSControlEditor(0);
	pressureEditor_->setTitle("Pressure");

	temperatureEditor_ = new CLSControlEditor(0);
	temperatureEditor_->setTitle("Temperature");

	// Create and set layout.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->addWidget(pressureEditor_);
	layout->addWidget(temperatureEditor_);

	setLayout(layout);

	// Current settings.

	setControl(cryostat);
}

BioXASCryostatView::~BioXASCryostatView()
{

}

void BioXASCryostatView::refresh()
{
	updatePressureEditor();
	updateTemperatureEditor();
}

void BioXASCryostatView::setControl(BioXASCryostat *newControl)
{
	if (cryostat_ != newControl) {

		if (cryostat_)
			disconnect( cryostat_, 0, this, 0 );

		cryostat_ = newControl;

		if (cryostat_) {
			connect( cryostat_, SIGNAL(pressureChanged(AMControl*)), this, SLOT(updatePressureEditor()) );
			connect( cryostat_, SIGNAL(temperatureChanged(AMControl*)), this, SLOT(updateTemperatureEditor()) );
		}

		refresh();

		emit controlChanged(cryostat_);
	}
}

void BioXASCryostatView::updatePressureEditor()
{
	if (cryostat_)
		pressureEditor_->setControl(cryostat_->pressure());
	else
		pressureEditor_->setControl(0);
}

void BioXASCryostatView::updateTemperatureEditor()
{
	if (cryostat_)
		temperatureEditor_->setControl(cryostat_->temperature());
	else
		temperatureEditor_->setControl(0);
}
