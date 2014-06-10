/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef REIXSXESCALIBRATION_H
#define REIXSXESCALIBRATION_H

#include "dataman/database/AMDbObject.h"
#include "dataman/info/AMControlInfoList.h"
#include "acquaman/REIXS/REIXSXESScanConfiguration.h"
#include "acquaman.h"

#include <QVector3D>
#include <cmath>

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
	Q_PROPERTY(QVector3D hexapodOrigin READ hexapodOrigin WRITE setHexapodOrigin)
	Q_PROPERTY(QVector3D detectorPos0 READ detectorPos0 WRITE setDetectorPos0)
	Q_PROPERTY(QVector3D slitPos READ slitPos WRITE setSlitPos)
	Q_PROPERTY(AMDoubleList opticalOriginsX READ opticalOriginsX WRITE setOpticalOriginsX)
	Q_PROPERTY(AMDoubleList opticalOriginsY READ opticalOriginsY WRITE setOpticalOriginsY)
	Q_PROPERTY(AMDoubleList opticalOriginsZ READ opticalOriginsZ WRITE setOpticalOriginsZ)
	Q_PROPERTY(double spectrometerTheta0 READ spectrometerTheta0 WRITE setSpectrometerTheta0)
	Q_PROPERTY(double spectrometerTheta0m READ spectrometerTheta0m WRITE setSpectrometerTheta0m)
	Q_PROPERTY(double spectrometerRotDrive0 READ spectrometerRotDrive0 WRITE setSpectrometerRotDrive0)
	Q_PROPERTY(double frameA READ frameA WRITE setFrameA)
	Q_PROPERTY(double frameATheta READ frameATheta WRITE setFrameATheta)
	Q_PROPERTY(double frameB READ frameB WRITE setFrameB)
	Q_PROPERTY(double frameL READ frameL WRITE setFrameL)
	Q_PROPERTY(double frameH0 READ frameH0 WRITE setFrameH0)
	Q_PROPERTY(double tiltA READ tiltA WRITE setTiltA)
	Q_PROPERTY(double tiltB READ tiltB WRITE setTiltB)
	Q_PROPERTY(double tiltHomePos READ tiltHomePos WRITE setTiltHomePos)
	Q_PROPERTY(double detectorTranslation0 READ detectorTranslation0 WRITE setDetectorTranslation0)
	Q_PROPERTY(AMDoubleList gratingEVRangesMin READ gratingEVRangesMin WRITE setGratingEVRangesMin)
	Q_PROPERTY(AMDoubleList gratingEVRangesMax READ gratingEVRangesMax WRITE setGratingEVRangesMax)
	Q_PROPERTY(double detectorHeightError READ detectorHeightError WRITE setDetectorHeightError)



