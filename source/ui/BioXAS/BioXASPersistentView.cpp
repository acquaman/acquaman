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
#include "ui/BioXAS/BioXASControlEditor.h"
#include "ui/BioXAS/BioXASCryostatView.h"
#include "ui/BioXAS/BioXASSIS3820ScalerChannelsView.h"

BioXASPersistentView::BioXASPersistentView(QWidget *parent) :
    QWidget(parent)
{
	// Create and set main layout.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->setMargin(0);

	setLayout(layout);

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

	BioXASCryostat *cryostat = BioXASBeamline::bioXAS()->cryostat();
	if (cryostat) {
		BioXASCryostatView *cryostatView = new BioXASCryostatView(cryostat);

		QVBoxLayout *cryostatBoxLayout = new QVBoxLayout();
		cryostatBoxLayout->addWidget(cryostatView);

		QGroupBox *cryostatBox = new QGroupBox();
		cryostatBox->setTitle("Cryostat");
		cryostatBox->setLayout(cryostatBoxLayout);

		layout->addWidget(cryostatBox);
	}

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

	// Add final stretch to the layout, so the widgets appear new the top of the view.

	layout->addStretch();
}

BioXASPersistentView::~BioXASPersistentView()
{

}
