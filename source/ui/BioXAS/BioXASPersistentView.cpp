/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "BioXASPersistentView.h"

#include "beamline/BioXAS/BioXASBeamline.h"

#include "ui/beamline/AMControlToolButton.h"
#include "ui/CLS/CLSControlEditor.h"
#include "ui/CLS/CLSBeamlineStatusView.h"
#include "ui/BioXAS/BioXASSSRLMonochromatorBasicView.h"
#include "ui/BioXAS/BioXASCryostatView.h"
#include "ui/BioXAS/BioXASSIS3820ScalerChannelsView.h"
#include "ui/BioXAS/BioXASBeamStatusButtonBar.h"

BioXASPersistentView::BioXASPersistentView(QWidget *parent) :
    QWidget(parent)
{
	// Create and set main layout.

	QVBoxLayout *mainViewLayout = new QVBoxLayout();
	setLayout(mainViewLayout);

	// Create SR1 current view.

	CLSControlEditor *sr1CurrentEditor = new CLSControlEditor(CLSStorageRing::storageRing()->ringCurrentControl());
	sr1CurrentEditor->setTitle("SR1 current");
	sr1CurrentEditor->setReadOnly(true);

	mainViewLayout->addWidget(sr1CurrentEditor);

	// Create the beam status view.

	BioXASBeamStatusButtonBar *beamStatusButtonBar = new BioXASBeamStatusButtonBar(BioXASBeamline::bioXAS()->beamStatus());
	connect( beamStatusButtonBar, SIGNAL(componentClicked(AMControl*)), this, SIGNAL(beamStatusControlClicked(AMControl*)) );

	QHBoxLayout *beamStatusBoxLayout = new QHBoxLayout();
	beamStatusBoxLayout->addStretch();
	beamStatusBoxLayout->addWidget(beamStatusButtonBar);
	beamStatusBoxLayout->addStretch();

	QGroupBox *beamStatusBox = new QGroupBox("Beam status");
	beamStatusBox->setLayout(beamStatusBoxLayout);

	mainViewLayout->addWidget(beamStatusBox);

	// Create mono view.

	BioXASSSRLMonochromator *mono = BioXASBeamline::bioXAS()->mono();

	if (mono) {
		BioXASSSRLMonochromatorBasicView *monoView = new BioXASSSRLMonochromatorBasicView(mono);

		QVBoxLayout *monoBoxLayout = new QVBoxLayout();
		monoBoxLayout->addWidget(monoView);

		QGroupBox *monoBox = new QGroupBox();
		monoBox->setTitle("Monochromator");
		monoBox->setLayout(monoBoxLayout);

		mainViewLayout->addWidget(monoBox);
	}

	// Create endstation shutter view.

	CLSExclusiveStatesControl *soeShutter = BioXASBeamline::bioXAS()->soeShutter();

	if (soeShutter) {
		CLSControlEditor *soeShutterEditor = new CLSControlEditor(soeShutter);
		soeShutterEditor->setTitle("SOE shutter");

		mainViewLayout->addWidget(soeShutterEditor);
	}

	// Create kill switch status view.

	   AMReadOnlyPVControl *endStationKillSwitchStatus = BioXASBeamline::bioXAS()->endStationKillSwitch();

	   if (endStationKillSwitchStatus) {

		   CLSControlEditor *killSwitchEditor = new CLSControlEditor(endStationKillSwitchStatus);
		   killSwitchEditor->setTitle("Endstation motors kill-switch");

		   mainViewLayout->addWidget(killSwitchEditor);
	   }

	// Create fast shutter view.

	BioXASFastShutter* fastShutter = BioXASBeamline::bioXAS()->fastShutter();

	if (fastShutter) {
		CLSControlEditor *fastShutterEditor = new CLSControlEditor(fastShutter);
		fastShutterEditor->setTitle("Fast shutter");

		mainViewLayout->addWidget(fastShutterEditor);
	}

	// Create the cryostat view.

	cryostatView_ = new BioXASCryostatView(0);

	QVBoxLayout *cryostatBoxLayout = new QVBoxLayout();
	cryostatBoxLayout->addWidget(cryostatView_);

	cryostatBox_ = new QGroupBox();
	cryostatBox_->setTitle("Cryostat");
	cryostatBox_->setLayout(cryostatBoxLayout);

	mainViewLayout->addWidget(cryostatBox_);

	connect( BioXASBeamline::bioXAS(), SIGNAL(usingCryostatChanged(bool)), this, SLOT(updateCryostatBox()) );

	// Create the scaler channels view.

	CLSSIS3820Scaler *scaler = BioXASBeamline::bioXAS()->scaler();

	if (scaler) {
		BioXASSIS3820ScalerChannelsView *channelsView = new BioXASSIS3820ScalerChannelsView(scaler);

		QVBoxLayout *channelsBoxLayout = new QVBoxLayout();
		channelsBoxLayout->addWidget(channelsView);

		QGroupBox *channelsBox = new QGroupBox();
		channelsBox->setTitle("Scaler channels");
		channelsBox->setLayout(channelsBoxLayout);

		mainViewLayout->addWidget(channelsBox);
	}

	// Add final stretch to the layout, so the widgets appear new the top of the view.

	mainViewLayout->addStretch();

	// Current settings.

	updateView();
}

BioXASPersistentView::~BioXASPersistentView()
{

}

void BioXASPersistentView::updateView()
{
	updateCryostatBox();
}

void BioXASPersistentView::updateCryostatBox()
{
	if (BioXASBeamline::bioXAS()->usingCryostat()) {
		cryostatView_->setControl(BioXASBeamline::bioXAS()->cryostat());
		cryostatBox_->show();

	} else {
		cryostatBox_->hide();
	}
}
