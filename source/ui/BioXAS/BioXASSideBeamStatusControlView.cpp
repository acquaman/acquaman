#include "BioXASSideBeamStatusControlView.h"

BioXASSideBeamStatusControlView::BioXASSideBeamStatusControlView(BioXASSideBeamStatusControl *control, QWidget *parent) :
    QWidget(parent)
{
	// Initialize class variables.

	control_ = 0;

	// Create UI elements.

	beamStatusEditor_ = new AMExtendedControlEditor(0);
	beamStatusEditor_->setTitle("Beam status");
	beamStatusEditor_->setNoUnitsBox(true);

	frontEndView_ = new BioXASFrontEndBeamStatusControlEditorsView(0);
	frontEndView_->beamStatusEditor()->setTitle("Front-end status");
	frontEndView_->shuttersEditor()->setTitle("Shutters");
	frontEndView_->shuttersEditor()->setMinimumWidth(150);
	frontEndView_->valvesEditor()->setTitle("Valves");
	frontEndView_->valvesEditor()->setMinimumWidth(150);

	poeView_ = new BioXASSidePOEBeamStatusControlEditorsView(0);
	poeView_->beamStatusEditor()->setTitle("POE status");
	poeView_->mirrorMaskEditor()->setTitle("M1 mirror mask");
	poeView_->mirrorMaskEditor()->setMinimumWidth(150);
	poeView_->monoMaskEditor()->setTitle("Mono mask");
	poeView_->monoMaskEditor()->setMinimumWidth(150);

	soeView_ = new BioXASSideSOEBeamStatusControlEditorsView(0);
	soeView_->beamStatusEditor()->setTitle("SOE status");
	soeView_->shutterEditor()->setTitle("Shutter");
	soeView_->shutterEditor()->setMinimumWidth(150);

	// Create and set layouts.

	QVBoxLayout *frontEndLayout = new QVBoxLayout();
	frontEndLayout->setMargin(0);
	frontEndLayout->addWidget(frontEndView_);
	frontEndLayout->addStretch();

	QVBoxLayout *poeLayout = new QVBoxLayout();
	poeLayout->setMargin(0);
	poeLayout->addWidget(poeView_);
	poeLayout->addStretch();

	QVBoxLayout *soeLayout = new QVBoxLayout();
	soeLayout->setMargin(0);
	soeLayout->addWidget(soeView_);
	soeLayout->addStretch();

	QHBoxLayout *componentsLayout = new QHBoxLayout();
	componentsLayout->setMargin(0);
	componentsLayout->addLayout(frontEndLayout);
	componentsLayout->addLayout(poeLayout);
	componentsLayout->addLayout(soeLayout);

	QVBoxLayout *layout =  new QVBoxLayout();
	layout->addWidget(beamStatusEditor_);
	layout->addLayout(componentsLayout);

	setLayout(layout);

	// Current settings.

	setControl(control);
	refresh();
}

BioXASSideBeamStatusControlView::~BioXASSideBeamStatusControlView()
{

}

void BioXASSideBeamStatusControlView::clear()
{
	frontEndView_->setControl(0);
	poeView_->setControl(0);
	soeView_->setControl(0);
}

void BioXASSideBeamStatusControlView::update()
{
	frontEndView_->update();
	poeView_->update();
	soeView_->update();
}

void BioXASSideBeamStatusControlView::refresh()
{
	// Clear the view.

	clear();

	// Apply current control settings.

	beamStatusEditor_->setControl(control_);

	if (control_) {

		if (control_->poeBeamStatus())
			frontEndView_->setControl(control_->poeBeamStatus()->frontEndBeamStatus());

		poeView_->setControl(control_->poeBeamStatus());

		soeView_->setControl(control_->soeBeamStatus());
	}

	// Update the view.

	update();
}

void BioXASSideBeamStatusControlView::setControl(BioXASSideBeamStatusControl *newControl)
{
	if (control_ != newControl) {

		if (control_) {
			disconnect( control_, 0, this, 0 );
			disconnect( control_->poeBeamStatus(), 0, this, 0 );
		}

		control_ = newControl;

		if (control_) {
			connect( control_, SIGNAL(connected(bool)), this, SLOT(update()) );
			connect( control_->poeBeamStatus(), SIGNAL(frontEndBeamStatusChanged(AMControl*)), this, SLOT(refresh()) );
		}

		refresh();

		emit controlChanged(control_);
	}
}

