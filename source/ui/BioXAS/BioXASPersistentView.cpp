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

#include "ui/BioXAS/BioXASBeamStatusButtonBar.h"
#include "ui/BioXAS/BioXASSSRLMonochromatorBasicView.h"
#include "ui/BioXAS/BioXASControlEditor.h"
#include "ui/BioXAS/BioXASCryostatView.h"
#include "ui/BioXAS/BioXASSIS3820ScalerChannelsView.h"
#include "ui/AMToolButton.h"

BioXASPersistentView::BioXASPersistentView(QWidget *parent) :
    QWidget(parent)
{
	// Create and set main layout.

	QVBoxLayout *layout = new QVBoxLayout();
	setLayout(layout);

	// Create SR1 current view.

	BioXASControlEditor *sr1CurrentEditor = new BioXASControlEditor(CLSStorageRing::storageRing()->ringCurrentControl());
	sr1CurrentEditor->setTitle("SR1 current");
	sr1CurrentEditor->setReadOnly(true);

	layout->addWidget(sr1CurrentEditor);

	// Create the beam status view.

	BioXASBeamStatus *beamStatus = BioXASBeamline::bioXAS()->beamStatus();

	if (beamStatus) {

		BioXASBeamStatusButtonBar *beamStatusButtons = new BioXASBeamStatusButtonBar(BioXASBeamline::bioXAS()->beamStatus());
		connect( beamStatusButtons, SIGNAL(selectedControlChanged(AMControl*)), this, SIGNAL(beamStatusButtonsSelectedControlChanged(AMControl*)) );

		QHBoxLayout *beamStatusBoxLayout = new QHBoxLayout();
		beamStatusBoxLayout->addStretch();
		beamStatusBoxLayout->addWidget(beamStatusButtons);
		beamStatusBoxLayout->addStretch();

		QGroupBox *beamStatusBox = new QGroupBox("Beam status");
		beamStatusBox->setLayout(beamStatusBoxLayout);

		layout->addWidget(beamStatusBox);
	}

	// Create mono view.

	BioXASSSRLMonochromator *mono = BioXASBeamline::bioXAS()->mono();

	if (mono) {
		BioXASSSRLMonochromatorBasicView *monoView = new BioXASSSRLMonochromatorBasicView(mono);

		QVBoxLayout *monoBoxLayout = new QVBoxLayout();
		monoBoxLayout->addWidget(monoView);

		QGroupBox *monoBox = new QGroupBox();
		monoBox->setTitle("Monochromator");
		monoBox->setLayout(monoBoxLayout);

		layout->addWidget(monoBox);
	}

	// Create fast shutter view.

	BioXASFastShutter* fastShutter = BioXASBeamline::bioXAS()->fastShutter();

	if (fastShutter) {
		BioXASControlEditor *fastShutterEditor = new BioXASControlEditor(fastShutter);
		fastShutterEditor->setTitle("Fast shutter");

		layout->addWidget(fastShutterEditor);
	}

	// Create the cryostat view.

	cryostatView_ = new BioXASCryostatView(0);

	QVBoxLayout *cryostatBoxLayout = new QVBoxLayout();
	cryostatBoxLayout->addWidget(cryostatView_);

	cryostatBox_ = new QGroupBox();
	cryostatBox_->setTitle("Cryostat");
	cryostatBox_->setLayout(cryostatBoxLayout);

	layout->addWidget(cryostatBox_);

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

		layout->addWidget(channelsBox);
	}

    // Create end station shutter view.

    CLSExclusiveStatesControl *soeShutter = BioXASBeamline::bioXAS()->soeShutter();
    if(soeShutter){

        soeLowerButton_ = new QPushButton("Open Shutter");
        soeLowerButton_->setCheckable(true);
        connect(soeLowerButton_, SIGNAL(clicked()), this, SLOT(toggleSOEShutter()));

        soeLabel_ = new QLabel;
        soeLabel_->setPixmap(QIcon(":/32x32/redLEDOn.png").pixmap(25));
        connect(soeShutter, SIGNAL(soeShutterChanged(soeShutter)), this, SLOT(onSOEShutterChanged()));

        QHBoxLayout *soeShutterBoxLayout = new QHBoxLayout();
        soeShutterBoxLayout->addWidget(soeLowerButton_);
        soeShutterBoxLayout->addStretch();
        soeShutterBoxLayout->addWidget(soeLabel_);

        QGroupBox *soeShutterBox = new QGroupBox();
        soeShutterBox->setTitle("SOE Shutter");
        soeShutterBox->setLayout(soeShutterBoxLayout);

        layout->addWidget(soeShutterBox);

    }
	// Add final stretch to the layout, so the widgets appear new the top of the view.

	layout->addStretch();

	// Current settings.

	refresh();
}

BioXASPersistentView::~BioXASPersistentView()
{

}

void BioXASPersistentView::refresh()
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

void BioXASPersistentView::onSOEShutterChanged(bool state)
{
    if (state){

        soeLabel_->setPixmap(QIcon(":/32x32/greenLEDOn.png").pixmap(25));
        soeLowerButton_->setText("Close Shutter");
    }

    else{

        soeLabel_->setPixmap(QIcon(":/32x32/redLEDOn.png").pixmap(25));
        soeLowerButton_->setText("Open Shutter");
    }
}
