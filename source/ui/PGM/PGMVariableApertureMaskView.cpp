#include "PGMVariableApertureMaskView.h"

PGMVariableApertureMaskView::PGMVariableApertureMaskView(PGMVariableApertureMask *mask, QWidget *parent) :
	QWidget(parent)
{	
	vam_ = 0;

	// Create UI elements.

	upperBladeEditor_ = new AMExtendedControlEditor(0);
	upperBladeEditor_->setTitle("Upper blade");

	lowerBladeEditor_ = new AMExtendedControlEditor(0);
	lowerBladeEditor_->setTitle("Lower blade");

	outboardBladeEditor_ = new AMExtendedControlEditor(0);
	outboardBladeEditor_->setTitle("Outboard blade");

	inboardBladeEditor_ = new AMExtendedControlEditor(0);
	inboardBladeEditor_->setTitle("Inboard blade");

	// Create and set main layout.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(upperBladeEditor_);
	layout->addWidget(lowerBladeEditor_);
	layout->addWidget(outboardBladeEditor_);
	layout->addWidget(inboardBladeEditor_);

	setLayout(layout);

	// Current settings.

	setVAM(mask);
}

PGMVariableApertureMaskView::~PGMVariableApertureMaskView()
{

}

void PGMVariableApertureMaskView::setVAM(PGMVariableApertureMask *newMask)
{
	if (vam_ != newMask) {
		vam_ = newMask;
		emit vamChanged(vam_);
	}

	updateBladeEditors();
}

void PGMVariableApertureMaskView::updateBladeEditors()
{
	if (vam_) {
		upperBladeEditor_->setControl(vam_->upperBlade());
		lowerBladeEditor_->setControl(vam_->lowerBlade());
		outboardBladeEditor_->setControl(vam_->outboardBlade());
		inboardBladeEditor_->setControl(vam_->inboardBlade());

	} else {
		upperBladeEditor_->setControl(0);
		lowerBladeEditor_->setControl(0);
		outboardBladeEditor_->setControl(0);
		inboardBladeEditor_->setControl(0);
	}
}

