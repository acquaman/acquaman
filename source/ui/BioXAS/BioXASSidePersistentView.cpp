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

#include "beamline/BioXAS/BioXASSideBeamline.h"

BioXASSidePersistentView::BioXASSidePersistentView(QWidget *parent) :
    QWidget(parent)
{
	// Create UI elements.

    energyControlEditor_ = new AMExtendedControlEditor(BioXASSideBeamline::bioXAS()->mono()->energyControl());
    energyControlEditor_->setTitle("Energy");
    energyControlEditor_->setControlFormat('f', 2);

    regionControlEditor_ = new BioXASSSRLMonochromatorRegionControlEditor(BioXASSideBeamline::bioXAS()->mono()->regionControl());
    regionControlEditor_->setTitle("Region");

    braggControlEditor_ = new AMExtendedControlEditor(BioXASSideBeamline::bioXAS()->mono()->braggMotor());
    braggControlEditor_->setTitle("Bragg motor position");

    calibrateEnergyButton_ = new QPushButton("Calibrate energy");

    regionView_ = new BioXASSSRLMonochromatorRegionControlView(BioXASSideBeamline::bioXAS()->mono()->regionControl());

    // Create and set layouts.

    QHBoxLayout *energyButtonLayout = new QHBoxLayout();
    energyButtonLayout->addStretch();
    energyButtonLayout->addWidget(calibrateEnergyButton_);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(energyControlEditor_);
    layout->addWidget(regionControlEditor_);
    layout->addWidget(braggControlEditor_);
    layout->addLayout(energyButtonLayout);
    layout->addWidget(regionView_);
    layout->addStretch();

    setLayout(layout);
    setFixedWidth(300);

    // Make connections.

    connect( calibrateEnergyButton_, SIGNAL(clicked()), this, SLOT(onCalibrateEnergyButtonClicked()) );
}

BioXASSidePersistentView::~BioXASSidePersistentView()
{

}

void BioXASSidePersistentView::onCalibrateEnergyButtonClicked()
{
	bool inputOK = false;
	double newEnergy = QInputDialog::getDouble(this, "Monochromator Energy Calibration", "Enter current calibrated energy:", BioXASSideBeamline::bioXAS()->mono()->energyControl()->value(), -10000000, 10000000, 1, &inputOK, Qt::Sheet);

	if (inputOK) {
		BioXASSideBeamline::bioXAS()->mono()->setEnergyCalibration(newEnergy);
	}
}
