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

#include "ui/beamline/AMExtendedControlEditor.h"
#include "ui/BioXAS/BioXASSIS3820ScalerChannelsView.h"
#include "ui/BioXAS/BioXASSSRLMonochromatorRegionControlEditor.h"

BioXASPersistentView::BioXASPersistentView(QWidget *parent) :
    QWidget(parent)
{
	// Create UI elements.

	AMExtendedControlEditor *energyEditor = new AMExtendedControlEditor(BioXASBeamline::bioXAS()->mono()->energyControl());
	energyEditor->setTitle("Mono Energy");
	energyEditor->setControlFormat('f', 2);

	BioXASSSRLMonochromatorRegionControlEditor *regionEditor = new BioXASSSRLMonochromatorRegionControlEditor(BioXASBeamline::bioXAS()->mono()->regionControl());
	regionEditor->setTitle("Mono Crystal Region");

	AMExtendedControlEditor *braggEditor = new AMExtendedControlEditor(BioXASBeamline::bioXAS()->mono()->braggMotor());
	braggEditor->setTitle("Mono Goniometer Angle");
	braggEditor->setControlFormat('f', 2);

	BioXASSIS3820ScalerChannelsView *channelsView = new BioXASSIS3820ScalerChannelsView(BioXASBeamline::bioXAS()->scaler());

	QVBoxLayout *channelsLayout = new QVBoxLayout();
	channelsLayout->addWidget(channelsView);

	QGroupBox *channelsBox = new QGroupBox();
	channelsBox->setTitle("Scaler channels");
	channelsBox->setLayout(channelsLayout);

	// Create and set main layout.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->addWidget(energyEditor);
	layout->addWidget(regionEditor);
	layout->addWidget(braggEditor);
	layout->addWidget(channelsBox);

	setLayout(layout);
}

BioXASPersistentView::~BioXASPersistentView()
{

}
