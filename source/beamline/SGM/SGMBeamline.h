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
class AM1DControlDetectorEmulator;
class CLSAmptekSDD123DetectorNew;
class SGMEnergyControlSet;
class SGMHexapod;
class SGMXPSLadder;
class SGMBypassLadder;
class SGMXASLadder;
class CLSAMDSScaler;
class CLSAMDSScalerChannelDetector;
class SGMSampleChamber;
class CLSQE65000Detector;

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
	 * The control for the beam status.
	 */
	AMControl* beamStatusControl() const;

	/*!
	  * The control to commence the beam on operation
	  */
	AMControl* beamOnOperationControl() const;

	/*!
	  * The control to commence the beam off operation
	  */
	AMControl* beamOffOperationControl() const;

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

	/// Hexapod control set for scanning.
	AMControlSet *hexapodControlSet() const;

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

	virtual CLSAMDSScaler* amdsScaler() const;

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

//	AMPVControl* endStationLinearStage() const;
	AMPVwStatusControl* endStationLinearStage() const;

	/*!
	 *  The sample chamber.
	 */
	SGMSampleChamber* sampleChamber() const;

	/*!
	  * The QE65000 detector
	  */
	AMDetector* qe6500Detector() const;

	/*!
	  * Configures the beamline components which require an AMDS.
	  * \param hostIdentifier ~ The ip address and port of the AMDS which controls
	  * are to be configured for.
	  */
	void configAMDSServer(const QString& hostIdentifier);

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

protected:

	AMControl* beamStatusControl_;
	AMControl* beamOnOperationControl_;
	AMControl* beamOffOperationControl_;
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
	AMControlSet *hexapodControlSet_;

	CLSAMDSScaler *amdsScaler_;

	CLSAMDSScalerChannelDetector *teyDetector_;
	CLSAMDSScalerChannelDetector *constantFrequencyDetector_;
	CLSAMDSScalerChannelDetector *i0Detector_;
	CLSAMDSScalerChannelDetector *pdDetector_;

	CLSAMDSScalerChannelDetector *amptekROI1Detector_;
	CLSAMDSScalerChannelDetector *amptekROI2Detector_;
	CLSAMDSScalerChannelDetector *amptekROI3Detector_;
	CLSAMDSScalerChannelDetector *amptekROI4Detector_;

	CLSAMDSScalerChannelDetector *hexapodRedDetector_;
	CLSAMDSScalerChannelDetector *hexapodBlackDetector_;
	CLSAMDSScalerChannelDetector *encoderUpDetector_;
	CLSAMDSScalerChannelDetector *encoderDownDetector_;

	CLSAmptekSDD123DetectorNew* amptekSDD1_;
	CLSAmptekSDD123DetectorNew* amptekSDD2_;
	CLSAmptekSDD123DetectorNew* amptekSDD3_;
	CLSAmptekSDD123DetectorNew* amptekSDD4_;

	AMBasicControlDetectorEmulator *gratingEncoderDetector_;
	AM1DControlDetectorEmulator *hexapodXRecoderDetector_;
	AM1DControlDetectorEmulator *hexapodYRecoderDetector_;
	AM1DControlDetectorEmulator *hexapodZRecoderDetector_;
	AM1DControlDetectorEmulator *hexapodTimeRecoderDetector_;
	CLSQE65000Detector *qe65000Detector_;

	/// The XPS diagnostic ladder control.
	SGMXPSLadder *xpsLadder_;
	/// The bypass diagnostic ladder control.
	SGMBypassLadder *bypassLadder_;
	/// The XAS diagnostic ladder control.
	SGMXASLadder *xasLadder_;

	AMPVwStatusControl* endStationLinearStage_;
	/// The sample chamber.
	SGMSampleChamber *sampleChamber_;

	bool cachedConnectedState_;
};


#endif // AM_SGMBEAMLINE_H
