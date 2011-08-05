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
