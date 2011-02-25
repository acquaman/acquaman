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

#ifndef REIXSBEAMLINE_H
#define REIXSBEAMLINE_H

#include "beamline/AMBeamline.h"
#include "beamline/AMControlSet.h"
#include "acquaman/REIXS/REIXSXESMCPDetector.h"	///< \todo Move this to beamline, not acquaman.


/// The REIXSHexapod control is just a container for the set of coupled controls which make up the hexapod:
/*!
- x(), y(), z(): Stage position in mm
- u(), v(), w(): Stage angle, in deg
- r(), s(), t(): Rotation point relative to stage origin, in mm
*/

class REIXSHexapod : public AMControl {
	Q_OBJECT
public:
	REIXSHexapod(QObject* parent = 0);

	AMControl* x() { return x_; }
	AMControl* y() { return y_; }
	AMControl* z() { return z_; }
	AMControl* u() { return u_; }
	AMControl* v() { return v_; }
	AMControl* w() { return w_; }
	AMControl* r() { return r_; }
	AMControl* s() { return s_; }
	AMControl* t() { return t_; }


protected:
	/// Controls, connected to the hexapod PVs
	AMControl *x_, *y_, *z_, *u_, *v_, *w_, *r_, *s_, *t_;
};

/// The REIXSSpectrometer control is a container for the set of (low-level, physical) controls which make up the spectrometer:
/*!
- angleDrive(): The position of the ball screw that lifts the spectrometer, in mm, up from the home (lowered) position.
- detectorTranslation(): The position of the ball screw that translates the detector along the chamber, in mm, away from the upstream home position
- detectorTiltDrive():  The position of the linear stage that tilts the detector, in mm, up from its vertically lowest home position
- detectorRotationDrive(): The position of the linear stage which rotates the detector, in mm, away from the most upstream home position
- hexapod(): The REIXSHexapod controls
*/
class REIXSSpectrometer : public AMControl {
	Q_OBJECT
public:
	REIXSSpectrometer(QObject* parent = 0);

	AMControl* angleDrive() { return angleDrive_; }
	AMControl* detectorTranslation() { return detectorTranslation_; }
	AMControl* detectorTiltDrive() { return detectorTiltDrive_; }
	AMControl* detectorRotationDrive() { return detectorRotationDrive_; }
	REIXSHexapod* hexapod() { return hexapod_; }

protected:
	AMControl *angleDrive_, *detectorTranslation_, *detectorTiltDrive_, *detectorRotationDrive_;
	REIXSHexapod* hexapod_;
};

/// The REIXSSampleChamber control is a container for the motor controls that make up the sample manipulator and load lock.
class REIXSSampleChamber : public AMControl {
	Q_OBJECT
public:
	REIXSSampleChamber(QObject* parent = 0);

	AMControl* x() { return x_; }
	AMControl* y() { return y_; }
	AMControl* z() { return z_; }
	AMControl* r() { return r_; }

	AMControl* loadLockZ() { return loadLockZ_; }
	AMControl* loadLockR() { return loadLockR_; }

protected:
	AMControl* x_, *y_, *z_, *r_, *loadLockZ_, *loadLockR_;
};

/// This class
class REIXSBeamline : public AMBeamline
{
	Q_OBJECT
public:
	static REIXSBeamline* bl() {
		if(!instance_)
			instance_ = new REIXSBeamline();
		return static_cast<REIXSBeamline*>(instance_);
	}

	// Accessing control elements:

	/// Access the spectromter controls:
	REIXSSpectrometer* spectrometer() { return spectrometer_; }
	/// Access the sample chamber and load-lock controls:
	REIXSSampleChamber* sampleChamber() { return sampleChamber_; }
	/// Access the live MCP detector object
	REIXSXESMCPDetector* mcpDetector() { return mcpDetector_; }

	// These Control Sets are logical groups of controls, that are commonly used by different Acquaman components

	/// The sampleX, sampleY, sampleZ, and sampleR (ie: theta) controls for moving the sample plate, used by the Sample Manipulator
	AMControlSet* sampleManipulatorSet() { return sampleManipulatorSet_; }
	/// All the controls for positioning the Spectrometer
	AMControlSet* spectrometerPositionSet() { return spectrometerPositionSet_; }

signals:

public slots:

protected:
	/// Constructor. This is a singleton class; access it through REIXSBeamline::bl().
	REIXSBeamline();

	/// \todo: beamline front-end controls
	// AMControl* incidentEV_;
	// AMControl* monoGrating_;

