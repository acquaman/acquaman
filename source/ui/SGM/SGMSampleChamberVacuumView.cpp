#include "SGMSampleChamberVacuumView.h"

SGMSampleChamberVacuumView::SGMSampleChamberVacuumView(SGMSampleChamberVacuum *vacuumControl, QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables

	vacuum_ = 0;

	// Create UI elements.

	vacuumEditor_ = new AMExtendedControlEditor(0);

	pressureEditor_ = new AMExtendedControlEditor(0);

	vatValveView_ = new SGMVATValveView(0);

	// Create and set layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(vacuumEditor_);
	layout->addWidget(pressureEditor_);
	layout->addWidget(vatValveView_);

	setLayout(layout);

	// Current settings.

	setVacuum(vacuumControl);
}

SGMSampleChamberVacuumView::~SGMSampleChamberVacuumView()
{

}

void SGMSampleChamberVacuumView::clear()
{
	vacuumEditor_->setControl(0);
	pressureEditor_->setControl(0);
	vatValveView_->setValve(0);
}

void SGMSampleChamberVacuumView::refresh()
{
	refreshVacuumEditor();
	refreshPressureEditor();
	refreshVATValveView();
}

void SGMSampleChamberVacuumView::setVacuum(SGMSampleChamberVacuum *newControl)
{
	if (vacuum_ != newControl) {

		if (vacuum_)
			disconnect( vacuum_, 0, this, 0 );

		vacuum_ = newControl;

		if (vacuum_) {
			connect( vacuum_, SIGNAL(pressureChanged(AMControl*)), this, SLOT(refreshPressureEditor()) );
			connect( vacuum_, SIGNAL(vatValveChanged(SGMVATValve*)), this, SLOT(refreshVATValveView()) );
		}

		refresh();

		emit vacuumChanged(vacuum_);
	}
}

void SGMSampleChamberVacuumView::refreshVacuumEditor()
{
	vacuumEditor_->setControl(0);
	vacuumEditor_->setControl(vacuum_);
}

void SGMSampleChamberVacuumView::refreshPressureEditor()
{
	pressureEditor_->setControl(0);

	if (vacuum_)
		pressureEditor_->setControl(vacuum_->pressure());
}

void SGMSampleChamberVacuumView::refreshVATValveView()
{
	vatValveView_->setValve(0);

	if (vacuum_)
		vatValveView_->setValve(vacuum_->vatValve());
}
