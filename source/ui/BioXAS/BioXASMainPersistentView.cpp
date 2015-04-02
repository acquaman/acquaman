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
#include "ui/BioXAS/BioXASSIS3820ScalerChannelView.h"


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

	CLSSIS3820Scaler *scaler = BioXASMainBeamline::bioXAS()->scaler();
	QVBoxLayout *channelLayout = new QVBoxLayout();

	if (scaler) {
		BioXASSIS3820ScalerChannelView *i0View = new BioXASSIS3820ScalerChannelView(scaler->channelAt(0));
		channelLayout->addWidget(i0View);

		BioXASSIS3820ScalerChannelView *iTView = new BioXASSIS3820ScalerChannelView(scaler->channelAt(1));
		channelLayout->addWidget(iTView);

		BioXASSIS3820ScalerChannelView *i2View = new BioXASSIS3820ScalerChannelView(scaler->channelAt(15));
		channelLayout->addWidget(i2View);
	}

	QGroupBox *channelViews = new QGroupBox();
	channelViews->setLayout(channelLayout);

	// Create and set layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(energyControlEditor_);
	layout->addWidget(regionControlEditor_);
	layout->addWidget(braggControlEditor_);
	layout->addWidget(channelViews);
	layout->addStretch();

	setLayout(layout);
	setFixedWidth(350);
}

BioXASMainPersistentView::~BioXASMainPersistentView()
{

}
