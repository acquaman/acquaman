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
class CLSScalerChannelDetector;
class AMBasicControlDetectorEmulator;
class CLSAmptekSDD123DetectorNew;
class SGMEnergyControlSet;
class SGMHexapod;
class SGMXPSLadder;
class SGMBypassLadder;
class SGMXASLadder;

class AMDSServerConfiguration;

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
	  * The end station step setpoint
	  */
	AMControl* endStationTranslationSetpoint() const;

	/*!
	  * The end station translation feedback
	  */
	AMControl* endStationTranslationFeedback() const;

	/*!
	  * The set of energy controls
	  */
	SGMEnergyControlSet* energyControlSet() const;

	/*!
	  * The exit slit gap control.
	  */
	AMControl* exitSlitGap() const;

	/*!
	  * The control set for the hexapod
	  */
	SGMHexapod* hexapod() const;

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
	AMMotorGroup* sampleManipulatorsMotorGroup() const;

	/*!
	  * The Amptek detector
	  */
	CLSAmptekSDD123DetectorNew* amptekSDD1() const;
	CLSAmptekSDD123DetectorNew* amptekSDD2() const;
	CLSAmptekSDD123DetectorNew* amptekSDD3() const;
	CLSAmptekSDD123DetectorNew* amptekSDD4() const;

	/*!
	  * The scaler.
	  */
	virtual CLSSIS3820Scaler* scaler() const;

	/*!
	 * The sample chamber pressure.
	 */
	AMReadOnlyPVControl* sampleChamberPressure() const;

	/*!
	 * The sample chamber turbo pump (turbo #5) running status.
	 */
	AMSinglePVControl* turboPump5Running() const;

	/*!
	 * The sample chamber turbo pump (turbo #6) running status.
	 */
	AMSinglePVControl* turboPump6Running() const;

	/*!
	 * The XPS ladder.
	 */
	virtual SGMXPSLadder* xpsLadder() const;

	/*!
	 * The bypass ladder.
	 */
	virtual SGMBypassLadder* bypassLadder() const;

	/*!
	 * The XAS ladder.
	 */
	virtual SGMXASLadder* xasLadder() const;

	/*!
	 * The VAT valve position.
	 */
	AMSinglePVControl* vatValvePosition() const;

	/*!
	 * The VAT valve speed.
	 */
	AMSinglePVControl* vatValveSpeed() const;

public slots:

signals:

protected slots:
	/*!
	  * Signal which indicates that the connection state of one of the core
	  * beamline components has been altered.
	  */
	void onConnectionStateChanged(bool);

	/// helper function to initialize the AcquamanDataServer
	/// ideally, this should be called in super class when Acquaman Data server is a generalized feature for all BLs using Acquaman
	void connectAMDSServers();

	void onAMDSServerConnected(const QString &hostIdentifier);

protected:

	/*!
	  * Initializes the Acquaman Data Server client app scontroller.
	  */
	void setupAMDSClientAppController();

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

protected:
	QMap<QString, AMDSServerConfiguration> AMDSServerDefs_;

	// New Energy Controls
	SGMEnergyControlSet* energyControlSet_;

	// Old Energy Controls

	AMControl *exitSlitGap_;
	AMControl *endStationTranslationSetpont_;
	AMControl *endStationTranslationFeedback_;

	AMMotorGroup *sampleManipulatorsMotorGroup_;

	SGMMAXvMotor *ssaManipulatorX_;
	SGMMAXvMotor *ssaManipulatorY_;
	SGMMAXvMotor *ssaManipulatorZ_;
	SGMMAXvMotor *ssaManipulatorRot_;

	SGMHexapod* hexapod_;

	CLSSIS3820Scaler *scaler_;

	CLSScalerChannelDetector *teyDetector_;
	CLSScalerChannelDetector *tfyDetector_;
	CLSScalerChannelDetector *i0Detector_;
	CLSScalerChannelDetector *pdDetector_;

	CLSScalerChannelDetector *filteredPD1Detector_;
	CLSScalerChannelDetector *filteredPD2Detector_;
	CLSScalerChannelDetector *filteredPD3Detector_;
	CLSScalerChannelDetector *filteredPD4Detector_;
//	CLSScalerChannelDetector *filteredPD5Detector_;

	CLSScalerChannelDetector *hexapodRedDetector_;
	CLSScalerChannelDetector *hexapodBlackDetector_;
	CLSScalerChannelDetector *encoderUpDetector_;
	CLSScalerChannelDetector *encoderDownDetector_;

	CLSAmptekSDD123DetectorNew* amptekSDD1_;
	CLSAmptekSDD123DetectorNew* amptekSDD2_;
	CLSAmptekSDD123DetectorNew* amptekSDD3_;
	CLSAmptekSDD123DetectorNew* amptekSDD4_;

	AMBasicControlDetectorEmulator *gratingEncoderDetector_;

	/// The sample chamber pressure control.
	AMReadOnlyPVControl *sampleChamberPressure_;
	/// The fifth beamline turbo pump running status, located at the sample chamber.
	AMSinglePVControl *turboPump5Running_;
	/// The sixth beamline turbo pump running status, located at the sample chamber.
	AMSinglePVControl *turboPump6Running_;
	/// The sample chamber VAT valve position.
	AMSinglePVControl *vatValvePosition_;
	/// The sample chamber VAT valve speed.
	AMSinglePVControl *vatValveSpeed_;

	/// The XPS diagnostic ladder control.
	SGMXPSLadder *xpsLadder_;
	/// The bypass diagnostic ladder control.
	SGMBypassLadder *bypassLadder_;
	/// The XAS diagnostic ladder control.
	SGMXASLadder *xasLadder_;

	bool cachedConnectedState_;
};


#endif // AM_SGMBEAMLINE_H
