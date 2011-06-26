#ifndef REIXSXESCALIBRATION_H
#define REIXSXESCALIBRATION_H

#include "dataman/AMDbObject.h"
#include "dataman/AMControlInfoList.h"
#include "acquaman/REIXS/REIXSXESScanConfiguration.h"
#include "acquaman.h"

class REIXSXESCalibration : public AMDbObject
{
	Q_OBJECT

	Q_PROPERTY(int gratingCount READ gratingCount)
	Q_PROPERTY(QStringList gratingNames READ gratingNames WRITE setGratingNames)
	Q_PROPERTY(AMDoubleList gratingGrooveDensities READ gratingGrooveDensities WRITE setGratingGrooveDensities)
	Q_PROPERTY(AMDoubleList gratingRadii READ gratingRadii WRITE setGratingRadii)
	Q_PROPERTY(AMDoubleList gratingCenterPositionsX READ gratingCenterPositionsX WRITE setGratingCenterPositionsX)
	Q_PROPERTY(AMDoubleList gratingCenterPositionsY READ gratingCenterPositionsY WRITE setGratingCenterPositionsY)
	Q_PROPERTY(AMDoubleList gratingCenterPositionsZ READ gratingCenterPositionsZ WRITE setGratingCenterPositionsZ)
	Q_PROPERTY(AMDoubleList gratingMountTilt READ gratingMountTilt WRITE setGratingMountTilt)
	Q_PROPERTY(AMDoubleList gratingMountRoll READ gratingMountRoll WRITE setGratingMountRoll)
	Q_PROPERTY(AMDoubleList hexapodOrigin READ hexapodOrigin WRITE setHexapodOrigin)
	Q_PROPERTY(AMDoubleList detectorPos0 READ detectorPos0 WRITE setDetectorPos0)
	Q_PROPERTY(AMDoubleList slitPos READ slitPos WRITE setSlitPos)
	Q_PROPERTY(AMDoubleList opticalOrigin READ opticalOrigin WRITE setOpticalOrigin)
	Q_PROPERTY(double spectrometerRotAngle0 READ spectrometerRotAngle0 WRITE setSpectrometerRotAngle0)
	Q_PROPERTY(double spectrometerRotAngleDrive0 READ spectrometerRotAngleDrive0 WRITE setSpectrometerRotAngleDrive0)
	Q_PROPERTY(double detectorTranslation0 READ detectorTranslation0 WRITE setDetectorTranslation0)
	Q_PROPERTY(AMDoubleList gratingEVRangesMin READ gratingEVRangesMin WRITE setGratingEVRangesMin)
	Q_PROPERTY(AMDoubleList gratingEVRangesMax READ gratingEVRangesMax WRITE setGratingEVRangesMax)



public:
	explicit REIXSXESCalibration(QObject *parent = 0);

	int gratingCount() const { return gratingNames_.count(); } ///< returns the number of gratings defined in this calibration
	QStringList gratingNames() const { return gratingNames_; } 	///< A list of the names of all the gratings defined in this calibration (ie: LEG, Impurity, MEG, HEG, etc.)
	AMDoubleList gratingGrooveDensities() const { return gratingGrooveDensities_; } ///< A list of the groove densities (in lines/mm) for each of the gratings
	AMDoubleList gratingRadii() const { return gratingRadii_; } ///< Radius of each grating, in mm
	AMDoubleList gratingCenterPositionsX() const { return gratingCenterPositionsX_; } ///< Where the gratings are (in working coordinates) when the hexapod is in its home position. (mm)
	AMDoubleList gratingCenterPositionsY() const { return gratingCenterPositionsY_; }
	AMDoubleList gratingCenterPositionsZ() const { return gratingCenterPositionsZ_; }
	AMDoubleList gratingMountTilt() const{ return gratingMountTilt_; } ///< The angle between the long axis of the grating and the y-axis (deg), when the gratings are in the hexapod home position. Positive if the angle is above the positive y-axis.
	AMDoubleList gratingMountRoll() const { return gratingMountRoll_; } ///< The angle between the short axis of the grating and the positive x-axis (deg), when the gratings are in the hexapod home position.
	AMDoubleList hexapodOrigin() const { return hexapodOrigin_; } ///< 3D Point. The center of the top plate of the hexapod, lowered by 26.433mm (This offset due to the fact that the hexapod's internal rotation coord system is 26.433mm below the top surface of the plate)
	AMDoubleList detectorPos0() const { return detectorPos0_; } ///< 3D Point. Important: the height (y coord) and distance (z coord) of the detector center, when at home position.
	AMDoubleList slitPos() const { return slitPos_; } ///< 3D point. Center of the entrance slit
	AMDoubleList opticalOrigin() const { return opticalOrigin_; } ///< 3D point. Optical origin (ie: where the top center of the grating in use should be placed)  This should have a y-z value of 0 (on the x axis / spectrometer rotation axis) and possibly a non-zero x value (to account for the hexapod being mounted with an offset)
	double spectrometerRotAngle0() const { return spectrometerRotAngle0_; } ///< The angle the detector ray makes above the positive y-axis, when at the spectrometer rotation motor's home (deg)
	double spectrometerRotAngleDrive0() const { return spectrometerRotAngleDrive0_; } 	///< The position of the spectrometer rotation motor at home (mm)
	double detectorTranslation0() const { return detectorTranslation0_; } ///< The position of the detector translation motor at home (mm)
	AMDoubleList gratingEVRangesMin() const { return gratingEVRangesMin_; } ///< min EV reachable by grating; (imposed safety limits)
	AMDoubleList gratingEVRangesMax() const { return gratingEVRangesMax_; } ///< the max EV reachable by grating; (imposed safety limits)


public slots:

