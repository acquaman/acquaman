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
#include "ui/beamline/AMControlConnectedLEDView.h"
#include "ui/BioXAS/BioXASFrontEndBeamStatusControlLEDView.h"
#include "ui/BioXAS/BioXASFrontEndBeamStatusView.h"

BioXASMainPersistentView::BioXASMainPersistentView(QWidget *parent) :
    QWidget(parent)
{
	// Create UI elements.

	generalView_ = new BioXASPersistentView(BioXASMainBeamline::bioXAS()->mono(), BioXASMainBeamline::bioXAS()->scaler());

	AMExtendedControlEditor *psh1 = new AMExtendedControlEditor(BioXASMainBeamline::bioXAS()->shutters()->photonShutterUpstream());
	AMExtendedControlEditor *psh2 = new AMExtendedControlEditor(BioXASMainBeamline::bioXAS()->shutters()->photonShutterDownstream());
	AMExtendedControlEditor *ssh = new AMExtendedControlEditor(BioXASMainBeamline::bioXAS()->shutters()->safetyShutter());
	AMExtendedControlEditor *shutters = new AMExtendedControlEditor(BioXASMainBeamline::bioXAS()->shutters());
	AMExtendedControlEditor *valves = new AMExtendedControlEditor(BioXASMainBeamline::bioXAS()->valves());
	AMExtendedControlEditor *frontEndValves = new AMExtendedControlEditor(BioXASMainBeamline::bioXAS()->valves()->frontEndValves());
	AMExtendedControlEditor *frontEndValve = new AMExtendedControlEditor(BioXASMainBeamline::bioXAS()->valves()->frontEndValves()->vvr2());
	AMExtendedControlEditor *sideValves = new AMExtendedControlEditor(BioXASMainBeamline::bioXAS()->valves()->sideValves());
	AMExtendedControlEditor *mainValves = new AMExtendedControlEditor(BioXASMainBeamline::bioXAS()->valves()->mainValves());
	AMExtendedControlEditor *imagingValves = new AMExtendedControlEditor(BioXASMainBeamline::bioXAS()->valves()->imagingValves());


	// Create and set layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(generalView_);
	layout->addWidget(psh1);
	layout->addWidget(psh2);
	layout->addWidget(ssh);
	layout->addWidget(shutters);
	layout->addWidget(valves);
	layout->addWidget(frontEndValves);
	layout->addWidget(frontEndValve);
	layout->addWidget(sideValves);
	layout->addWidget(mainValves);
	layout->addWidget(imagingValves);
	layout->addStretch();

	setLayout(layout);

	// Make connections.

	connect( BioXASMainBeamline::bioXAS()->scaler(), SIGNAL(connectedChanged(bool)), generalView_, SLOT(setScalerChannelsVisible(bool)) );

	// Current settings.

	generalView_->setScalerChannelsVisible(BioXASMainBeamline::bioXAS()->scaler()->isConnected());
}

BioXASMainPersistentView::~BioXASMainPersistentView()
{

}
