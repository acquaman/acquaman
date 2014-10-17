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


#ifndef REIXSXESCALIBRATION2_H
#define REIXSXESCALIBRATION2_H

#include "dataman/database/AMDbObject.h"
#include <QVector3D>
#include <cmath>
#include "dataman/info/AMControlInfoList.h"

class REIXSXESScanConfiguration;

class REIXSXESGratingInfo : public AMDbObject {
	Q_OBJECT

	Q_PROPERTY(double grooveDensity READ grooveDensity WRITE setGrooveDensity)
	Q_PROPERTY(double radius READ radius WRITE setRadius)
	Q_PROPERTY(double alpha READ alpha WRITE setAlpha)
	Q_PROPERTY(double r READ r WRITE setR)
	Q_PROPERTY(QVector3D centerPosition READ centerPosition WRITE setCenterPosition)
	Q_PROPERTY(QVector3D opticalOrigin READ opticalOrigin WRITE setOpticalOrigin)
	Q_PROPERTY(QVector3D hexapodUVW READ hexapodUVW WRITE setHexapodUVW)
	Q_PROPERTY(double angleCorrection READ angleCorrection WRITE setAngleCorrection)
	Q_PROPERTY(double heightCorrection READ heightCorrection WRITE setHeightCorrection)
	Q_PROPERTY(double endstationTranslation READ endstationTranslation WRITE setEndstationTranslation)
	Q_PROPERTY(double evRangeMin READ evRangeMin WRITE setEvRangeMin)
	Q_PROPERTY(double evRangeMax READ evRangeMax WRITE setEvRangeMax)

public:
 	virtual ~REIXSXESGratingInfo();
	explicit REIXSXESGratingInfo(QObject *parent = 0);

	REIXSXESGratingInfo(const QString& name, double grooveDensity, double radius, double alpha, double r, const QVector3D& centerPosition, const QVector3D& opticalOrigin, const QVector3D& targetUVW, double angleCorrection, double heightCorrection, double endstationTranslation, double evRangeMin, double evRangeMax, QObject* parent = 0) : AMDbObject(parent) {

		grooveDensity_ = grooveDensity;
		radius_ = radius;
		alpha_ = alpha;
		r_ = r;
		centerPosition_ = centerPosition;
		opticalOrigin_ = opticalOrigin;
		hexapodUVW_ = targetUVW;
		angleCorrection_ = angleCorrection;
		heightCorrection_ = heightCorrection;
		endstationTranslation_ = endstationTranslation;
		evRangeMin_ = evRangeMin;
		evRangeMax_ = evRangeMax;

		setName(name);
	}

	// name: already provided in AMDbObject

	/// groove density, in lines/mm
	double grooveDensity() const { return grooveDensity_; }
	/// grating radius, in mm
	double radius() const { return radius_; }
	/// Nominal [design] alpha (incident angle), in deg
	double alpha() const { return alpha_; }
	/// Nominal [design] r (entrance arm distance), in deg
	double r() const { return r_; }

	/// Position of grating top center, when the hexapod is in its home position, in working coordinates. [x,y,z]
	QVector3D centerPosition() const { return centerPosition_; }
	/// Position of optical origin used for this grating, in working coordinates [ideally 0,0,0, unless adjustements are required]
	QVector3D opticalOrigin() const { return opticalOrigin_; }
	/// Optimized u angle, and target v,w angles determined from measurements. This is the [u,v,w] degrees that the hexapod should be set to to use this grating
	QVector3D hexapodUVW() const { return hexapodUVW_; }

	/// An angle correction that takes into account all constant angle offsets [grating tilt, rail initial angle, etc.]
	double angleCorrection() const { return angleCorrection_; }
	/// A correction that takes into account the mounting height error of the detector, and any other deterctor-distance-dependent angle offset
	double heightCorrection() const { return heightCorrection_; }

	/// position of the ideal endstation translaton for this grating, used to optimize the entrance distance
	double endstationTranslation() const { return endstationTranslation_; }

	/// Minimum allowed eV, accessible for this grating, in 1st order
	double evRangeMin() const { return evRangeMin_; }
	/// Maximum allowed eV, accessible for this grating, in 1st order
	double evRangeMax() const { return evRangeMax_; }


public slots:


	/// set groove density, in lines/mm
	void setGrooveDensity(double grooveDensity) { grooveDensity_ = grooveDensity; setModified(true); }
	/// set grating radius, in mm
	void setRadius(double radius) { radius_ = radius; setModified(true); }
	/// set Nominal [design] alpha (incident angle), in deg
	void setAlpha(double alpha) { alpha_ = alpha; setModified(true); }
	/// set Nominal [design] r (entrance arm distance), in deg
	void setR(double r) { r_ = r; setModified(true); }