public:
 	virtual ~REIXSXESCalibration();
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
	QVector3D hexapodOrigin() const { return hexapodOrigin_; } ///< 3D Point. The center of the top plate of the hexapod
	QVector3D detectorPos0() const { return detectorPos0_; } ///< 3D Point. The position of the detector, when at home position.
	QVector3D slitPos() const { return slitPos_; } ///< 3D point. Center of the entrance slit

	AMDoubleList opticalOriginsX() const { return opticalOriginsX_; } ///< X coordinate of the optical origin (ie: where the grating top center should be placed.) Can have a different value for each grating. Nominally this should be 0 (the origin of the working coordinates and center of mechanical rotation), but we can place it elsewhere if required.  If it's too far away, light may be blocked from reaching or leaving the grating.
	AMDoubleList opticalOriginsY() const { return opticalOriginsY_; } ///< Y coordinate of the optical origin (ie: where the grating top center should be placed.)  Can have a different value for each grating.
	AMDoubleList opticalOriginsZ() const { return opticalOriginsZ_; } ///< Z coordinate of the optical origin (ie: where the grating top center should be placed.)  Can have a different value for each grating.

	double spectrometerTheta0() const { return spectrometerTheta0_; } ///< The angle (deg) the detector ray makes above the positive y-axis, when at the spectrometer rotation motor's calibration position.  This is still in working coordinates (ie: oriented to hexapod, not frame rails)
	double spectrometerTheta0m() const { return spectrometerTheta0m_; } ///< The angle (deg) the inclineable frame rail makes about the stationary frame rail, when at the spectrometer motor's calibration position. This makes the transition to "frame rail relative" coordinates.
	double spectrometerRotDrive0() const { return spectrometerRotDrive0_; } 	///< The linear position offset (mm) of the spectrometer rotation motor when at the RESTING position

	double frameA() const { return frameA_; }	///< Rotation lift frame: distance (mm) from rotation axis to endpoint of virtual arm A [the long arm]
	double frameATheta() const { return frameATheta_; } ///< Rotation lift frame: included angle (deg) inside the frame from virtual arm A down to the physical arm
	double frameB() const { return frameB_; }	///< Rotation lift frame: length (mm) of arm B (pin to pin) [the short arm]
	double frameL() const { return frameL_; } ///< Horizontal length (mm) from the origin (aka: rotation axis) to the ball screw axis
	double frameH0() const { return frameH0_; } ///< Height offset (mm) from the origin (aka: rotation axis) to the ballscrew joint when in the RESTING poition

	double tiltA() const { return tiltA_; } ///< Distance (mm) between pushrod arm attachment point and fixed pivot point.
	double tiltB() const { return tiltB_; } ///< Length (mm) of pushrod arm
	double tiltHomePos() const{ return tiltHomePos_; } ///< Measurement (mm) of the home position of the tilt stage, homed toward the motor (Length from outside wall at motor end to far opposite edge of platform)

	double detectorTranslation0() const { return detectorTranslation0_; } ///< The position of the detector translation motor at home (mm)

	AMDoubleList gratingEVRangesMin() const { return gratingEVRangesMin_; } ///< min EV reachable by grating; (imposed safety limits)
	AMDoubleList gratingEVRangesMax() const { return gratingEVRangesMax_; } ///< the max EV reachable by grating; (imposed safety limits)

	double detectorHeightError() const { return detectorHeightError_; } ///< The center of the detector should be on a line parallel to the translation rails, intersecting the rotation axle. This error is positive if it's above that ideal mounting height, and negative if below. The height is measured perpendicular to the frame rails -- not the y-axis.

