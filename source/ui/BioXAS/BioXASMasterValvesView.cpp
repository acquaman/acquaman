#include "BioXASMasterValvesView.h"
#include "beamline/BioXAS/BioXASValves.h"
#include "beamline/BioXAS/BioXASMasterValves.h"
#include "beamline/BioXAS/BioXASFrontEndValves.h"
#include "beamline/BioXAS/BioXASSideValves.h"
#include "beamline/BioXAS/BioXASMainValves.h"
#include "beamline/BioXAS/BioXASImagingValves.h"
#include "ui/beamline/AMExtendedControlEditor.h"
#include "ui/BioXAS/BioXASValvesView.h"

BioXASMasterValvesView::BioXASMasterValvesView(BioXASMasterValves *valves, QWidget *parent) :
    QWidget(parent)
{
	// Initialize member variables.

	valves_ = 0;

	// Create main valves editor.

	valvesEditor_ = new AMExtendedControlEditor(0);
	valvesEditor_->setTitle("Valves");
	valvesEditor_->setNoUnitsBox(true);

	// Create front-end valves view.

	frontEndView_ = new BioXASValvesView(0);

	// Create Side valves view.

	sideView_ = new BioXASValvesView(0);

	// Create Main valves view.

	mainView_ = new BioXASValvesView(0);

	// Create Imaging valves view.

	imagingView_ = new BioXASValvesView(0);

	// Create and set main layouts.

	QHBoxLayout *valvesLayout = new QHBoxLayout();
	valvesLayout->addWidget(frontEndView_);
	valvesLayout->addWidget(sideView_);
	valvesLayout->addWidget(mainView_);
	valvesLayout->addWidget(imagingView_);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(valvesEditor_);
	layout->addLayout(valvesLayout);

	setLayout(layout);

	// Current settings.

	setValves(valves);

	refresh();
}

BioXASMasterValvesView::~BioXASMasterValvesView()
{

}

void BioXASMasterValvesView::refresh()
{
	// Clear the view.

	valvesEditor_->setControl(0);

	frontEndView_->setValves(0);
	sideView_->setValves(0);
	mainView_->setValves(0);
	imagingView_->setValves(0);

	// Update view elements.

	valvesEditor_->setControl(valves_);

	updateFrontEndView();
	updateSideView();
	updateMainView();
	updateImagingView();
}

void BioXASMasterValvesView::setValves(BioXASMasterValves *newValves)
{
	if (valves_ != newValves) {

		if (valves_)
			disconnect( valves_, 0, this, 0 );

		valves_ = newValves;

		if (valves_) {
			connect( valves_, SIGNAL(frontEndValvesChanged(BioXASFrontEndValves*)), this, SLOT(updateFrontEndView()) );
			connect( valves_, SIGNAL(sideValvesChanged(BioXASSideValves*)), this, SLOT(updateSideView()) );
			connect( valves_, SIGNAL(mainValvesChanged(BioXASMainValves*)), this, SLOT(updateMainView()) );
			connect( valves_, SIGNAL(imagingValvesChanged(BioXASImagingValves*)), this, SLOT(updateImagingView()) );
		}

		refresh();

		emit valvesChanged(valves_);
	}
}

void BioXASMasterValvesView::updateFrontEndView()
{
	BioXASValves *frontEndValves = 0;

	if (valves_)
		frontEndValves = valves_->frontEndValves();

	frontEndView_->setValves(frontEndValves);
}

void BioXASMasterValvesView::updateSideView()
{
	BioXASValves *sideValves = 0;

	if (valves_)
		sideValves = valves_->sideValves();

	sideView_->setValves(sideValves);
}

void BioXASMasterValvesView::updateMainView()
{
	BioXASValves *mainValves = 0;

	if (valves_)
		mainValves = valves_->mainValves();

	mainView_->setValves(mainValves);
}

void BioXASMasterValvesView::updateImagingView()
{
	BioXASValves *imagingValves = 0;

	if (valves_)
		imagingValves = valves_->imagingValves();

	imagingView_->setValves(imagingValves);
}
