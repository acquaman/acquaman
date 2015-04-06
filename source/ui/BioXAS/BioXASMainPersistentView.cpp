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


#include "BioXASMainPersistentView.h"

#include "beamline/BioXAS/BioXASMainBeamline.h"
#include "ui/beamline/AMExtendedControlEditor.h"
#include "ui/CLS/CLSSIS3820ScalerView.h"

BioXASMainPersistentView::BioXASMainPersistentView(QWidget *parent) :
    QWidget(parent)
{
	// Create UI elements.

	energyControlEditor_ = new AMExtendedControlEditor(BioXASMainBeamline::bioXAS()->mono()->energyControl());
	energyControlEditor_->setTitle("Energy");
	energyControlEditor_->setControlFormat('f', 2);

	regionControlEditor_ = new BioXASSSRLMonochromatorRegionControlEditor(BioXASMainBeamline::bioXAS()->mono()->regionControl());
	regionControlEditor_->setTitle("Region");

	braggControlEditor_ = new AMExtendedControlEditor(BioXASMainBeamline::bioXAS()->mono()->braggMotor());
	braggControlEditor_->setTitle("Bragg motor position");

	// Create the scaler channel views.

	BioXASSIS3820ScalerChannelsView *channels = new BioXASSIS3820ScalerChannelsView(BioXASMainBeamline::bioXAS()->scaler());

	QVBoxLayout *channelsLayout = new QVBoxLayout();
	channelsLayout->addWidget(channels);

	channelsView_ = new QGroupBox();
	channelsView_->setTitle("Scaler channels");
	channelsView_->setLayout(channelsLayout);

	// Create and set main layout.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(energyControlEditor_);
	layout->addWidget(regionControlEditor_);
	layout->addWidget(braggControlEditor_);
	layout->addWidget(channelsView_);
	layout->addStretch();

	setLayout(layout);

	// Initial settings.

	channelsView_->hide();

	// Make connections.

	connect( BioXASMainBeamline::bioXAS()->scaler(), SIGNAL(connectedChanged(bool)), this, SLOT(showScalerChannels(bool)) );

	// Current settings.

	showScalerChannels(BioXASMainBeamline::bioXAS()->scaler()->isConnected());
}

BioXASMainPersistentView::~BioXASMainPersistentView()
{

}

void BioXASMainPersistentView::showScalerChannels(bool show)
{
	channelsView_->setShown(show);
}
