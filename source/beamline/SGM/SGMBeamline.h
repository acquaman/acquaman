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


#ifndef AM_SGMBEAMLINE_H
#define AM_SGMBEAMLINE_H

#include "beamline/CLS/CLSBeamline.h"

class SGMMAXvMotor;
class AMMotorGroup;
class CLSAdvancedScalerChannelDetector;
/*!
  * A singleton class which represents the SGM beamline. The beamline class can
  * be accessed through the SGMBeamline::sgm() function.
  */
class SGMBeamline : public CLSBeamline
{
Q_OBJECT
public:

	/*!
	  * Singletone class accessor. Provides access to the SGMBeamline class
	  * instance.
	  */
	static SGMBeamline* sgm();

	/*!
	  * Virtual destructor for the SGMBeamline class.
	  */
	virtual ~SGMBeamline();

	/*!
	  * Whether the beamline is currently connected.
	  */
	virtual bool isConnected() const;

	/*!
	  * The energy control.
	  */
	AMControl* energy() const;

	/*!
	  * The exit slit gap control.
	  */
	AMControl* exitSlitGap() const;

	/*!
	  * The grating control.
	  */
	AMControl* grating() const;

	/*!
	  * The motor which controls the sample stage X position.
	  */
	SGMMAXvMotor* ssaManipulatorX() const;

	/*!
	  * The motor which controls the sample stage Y position.
	  */
	SGMMAXvMotor* ssaManipulatorY() const;

	/*!
	  * The motor which controls the sample stage Z position.
	  */
	SGMMAXvMotor* ssaManipulatorZ() const;

	/*!
	  * The motor which controls the sample stage rotational position.
	  */
	SGMMAXvMotor* ssaManipulatorRot() const;

	/*!
	  * The sample stage motor group.
	  */
	AMMotorGroup* motorGroup() const;

	/*!
	  * The scaler.
	  */
	virtual CLSSIS3820Scaler* scaler() const;
public slots:

signals:

protected slots:
	/*!
	  * Signal which indicates that the connection state of one of the core
	  * beamline components has been altered.
	  */
	void onConnectionStateChanged(bool);
protected:

	/*!
	  * Initializes the beamline controls (energy, exit slit etc.).
	  */
	void setupBeamlineComponents();

	/*!
	  * Initializes the beamline motor groups (sample maniuplator).
	  */
	void setupMotorGroups();

	/*!
	  * Initializes the beamline detectors (tfy, tey, i0 etc.).
	  */
	void setupDetectors();

	/*!
	  * Adds the required controls to the list of exposed controls.
	  */
	void setupExposedControls();

	/*!
	  * Adds the required detectors to the list of exposed detectors.
	  */
	void setupExposedDetectors();


	/*!
	  * Protected constructor for the SGMBeamline to ensure singleton structure.
	  */
	SGMBeamline();

	AMControl *energy_;
	AMControl *exitSlitGap_;
	AMControl *grating_;

	AMMotorGroup *motorGroup_;

	SGMMAXvMotor *ssaManipulatorX_;
	SGMMAXvMotor *ssaManipulatorY_;
	SGMMAXvMotor *ssaManipulatorZ_;
	SGMMAXvMotor *ssaManipulatorRot_;

	CLSSIS3820Scaler *scaler_;

	CLSAdvancedScalerChannelDetector *teyDetector_;
	CLSAdvancedScalerChannelDetector *tfyDetector_;
	CLSAdvancedScalerChannelDetector *i0Detector_;
	CLSAdvancedScalerChannelDetector *pdDetector_;
	CLSAdvancedScalerChannelDetector *filteredPD1Detector_;
	CLSAdvancedScalerChannelDetector *filteredPD2Detector_;
	CLSAdvancedScalerChannelDetector *filteredPD3Detector_;
	CLSAdvancedScalerChannelDetector *filteredPD4Detector_;
	CLSAdvancedScalerChannelDetector *filteredPD5Detector_;

	bool cachedConnectedState_;

};


#endif // AM_SGMBEAMLINE_H
