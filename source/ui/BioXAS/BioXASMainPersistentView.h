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


#ifndef BIOXASMAINPERSISTENTVIEW_H
#define BIOXASMAINPERSISTENTVIEW_H

#include <QWidget>
#include <QLayout>
#include <QPushButton>

#include "beamline/BioXAS/BioXASMainBeamline.h"
#include "ui/beamline/AMExtendedControlEditor.h"
#include "ui/BioXAS/BioXASMainMonochromatorCrystalChangeView.h"

class AMExtendedControlEditor;

class BioXASMainPersistentView : public QWidget
{
    Q_OBJECT

public:
    /// Constructor.
    explicit BioXASMainPersistentView(QWidget *parent = 0);
    /// Destructor.
    virtual ~BioXASMainPersistentView();

signals:

public slots:

protected:
    /// Basic editor for selecting mono energy.
    AMExtendedControlEditor *energyControlEditor_;
    /// A view for the elements important to a crystal change.
    BioXASMainMonochromatorCrystalChangeView *crystalChangeView_;
};

#endif // BIOXASMAINPERSISTENTVIEW_H
