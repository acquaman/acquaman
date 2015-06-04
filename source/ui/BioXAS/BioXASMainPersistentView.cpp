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

BioXASMainPersistentView::BioXASMainPersistentView(QWidget *parent) :
    QWidget(parent)
{
	// Create UI elements.

	generalView_ = new BioXASPersistentView(BioXASMainBeamline::bioXAS()->mono()->energyControl(), BioXASMainBeamline::bioXAS()->mono()->regionControl(), BioXASMainBeamline::bioXAS()->mono()->braggControl(), BioXASMainBeamline::bioXAS()->scaler());

	// Create and set layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(generalView_);
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
