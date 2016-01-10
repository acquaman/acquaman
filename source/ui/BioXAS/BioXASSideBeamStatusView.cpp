#include "BioXASSideBeamStatusView.h"
#include "beamline/BioXAS/BioXASSideBeamStatus.h"
#include "beamline/BioXAS/BioXASFrontEndBeamStatus.h"
#include "beamline/BioXAS/BioXASSidePOEBeamStatus.h"
#include "beamline/BioXAS/BioXASSideSOEBeamStatus.h"
#include "ui/BioXAS/BioXASControlEditor.h"
#include "ui/BioXAS/BioXASFrontEndBeamStatusView.h"
#include "ui/BioXAS/BioXASSidePOEBeamStatusView.h"
#include "ui/BioXAS/BioXASSideSOEBeamStatusView.h"

BioXASSideBeamStatusView::BioXASSideBeamStatusView(BioXASSideBeamStatus *beamStatus, QWidget *parent) :
    QWidget(parent)
{
	// Initialize class variables.

	beamStatus_ = 0;

	// Create UI elements.

	beamStatusEditor_ = new BioXASControlEditor(0);
	beamStatusEditor_->setTitle("Beam status");

	frontEndView_ = new BioXASFrontEndBeamStatusView(0);
	frontEndView_->setMinimumWidth(250);

	sidePOEView_ = new BioXASSidePOEBeamStatusView(0);
	sidePOEView_->setMinimumWidth(250);

	sideSOEView_ = new BioXASSideSOEBeamStatusView(0);
	sideSOEView_->setMinimumWidth(250);

	// Create and set layouts.

	QHBoxLayout *subControlsLayout = new QHBoxLayout();
	subControlsLayout->addWidget(frontEndView_);
	subControlsLayout->addWidget(sidePOEView_);
	subControlsLayout->addWidget(sideSOEView_);

	QGroupBox *subControlsBox = new QGroupBox();
	subControlsBox->setFlat(true);
	subControlsBox->setLayout(subControlsLayout);

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->addWidget(beamStatusEditor_);
	layout->addWidget(subControlsBox);

	setLayout(layout);

	// Current settings.

	setBeamStatus(beamStatus);
	refresh();
}

BioXASSideBeamStatusView::~BioXASSideBeamStatusView()
{

}

void BioXASSideBeamStatusView::refresh()
{
	// Clear the view.

	beamStatusEditor_->setControl(0);
	frontEndView_->setBeamStatus(0);
	sidePOEView_->setBeamStatus(0);
	sideSOEView_->setBeamStatus(0);

	// Update UI elements.

	beamStatusEditor_->setControl(beamStatus_);

	updateFrontEndView();
	updateSidePOEView();
	updateSideSOEView();
}

void BioXASSideBeamStatusView::setBeamStatus(BioXASSideBeamStatus *newStatus)
{
	if (beamStatus_ != newStatus) {

		if (beamStatus_)
			disconnect( beamStatus_, 0, this, 0 );

		beamStatus_ = newStatus;

		if (beamStatus_) {
			connect( beamStatus_, SIGNAL(frontEndStatusChanged(BioXASFrontEndBeamStatus*)), this, SLOT(updateFrontEndView()) );
			connect( beamStatus_, SIGNAL(poeStatusChanged(BioXASSidePOEBeamStatus*)), this, SLOT(updateSidePOEView()) );
			connect( beamStatus_, SIGNAL(soeStatusChanged(BioXASSideSOEBeamStatus*)), this, SLOT(updateSideSOEView()) );
		}

		refresh();

		emit beamStatusChanged(beamStatus_);
	}
}

void BioXASSideBeamStatusView::updateFrontEndView()
{
	BioXASFrontEndBeamStatus *frontEndControl = 0;

	if (beamStatus_)
		frontEndControl = beamStatus_->frontEndStatus();

	frontEndView_->setBeamStatus(frontEndControl);
}

void BioXASSideBeamStatusView::updateSidePOEView()
{
	BioXASSidePOEBeamStatus *sidePOEControl = 0;

	if (beamStatus_)
		sidePOEControl = beamStatus_->poeStatus();

	sidePOEView_->setBeamStatus(sidePOEControl);
}

void BioXASSideBeamStatusView::updateSideSOEView()
{
	BioXASSideSOEBeamStatus *sideSOEControl = 0;

	if (beamStatus_)
		sideSOEControl = beamStatus_->soeStatus();

	sideSOEView_->setBeamStatus(sideSOEControl);
}
