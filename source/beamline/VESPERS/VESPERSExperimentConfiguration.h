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

#ifndef VESPERSEXPERIMENTCONFIGURATION_H
#define VESPERSEXPERIMENTCONFIGURATION_H

#include <QObject>

#include "beamline/CLS/CLSSynchronizedDwellTime.h"
#include "beamline/VESPERS/VESPERSSampleStageControl.h"
#include "beamline/VESPERS/XRFDetector.h"
#include "actions/AMBeamlineActionItem.h"

/*!
  The following is the model for the experiment configuration for VESPERS.  It handles enabling some of the key components that users need to worry about when setting up
  an experiment.  Like many other classes, the ability to set an experiment configuration can be done directly or using actions.

  For VESPERS, the synchronized dwell time, the sample stage, and both vortex detectors are required and therefore passed into the constructor.  The other controls are created internally in this class.
  */
class VESPERSExperimentConfiguration : public QObject
{
	Q_OBJECT
public:
	/// Enum for the different types of experiments.
	enum ExperimentType { Custom = 0, XAS, XASw1el, XASw4el, XRFw1el, XRFw1elAndXRD, XRFw4el, XRFw4elAndXRD };

	/// Constructor.  Builds the configuration from the synchronized dwell time and some internal controls.
	explicit VESPERSExperimentConfiguration(CLSSynchronizedDwellTime *dwellTime, VESPERSSampleStageControl *pseudoSampleStage, XRFDetector *vortex1E, XRFDetector *vortex4E, QObject *parent = 0);

	/// Returns the experiment type.
	ExperimentType type() const { return type_; }

signals:
	/// Notifier that the beamline is ready for to take experiments.
	void experimentReady(bool);
	/// Notifier that the experiment type has been changed.
	void experimentTypeChanged(VESPERSExperimentConfiguration::ExperimentType);
	/// Notifier that the beam has gone down.  When the POE status enable goes from true to false this notifies the beamline that the beam has dumped.
	void beamDumped();
	/// Notifier that the POE status has changed.  Passes the new state.
	void POEStatusChanged(bool);
	/// Notifier that the fast shutter status has changed.  Passes the new state.
	void fastShutterStatusChanged(bool);
	/// Notifier that the CCD status has changed.  Passes the new state.
	void CCDStatusChanged(bool);
	/// Notifier that the sample stage status has changed.  Passes the new state.
	void sampleStageStatusChanged(bool);
	/// Notifier that the single element vortex status has changed.  Passes the new state.
	void singleElementVortexStatusChanged(bool);
	/// Notifier that the four element vortex status has changed.  Passes the new state.
	void fourElementVortexStatusChanged(bool);

public slots:
	/// Sets the experiment type to \param type.
	void setType(VESPERSExperimentConfiguration::ExperimentType type);
	/// Overload.  Sets the type of the experiment to \param type.
	void setType(int type) { setType((ExperimentType)type); }

	/// Enables/Disables the POE status from the experiment ready status.
	void usePOEStatus(bool use);
	/// Enables/Disables the fast shutter from experiment ready status.
	void useFastShutterStatus(bool use);
	/// Enables/Disables the CCD from the experiment ready status.
	void useCCDStatus(bool use);
	/// Enables/Disables the sample stage from the experiment ready status.
	void useSampleStageStatus(bool use);
	/// Enables/Disables the single element vortex detector from the experiment ready status.
	void useSingleElementVortex(bool use);
	/// Enables/Disables the four element vortex detector from the experiment ready status.
	void useFourElementVortex(bool use);

protected slots:
	/// Determines whether the state of the experiment ready status.
	void determineExperimentStatus();
	/// Handles changes from the beamline to the POE status.
	void onPOEEnableChanged(double val) { usePOEStatus((int)val == 0 ? true : false); }
	/// Handles the synchronized dwell time configuration at startup.
	void onSynchronizedDwellTimeStartup(bool connected);
	/// Determines whether the beam has dumped or not.
	void onPOEStatusChanged(double state);

protected:
	/// The pointer to the dwell time.
	CLSSynchronizedDwellTime *synchronizedDwellTime_;
	/// The pointer to the sample stage.
	VESPERSSampleStageControl *pseudoSampleStage_;
	/// The pointer to the single element vortex.
	XRFDetector *vortex1E_;
	/// The pointer to the four element vortex.
	XRFDetector *vortex4E_;

	// Experiment Ready controls.
	/// The POE beam status control.
	AMControl *poeBeamStatus_;
	/// The POE beam status enable.
	AMControl *poeBeamStatusEnable_;
	/// The fast shutter control.
	AMControl *fastShutterReady_;
	/// The CCD status control.
	AMControl *ccdStatus_;

	/// The type of the experiment.  Logical OR combination of the different choices.
	ExperimentType type_;

	/// Flag to keep track of whether or not the POE is enabled.
	bool usePOE_;
	/// Flag to keep track of whether or not the fast shutter is enabled.
	bool useFastShutter_;
	/// Flag to keep track of whether or not the CCD is enabled.
	bool useCCD_;
	/// Flag to keep track of whether or not the sample stage is enabled.
	bool useSampleStage_;
	/// Flag to keep track of whether or not the 1-element vortex is enabled.
	bool useSingleEl_;
	/// Flag to keep track of whether or not the 4-element vortex is enabled.
	bool useFourEl_;
};

#endif // VESPERSEXPERIMENTCONFIGURATION_H
