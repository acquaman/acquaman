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

#ifndef PGMBEAMLINE_H
#define PGMBEAMLINE_H

#include "beamline/AMControlSet.h"
#include "beamline/AMMotorGroup.h"
#include "beamline/PGM/PGMPicoAmmeter.h"

#include "util/AMErrorMonitor.h"

#include "beamline/CLS/CLSBeamline.h"
#include "beamline/CLS/CLSSynchronizedDwellTime.h"

class AMBasicControlDetectorEmulator;

/// This class is the master class that holds EVERY control inside the PGM beamline.
class PGMBeamline : public CLSBeamline
{
	Q_OBJECT

public:
	/// Returns the instance of the beamline that has been created.
	static PGMBeamline *pgm()
	{
		if(!instance_){
			instance_ = new PGMBeamline();
			instance_->initializeBeamlineSupport();
		}

		return static_cast<PGMBeamline*>(instance_);
	}

	/// Destructor.
	virtual ~PGMBeamline();

	/// Returns energy control for PGM
	AMPVwStatusControl* energy() const { return energy_; }

	/// Returns the read only control for Exit slit lower blade current - branch A
	PGMPicoAmmeter *exitSlitLowerBladeCurrentA() const { return exitSlitLowerBladeCurrentADetector_; }
	/// Returns the read only control for Exit slit upper blade current - branch A
	PGMPicoAmmeter *exitSlitUpperBladeCurrentA() const { return exitSlitUpperBladeCurrentADetector_; }
	/// Returns the read only control for Exit slit lower blade current - branch B
	PGMPicoAmmeter *exitSlitLowerBladeCurrentB() const { return exitSlitLowerBladeCurrentBDetector_; }
	/// Returns the read only control for Exit slit upper blade current - branch B
	PGMPicoAmmeter *exitSlitUpperBladeCurrentB() const { return exitSlitUpperBladeCurrentBDetector_; }

	/// Returns the read only control for Entrance slit lower blade current
	PGMPicoAmmeter *entranceSlitLowerBladeCurrent() const { return entranceSlitLowerBladeCurrentDetector_; }
	/// Returns the read only control for Entrance slit upper blade current
	PGMPicoAmmeter *entranceSlitUpperBladeCurrent() const { return entranceSlitUpperBladeCurrentDetector_; }

	/// Returns the read only control for TEY
	PGMPicoAmmeter *teyBladeCurrentControl() const { return teyBladeCurrentDetector_; }
	/// Returns the read only control for FLY
	PGMPicoAmmeter *flyBladeCurrentControl() const { return flyBladeCurrentDetector_; }
	/// Returns the read only control for endstation Ni I0 current
	PGMPicoAmmeter *i0EndstationBladeCurrentControl() const { return i0EndstationBladeCurrentDetector_; }
	/// Returns the read only control for beamline Ni I0 current
	PGMPicoAmmeter *i0BeamlineBladeCurrentControl() const { return i0BeamlineBladeCurrentDetector_; }
	/// Returns the read only control for photodiode current
	PGMPicoAmmeter *photodiodeBladeCurrentControl() const { return photodiodeBladeCurrentDetector_; }

	/// Returns the CLS Synchronized dwell time.
	AMSynchronizedDwellTime *synchronizedDwellTime() const { return synchronizedDwellTime_; }

signals:

public slots:

protected:
	/// Sets up the readings such as pressure, flow switches, temperature, etc.
	void setupDiagnostics();
	/// Sets up logical groupings of controls into sets.
	void setupControlSets();
	/// Sets up all the detectors.
	void setupDetectors();
	/// Sets up the sample stage motors.
	void setupSampleStage();
	/// Sets up mono settings.
	void setupMono();
	/// Sets up various beamline components.
	void setupComponents();
	/// Sets up the exposed actions.
	void setupExposedControls();
	/// Sets up the exposed detectors.
	void setupExposedDetectors();
	/// Sets up the motor group for the various sample stages.
	void setupMotorGroup();
	/// Sets up all of the detectors that need to be added to scans that aren't a part of typical detectors.  This may just be temporary, not sure.
	void setupControlsAsDetectors();

	/// Constructor. This is a singleton class, access it through IDEASBeamline::ideas().
	PGMBeamline();

	/// Energy control for PGM
	AMPVwStatusControl *energy_;

	// Detectors
	PGMPicoAmmeter *exitSlitLowerBladeCurrentADetector_;
	PGMPicoAmmeter *exitSlitUpperBladeCurrentADetector_;
	PGMPicoAmmeter *exitSlitLowerBladeCurrentBDetector_;
	PGMPicoAmmeter *exitSlitUpperBladeCurrentBDetector_;

	PGMPicoAmmeter *entranceSlitLowerBladeCurrentDetector_;
	PGMPicoAmmeter *entranceSlitUpperBladeCurrentDetector_;

	PGMPicoAmmeter *teyBladeCurrentDetector_;
	PGMPicoAmmeter *flyBladeCurrentDetector_;
	PGMPicoAmmeter *i0EndstationBladeCurrentDetector_;
	PGMPicoAmmeter *i0BeamlineBladeCurrentDetector_;
	PGMPicoAmmeter *photodiodeBladeCurrentDetector_;

	// Synchronized dwell time.
	CLSSynchronizedDwellTime *synchronizedDwellTime_;
};

#endif // PGMBEAMLINE_H
