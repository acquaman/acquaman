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

#ifndef IDEASBEAMLINE_H
#define IDEASBEAMLINE_H

#include "beamline/AMBeamline.h"
#include "beamline/AMControlSet.h"
#include "beamline/CLS/CLSSynchronizedDwellTime.h"
#include "beamline/CLS/CLSSIS3820Scaler.h"
#include "beamline/AMMotorGroup.h"
#include "beamline/CLS/CLSPseudoMotorGroup.h"

#include "util/AMErrorMonitor.h"
#include "util/AMBiHash.h"

#include "beamline/CLS/CLSBasicScalerChannelDetector.h"
#include "beamline/CLS/CLSBasicCompositeScalerChannelDetector.h"

/// This class is the master class that holds EVERY control inside the VESPERS beamline.
class IDEASBeamline : public AMBeamline
{
	Q_OBJECT

public:

	/// Returns the instance of the beamline that has been created.
	static IDEASBeamline* ideas() {
		if(!instance_)
			instance_ = new IDEASBeamline();
		return static_cast<IDEASBeamline*>(instance_);
	}

	/// Destructor.
	virtual ~IDEASBeamline();

signals:

public slots:

protected slots:

protected:
	/// Sets up the synchronized dwell time.
	void setupSynchronizedDwellTime();
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

	/// Constructor. This is a singleton class; access it through IDEASBeamline::ideas().
	IDEASBeamline();


	/// Control for the mono
	AMPVwStatusControl *monoEnergy_;

	/// The synchronized dwell time app for IDEAS
	CLSSynchronizedDwellTime *synchronizedDwellTime_;

	AMReadOnlyPVControl *oldIonChamberAmmeter_;
	AMReadOnlyPVControl *oxfordI0IonChamberAmmeter_;
	AMReadOnlyPVControl *oxfordSampleIonChamberAmmeter_;
	AMReadOnlyPVControl *oxfordReferenceIonChamberAmmeter_;

	AMDetector *oldIonChamberDetector_;
	AMDetector *oxfordI0IonChamberDetector_;
	AMDetector *oxfordSampleIonChamberDetector_;
	AMDetector *oxfordReferenceIonChamberDetector_;
};

#endif // IDEASSBEAMLINE_H
