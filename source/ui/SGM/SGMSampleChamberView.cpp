#include "SGMSampleChamberView.h"
#include "beamline/SGM/SGMTurboPump.h"
#include "beamline/SGM/SGMSampleChamber.h"
#include "beamline/SGM/SGMSampleChamberVacuum.h"
#include "beamline/SGM/SGMSampleChamberVacuumMoveControl.h"
#include "ui/beamline/AMExtendedControlEditor.h"
#include "ui/SGM/SGMVATValveView.h"

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

	vatValveView_ = new SGMVATValveView(0);

	QGroupBox *turbosView = new QGroupBox();
	turbosView->setTitle("Turbos");

	roughingPumpEditor_ = new AMExtendedControlEditor(0);
	roughingPumpEditor_->setTitle("Roughing pump");

	// Create and set layouts.

	QVBoxLayout *vatValveViewBoxLayout = new QVBoxLayout();
	vatValveViewBoxLayout->setMargin(0);
	vatValveViewBoxLayout->addWidget(vatValveView_);

	QGroupBox *vatValveViewBox = new QGroupBox();
	vatValveViewBox->setTitle("VAT valve");
	vatValveViewBox->setLayout(vatValveViewBoxLayout);
	vatValveViewBox->setMinimumWidth(500);

	turbosViewLayout_ = new QHBoxLayout();
	turbosViewLayout_->setMargin(0);

	turbosView->setLayout(turbosViewLayout_);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(vacuumEditor_);
	layout->addWidget(doorEditor_);
	layout->addWidget(pressureEditor_);
	layout->addWidget(vatValveViewBox);
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
	updateVATValveView();
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
	pressureEditor_->setControl(0);
	vatValveView_->setValve(0);

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
		doorControl = sampleChamber_->door();

	doorEditor_->setControl(doorControl);
}

void SGMSampleChamberView::updatePressureEditor()
{
	AMControl *pressureControl = 0;

	if (sampleChamber_)
		pressureControl = sampleChamber_->pressure();

	pressureEditor_->setControl(pressureControl);
}

void SGMSampleChamberView::updateVATValveView()
{
	SGMVATValve *valve = 0;

	if (sampleChamber_)
		valve = sampleChamber_->vatValve();

	vatValveView_->setValve(valve);
}

void SGMSampleChamberView::updateRoughingPumpEditor()
{
	roughingPumpEditor_->setControl(0);
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
