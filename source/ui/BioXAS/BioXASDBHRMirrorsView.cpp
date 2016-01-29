#include "BioXASDBHRMirrorsView.h"

BioXASDBHRMirrorsView::BioXASDBHRMirrorsView(BioXASDBHRMirrors *mirrors, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	mirrors_ = 0;

	m1Editor_ = new AMExtendedControlEditor(0);
	m1Editor_->setTitle("M1 Vertical");

	m2Editor_ = new AMExtendedControlEditor(0);
	m2Editor_->setTitle("M2 Vertical");

	pitchEditor_ = new AMExtendedControlEditor(0);
	pitchEditor_->setTitle("Pitch");

	// Create and set layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(m1Editor_);
	layout->addWidget(m2Editor_);
	layout->addWidget(pitchEditor_);

	setLayout(layout);

	// Current settings.

	setMirrors(mirrors);
}

BioXASDBHRMirrorsView::~BioXASDBHRMirrorsView()
{

}

void BioXASDBHRMirrorsView::setMirrors(BioXASDBHRMirrors *newMirrors)
{
	if (mirrors_ != newMirrors) {

		if (mirrors_) {

			// Clear UI elements.

			m1Editor_->setControl(0);
			m2Editor_->setControl(0);
			pitchEditor_->setControl(0);
		}

		mirrors_ = newMirrors;

		if (mirrors_) {

			// Update UI elements.

			m1Editor_->setControl(mirrors_->m1VerticalControl());
			m2Editor_->setControl(mirrors_->m2VerticalControl());
			pitchEditor_->setControl(mirrors_->pitchControl());
		}

		emit mirrorsChanged(mirrors_);
	}
}


