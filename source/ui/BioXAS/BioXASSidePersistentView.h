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


#ifndef BIOXASSIDEPERSISTENTVIEW_H
#define BIOXASSIDEPERSISTENTVIEW_H

#include <QWidget>
#include <QLayout>
#include <QPushButton>
#include <QInputDialog>

class AMExtendedControlEditor;

class BioXASSidePersistentView : public QWidget
{
    Q_OBJECT

public:
    /// Constructor.
    explicit BioXASSidePersistentView(QWidget *parent = 0);
    virtual ~BioXASSidePersistentView();

signals:

public slots:

protected slots:
	/// Presents a view for the user to calibrate the mono energy.
	void onCalibrateEnergyButtonClicked();

protected:
    AMExtendedControlEditor *energyControlEditor_;
    QPushButton *calibrateEnergyButton_;
};

#endif // BIOXASSIDEPERSISTENTVIEW_H
