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

BioXASMainPersistentView::BioXASMainPersistentView(QWidget *parent) :
    QWidget(parent)
{
    // create UI elements.

    energyControlEditor_ = new AMExtendedControlEditor(BioXASMainBeamline::bioXAS()->mono()->energyControl());
    energyControlEditor_->setControlFormat('f', 2);

    crystalChangeView_ = new BioXASMainMonochromatorCrystalChangeView(BioXASMainBeamline::bioXAS()->mono(), this);

    crystalChange_ = new QPushButton("Crystal Change", this);

    // create and set layouts.

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(crystalChange_);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(energyControlEditor_);
    layout->addWidget(crystalChangeView_);
    layout->addLayout(buttonLayout);
    layout->addStretch();

    setLayout(layout);

    // make connections.

    connect( crystalChange_, SIGNAL(clicked()), this, SLOT(onCrystalChangeButtonClicked()) );
}

BioXASMainPersistentView::~BioXASMainPersistentView()
{

}

void BioXASMainPersistentView::onCrystalChangeButtonClicked()
{
    BioXASMainMonochromatorCrystalChangeControlView *crystalChangeView = new BioXASMainMonochromatorCrystalChangeControlView(BioXASMainBeamline::bioXAS()->mono()->crystalChangeControl(), this);
    crystalChangeView->setWindowFlags(Qt::Sheet);
    crystalChangeView->show();
}
