/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

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


#ifndef AMSAMPLEMANIPULATORVIEW_H
#define AMSAMPLEMANIPULATORVIEW_H

#include <QGroupBox>
#include <QGridLayout>
#include <QPushButton>
#include <QIcon>
#include "beamline/AMControlSet.h"
#include "AMControlEditor.h"

class AMSampleManipulatorView : public QGroupBox
{
Q_OBJECT
public:
	explicit AMSampleManipulatorView(QString title = "Manipulator", AMControlSet *manipulator = 0, QWidget *parent = 0);

	AMControlSet *manipulator();

signals:

public slots:

protected:
	AMControlSet *manipulator_;
};



#endif // AMSAMPLEMANIPULATORVIEW_H
