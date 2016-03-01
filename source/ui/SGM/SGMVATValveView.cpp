#include "SGMVATValveView.h"
#include "beamline/AMPVControl.h"
#include "beamline/SGM/SGMVATValveState.h"

SGMVATValveView::SGMVATValveView(SGMVATValve *valve, QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables.

	valve_ = 0;

	// Create UI elements.

	statusEditor_ = new AMExtendedControlEditor(0);
	statusEditor_->setTitle("Status");
	statusEditor_->setNoUnitsBox(true);

	positionEditor_ = new AMExtendedControlEditor(0);
	positionEditor_->setTitle("Position");
	positionEditor_->setControlFormat('f', 0);
	positionEditor_->setUnits("step");

	speedEditor_ = new AMExtendedControlEditor(0);
	speedEditor_->setTitle("Speed");
	speedEditor_->setNoUnitsBox(true);

	// Create and set layouts.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->addWidget(statusEditor_);
	layout->addWidget(positionEditor_);
	layout->addWidget(speedEditor_);

	setLayout(layout);

	// Current settings.

	setValve(valve);

	refresh();
}

SGMVATValveView::~SGMVATValveView()
{

}

void SGMVATValveView::clear()
{
	statusEditor_->setControl(0);
	positionEditor_->setControl(0);
	speedEditor_->setControl(0);
}

void SGMVATValveView::refresh()
{
	clear();

	if (valve_) {
		statusEditor_->setControl(valve_->state());

		positionEditor_->setControl(valve_->position());
		speedEditor_->setControl(valve_->speed());
	}
}

void SGMVATValveView::setValve(SGMVATValve *newControl)
{
	if (valve_ != newControl) {

		valve_ = newControl;
		refresh();

		emit valveChanged(valve_);
	}
}