public slots:

	void setGratingNames(const QStringList& gratingNames) { gratingNames_ = gratingNames; setModified(true); }
	void setGratingGrooveDensities(const AMDoubleList& gratingGrooveDensities) { gratingGrooveDensities_ = gratingGrooveDensities;setModified(true);  }
	void setGratingRadii(const AMDoubleList& gratingRadii) { gratingRadii_ = gratingRadii;setModified(true);  }
	void setGratingCenterPositionsX(const AMDoubleList& gratingCenterPositionsX) { gratingCenterPositionsX_ = gratingCenterPositionsX;setModified(true);  }
	void setGratingCenterPositionsY(const AMDoubleList& gratingCenterPositionsY) { gratingCenterPositionsY_ = gratingCenterPositionsY;setModified(true);  }
	void setGratingCenterPositionsZ(const AMDoubleList& gratingCenterPositionsZ) { gratingCenterPositionsZ_ = gratingCenterPositionsZ;setModified(true);  }
	void setGratingMountTilt(const AMDoubleList& gratingMountTilt) { gratingMountTilt_ = gratingMountTilt; setModified(true); }
	void setGratingMountRoll(const AMDoubleList& gratingMountRoll) { gratingMountRoll_ = gratingMountRoll; setModified(true); }
	void setHexapodOrigin(const QVector3D& hexapodOrigin) { hexapodOrigin_ = hexapodOrigin; setModified(true); }
	void setDetectorPos0(const QVector3D& detectorPos0) { detectorPos0_ = detectorPos0; setModified(true); }
	void setSlitPos(const QVector3D& slitPos) { slitPos_ = slitPos; setModified(true); }
	void setOpticalOriginsX(const AMDoubleList& opticalOriginsX) { opticalOriginsX_ = opticalOriginsX; setModified(true); }
	void setOpticalOriginsY(const AMDoubleList& opticalOriginsY) { opticalOriginsY_ = opticalOriginsY; setModified(true); }
	void setOpticalOriginsZ(const AMDoubleList& opticalOriginsZ) { opticalOriginsZ_ = opticalOriginsZ; setModified(true); }
	void setSpectrometerTheta0(double spectrometerTheta0) { spectrometerTheta0_ = spectrometerTheta0; setModified(true); }
	void setSpectrometerTheta0m(double spectrometerTheta0m) { spectrometerTheta0m_ = spectrometerTheta0m; setModified(true); }

	void setFrameA(double frameA) { frameA_ = frameA; setModified(true); }
	void setFrameATheta(double frameATheta) { frameATheta_ = frameATheta; setModified(true); }
	void setFrameB(double frameB) { frameB_ = frameB; setModified(true); }
	void setFrameL(double frameL) { frameL_ = frameL; setModified(true); }
	void setFrameH0(double frameH0) { frameH0_ = frameH0; setModified(true); }

	void setTiltHomePos(double tiltHomePos) { tiltHomePos_ = tiltHomePos; setModified(true); }
	void setTiltA(double tiltA) { tiltA_ = tiltA; }
	void setTiltB(double tiltB) { tiltB_ = tiltB; }

	void setSpectrometerRotDrive0(double spectrometerRotDrive0) { spectrometerRotDrive0_ = spectrometerRotDrive0; setModified(true); }
	void setDetectorTranslation0(double detectorTranslation0) { detectorTranslation0_ = detectorTranslation0; setModified(true); }
	void setDetectorHeightError(double heightError) { detectorHeightError_ = heightError; setModified(true); }



	void setGratingEVRangesMin(const AMDoubleList& gratingEVRangesMin) { gratingEVRangesMin_ = gratingEVRangesMin; setModified(true); } ///< set min EV reachable by grating; (imposed safety limits)
	void setGratingEVRangesMax(const AMDoubleList& gratingEVRangesMax) { gratingEVRangesMax_ = gratingEVRangesMax; setModified(true); } ///< set the max EV reachable by grating; (imposed safety limits)


	//////////////////////

