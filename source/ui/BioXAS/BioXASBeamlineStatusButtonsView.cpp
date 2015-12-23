#include "BioXASBeamlineStatusButtonsView.h"
#include "beamline/BioXAS/BioXASBeamline.h"
#include "ui/beamline/AMExtendedControlEditor.h"
#include "ui/BioXAS/BioXASShuttersButton.h"
#include "ui/BioXAS/BioXASValvesButton.h"

BioXASBeamlineStatusButtonsView::BioXASBeamlineStatusButtonsView(QWidget *parent) :
    QWidget(parent)
{
	// Create buttons.

	frontEndShuttersButton_ = new BioXASShuttersButton(BioXASBeamline::bioXAS()->shutters());
	frontEndShuttersButton_->setToolTip("Front-end shutters");

	valvesButton_ = new BioXASValvesButton(BioXASBeamline::bioXAS()->valves());
	valvesButton_->setToolTip("Valves");

	QHBoxLayout *buttonsBoxLayout = new QHBoxLayout();
	buttonsBoxLayout->setMargin(0);
	buttonsBoxLayout->addWidget(frontEndShuttersButton_);
	buttonsBoxLayout->addWidget(valvesButton_);
	buttonsBoxLayout->addStretch();

	QWidget *buttonsBox = new QWidget();
	buttonsBox->setLayout(buttonsBoxLayout);

	// Create editors.

	frontEndShuttersEditor_ = new AMExtendedControlEditor(BioXASBeamline::bioXAS()->shutters());
	frontEndShuttersEditor_->setTitle("Front-end shutters");
	frontEndShuttersEditor_->setNoUnitsBox(true);

	valvesEditor_ = new AMExtendedControlEditor(BioXASBeamline::bioXAS()->valves());
	valvesEditor_->setTitle("Valves");
	valvesEditor_->setNoUnitsBox(true);

	QVBoxLayout *editorsBoxLayout = new QVBoxLayout();
	editorsBoxLayout->addWidget(frontEndShuttersEditor_);
	editorsBoxLayout->addWidget(valvesEditor_);

	editorsBox_ = new QGroupBox();
	editorsBox_->setFlat(true);
	editorsBox_->setLayout(editorsBoxLayout);

	// Create and set main layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->addWidget(buttonsBox);
	layout->addWidget(editorsBox_);

	setLayout(layout);
}

BioXASBeamlineStatusButtonsView::~BioXASBeamlineStatusButtonsView()
{

}
