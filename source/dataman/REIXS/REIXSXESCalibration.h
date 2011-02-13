#ifndef REIXSXESCALIBRATION_H
#define REIXSXESCALIBRATION_H

#include "dataman/AMDbObject.h"
#include "dataman/AMControlInfoList.h"
#include "acquaman/REIXS/REIXSXESScanConfiguration.h"
#include "acquaman.h"

class REIXSXESCalibration : public AMDbObject
{
Q_OBJECT

	/*
Q_PROPERTY(QStringList gratingNames READ gratingNames WRITE setGratingNames);
Q_PROPERTY(AMDoubleList gratingGrooveDensities READ gratingGrooveDensities WRITE setGratingGrooveDensities);
Q_PROPERTY(AMDoubleList gratingRadii READ gratingRadii WRITE setGratingRadii);
Q_PROPERTY(AMDoubleList gratingCenterPositions READ gratingCenterPositions WRITE setGratingCenterPositions);
Q_PROPERTY(AMDoubleList gratingMountTilt READ gratingMountTilt WRITE setGratingMountTilt);
Q_PROPERTY(AMDoubleList gratingMountRoll READ gratingMountRoll WRITE setGratingMountRoll);
Q_PROPERTY(AMDoubleList hexapodOrigin READ hexapodOrigin WRITE setHexapodOrigin);
Q_PROPERTY(AMDoubleList detectorPos0 READ detectorPos0 WRITE setDetectorPos0);
Q_PROPERTY(AMDoubleList slitPos READ slitPos WRITE setslitPos);
Q_PROPERTY(AMDoubleList opticalOrigin READ opticalOrigin WRITE setOpticalOrigin);
Q_PROPERTY(double spectrometerRotAngle0 READ spectrometerRotAngle0 WRITE setSpectrometerRotAngle0);
Q_PROPERTY(double spectrometerRotAngleDrive0 READ spectrometerRotAngleDrive0 WRITE setSpectrometerRotAngleDrive0);
Q_PROPERTY(double detectorTranslation0 READ detectorTranslation0 WRITE setDetectorTranslation0);
Q_PROPERTY(AMDoubleList gratingEVRangesMin READ gratingEVRangesMin WRITE setGratingEVRangesMin);
Q_PROPERTY(AMDoubleList gratingEVRangesMax READ gratingEVRangesMax WRITE setGratingEVRangesMax);
*/

public:
	explicit REIXSXESCalibration(QObject *parent = 0);

	AMControlInfoList computeSpectrometerPosition(REIXSXESScanConfiguration* scanConfiguration) const {
		return AMControlInfoList();
	}

	QPair<double,double> evRangeForGrating(int gratingNumber) const {
		return QPair<double,double>(10, 1400);
	}

	QStringList gratingNames() const { return QStringList(); }

signals:

public slots:

protected:

	/// all coordinates in our "working" coordinate system. This is defined by z: the vertical axis of the hexapod, x: the rotation axle of the spectrometer; y perp to x/y (downstream).  The origin is along the rotation axle, where the hexapod's y-z plane intersects it.  Note that this origin is not the "optical origin" where we want to place the gratings; if the hexapod mounting is off, there will be an x-offset to the optical origin.

	QStringList gratingNames_;	///< LEG, MEG, etc..
	AMDoubleList gratingGrooveDensities_; ///< lines/mm
	AMDoubleList gratingRadii_;	///< mm
	AMDoubleList gratingCenterPositions_;	///< where the gratings are, when the hexapod is homed (mm)
	AMDoubleList gratingMountTilt_; ///< The angle between the long axis of the grating and the y-axis (deg), when the gratings are in the hexapod home position. Positive if the angle is above the positive y-axis.
	AMDoubleList gratingMountRoll_; ///< The angle between the short axis of the grating and the positive x-axis (deg), when the gratings are in the hexapod home position.

	AMDoubleList hexapodOrigin_;	///< 3D Point. The center of the top plate of the hexapod, lowered by 26.433mm (This offset due to the fact that the hexapod's internal rotation coord system is 26.433mm below the top surface of the plate)

	AMDoubleList detectorPos0_;	///< 3D Point. Important: the height (y coord) and distance (z coord) of the detector center, when at home position.

	AMDoubleList slitPos_;	///< 3D point. Center of the entrance slit
	AMDoubleList opticalOrigin_; ///< 3D point. Optical origin (ie: where the top center of the grating in use should be placed)  This should have a y-z value of 0 (on the x axis / spectrometer rotation axis) and possibly a non-zero x value (to account for the hexapod being mounted with an offset)


	double spectrometerRotAngle0_;	///< The angle the detector ray makes above the positive y-axis, when at the spectrometer rotation motor's home (deg)
	double spectrometerRotAngleDrive0_;	///< The position of the spectrometer rotation motor at home (mm)

	double detectorTranslation0_;	///< The position of the detector translation motor at home (mm)

	AMDoubleList gratingEVRangesMin_;	///< min EV reachable by grating; (imposed safety limits)
	AMDoubleList gratingEVRangesMax_; ///< the max EV reachable by grating; (imposed safety limits)






};

#endif // REIXSXESCALIBRATION_H
