/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef VESPERSIONCHAMBERCALIBRATIONVIEW_H
#define VESPERSIONCHAMBERCALIBRATIONVIEW_H

#include <QWidget>

#include "beamline/VESPERS/VESPERSIonChamberCalibration.h"

/*!
  This class is very VESPERS specific.  It takes a VESPERSIonChamberCalibration as an argument, puts every ion chamber inside its own view and adds some titles.  Essentially acting like a manager view.
  */
class VESPERSIonChamberCalibrationView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.  Takes in a VESPERSIonChamberCalibration \param calibration.
	explicit VESPERSIonChamberCalibrationView(VESPERSIonChamberCalibration *calibration, QWidget *parent = 0);

signals:

public slots:

protected:
	/// Pointer to the calibration.
	VESPERSIonChamberCalibration *calibration_;

};

#endif // VESPERSIONCHAMBERCALIBRATIONVIEW_H
