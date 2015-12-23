#include "BioXASBeamlineStatusButtonsView.h"
#include "beamline/BioXAS/BioXASBeamline.h"
#include "ui/BioXAS/BioXASShuttersButton.h"
#include "ui/BioXAS/BioXASValvesButton.h"

BioXASBeamlineStatusButtonsView::BioXASBeamlineStatusButtonsView(QWidget *parent) :
    QWidget(parent)
{
	// Create shutters button.

	frontEndShuttersButton_ = new BioXASShuttersButton(BioXASBeamline::bioXAS()->shutters());
	frontEndShuttersButton_->setToolTip("Front-end shutters");

	// Create valves button.

	valvesButton_ = new BioXASValvesButton(BioXASBeamline::bioXAS()->valves());
	valvesButton_->setToolTip("Valves");

	// Create and set main layouts.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->setMargin(0);
	layout->addWidget(frontEndShuttersButton_);
	layout->addWidget(valvesButton_);
	layout->addStretch();

	setLayout(layout);
}

BioXASBeamlineStatusButtonsView::~BioXASBeamlineStatusButtonsView()
{

}
