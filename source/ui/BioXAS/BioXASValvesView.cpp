#include "BioXASValvesView.h"

BioXASValvesView::BioXASValvesView(BioXASValves *valves, QWidget *parent) :
    QWidget(parent)
{
	// Initialize class variables.

	valves_ = 0;

	// Create UI elements.

	allValvesEditor_ = new AMExtendedControlEditor(0);
	allValvesEditor_->setTitle("Valves status");

	frontEnd1_ = new AMExtendedControlEditor(0);
	frontEnd2_ = new AMExtendedControlEditor(0);
	frontEnd3_ = new AMExtendedControlEditor(0);
	frontEnd4_ = new AMExtendedControlEditor(0);
	frontEnd5_ = new AMExtendedControlEditor(0);
	frontEnd6_ = new AMExtendedControlEditor(0);

	// Create and set layouts.

	QVBoxLayout *frontEndLayout = new QVBoxLayout();
	frontEndLayout->addWidget(frontEnd1_);
	frontEndLayout->addWidget(frontEnd2_);
	frontEndLayout->addWidget(frontEnd3_);
	frontEndLayout->addWidget(frontEnd4_);
	frontEndLayout->addWidget(frontEnd5_);
	frontEndLayout->addWidget(frontEnd6_);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->addWidget(allValvesEditor_);

	setLayout(layout);

	// Current settings.

	setValves(valves);

	refresh();
}

BioXASValvesView::~BioXASValvesView()
{

}

void BioXASValvesView::clear()
{
	allValvesEditor_->setControl(0);
}

void BioXASValvesView::update()
{
	if (valves_) {
		allValvesEditor_->setControl(valves_);

		if (valves_->frontEndValves()) {
			frontEnd1_->setControl(valves_->frontEndValves()->vvr1());
			frontEnd2_->setControl(valves_->frontEndValves()->vvr2());
			frontEnd3_->setControl(valves_->frontEndValves()->vvr3());
			frontEnd4_->setControl(valves_->frontEndValves()->vvr4());
			frontEnd5_->setControl(valves_->frontEndValves()->vvr5());
			frontEnd6_->setControl(valves_->frontEndValves()->vvr6());
		}
	}
}

void BioXASValvesView::refresh()
{
	// Clear the view.

	clear();

	// Update the view.

	update();
}

void BioXASValvesView::setValves(BioXASValves *newValves)
{
	if (valves_ != newValves) {
		valves_ = newValves;
		refresh();

		emit valvesChanged(valves_);
	}
}
