#include "BioXASShuttersView.h"
#include "beamline/BioXAS/BioXASShutters.h"
#include "ui/BioXAS/BioXASControlEditor.h"

BioXASShuttersView::BioXASShuttersView(BioXASShutters *shutters, QWidget *parent) : QWidget(parent)
{
	// Initialize class variables.

	shutters_ = 0;

	// Create and set the main layout.

	layout_ = new QVBoxLayout();
	layout_->setMargin(0);

	setLayout(layout_);
	
	// Current settings.
	
	setShutters(shutters);
	
	refresh();
}

BioXASShuttersView::~BioXASShuttersView()
{
	
}

void BioXASShuttersView::refresh()
{
	// Clear the view.

	foreach (BioXASControlEditor *editor, editors_) {
		if (editor) {
			layout_->removeWidget(editor);
			editors_.removeOne(editor);

			editor->deleteLater();
		}
	}

	// Create new editors.

	if (shutters_) {
		QList<AMControl*> shutters = shutters_->shuttersList();

		foreach (AMControl *shutter, shutters) {
			BioXASControlEditor *editor = new BioXASControlEditor(shutter);

			layout_->addWidget(editor);
			editors_.append(editor);
		}
	}
}

void BioXASShuttersView::setShutters(BioXASShutters *newShutters)
{
	if (shutters_ != newShutters) {
		
		if (shutters_)
			disconnect( shutters_, 0, this, 0 );
		
		shutters_ = newShutters;

		if (shutters_)
			connect( shutters_, SIGNAL(shuttersChanged()), this, SLOT(refresh()) );

		refresh();

		emit shuttersChanged(shutters_);
	}
}