	/// set Position of top center, when the hexapod is in its home position, in working coordinates. [x,y,z]
	void setCenterPosition(const QVector3D& centerPosition) { centerPosition_ = centerPosition; setModified(true); }
	/// set Position of optical origin in working coordinates [ideally 0,0,0, unless adjustements are required]
	void setOpticalOrigin(const QVector3D& opticalOrigin) { opticalOrigin_ = opticalOrigin; setModified(true); }
	/// set Optimized u angle, and target v,w angles determined from measurements. This is the [u,v,w] degrees that the hexapod should be set to to use this grating
	void setHexapodUVW(const QVector3D& uvw) { hexapodUVW_ = uvw; setModified(true); }

	/// set An angle correction that takes into account all constant angle offsets [grating tilt, rail initial angle, etc.]
	void setAngleCorrection(double angleCorrection) { angleCorrection_ = angleCorrection; setModified(true); }
	/// set A correction that takes into account the mounting height error of the detector, and any other deterctor-distance-dependent angle offset
	void setHeightCorrection(double heightCorrection) { heightCorrection_ = heightCorrection; setModified(true); }

	/// set position of the ideal endstation translaton for this grating, used to optimize the entrance distance
	void setEndstationTranslation(double spectrometerTranslation) { endstationTranslation_ = spectrometerTranslation; setModified(true); }

	/// set Minimum allowed eV, accessible for this grating, in 1st order
	void setEvRangeMin(double evRangeMin) { evRangeMin_ = evRangeMin; setModified(true); }
	/// set Maximum allowed eV, accessible for this grating, in 1st order
	void setEvRangeMax(double evRangeMax) { evRangeMax_ = evRangeMax; setModified(true); }


protected:
	double grooveDensity_, radius_, angleCorrection_, heightCorrection_, alpha_, r_, endstationTranslation_, evRangeMin_, evRangeMax_;
	QVector3D centerPosition_, opticalOrigin_, hexapodUVW_;

};



class REIXSXESCalibration2 : public AMDbObject
{
    Q_OBJECT
	Q_PROPERTY(AMDbObjectList gratings READ dbReadGratings WRITE dbLoadGratings)
	Q_PROPERTY(QVector3D hexapodOrigin READ hexapodOrigin WRITE setHexapodOrigin)
	Q_PROPERTY(double detectorWidth READ detectorWidth WRITE setDetectorWidth)

public:
	/// Default constructor
 	virtual ~REIXSXESCalibration2();
    explicit REIXSXESCalibration2(QObject *parent = 0);

	/// returns the number of gratings defined in this calibration
	int gratingCount() const { return gratings_.count(); }
	/// Access the details of a grating
	const REIXSXESGratingInfo& gratingAt(int i) const { return gratings_.at(i); }
	/// Modify the details of a grating
	REIXSXESGratingInfo& gratingAt(int i) { return gratings_[i]; }


	/// Returns the width of the MCP detector (along the energy axis) [mm]
	double detectorWidth() const { return detectorWidth_; }
	/// Set the width of the MCP detector along the energy axis [mm]
	void setDetectorWidth(double widthMm) { detectorWidth_ = widthMm; setModified(true); }


	// Positioning results and calculations
	//////////////////////////////////////////

	/// Returns the center of the top plate of the hexapod
	QVector3D hexapodOrigin() const { return hexapodOrigin_; }

	/// Returns the hexapod translation motions (X,Y,Z) required to place a given grating (index \c gi) at the optical origin
	QVector3D hexapodXYZ(int gi) const {
		return gratings_.at(gi).opticalOrigin() - gratings_.at(gi).centerPosition();
	}

	/// Returns the hexapod angles U,V,W for a given grating \c gi
	QVector3D hexapodUVW(int gi) const {
		return gratings_.at(gi).hexapodUVW();
	}

	/// Returns the R,S,T hexapod target for grating \c gi
	/*! Returns the "R,S,T" coordinates for the hexapod -- the center of rotation required to place a chosen grating (index \c gi) at the center of rotation. This is the grating center position, mapped into hexapod coordinates. (The offset is due to the fact that hexapodOrigin() is the top surface of the plate, but the hexapod's internal rotation coord system is 26.443mm below that).*/
	QVector3D hexapodRST(int gi) const {
		QVector3D hexapodRotationOrigin = hexapodOrigin_ + QVector3D(0,0,-26.443);

		return gratings_.at(gi).centerPosition() - hexapodRotationOrigin;
	}


	/// Converts degrees to radians.
	static double d2r(double degrees) { return degrees / 180. * M_PI; }
	/// Converts radians to degrees.
	static double r2d(double rad) { return rad * 180. / M_PI; }

