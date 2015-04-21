#include "BioXASDBHRMirrorView.h"

BioXASDBHRMirrorView::BioXASDBHRMirrorView(BioXASDBHRMirror *mirror, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	mirror_ = 0;

	pitchEditor_ = new AMExtendedControlEditor(0);
	pitchEditor_->setTitle("Pitch");

	m1Editor_ = new AMExtendedControlEditor(0);
	m1Editor_->setTitle("M1 Vertical");

	m2Editor_ = new AMExtendedControlEditor(0);
	m2Editor_->setTitle("M2 Vertical");

	// Create and set layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(pitchEditor_);
	layout->addWidget(m1Editor_);
	layout->addWidget(m2Editor_);

	setLayout(layout);

	// Current settings.

	setMirror(mirror);
}

BioXASDBHRMirrorView::~BioXASDBHRMirrorView()
{

}

void BioXASDBHRMirrorView::setMirror(BioXASDBHRMirror *newMirror)
{
	if (mirror_ != newMirror) {

		if (mirror_) {

			// Clear UI elements.

			pitchEditor_->setControl(0);
			m1Editor_->setControl(0);
			m2Editor_->setControl(0);
		}

		mirror_ = newMirror;

		if (mirror_) {

			// Update UI elements.

			pitchEditor_->setControl(mirror_->pitchControl());
			m1Editor_->setControl(mirror_->m1VerticalControl());
			m2Editor_->setControl(mirror_->m2VerticalControl());
		}

		emit mirrorChanged(mirror_);
	}
}


