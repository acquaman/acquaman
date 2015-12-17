#include "BioXASValvesView.h"

BioXASValvesView::BioXASValvesView(BioXASValves *valves, QWidget *parent) :
    QWidget(parent)
{
	// Initialize class variables.

	valves_ = 0;

	// Create UI elements.

	valvesEditor_ = new AMExtendedControlEditor(0);
	valvesEditor_->setNoUnitsBox(true);

	// Create and set layouts.

	childValvesLayout_ = new QVBoxLayout();

	QGroupBox *childValvesBox = new QGroupBox();
	childValvesBox->setFlat(true);
	childValvesBox->setLayout(childValvesLayout_);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->addWidget(valvesEditor_);
	layout->addWidget(childValvesBox);

	setLayout(layout);

	// Current settings.

	setValves(valves);

	refresh();
}

BioXASValvesView::~BioXASValvesView()
{

}

void BioXASValvesView::refresh()
{
	// Clear the view.

	valvesEditor_->setControl(0);

	clearChildValves();

	// Update the view.

	valvesEditor_->setControl(valves_);

	if (valves_) {

		QList<AMControl*> valvesList = valves_->valvesList();

		foreach (AMControl *valve, valvesList) {
			AMExtendedControlEditor *editor = new AMExtendedControlEditor(valve);
			editor->setNoUnitsBox(true);

			childValvesEditors_ << editor;

			childValvesLayout_->addWidget(editor);
		}
	}
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
