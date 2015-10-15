#include "BioXASValvesView.h"

BioXASValvesView::BioXASValvesView(BioXASValves *valves, QWidget *parent) :
    QWidget(parent)
{
	// Initialize class variables.

	valves_ = 0;

	// Create UI elements.

	// Create and set layouts.

	frontEndValvesBox_ = new QGroupBox();

	QGridLayout *layout = new QGridLayout();
	layout->setMargin(0);

	setLayout(layout);

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

}

void BioXASValvesView::setValves(BioXASValves *valves)
{
	Q_UNUSED(valves)
}
