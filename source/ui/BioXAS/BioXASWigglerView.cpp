#include "BioXASWigglerView.h"

BioXASWigglerView::BioXASWigglerView(BioXASWiggler *wiggler, QWidget *parent) :
    QWidget(parent)
{
	wiggler_ = 0;

	// Create UI elements.

	gapEditor_ = new CLSControlEditor(0);
	gapEditor_->setTitle("Gap");
	gapEditor_->setMinimumWidth(200);

	gapStatusEditor_ = new CLSControlEditor(0);
	gapStatusEditor_->setTitle("Gap status");
	gapStatusEditor_->setMinimumWidth(200);

	// Create and set main layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->addWidget(gapEditor_);
	layout->addWidget(gapStatusEditor_);

	setLayout(layout);

	// Current settings.

	setWiggler(wiggler);
}

BioXASWigglerView::~BioXASWigglerView()
{

}

void BioXASWigglerView::setWiggler(BioXASWiggler *newWiggler)
{
	if (wiggler_ != newWiggler) {
		wiggler_ = newWiggler;
		emit wigglerChanged(wiggler_);
	}

	updateGapEditor();
	updateGapStatusEditor();
}

void BioXASWigglerView::updateGapEditor()
{
	if (wiggler_)
		gapEditor_->setControl(wiggler_->gap());
	else
		gapEditor_->setControl(0);
}

void BioXASWigglerView::updateGapStatusEditor()
{
	if (wiggler_)
		gapStatusEditor_->setControl(wiggler_->gapStatus());
	else
		gapStatusEditor_->setControl(0);
}