//	AMControlInfoList computeSpectrometerPosition(REIXSXESScanConfiguration* scanConfiguration) const;
	AMControlInfoList computeSpectrometerPosition(int gratingIndex, double eV, double focusOffsetMm, double tiltOffsetDeg) const;
	double computeEVFromSpectrometerPosition(int gratingIndex, double spectrometerRotationDriveMm, double detectorTranslationMM) const;

	QPair<double,double> evRangeForGrating(int gratingNumber) const {
		return QPair<double,double>(gratingEVRangesMin_.at(gratingNumber), gratingEVRangesMax_.at(gratingNumber));
	}


	/////////////////////////
	// Math helper functions - can be used publicly
	////////////////////////

	/// Returns the optical origin (X,Y,Z) for a given grating. \c gratingIndex must be valid.)
	QVector3D opticalOrigin(int gratingIndex) const { return QVector3D(opticalOriginsX_.at(gratingIndex), opticalOriginsY_.at(gratingIndex), opticalOriginsZ_.at(gratingIndex)); }
	/// Returns the hexapodOrigin(), lowered by 26.433mm. (This offset due to the fact that hexapodOrigin() is the top surface of the plate, but the hexapod's internal rotation coord system is 26.443mm below that)).
	QVector3D hexapodRotationOrigin() const { return hexapodOrigin_ + QVector3D(0,0,-26.443); }

	/// Returns the "R,S,T" coordinates for the hexapod -- the center of rotation required to place a chosen grating (index \c gi) at the center of rotation. This is the grating location coordinate, mapped into hexapod coordinates.
	QVector3D hexapodRST(int gi) const {
		QVector3D gratingWC(gratingCenterPositionsX_.at(gi),
							gratingCenterPositionsY_.at(gi),
							gratingCenterPositionsZ_.at(gi));
		return gratingWC - hexapodRotationOrigin();
	}

	/// Returns the 'U' rotation (in degrees) for the hexapod -- the tilt to apply in the Y-Z plane, up from the Y axis (ie: rotation around the X axis). This comes from the required angle of the grating surface \c gratingAngle__ (rad), less the initial mounting angle.  The initial mounting angle is specific to the grating with index \c gi.
	double hexapodU(double gratingAngle, int gi) const {
		return r2d(gratingAngle) - gratingMountTilt_.at(gi);
	}

	double hexapodU(int gi) const { return hexapodU(gratingAngle(gi), gi); }


	/// Returns the hexapod translation motions (X,Y,Z) required to place a given grating (index \c gi) at the optical origin
	QVector3D hexapodXYZ(int gi) const {
		QVector3D gratingWC(gratingCenterPositionsX_.at(gi),
							gratingCenterPositionsY_.at(gi),
							gratingCenterPositionsZ_.at(gi));
		return opticalOrigin(gi) - gratingWC;
	}

	/// Converts degrees to radians.
	static double d2r(double degrees) { return degrees / 180. * M_PI; }
	/// Converts radians to degrees.
	static double r2d(double rad) { return rad * 180. / M_PI; }

	/// distance from slit to grating center, in mm.  ('r' in Rowlond circle nomenclature). Depends on the grating ( \c gratingIndex) because each can use a different optical origin.
	double r(int gratingIndex) const { return (slitPos_ - opticalOrigin(gratingIndex)).length(); }

	/// Alpha (in rad) calculated using rowland circle condition from r() and grating radius.  alpha = acos(r/R)
	double alpha(int gi) const { return acos( r(gi) / gratingRadii_.at(gi) ); }

	/// Beta (in rad) calculated using rowland circle condition from alpha() and \c energy (in eV).
	double beta(double eV, int gi) const { return asin(sin(alpha(gi))-0.0012398417*gratingGrooveDensities_.at(gi)/eV); }

	/// Angle (in rad) of line from slit to optical origin (sTheta), up from y axis.  The optical origin can depend on the grating.
	double sTheta(int gratingIndex) const {
		QVector3D v = opticalOrigin(gratingIndex) - slitPos_;
		return atan2(-v.z(), v.y());
	}

	/// Required angle (in rad) of the grating surface, up from y axis
	double gratingAngle(int gi) const { return M_PI/2.0 - sTheta(gi) - alpha(gi); }


	/// The length (mm) of the exit arm (r', in rowland circle nomenclature)
	double rPrime(double eV, int gi) const { return gratingRadii_.at(gi)*cos(beta(eV, gi)); }


	/// The angle (rad) of the center of the detector, up from the positive y-axis.  dTheta and rPrime provide polar coordinates for the center of the detector, around the optical origin.
	double dTheta(double eV, int gi) const { return -sTheta(gi) - alpha(gi) - beta(eV, gi) + M_PI; }

	/// The required position (mm) of the center of the detector for a given eV and grating, in working coordinates (x,y,z)
	QVector3D detectorPos(double eV, int gi) const {
		double rPrime__ = rPrime(eV, gi);
		double dTheta__ = dTheta(eV, gi);
		return opticalOrigin(gi) + QVector3D(0,
										  rPrime__ * cos(dTheta__),
										  rPrime__ * sin(dTheta__));
	}


	/// The rotation angle (rad) required to place the detector at detectorPos() \c d in working coordinates. This is a mathematically-defined angle up from the y-axis in the working coordinate system... which is useful in a lot of calculations.  Note that it is NOT relative to the resting position of the detector (yet).
	double spectrometerTheta(const QVector3D& d) const {
		double herr = detectorHeightError_;
		double dy2 = d.y()*d.y();
		double dz2 = d.z()*d.z();

		return acos(
					(-d.z()*herr+sqrt(dy2*(dy2+dz2-herr*herr)))
					/
					(dy2 + dz2)
					 );
	}

	/// The detector translation (mm) away from its initial position, to place the detector at detectorPos() in working coordinates. Here we simplify by providing the associated angle \c spectrometerTheta, asociated with that position via spectrometerTheta().
	double detectorTranslation(const QVector3D& d, double spectrometerTheta, double focusOffsetMm = 0) const {
		return (d.y() - detectorHeightError_*sin(spectrometerTheta)) / cos(spectrometerTheta) - detectorTranslation0_ + focusOffsetMm;
	}




	/// The linear motor position (mm) of the spectrometer rotation drive motor, for a desired spectrometerTheta().  This incorporates the linear offset to get to the calibration position.
	double spectrometerRotationDrive(double spectrometerTheta) const {
		// theta': angle above calibration position = theta - theta_0 [either aligned to working coordianates, or to frame rails. Doesn't matter, since it's a difference between two angles.]
		// theta_m: angle of inclineable rails above horizontal frame rails = theta' + theta_0m

		double theta_m = spectrometerTheta - d2r(spectrometerTheta0_) + d2r(spectrometerTheta0m_);

		double liftHeight = -frameA_*sin(theta_m - d2r(frameATheta_)) + sqrt( frameB_*frameB_ - pow(frameL_ - frameA_ * cos(theta_m - d2r(frameATheta_)),2));

		return spectrometerRotDrive0_ + (frameH0_ - liftHeight);
	}

	/// Inverse of spectrometerRotationDrive().  Calculates the spectrometerTheta() [rad] for an actual motor position of the \c spectrometerRotationDrive [mm].
	double spectrometerTheta(double spectrometerRotationDrive) const;

	/// Calculates detector position [mm] from a given spectrometerTheta() [rad] and detectorTranslation() [mm] away from the initial position
	QVector3D detectorPosFromThetaAndTranslation(double spectrometerTheta, double detectorTranslation) const {
		double dtrans = detectorTranslation + detectorTranslation0_;

		QVector3D rv(0,0,0);

		rv.setY(detectorHeightError_*sin(spectrometerTheta) + dtrans*cos(spectrometerTheta));
		rv.setZ(-detectorHeightError_*cos(spectrometerTheta) + dtrans*sin(spectrometerTheta));

		return rv;
	}

	/// Inverse of detectorPos()... Calculates the polar angle dTheta [rad] for a given detectorPos() [mm].  Requires the grating \c gratingIndex to know which optical origin was used.
	double dThetaFromDetectorPos(const QVector3D& pos, int gratingIndex) const {

		QVector3D rPrimeVector = pos - opticalOrigin(gratingIndex); // = r' cos(dtheta) j^ + r' sin(dtheta) k^

		return atan2(rPrimeVector.z(), rPrimeVector.y());
	}

	/// Inverse of beta()... Calculates the center energy [eV] at a polar angle \c dTheta [rad], assuming a grating \c gratingIndex
	double eVFromDTheta(double dTheta, int gratingIndex) const {
		double beta = -sTheta(gratingIndex) - alpha(gratingIndex) - dTheta + M_PI;

		return 0.0012398417*gratingGrooveDensities_.at(gratingIndex) / (sin(alpha(gratingIndex)) - sin(beta) );
	}


	// Detector Tilt
	////////////////////////

	/// Angle (rad) that the detector surface should make down to the y-axis (working coordinates) to make it tangent to the rowland circle
	double detectorPhi(double eV, int gi) const {
		return M_PI*3./2. - alpha(gi) - sTheta(gi) - 2*beta(eV, gi);
	}

	/// Motor position (mm) of the tilt stage, incorporating an angle offset (deg., positive = more normal, negative = more grazing) from the default position of tangent to the rowland circle.
	double tiltStageDrive(double eV, int gi, double spectrometerTheta, double tiltOffset = 0) const {
		double detectorPhiPrime = detectorPhi(eV, gi) - spectrometerTheta;	// angle we need on top of our orientation we already get along spectrometerTheta().  If there was no detector mounting height error, and the optical origin was at the center of mechanical rotation, this would be equivalent to the grazing incidence angle onto the detector, ie: 90deg. - beta.

		detectorPhiPrime += d2r(tiltOffset);

		// Previously: using analytical formula using linkage lengths
//		double h = tiltA_ * sin(detectorPhiPrime) - tiltB_ + sqrt(tiltB_*tiltB_ - tiltA_*tiltA_*pow(1-cos(detectorPhiPrime), 2));

//		return h + tiltHomePos_ - 89.411;

		// using empirical formula from survey data.
		return 1.1928*r2d(detectorPhiPrime) - 19.15;
	}







	QString gratingName(int gratingIndex) const {

		if(gratingIndex < 0 || gratingIndex >= gratingCount())
			return "No grating";

		return gratingNames_.at(gratingIndex);
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

	QVector3D hexapodOrigin_;	///< 3D Point. The center of the top plate of the hexapod, lowered by 26.433mm (This offset due to the fact that the hexapod's internal rotation coord system is 26.433mm below the top surface of the plate)

	QVector3D detectorPos0_;	///< 3D Point. Important: the height (y coord) and distance (z coord) of the detector center, when at home position.

	QVector3D slitPos_;	///< 3D point. Center of the entrance slit
	AMDoubleList opticalOriginsX_; ///< X coordinate of the optical origin (ie: where the grating top center should be placed.)  Can have a different value for each grating.
	AMDoubleList opticalOriginsY_; ///< Y coordinate of the optical origin (ie: where the grating top center should be placed.)  Can have a different value for each grating.
	AMDoubleList opticalOriginsZ_; ///< Z coordinate of the optical origin (ie: where the grating top center should be placed.)  Can have a different value for each grating.


	double spectrometerTheta0_;	///< The angle (deg) the detector ray makes above the positive y-axis, when at the spectrometer rotation motor's calibration position. This is still in working coordinates (ie: oriented to hexapod, not frame rails)
	double spectrometerTheta0m_;///< The angle (deg) the inclineable frame rail makes about the stationary frame rail, when at the spectrometer motor's calibration position. This is now in "frame rail" coordinates.

	// Spectrometer rotation frame geometry
	double frameA_;	///< Rotation lift frame: distance (mm) from rotation axis to endpoint of virtual arm A [the long arm]
	double frameATheta_; ///< Rotation lift frame: included angle (deg) inside the frame from virtual arm A down to the physical arm
	double frameB_;	///< Rotation lift frame: length (mm) of arm B (pin to pin) [the short arm]
	double frameL_; ///< Horizontal length (mm) from the origin (aka: rotation axis) to the ball screw axis
	double frameH0_;///< Height offset (mm) from the origin (aka: rotation axis) to the ballscrew joint when in the RESTING poition

	double spectrometerRotDrive0_;	///< The linear position offset (mm) of the spectrometer rotation motor when at the RESTING position

	double detectorTranslation0_;	///< The position of the detector translation motor at home (mm)


	// Tilt stage frame geometry
	double tiltA_; ///< Distance (mm) between pushrod arm attachment point and fixed pivot point.
	double tiltB_; ///< Length (mm) of pushrod arm
	double tiltHomePos_; ///< Measurement (mm) of the home position of the tilt stage, homed toward the motor (Length from outside wall at motor end to far opposite edge of platform)


	AMDoubleList gratingEVRangesMin_;	///< min EV reachable by grating; (imposed safety limits)
	AMDoubleList gratingEVRangesMax_; ///< the max EV reachable by grating; (imposed safety limits)

	double detectorHeightError_; ///< The center of the detector should be on a line parallel to the translation rails, intersecting the rotation axle. This error is positive if it's above that ideal mounting height, and negative if below. The height is measured perpendicular to the frame rails -- not the y-axis.


};

#endif // REIXSXESCALIBRATION_H
