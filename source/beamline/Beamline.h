#ifndef BEAMLINE_H_
#define BEAMLINE_H_

#include "Settings.h"
#include "PVNames.h"

#include <QObject>
#include <QList>
#include "Control.h"
#include "AmpDetector.h"
#include "DiagnosticPaddle.h"
#include "InsertionDevice.h"
#include "LoadLock.h"
#include "Mono.h"
#include "SampleHolder.h"
#include "Spectrometer.h"
#include "VariableAperture.h"



/* [This is comment is really old. ignore...]
 * This class is a software-representation of the controllable aspects of the whole beamline.
 * It's a singleton class so only one instace should ever exist in the program.
 * The objects reachable through the beamline are:
 * 
 * Beamline
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


class Beamline : public Control {
	
	Q_OBJECT
	
public:
	static Beamline* bl();		// singleton-class accessor
	static void releaseBl();	// releases memory for Beamline
	
	virtual ~Beamline();
	
	// What does this Beamline have? (These objects will be useful in the scripting world too!)
	///////////////////////////////////
	
	ReadOnlyPVControl* ringCurrent() const			{ return ringCurrent_; }
	//	double ringCurrent() const								{ return ringCurrentPV_->lastValue(); }
	
	InsertionDevice* reixsID() const						{ return reixsID_; }
	InsertionDevice* smID() const							{ return smID_; }
	
	Control* variableAperture() const				{ return variableAperture_; }
	Mono* mono() const										{ return mono_; }
	Spectrometer* spectrometer() const						{ return spectrometer_; }
	
	AmpDetector* meshCurrent() const						{ return xasDetectors_[0]; }
	AmpDetector* electronYield() const						{ return xasDetectors_[1]; }
	AmpDetector* fluorescenceYield() const					{ return xasDetectors_[2]; }
	
	const QList<AmpDetector*>& xasDetectorsAvailable() const{ return xasDetectors_; }
	
	Control* sampleHolder() const						{ return sampleHolder_; }
	Control* loadLock() const								{ return loadLock_; }
	
	Control* diagnosticPaddle(int num) const		{ return diagnosticPaddles_[num % diagnosticPaddles_.size()]; }
	
	
	
protected:
	// Singleton implementation:
	Beamline();					// protected constructor... only access through Beamline::bl()
	static Beamline* instance_;
	
	// Parts of this beamline:
	///////////////////////////////
	
	ReadOnlyPVControl* ringCurrent_;
	
	InsertionDevice* reixsID_, *smID_;
	
	Control* variableAperture_;
	Mono* mono_;
	Spectrometer* spectrometer_;
	
	QList<AmpDetector*> xasDetectors_;
	
	Control* sampleHolder_;
	Control* loadLock_;
	
	QList<Control*> diagnosticPaddles_;
	
	
};

#endif /*BEAMLINE_H_*/
