#include "SGMSampleChamberVacuumView.h"

SGMSampleChamberVacuumView::SGMSampleChamberVacuumView(SGMSampleChamberVacuum *vacuumControl, QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables

	vacuum_ = 0;

	// Create UI elements.

	vacuumEditor_ = new AMExtendedControlEditor(0);
	vacuumEditor_->setTitle("Vacuum status");
	vacuumEditor_->setNoUnitsBox(true);

	pressureEditor_ = new AMExtendedControlEditor(0);
	pressureEditor_->setTitle("Pressure");
	pressureEditor_->setUnits("Torr");

	vatValveView_ = new SGMVATValveView(0);

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

	QGroupBox *turbosView = new QGroupBox();
	turbosView->setTitle("Turbos");
	turbosView->setLayout(turbosViewLayout_);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(vacuumEditor_);
	layout->addWidget(pressureEditor_);
	layout->addWidget(vatValveViewBox);
	layout->addWidget(turbosView);

	setLayout(layout);

	// Current settings.

	setVacuum(vacuumControl);

	refresh();
}

SGMSampleChamberVacuumView::~SGMSampleChamberVacuumView()
{

}

void SGMSampleChamberVacuumView::clear()
{
	// Clear control editors.

	vacuumEditor_->setControl(0);
	pressureEditor_->setControl(0);
	vatValveView_->setValve(0);

	// Clear turbos view.

	clearTurbosView();
}

void SGMSampleChamberVacuumView::refresh()
{
	// Clear the view.

	clear();

	// Refresh turbos view.

	refreshTurbosView();

	// Update control editors.

	updateVacuumEditor();
	updatePressureEditor();
	updateVATValveView();
}

void SGMSampleChamberVacuumView::setVacuum(SGMSampleChamberVacuum *newControl)
{
	if (vacuum_ != newControl) {

		if (vacuum_)
			disconnect( vacuum_, 0, this, 0 );

		vacuum_ = newControl;

		if (vacuum_) {
			connect( vacuum_, SIGNAL(pressureChanged(AMControl*)), this, SLOT(updatePressureEditor()) );
			connect( vacuum_, SIGNAL(vatValveChanged(SGMVATValve*)), this, SLOT(updateVATValveView()) );
			connect( vacuum_, SIGNAL(turbosChanged(AMControlSet*)), this, SLOT(refreshTurbosView()) );
		}

		refresh();

		emit vacuumChanged(vacuum_);
	}
}

void SGMSampleChamberVacuumView::clearTurbosView()
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

void SGMSampleChamberVacuumView::updateVacuumEditor()
{
	vacuumEditor_->setControl(vacuum_);
}

void SGMSampleChamberVacuumView::updatePressureEditor()
{
	AMControl *pressureControl = 0;

	if (vacuum_)
		pressureControl = vacuum_->pressure();

	pressureEditor_->setControl(pressureControl);
}

void SGMSampleChamberVacuumView::updateVATValveView()
{
	SGMVATValve *valve = 0;

	if (vacuum_)
		valve = vacuum_->vatValve();

	vatValveView_->setValve(valve);
}

void SGMSampleChamberVacuumView::refreshTurbosView()
{
	// Clear the turbos view.

	clearTurbosView();

	// Iterate through the vacuum control's turbos,
	// creating a control editor for each.

	if (vacuum_ && vacuum_->turbos()) {
		QList<AMControl*> controls = vacuum_->turbos()->toList();

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