	/// A hierarchichal group of controls making up the spectrometer
	REIXSSpectrometer* spectrometer_;
	/// A hierarchichal group of controls making up the sample chamber
	REIXSSampleChamber* sampleChamber_;
	/// An object for controlling the MCP detector and downloading its image values
	REIXSXESMCPDetector* mcpDetector_;


	// These Control Sets are logical groups of controls, that are commonly used by different Acquaman components
	/// The sampleX, sampleY, sampleZ, and sampleR (ie: theta) controls for moving the sample plate, used by the Sample Manipulator
	AMControlSet* sampleManipulatorSet_;
	/// All the controls for positioning the Spectrometer (angleDrive, detectorTranslation, detectorTiltDrive, detectorRotationDrive, hexapod{X, Y, Z, U, V, W, R, S, T}
	AMControlSet* spectrometerPositionSet_;


};

#endif // REIXSBEAMLINE_H






///* [This is comment is really old. ignore...]
// * This class is a software-representation of the controllable aspects of the whole beamline.
// * It's a singleton class so only one instace should ever exist in the program.
// * The objects reachable through the beamline are:
// *
// * AMBeamline
// *
// * 		property: ringCurrent
// *
// *
// * 		InsertionDevice reixsID
// * 			Motor gapMotor			// maybe Tony's eV PV in Mono will take care of this...
// * 			Shutters ??
// * 			property: eV
// * 			property: polarizationType
// * 			property: polarizationAngle
// *
// * 		?InsertionDevice smID?
// *
// * 		VariableAperture variableAperture
// *
// * 	/////////////////
// * 		Physically:
// * 			M1		// doesn't move
// * 			Photon shutter
// * 			4-jaw #1		-- maybe control?
// * /////////////////////
// *
// * 		Mono
// * 			Motor eV (if Tony makes a single process var. to do this)
// * 			Motor: slitWidthMotor
// * 			property: grating
// * 			property: mirror
// *
// * ///////////////
// * 	Physically:
// * 		4-jaw #2			-- maybe control
// * 		chopper
// * 		M3	// need to move M3 out of the way to get light (bool PV)
// * 		M4	//
// *
// * 		Diagnostic panels (4): mesh, yag crystal, and cam.
// * 			- option: beam clear, mesh in place, crystal in place, etc.
// *
// *
// * 		AmpDetector meshCurrent (I0)
// * 			- todo...
// * 		AmpDetector electronYield (TEY)
// * 		AmpDetector fluorescenceYield (TFY)	// triggering, sampling, dwell times, etc: steal.
// * 		AmpDetector photodiode
// * 		TODO: figure out PFY - lots of options...
// * 		property: xasDetectorsAvailable()
// * 		property:
// *
// * 		Spectrometer
// * 			Hexapod hexapod
// * 				Motor xMotor
// * 				Motor yMotor
// * 				Motor zMotor
// * 				Motor uMotor
// * 				Motor vMotor
// * 				Motor wMotor
// * 				set: rotationPoint(P, Q, R)
// * 			Motor rMotor
// * 			Motor slitWidthMotor		// PV... I hope - needs calibrating. (Piezos - use PLC 0-10output and calibrate ourselves)
// * 			Motor dMotor
// * 			Motor detectorAngleMotor
// * 			Motor detectorOrientationMotor
// * 			2DCountDetector mcp
// * 				- getImage
// * 				- clearImage
// * 			property: eV
// * 			property: grating
// * 			property: detectorOrientationMode
// *
// * 		SampleHolder (make motors private and implement position-checking security?)
// * 			Motor xMotor
// * 			Motor yMotor
// * 			Motor zMotor
// * 			Motor rMotor
// * 			Motor sMotor
// * 			property: position(x, y, z, r, s)
// * 			TempController sampleHeater (homemade coil -- high temp)	Heatwave labs controller
// * 			TempController crystatController	(cryostat cooling)		Lakeshore controller
// *
// * 			Future: SD-65 (lakeshore) thermal silicon diode
// *
// *
// * 		LoadLock
// * 			Motor zMotor
// * 			Motor rMotor
// *
// * 		// staff setup panel?
// *		Motor endstationPosition
// * 		(or just... inBeam/outOfBeam)
// *
// *
// * Questions - BL8 experience:
// * 		// Do we (user) ever need to scan to optimize the gap?
// * 		// Do we (user) ever need to mess with mirrors/mirror feedback?
// *
// *
// * TODO: Channeltron (CEM): voltage on/off (no user adjust.)
// *
// *
// *
// *
// * */



//#endif /*BEAMLINE_H_*/
