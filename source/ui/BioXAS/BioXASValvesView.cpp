#include "BioXASValvesView.h"

BioXASValvesView::BioXASValvesView(BioXASValves *valves, QWidget *parent) :
    QWidget(parent)
{
	// Initialize class variables.

	valves_ = 0;

	// Create UI elements.

	valvesEditor_ = new AMExtendedControlEditor(0);
	valvesEditor_->setTitle("Valves");

	// Create and set layouts.

	childValvesLayout_ = new QVBoxLayout();
	childValvesLayout_->setMargin(0);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(valvesEditor_);
	layout->addLayout(childValvesLayout_);

	// Current settings.

	setValves(valves);

	refresh();
}

BioXASValvesView::~BioXASValvesView()
{

}

void BioXASValvesView::clear()
{

}

void BioXASValvesView::update()
{

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

		if (valves_)
			disconnect( valves_, 0, this, 0 );

		valves_ = newValves;

		if (valves_) {
			connect( valves_, SIGNAL(valueChanged(double)), this, SLOT(refresh()) );
			connect( valves_, SIGNAL(valvesChanged()), this, SLOT(refresh()) );
		}

		refresh();
		emit valvesChanged(valves_);
	}
}

void BioXASValvesView::clearChildValves()
{
	foreach (AMExtendedControlEditor *editor, childValvesEditors_) {
		childValvesLayout_->removeWidget(editor);
		childValvesEditors_.removeOne(editor);

		if (editor) {
			editor->disconnect();
			editor->deleteLater();
		}
	}
}

void BioXASValvesView::refreshChildValves()
{
	// Clear the child valves.

	clearChildValves();

	// Create new child valves editors.
}