	void setGratingNames(const QStringList& gratingNames) { gratingNames_ = gratingNames; } 	///< set list of the names of all the gratings defined in this calibration (ie: LEG, Impurity, MEG, HEG, etc.)
	void setGratingGrooveDensities(const AMDoubleList& gratingGrooveDensities) { gratingGrooveDensities_ = gratingGrooveDensities; } ///< set list of the groove densities (in lines/mm) for each of the gratings
	void setGratingRadii(const AMDoubleList& gratingRadii) { gratingRadii_ = gratingRadii; } ///< set Radius of each grating, in mm
	void setGratingCenterPositionsX(const AMDoubleList& gratingCenterPositionsX) { gratingCenterPositionsX_ = gratingCenterPositionsX; } ///< set Where the gratings are (in working coordinates) when the hexapod is in its home position. (mm)
	void setGratingCenterPositionsY(const AMDoubleList& gratingCenterPositionsY) { gratingCenterPositionsY_ = gratingCenterPositionsY; }
	void setGratingCenterPositionsZ(const AMDoubleList& gratingCenterPositionsZ) { gratingCenterPositionsZ_ = gratingCenterPositionsZ; }
	void setGratingMountTilt(const AMDoubleList& gratingMountTilt) { gratingMountTilt_ = gratingMountTilt; } ///< set The angle between the long axis of the grating and the y-axis (deg), when the gratings are in the hexapod home position. Positive if the angle is above the positive y-axis.
	void setGratingMountRoll(const AMDoubleList& gratingMountRoll) { gratingMountRoll_ = gratingMountRoll; } ///< set The angle between the short axis of the grating and the positive x-axis (deg), when the gratings are in the hexapod home position.
	void setHexapodOrigin(const AMDoubleList& hexapodOrigin) { hexapodOrigin_ = hexapodOrigin; } ///< 3D Point. set The center of the top plate of the hexapod, lowered by 26.433mm (This offset due to the fact that the hexapod's internal rotation coord system is 26.433mm below the top surface of the plate)
	void setDetectorPos0(const AMDoubleList& detectorPos0) { detectorPos0_ = detectorPos0; } ///< 3D Point. set the height (y coord) and distance (z coord) of the detector center, when at home position.
	void setSlitPos(const AMDoubleList& slitPos) { slitPos_ = slitPos; } ///< 3D point. set center of the entrance slit
	void setOpticalOrigin(const AMDoubleList& opticalOrigin) { opticalOrigin_ = opticalOrigin; } ///< 3D point. set Optical origin (ie: where the top center of the grating in use should be placed)  This should have a y-z value of 0 (on the x axis / spectrometer rotation axis) and possibly a non-zero x value (to account for the hexapod being mounted with an offset)
	void setSpectrometerRotAngle0(double spectrometerRotAngle0) { spectrometerRotAngle0_ = spectrometerRotAngle0; } ///< set The angle the detector ray makes above the positive y-axis, when at the spectrometer rotation motor's home (deg)
	void setSpectrometerRotAngleDrive0(double spectrometerRotAngleDrive0) { spectrometerRotAngleDrive0_ = spectrometerRotAngleDrive0; } 	///< set The position of the spectrometer rotation motor at home (mm)
	void setDetectorTranslation0(double detectorTranslation0) { detectorTranslation0_ = detectorTranslation0; } ///< set The position of the detector translation motor at home (mm)
	void setGratingEVRangesMin(const AMDoubleList& gratingEVRangesMin) { gratingEVRangesMin_ = gratingEVRangesMin; } ///< set min EV reachable by grating; (imposed safety limits)
	void setGratingEVRangesMax(const AMDoubleList& gratingEVRangesMax) { gratingEVRangesMax_ = gratingEVRangesMax; } ///< set the max EV reachable by grating; (imposed safety limits)


	AMControlInfoList computeSpectrometerPosition(REIXSXESScanConfiguration* scanConfiguration) const {
		return AMControlInfoList();
	}

	QPair<double,double> evRangeForGrating(int gratingNumber) const {
		return QPair<double,double>(gratingEVRangesMin_.at(gratingNumber), gratingEVRangesMax_.at(gratingNumber));
	}



signals:

public slots:

protected:

	/// all coordinates in our "working" coordinate system. This is defined by z: the vertical axis of the hexapod, x: the rotation axle of the spectrometer; y perp to x/y (downstream).  The origin is along the rotation axle, where the hexapod's y-z plane intersects it.  Note that this origin is not the "optical origin" where we want to place the gratings; if the hexapod mounting is off in the x-direction, there will be an x-offset to the optical origin.

	QStringList gratingNames_;	///< LEG, MEG, etc..
	AMDoubleList gratingGrooveDensities_; ///< lines/mm
	AMDoubleList gratingRadii_;	///< mm
	AMDoubleList gratingCenterPositionsX_, gratingCenterPositionsY_, gratingCenterPositionsZ_;	///< where the gratings are, when the hexapod is homed (mm)
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
