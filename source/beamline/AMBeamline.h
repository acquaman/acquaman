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


#ifndef ACQMAN_BEAMLINE_H_
#define ACQMAN_BEAMLINE_H_

#include "util/AMSettings.h"
#include "AMPVNames.h"

#include <QObject>
#include <QList>
#include "AMControl.h"
#include "AMAmpDetector.h"
#include "AMDiagnosticPaddle.h"
#include "AMInsertionDevice.h"
#include "AMLoadLock.h"
#include "AMMono.h"
#include "AMSampleHolder.h"
#include "AMSpectrometer.h"
#include "AMVariableAperture.h"

/*! THIS CLASS IS DEPRECATED... DO NOT USE OR WORRY ABOUT */

/* [This is comment is really old. ignore...]
 * This class is a software-representation of the controllable aspects of the whole beamline.
 * It's a singleton class so only one instace should ever exist in the program.
 * The objects reachable through the beamline are:
 *
 * AMBeamline
 *
 * 		property: ringCurrent
 *
 *
 * 		InsertionDevice reixsID
 * 			Motor gapMotor			// maybe Tony's eV PV in Mono will take care of this...
 * 			Shutters ??
 * 			property: eV
 * 			property: polarizationType
 * 			property: polarizationAngle
 *
 * 		?InsertionDevice smID?
 *
 * 		VariableAperture variableAperture
 *
 * 	/////////////////
 * 		Physically:
 * 			M1		// doesn't move
 * 			Photon shutter
 * 			4-jaw #1		-- maybe control?
 * /////////////////////
 *
 * 		Mono
 * 			Motor eV (if Tony makes a single process var. to do this)
 * 			Motor: slitWidthMotor
 * 			property: grating
 * 			property: mirror
 *
 * ///////////////
 * 	Physically:
 * 		4-jaw #2			-- maybe control
 * 		chopper
 * 		M3	// need to move M3 out of the way to get light (bool PV)
 * 		M4	//
 *
 * 		Diagnostic panels (4): mesh, yag crystal, and cam.
 * 			- option: beam clear, mesh in place, crystal in place, etc.
 *
 *
 * 		AmpDetector meshCurrent (I0)
 * 			- todo...
 * 		AmpDetector electronYield (TEY)
 * 		AmpDetector fluorescenceYield (TFY)	// triggering, sampling, dwell times, etc: steal.
 * 		AmpDetector photodiode
 * 		TODO: figure out PFY - lots of options...
 * 		property: xasDetectorsAvailable()
 * 		property:
 *
 * 		Spectrometer
 * 			Hexapod hexapod
 * 				Motor xMotor
 * 				Motor yMotor
 * 				Motor zMotor
 * 				Motor uMotor
 * 				Motor vMotor
 * 				Motor wMotor
 * 				set: rotationPoint(P, Q, R)
 * 			Motor rMotor
 * 			Motor slitWidthMotor		// PV... I hope - needs calibrating. (Piezos - use PLC 0-10output and calibrate ourselves)
 * 			Motor dMotor
 * 			Motor detectorAngleMotor
 * 			Motor detectorOrientationMotor
 * 			2DCountDetector mcp
 * 				- getImage
 * 				- clearImage
 * 			property: eV
 * 			property: grating
 * 			property: detectorOrientationMode
 *
 * 		SampleHolder (make motors private and implement position-checking security?)
 * 			Motor xMotor
 * 			Motor yMotor
 * 			Motor zMotor
 * 			Motor rMotor
 * 			Motor sMotor
 * 			property: position(x, y, z, r, s)
 * 			TempController sampleHeater (homemade coil -- high temp)	Heatwave labs controller
 * 			TempController crystatController	(cryostat cooling)		Lakeshore controller
 *
 * 			Future: SD-65 (lakeshore) thermal silicon diode
 *
 *
 * 		LoadLock
 * 			Motor zMotor
 * 			Motor rMotor
 *
 * 		// staff setup panel?
 *		Motor endstationPosition
 * 		(or just... inBeam/outOfBeam)
 *
 *
 * Questions - BL8 experience:
 * 		// Do we (user) ever need to scan to optimize the gap?
 * 		// Do we (user) ever need to mess with mirrors/mirror feedback?
 *
 *
 * TODO: Channeltron (CEM): voltage on/off (no user adjust.)
 *
 *
 *
 *
 * */


class AMBeamline : public AMControl {

	Q_OBJECT

public:
		static AMBeamline* bl();		// singleton-class accessor
		static void releaseBl();	// releases memory for AMBeamline

		virtual ~AMBeamline();

		// What does this AMBeamline have? (These objects will be useful in the scripting world too!)
	///////////////////////////////////

		AMReadOnlyPVControl* ringCurrent() const			{ return ringCurrent_; }
	//	double ringCurrent() const								{ return ringCurrentPV_->lastValue(); }

		AMInsertionDevice* reixsID() const						{ return reixsID_; }
		AMInsertionDevice* smID() const							{ return smID_; }

		AMControl* variableAperture() const				{ return variableAperture_; }
		AMMono* mono() const										{ return mono_; }
		AMSpectrometer* spectrometer() const						{ return spectrometer_; }

		AMAmpDetector* meshCurrent() const						{ return xasDetectors_[0]; }
		AMAmpDetector* electronYield() const						{ return xasDetectors_[1]; }
		AMAmpDetector* fluorescenceYield() const					{ return xasDetectors_[2]; }

		const QList<AMAmpDetector*>& xasDetectorsAvailable() const{ return xasDetectors_; }

		AMControl* sampleHolder() const						{ return sampleHolder_; }
		AMControl* loadLock() const								{ return loadLock_; }

		AMControl* diagnosticPaddle(int num) const		{ return diagnosticPaddles_[num % diagnosticPaddles_.size()]; }



protected:
	// Singleton implementation:
		AMBeamline();					// protected constructor... only access through AMBeamline::bl()
		static AMBeamline* instance_;

	// Parts of this beamline:
	///////////////////////////////

		AMReadOnlyPVControl* ringCurrent_;

		AMInsertionDevice* reixsID_, *smID_;

		AMControl* variableAperture_;
		AMMono* mono_;
		AMSpectrometer* spectrometer_;

		QList<AMAmpDetector*> xasDetectors_;

		AMControl* sampleHolder_;
		AMControl* loadLock_;

		QList<AMControl*> diagnosticPaddles_;


};

#endif /*BEAMLINE_H_*/
