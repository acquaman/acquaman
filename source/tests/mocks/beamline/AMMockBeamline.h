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

#include "beamline/CLS/CLSBeamline.h"

#ifndef AMMOCKBEAMLINE_H
#define AMMOCKBEAMLINE_H

class AMBasicControlDetectorEmulator;
class AMMockControl;
class AMMockDetector;
/*!
  * A class representing the mock beamline used by the AMMock application.
  */
class AMMockBeamline : public CLSBeamline
{
	Q_OBJECT

public:
	/// Returns the instance of the beamline that has been created.
	static AMMockBeamline* mockbl()
	{
		if(!instance_){
			instance_ = new AMMockBeamline();
			instance_->initializeBeamlineSupport();
		}

		return static_cast<AMMockBeamline*>(instance_);
	}

	/// Frees the resources associated with the mock beamline.
	virtual ~AMMockBeamline();

	/// Returns true. The simulated beamline is always considered connected.
	virtual bool isConnected() const;

signals:

public slots:

protected:
	/// Constructor. This is a singleton class, access it through
	/// AMMockBeamline::mockbl().
	AMMockBeamline();

	/// Sets up various beamline components.
	void setupComponents();
	/// Sets up all the mock detectors.
	void setupDetectors();
	/// Sets up logical groupings of controls into sets.
	void setupControlSets();
	/// Sets up the exposed mock controls.
	void setupExposedControls();
	/// Sets up the exposed mock detectors.
	void setupExposedDetectors();
	/// Sets up the SIGNAL and SLOT connections.
	void setupConnections();

protected slots:

protected:

	AMMockControl* mockControl1_;
	AMMockControl* mockControl2_;
	AMMockDetector* mockImmediateDetector1_;
	AMMockDetector* mockDwellDetector1_;
};

#endif // AMMOCKBEAMLINE_H
