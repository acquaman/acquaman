#include "SGMVATValveView.h"

SGMVATValveView::SGMVATValveView(SGMVATValve *valve, QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables.

	valve_ = 0;

	// Create UI elements.

	positionEditor_ = new AMExtendedControlEditor(0);
	positionEditor_->setTitle("Position");

	speedEditor_ = new AMExtendedControlEditor(0);
	speedEditor_->setTitle("Speed");

	// Create and set layouts.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->addWidget(positionEditor_);
	layout->addWidget(speedEditor_);

	setLayout(layout);

	// Current settings.

	setValve(valve);
}

SGMVATValveView::~SGMVATValveView()
{

}

void SGMVATValveView::clear()
{
	positionEditor_->setControl(0);
	speedEditor_->setControl(0);
}

void SGMVATValveView::refresh()
{
	clear();

	if (valve_) {
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
