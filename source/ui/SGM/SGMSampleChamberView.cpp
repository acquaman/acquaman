#include "SGMSampleChamberView.h"
#include "beamline/AMControlSet.h"
#include "beamline/SGM/SGMVATValve.h"
#include "beamline/SGM/SGMVATValveState.h"
#include "beamline/SGM/SGMTurboPump.h"
#include "beamline/SGM/SGMRoughingPump.h"
#include "beamline/SGM/SGMRoughingPumpRunningStatus.h"
#include "beamline/SGM/SGMSampleChamber.h"
#include "beamline/SGM/SGMSampleChamberVacuum.h"
#include "ui/beamline/AMExtendedControlEditor.h"

SGMSampleChamberView::SGMSampleChamberView(SGMSampleChamber *sampleChamber, QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables.

	sampleChamber_ = 0;

	// Create UI elements.

	vacuumEditor_ = new AMExtendedControlEditor(0);
	vacuumEditor_->setTitle("Vacuum status");
	vacuumEditor_->setNoUnitsBox(true);

	doorEditor_ = new AMExtendedControlEditor(0);
	doorEditor_->setTitle("Door status");
	doorEditor_->setNoUnitsBox(true);

	pressureEditor_ = new AMExtendedControlEditor(0);
	pressureEditor_->setTitle("Pressure");
	pressureEditor_->setUnits("Torr");

	ionGaugeEditor_ = new AMExtendedControlEditor(0);
	ionGaugeEditor_->setTitle("Ion gauge");
	ionGaugeEditor_->setNoUnitsBox(true);

	leakValveEditor_ = new AMExtendedControlEditor(0);
	leakValveEditor_->setTitle("Leak valve");
	leakValveEditor_->setNoUnitsBox(true);

	QGroupBox *turbosView = new QGroupBox();
	turbosView->setTitle("Turbos");

	roughingPumpEditor_ = new AMExtendedControlEditor(0);
	roughingPumpEditor_->setTitle("Roughing pump");
	roughingPumpEditor_->setNoUnitsBox(true);

	// Create and set layouts.

	turbosViewLayout_ = new QHBoxLayout();
	turbosViewLayout_->setMargin(0);

	turbosView->setLayout(turbosViewLayout_);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(vacuumEditor_);
	layout->addWidget(doorEditor_);
	layout->addWidget(pressureEditor_);
	layout->addWidget(ionGaugeEditor_);
	layout->addWidget(leakValveEditor_);
	layout->addWidget(roughingPumpEditor_);
	layout->addWidget(turbosView);

	setLayout(layout);

	// Current settings.

	setSampleChamber(sampleChamber);

	refresh();
}

SGMSampleChamberView::~SGMSampleChamberView()
{

}

void SGMSampleChamberView::refresh()
{
	// Clear the view.

	clear();

	// Refresh turbos view.

	refreshTurbosView();

	// Update control editors.

	updateVacuumEditor();
	updateDoorEditor();
	updatePressureEditor();
	updateIonGaugeEditor();
	updateLeakValveEditor();
	updateRoughingPumpEditor();
}

void SGMSampleChamberView::setSampleChamber(SGMSampleChamber *newSampleChamber)
{
	if (sampleChamber_ != newSampleChamber) {
		sampleChamber_ = newSampleChamber;

		refresh();

		emit sampleChamberChanged(sampleChamber_);
	}
}

void SGMSampleChamberView::clear()
{
	// Clear control editors.

	vacuumEditor_->setControl(0);
	doorEditor_->setControl(0);
	pressureEditor_->setControl(0);
	ionGaugeEditor_->setControl(0);
	leakValveEditor_->setControl(0);
	roughingPumpEditor_->setControl(0);

	// Clear turbos view.

	clearTurbosView();
}

void SGMSampleChamberView::updateVacuumEditor()
{
	AMControl *vacuumControl = 0;

	if (sampleChamber_)
		vacuumControl = sampleChamber_->vacuum();

	vacuumEditor_->setControl(vacuumControl);
}

void SGMSampleChamberView::updateDoorEditor()
{
	AMControl *doorControl = 0;

	if (sampleChamber_)
		doorControl = sampleChamber_->doorStatus();

	doorEditor_->setControl(doorControl);
}

void SGMSampleChamberView::updatePressureEditor()
{
	AMControl *pressureControl = 0;

	if (sampleChamber_)
		pressureControl = sampleChamber_->pressure();

	pressureEditor_->setControl(pressureControl);
}

void SGMSampleChamberView::updateIonGaugeEditor()
{
	AMControl *ionGaugeControl = 0;

	if (sampleChamber_)
		ionGaugeControl = sampleChamber_->ionGaugeRunning();

	ionGaugeEditor_->setControl(ionGaugeControl);
}

void SGMSampleChamberView::updateLeakValveEditor()
{
	AMControl *valveState = 0;

	if (sampleChamber_ && sampleChamber_->vatValve())
		valveState = sampleChamber_->vatValve()->state();

	leakValveEditor_->setControl(valveState);
}

void SGMSampleChamberView::updateRoughingPumpEditor()
{
	AMControl *roughingPumpRunning = 0;

	if (sampleChamber_ && sampleChamber_->roughingPump())
		roughingPumpRunning = sampleChamber_->roughingPump()->runningStatus();

	roughingPumpEditor_->setControl(roughingPumpRunning);
}

void SGMSampleChamberView::clearTurbosView()
{
	// Remove all turbos editors from the layout,
	// and remove from editors list.
	// Delete all editors.

	foreach (AMExtendedControlEditor *turboEditor, turboEditorsList_) {
		turbosViewLayout_->removeWidget(turboEditor);
		turboEditorsList_.removeOne(turboEditor);

		if (turboEditor)
			turboEditor->deleteLater();
	}
}

void SGMSampleChamberView::refreshTurbosView()
{
	// Clear the turbos view.

	clearTurbosView();

	// Iterate through the vacuum control's turbos,
	// creating a control editor for each.

	if (sampleChamber_ && sampleChamber_->turbos()) {
		QList<AMControl*> controls = sampleChamber_->turbos()->toList();

		foreach (AMControl *control, controls) {
			SGMTurboPump *turbo = qobject_cast<SGMTurboPump*>(control);

			if (turbo) {
				AMExtendedControlEditor *turboEditor = new AMExtendedControlEditor(turbo->running());
				turboEditor->setNoUnitsBox(true);
				turboEditor->setTitle(turbo->name());

				turboEditorsList_.append(turboEditor);

				turbosViewLayout_->addWidget(turboEditor);
			}
		}
	}
}
