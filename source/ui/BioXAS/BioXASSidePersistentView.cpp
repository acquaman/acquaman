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
    energyEditor_ = new BioXASSideMonoBasicEnergyView(BioXASSideBeamline::bioXAS()->mono(), this);

    viewCrystalChangeButton_ = new QPushButton("Crystal Change", this);
    viewCrystalChangeButton_->setToolTip("Mono Crystal Change Instructions");
    connect( viewCrystalChangeButton_, SIGNAL(clicked()), this, SLOT(toViewMonoCrystalChangeInstructions()) );

    keithleyView_ = BioXASSideBeamline::bioXAS()->i0Keithley()->createView();
    keithleyView_->setParent(this);
    keithleyView_->setPrecision(2);
    keithleyView_->setFormat('e');

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(energyEditor_);
    layout->addWidget(viewCrystalChangeButton_);
    layout->addWidget(keithleyView_);
    layout->addStretch();

    setLayout(layout);
}

BioXASSidePersistentView::~BioXASSidePersistentView()
{

}

void BioXASSidePersistentView::toViewMonoCrystalChangeInstructions()
{    
    BioXASSideMonoCrystalChangeView *crystalChangeView = new BioXASSideMonoCrystalChangeView(BioXASSideBeamline::bioXAS()->mono());
    crystalChangeView->exec();
}
