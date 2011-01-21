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


#ifndef SGMBEAMLINEATAGLANCEBAR_H
#define SGMBEAMLINEATAGLANCEBAR_H

#include <QFrame>
#include <QHBoxLayout>

#include "NumericControl.h"
#include "beamline/SGMBeamline.h"

class SGMBeamlineAtAGlanceBar : public QFrame
{
Q_OBJECT
public:
	explicit SGMBeamlineAtAGlanceBar(QWidget *parent = 0);

signals:

public slots:

protected:
	QHBoxLayout *hl_;

	AMControlEdit *energyNC_;
	AMControlEdit *gratingNC_;
	AMControlEdit *readyLabel_;
	AMControlEdit *moveStatusLabel_;

};

#endif // SGMBEAMLINEATAGLANCEBAR_H
