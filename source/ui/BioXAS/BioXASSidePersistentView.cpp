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
    energyControlEditor_->setMaximumWidth(200);
    energyControlEditor_->setControlFormat('f', 2);

    viewCrystalChangeButton_ = new QPushButton("Crystal Change", this);
    viewCrystalChangeButton_->setMaximumWidth(200);
    viewCrystalChangeButton_->setToolTip("Mono Crystal Change Instructions");

    scalerView_ = new BioXASSideScalerView(BioXASSideBeamline::bioXAS()->scaler(), this);

    QGroupBox *monoPanel = new QGroupBox("Monochromator");

    QGroupBox *scalerPanel = new QGroupBox("Preamp Settings");

    // Create and set layouts.

    QVBoxLayout *monoPanelLayout = new QVBoxLayout();
    monoPanelLayout->addWidget(energyControlEditor_);
    monoPanelLayout->addWidget(viewCrystalChangeButton_);
    monoPanelLayout->addWidget(scalerView_);

    monoPanel->setLayout(monoPanelLayout);

    QVBoxLayout *scalerPanelLayout = new QVBoxLayout();
    scalerPanelLayout->addWidget(scalerView_);

    scalerPanel->setLayout(scalerPanelLayout);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(monoPanel);
    layout->addWidget(scalerPanel);
    layout->addStretch();

    setLayout(layout);

    setMaximumWidth(400);
    setMinimumWidth(400);

    // Make connections.

    connect( viewCrystalChangeButton_, SIGNAL(clicked()), this, SLOT(toViewMonoCrystalChangeInstructions()) );
}

BioXASSidePersistentView::~BioXASSidePersistentView()
{

}

void BioXASSidePersistentView::toViewMonoCrystalChangeInstructions()
{    
//    BioXASSideMonoCrystalChangeView *crystalChangeView = new BioXASSideMonoCrystalChangeView(BioXASSideBeamline::bioXAS()->mono());
//    crystalChangeView->exec();
}
