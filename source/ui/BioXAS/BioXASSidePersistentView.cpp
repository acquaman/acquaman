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
#include "ui/beamline/AMExtendedControlEditor.h"

BioXASSidePersistentView::BioXASSidePersistentView(QWidget *parent) :
    QWidget(parent)
{
    QGroupBox* monoInfo = new QGroupBox(this);
    monoInfo->setTitle("Mono Info");

    QHBoxLayout *energyLayout = new QHBoxLayout();
    QLabel *energyLabel = new QLabel("Energy: ", this);
    energyView_ = new BioXASSideMonoBasicEnergyView(BioXASSideBeamline::bioXAS()->mono(), this);
    energyLabel->setBuddy(energyView_);
    energyLayout->addWidget(energyLabel);
    energyLayout->addWidget(energyView_);

    viewCrystalChangeButton_ = new QPushButton("Crystal Change", this);
    viewCrystalChangeButton_->setToolTip("BioXAS Side Mono Crystal Change Instructions");
    connect( viewCrystalChangeButton_, SIGNAL(clicked()), this, SLOT(toViewMonoCrystalChangeInstructions()) );

    QVBoxLayout *monoLayout = new QVBoxLayout();
    monoLayout->addLayout(energyLayout);
    monoLayout->addWidget(viewCrystalChangeButton_);
    monoInfo->setLayout(monoLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(monoInfo);
    mainLayout->addStretch();

    setLayout(mainLayout);
}

BioXASSidePersistentView::~BioXASSidePersistentView()
{

}

void BioXASSidePersistentView::toViewMonoCrystalChangeInstructions()
{    
    BioXASSideMonoCrystalChangeView *crystalChangeView = new BioXASSideMonoCrystalChangeView(BioXASSideBeamline::bioXAS()->mono());
    crystalChangeView->exec();
}
