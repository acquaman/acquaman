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


#include "BioXASSidePersistentView.h"
#include "ui/BioXAS/BioXASSIS3820ScalerChannelsView.h"
#include "beamline/BioXAS/BioXASSideBeamline.h"
#include "ui/beamline/AMControlEditor.h"

BioXASSidePersistentView::BioXASSidePersistentView(QWidget *parent) :
    QWidget(parent)
{
	// Energy control editor.

	energyControlEditor_ = new AMExtendedControlEditor(BioXASSideBeamline::bioXAS()->mono()->energyControl());
	energyControlEditor_->setTitle("Mono Energy");
	energyControlEditor_->setControlFormat('f', 2);

	// Region control editor.

	regionControlEditor_ = new BioXASSSRLMonochromatorRegionControlEditor(BioXASSideBeamline::bioXAS()->mono()->regionControl());
	regionControlEditor_->setTitle("Mono Region");

	// Bragg control editor.

	braggControlEditor_ = new AMExtendedControlEditor(BioXASSideBeamline::bioXAS()->mono()->braggMotor());
	braggControlEditor_->setTitle("Bragg motor position");

	// Scaler channel views.

	BioXASSIS3820ScalerChannelsView *channels = new BioXASSIS3820ScalerChannelsView(BioXASSideBeamline::bioXAS()->scaler());

	QVBoxLayout *channelsLayout = new QVBoxLayout();
	channelsLayout->addWidget(channels);

	channelViews_ = new QGroupBox();
	channelViews_->setTitle("Scaler channels");
	channelViews_->setLayout(channelsLayout);

	// AMExtendedControlEditor and AMControlEditor testing.

	AMExtendedControlEditor *extendedEditor = new AMExtendedControlEditor(0);
	extendedEditor->setControl(BioXASSideBeamline::bioXAS()->dbhrMirror()->pitchControl());

	AMControlEditor *controlEditor = new AMControlEditor(0);
	controlEditor->setControl(BioXASSideBeamline::bioXAS()->dbhrMirror()->pitchControl());
	controlEditor->setSecondaryControl(BioXASSideBeamline::bioXAS()->dbhrMirror()->m1VerticalControl());

	QVBoxLayout *testingLayout = new QVBoxLayout();
	testingLayout->addWidget(extendedEditor);
	testingLayout->addWidget(controlEditor);

	// Create and set main layout.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(energyControlEditor_);
	layout->addWidget(regionControlEditor_);
	layout->addWidget(braggControlEditor_);
	layout->addWidget(channelViews_);
	layout->addLayout(testingLayout);
	layout->addStretch();

	setLayout(layout);
	setFixedWidth(355);

	// Initial settings.

	channelViews_->hide();

	// Make connections.

	connect( BioXASSideBeamline::bioXAS()->scaler(), SIGNAL(connectedChanged(bool)), this, SLOT(onScalerConnectedChanged()) );

	// Current settings.

	onScalerConnectedChanged();
}

BioXASSidePersistentView::~BioXASSidePersistentView()
{

}

void BioXASSidePersistentView::onScalerConnectedChanged()
{
	CLSSIS3820Scaler *scaler = BioXASSideBeamline::bioXAS()->scaler();

	if (scaler && scaler->isConnected()) {
		channelViews_->show();
	} else {
		channelViews_->hide();
	}
}
