#include "BioXASUtilitiesView.h"
#include "beamline/BioXAS/BioXASUtilities.h"
#include "beamline/BioXAS/BioXASShutters.h"
#include "beamline/BioXAS/BioXASValves.h"
#include "beamline/BioXAS/BioXASUtilitiesGroup.h"
#include "ui/BioXAS/BioXASControlEditor.h"

BioXASUtilitiesView::BioXASUtilitiesView(BioXASUtilities *utilities, QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables.

	utilities_ = 0;

	// Create UI elements.

	statusEditor_ = new BioXASControlEditor(0);
	statusEditor_->setTitle("Status");

	shuttersEditor_ = new BioXASControlEditor(0);
	shuttersEditor_->setTitle("Shutters");
	shuttersEditor_->setMinimumWidth(150);

	valvesEditor_ = new BioXASControlEditor(0);
	valvesEditor_->setTitle("Valves");
	valvesEditor_->setMinimumWidth(150);

	ionPumpsEditor_ = new BioXASControlEditor(0);
	ionPumpsEditor_->setTitle("Ion pumps");
	ionPumpsEditor_->setMinimumWidth(150);

	flowSwitchesEditor_ = new BioXASControlEditor(0);
	flowSwitchesEditor_->setTitle("Flow switches");
	flowSwitchesEditor_->setMinimumWidth(150);

	pressureMonitorsEditor_ = new BioXASControlEditor(0);
	pressureMonitorsEditor_->setTitle("Pressures");
	pressureMonitorsEditor_->setMinimumWidth(150);

	temperatureMonitorsEditor_ = new BioXASControlEditor(0);
	temperatureMonitorsEditor_->setTitle("Temperatures");
	temperatureMonitorsEditor_->setMinimumWidth(150);

	flowTransducersEditor_ = new BioXASControlEditor(0);
	flowTransducersEditor_->setTitle("Flow transducers");
	flowTransducersEditor_->setMinimumWidth(150);

	// Create and set layouts.

	QVBoxLayout *controlsLayout = new QVBoxLayout();
	controlsLayout->addWidget(shuttersEditor_);
	controlsLayout->addWidget(valvesEditor_);
	controlsLayout->addWidget(ionPumpsEditor_);
	controlsLayout->addWidget(flowSwitchesEditor_);
	controlsLayout->addWidget(pressureMonitorsEditor_);
	controlsLayout->addWidget(temperatureMonitorsEditor_);
	controlsLayout->addWidget(flowTransducersEditor_);

	QGroupBox *controlsBox = new QGroupBox();
	controlsBox->setFlat(true);
	controlsBox->setLayout(controlsLayout);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(statusEditor_);
	layout->addWidget(controlsBox);

	setLayout(layout);

	// Current settings.

	setUtilities(utilities);
	refresh();
}

BioXASUtilitiesView::~BioXASUtilitiesView()
{

}

void BioXASUtilitiesView::refresh()
{
	updateStatusEditor();
	updateShuttersEditor();
	updateValvesEditor();
	updateIonPumpsEditor();
	updateFlowSwitchesEditor();
	updatePressureMonitorsEditor();
	updateTemperatureMonitorsEditor();
	updateFlowTransducersEditor();
}

void BioXASUtilitiesView::setUtilities(BioXASUtilities *newUtilities)
{
	if (utilities_ != newUtilities) {

		if (utilities_)
			disconnect( utilities_, 0, this, 0 );

		utilities_ = newUtilities;

		if (utilities_)
			connect( utilities_, SIGNAL(connected(bool)), this, SLOT(refresh()) );

		refresh();

		emit utilitiesChanged(utilities_);
	}
}

void BioXASUtilitiesView::updateStatusEditor()
{
	statusEditor_->setControl(utilities_);
}

void BioXASUtilitiesView::updateShuttersEditor()
{
	AMControl *shutters = 0;

	if (utilities_)
		shutters = utilities_->shutters();

	shuttersEditor_->setControl(shutters);
}

void BioXASUtilitiesView::updateValvesEditor()
{
	AMControl *valves = 0;

	if (utilities_)
		valves = utilities_->valves();

	valvesEditor_->setControl(valves);
}

void BioXASUtilitiesView::updateIonPumpsEditor()
{
	AMControl *ionPumps = 0;

	if (utilities_)
		ionPumps = utilities_->ionPumps();

	ionPumpsEditor_->setControl(ionPumps);
}

void BioXASUtilitiesView::updateFlowSwitchesEditor()
{
	AMControl *flowSwitches = 0;

	if (utilities_)
		flowSwitches = utilities_->flowSwitches();

	flowSwitchesEditor_->setControl(flowSwitches);
}

void BioXASUtilitiesView::updatePressureMonitorsEditor()
{
	AMControl *pressureMonitors = 0;

	if (utilities_)
		pressureMonitors = utilities_->pressureMonitors();

	pressureMonitorsEditor_->setControl(pressureMonitors);
}

void BioXASUtilitiesView::updateTemperatureMonitorsEditor()
{
	AMControl *temperatureMonitors = 0;

	if (utilities_)
		temperatureMonitors = utilities_->temperatureMonitors();

	temperatureMonitorsEditor_->setControl(temperatureMonitors);
}

void BioXASUtilitiesView::updateFlowTransducersEditor()
{
	AMControl *flowTransducers = 0;

	if (utilities_)
		flowTransducers = utilities_->flowTransducers();

	flowTransducersEditor_->setControl(flowTransducers);
}

