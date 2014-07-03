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
    motorControlEditor_ = new AMExtendedControlEditor(BioXASSideBeamline::bioXAS()->m1UpperSlit());
    motorControlEditor_->setControlFormat('g', 4);

    keithleyView_ = BioXASSideBeamline::bioXAS()->keithley()->createView();
    keithleyView_->setParent(this);
    keithleyView_->setPrecision(2);
    keithleyView_->setFormat('e');

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(motorControlEditor_);
    layout->addWidget(keithleyView_);
    layout->addStretch();

    setLayout(layout);
}
