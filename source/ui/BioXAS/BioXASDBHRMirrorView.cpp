#include "BioXASDBHRMirrorView.h"

BioXASDBHRMirrorView::BioXASDBHRMirrorView(BioXASDBHRMirror *mirror, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	mirror_ = 0;

	layout_ = new QVBoxLayout();
	setLayout(layout_);

	pitchEditor_ = 0;
	m1Editor_ = 0;
	m2Editor_ = 0;

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

			// remove mirror editors from view layout.

			layout_->removeWidget(pitchEditor_);
			layout_->removeWidget(m1Editor_);
			layout_->removeWidget(m2Editor_);

			// delete mirror editors.

			pitchEditor_->deleteLater();
			pitchEditor_ = 0;

			m1Editor_->deleteLater();
			m1Editor_ = 0;

			m2Editor_->deleteLater();
			m2Editor_ = 0;
		}

		mirror_ = newMirror;

		if (mirror_) {

			// create mirror editors, add to view layout.

			pitchEditor_ = new AMExtendedControlEditor(mirror_->pitchControl());
			pitchEditor_->setTitle("Pitch");
			layout_->addWidget(pitchEditor_);

			m1Editor_ = new AMExtendedControlEditor(mirror_->m1VerticalControl());
			m1Editor_->setTitle("M1 Vertical");
			layout_->addWidget(m1Editor_);

			m2Editor_ = new AMExtendedControlEditor(mirror_->m2VerticalControl());
			m2Editor_->setTitle("M2 Vertical");
			layout_->addWidget(m2Editor_);
		}

		emit mirrorChanged(mirror_);
	}
}


