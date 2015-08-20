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

#include <QComboBox>

BioXASSidePersistentView::BioXASSidePersistentView(QWidget *parent) :
	QWidget(parent)
{
	// Create UI elements.

	generalView_ = new BioXASPersistentView(BioXASSideBeamline::bioXAS()->mono(), BioXASSideBeamline::bioXAS()->scaler());

	stripTool_ = new BioXASStripTool();

	// Create and set main layout.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(generalView_);
	layout->addWidget(stripTool_);
	layout->addStretch();

	setLayout(layout);

	// Make connections.

	connect( BioXASSideBeamline::bioXAS()->scaler(), SIGNAL(connectedChanged(bool)), generalView_, SLOT(setScalerChannelsVisible(bool)) );

	// Current settings.

	generalView_->setScalerChannelsVisible(BioXASSideBeamline::bioXAS()->scaler()->isConnected());
}

BioXASSidePersistentView::~BioXASSidePersistentView()
{

}
