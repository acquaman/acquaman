#include "SGMLaddersView.h"

SGMLaddersView::SGMLaddersView(SGMXPSLadder *xpsLadder, SGMBypassLadder *bypassLadder, SGMXASLadder *xasLadder, QWidget *parent) :
	QWidget(parent)
{
	// Initialize class variables.

	xpsLadder_ = 0;
	bypassLadder_ = 0;
	xasLadder_ = 0;

	// Create UI elements.

	xpsLadderEditor_ = new AMExtendedControlEditor(0);
	xpsLadderEditor_->setTitle("XPS Ladder");
	xpsLadderEditor_->setMinimumWidth(200);
	xpsLadderEditor_->setNoUnitsBox(true);

	bypassLadderEditor_ = new AMExtendedControlEditor(0);
	bypassLadderEditor_->setTitle("Bypass Ladder");
	bypassLadderEditor_->setMinimumWidth(200);
	bypassLadderEditor_->setNoUnitsBox(true);

	xasLadderEditor_ = new AMExtendedControlEditor(0);
	xasLadderEditor_->setTitle("XAS Ladder");
	xasLadderEditor_->setMinimumWidth(200);
	xasLadderEditor_->setNoUnitsBox(true);

	xpsLadderMotorEditor_ = new AMExtendedControlEditor(0);
	xpsLadderMotorEditor_->setTitle("XPS Ladder Motor");
	xpsLadderMotorEditor_->setMinimumWidth(200);
	xpsLadderMotorEditor_->setControlFormat('f', 0);

	bypassLadderMotorEditor_ = new AMExtendedControlEditor(0);
	bypassLadderMotorEditor_->setTitle("Bypass Ladder Motor");
	bypassLadderMotorEditor_->setMinimumWidth(200);
	bypassLadderMotorEditor_->setControlFormat('f', 0);

	xasLadderMotorEditor_ = new AMExtendedControlEditor(0);
	xasLadderMotorEditor_->setTitle("XAS Ladder Motor");
	xasLadderMotorEditor_->setMinimumWidth(200);
	xasLadderMotorEditor_->setControlFormat('f', 0);

	// Create and set layouts.

	QHBoxLayout *controlsLayout = new QHBoxLayout();
	controlsLayout->addWidget(xpsLadderEditor_);
	controlsLayout->addWidget(bypassLadderEditor_);
	controlsLayout->addWidget(xasLadderEditor_);

	QHBoxLayout *motorsLayout = new QHBoxLayout();
	motorsLayout->addWidget(xpsLadderMotorEditor_);
	motorsLayout->addWidget(bypassLadderMotorEditor_);
	motorsLayout->addWidget(xasLadderMotorEditor_);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addLayout(controlsLayout);
	layout->addLayout(motorsLayout);

	setLayout(layout);

	// Current settings.

	setXPSLadder(xpsLadder);
	setBypassLadder(bypassLadder);
	setXASLadder(xasLadder);
}

SGMLaddersView::~SGMLaddersView()
{

}

void SGMLaddersView::setXPSLadder(SGMXPSLadder *newLadder)
{
	if (xpsLadder_ != newLadder) {
		xpsLadder_ = newLadder;

		xpsLadderEditor_->setControl(xpsLadder_);
		xpsLadderMotorEditor_->setControl(xpsLadder_->motor());

		emit xpsLadderChanged(xpsLadder_);
	}
}

void SGMLaddersView::setBypassLadder(SGMBypassLadder *newLadder)
{
	if (bypassLadder_ != newLadder) {
		bypassLadder_ = newLadder;

		bypassLadderEditor_->setControl(bypassLadder_);
		bypassLadderMotorEditor_->setControl(bypassLadder_->motor());

		emit bypassLadderChanged(bypassLadder_);
	}
}

void SGMLaddersView::setXASLadder(SGMXASLadder *newLadder)
{
	if (xasLadder_ != newLadder) {
		xasLadder_ = newLadder;

		xasLadderEditor_->setControl(xasLadder_);
		xasLadderMotorEditor_->setControl(xasLadder_->motor());

		emit xasLadderChanged(xasLadder_);
	}
}
