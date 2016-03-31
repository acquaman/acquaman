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

	// Create and set layouts.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->addWidget(xpsLadderEditor_);
	layout->addWidget(bypassLadderEditor_);
	layout->addWidget(xasLadderEditor_);

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

		emit xpsLadderChanged(xpsLadder_);
	}
}

void SGMLaddersView::setBypassLadder(SGMBypassLadder *newLadder)
{
	if (bypassLadder_ != newLadder) {
		bypassLadder_ = newLadder;

		bypassLadderEditor_->setControl(bypassLadder_);

		emit bypassLadderChanged(bypassLadder_);
	}
}

void SGMLaddersView::setXASLadder(SGMXASLadder *newLadder)
{
	if (xasLadder_ != newLadder) {
		xasLadder_ = newLadder;

		xasLadderEditor_->setControl(xasLadder_);

		emit xasLadderChanged(xasLadder_);
	}
}
