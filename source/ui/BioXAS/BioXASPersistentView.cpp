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

#include "ui/BioXAS/BioXASSSRLMonochromatorBasicView.h"
#include "ui/BioXAS/BioXASSIS3820ScalerChannelsView.h"

BioXASPersistentView::BioXASPersistentView(QWidget *parent) :
    QWidget(parent)
{
	// Create mono view.

	BioXASSSRLMonochromatorBasicView *monoView = new BioXASSSRLMonochromatorBasicView(BioXASBeamline::bioXAS()->mono());

	QVBoxLayout *monoBoxLayout = new QVBoxLayout();
	monoBoxLayout->addWidget(monoView);

	QGroupBox *monoBox = new QGroupBox();
	monoBox->setTitle("Monochromator");
	monoBox->setLayout(monoBoxLayout);

	// Create the scaler channels view.

	BioXASSIS3820ScalerChannelsView *channelsView = new BioXASSIS3820ScalerChannelsView(BioXASBeamline::bioXAS()->scaler());

	QVBoxLayout *channelsBoxLayout = new QVBoxLayout();
	channelsBoxLayout->addWidget(channelsView);

	QGroupBox *channelsBox = new QGroupBox();
	channelsBox->setTitle("Scaler channels");
	channelsBox->setLayout(channelsBoxLayout);

	// Create and set main layout.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);
	layout->addWidget(monoBox);
	layout->addWidget(channelsBox);

	setLayout(layout);
}

BioXASPersistentView::~BioXASPersistentView()
{

}