	/// Returns the target beta [degrees] for a given energy \c eV, using the diffraction grating equation, assuming grating \c gi and diffraction order \c order.
	/*! Specify \c order as positive integer, even though they're all inside orders.*/
	double beta(int gi, double eV, int order = 1) const {
		const REIXSXESGratingInfo& g = gratings_.at(gi);
		return r2d(asin(sin(d2r(g.alpha())) - 0.0012398417/eV*g.grooveDensity()*order));
	}
	/// Returns the energy [eV] for a given \c beta [degrees] and \c order, assuming grating \c gi. Inverse function of beta().
	double eV(int gi, double beta, int order = 1) const {
		const REIXSXESGratingInfo& g = gratings_.at(gi);
		return g.grooveDensity()*order*0.0012398417/(sin(d2r(g.alpha())) - sin(d2r(beta)));
	}

	/// Returns the rowland-circle exit arm length for a particular \c beta [degrees] and grating \c gi.
	double rPrime(int gi, double beta) const {
		return gratings_.at(gi).radius() * cos(d2r(beta));
	}

	/// Returns the "angle" [degrees] of the spectrometer for a given \c beta [degrees].  The angle is measured up from some arbitrary point, and found by applying the angleCorrection() and heightCorrection() of the spectrometer for the grating.
	double spectrometerAngle(int gi, double beta, double rPrime) const {
		const REIXSXESGratingInfo& g = gratings_.at(gi);
		return 90. - beta + g.angleCorrection() + r2d(atan(g.heightCorrection()/rPrime));
	}
	/// Returns \c beta [mm] corresponding to a spectrometer position of \c spectrometerAngle [degrees], \c rPrime [mm]. Inverse function of spectrometerAngle().
	double betaFromPosition(int gi, double spectrometerAngle, double rPrime) const {
		const REIXSXESGratingInfo& g = gratings_.at(gi);
		return 90. - spectrometerAngle + g.angleCorrection() + r2d(atan(g.heightCorrection()/rPrime));
	}

	/// Returns the required height of the lift stage [mm] to reach a given \c spectrometerAngle [degrees]. Uses a quadratic expression fit using corresponding Solidworks measurements of the angle and lift height
	double liftHeight(double spectrometerAngle) const {
		return -0.021995798*spectrometerAngle*spectrometerAngle + 49.908015462*spectrometerAngle - 90.289534099;
	}
	/// Returns the spectrometerAngle [degrees] from the \c liftHeight [mm]. Inverse function of liftHeight().
	double spectrometerAngle(double liftHeight) const {
		return (-49.908015462+sqrt(49.908015462*49.908015462+4*0.021995798*(-90.289534099-liftHeight)))/2/-0.021995798;
	}

	/// Returns the position of the detector tilt stage, to get the right incidence angle when at \c beta [degrees].  Applies an optional tilt \c tiltOffset [degrees].
	double tiltStage(double beta, double tiltOffset) const {
		double incidenceAngle = 90. - beta + tiltOffset;

		// using empirical formula from survey data.
		//return 1.1928*incidenceAngle - 19.15 OLD FROM MODEL
		return 1.1926*incidenceAngle - 19.136; //FROM SURVEY DATA ONLY
	}
	/// Returns the tilt offset [degrees] from a given \c tiltStage position [mm], assuming at diffraction angle \c beta [degrees]. Inverse function of tiltStage().
	double tiltOffset(double tiltStage, double beta) const {
		// using empirical formula from survey data.
		//double incidenceAngle = (tiltStage + 19.15)/1.1928 OLD FROM MODEL
		double incidenceAngle = (tiltStage + 19.136)/1.1926;

		return incidenceAngle + beta - 90.;
	}



	// Computing spectrometer positions:
	//////////////////////////////////////

	AMControlInfoList computeSpectrometerPosition(int gratingIndex, double eV, double focusOffsetMm, double tiltOffsetDeg) const;

	double computeEVFromSpectrometerPosition(int gratingIndex, double liftHeight, double rPrime, int order = 1) const;


	// Modification functions:
	//////////////////////////////

	void setHexapodOrigin(const QVector3D& hexapodOrigin) { hexapodOrigin_ = hexapodOrigin; setModified(true); }

	// Database loading, storing:
	///////////////////////////////

	/// Access gratings for loading from database:
	AMDbObjectList dbReadGratings();
	/// Load gratings from database. Called when the number loaded from the db doesn't match the number we have.
	void dbLoadGratings(const AMDbObjectList& newGratings);



signals:

public slots:

protected:
	QVector3D hexapodOrigin_;
	double detectorWidth_;
	QList<REIXSXESGratingInfo> gratings_;


};

#endif // REIXSXESCALIBRATION2_H
