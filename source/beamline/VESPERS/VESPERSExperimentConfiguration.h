#ifndef VESPERSEXPERIMENTCONFIGURATION_H
#define VESPERSEXPERIMENTCONFIGURATION_H

#include <QObject>

#include "beamline/CLS/CLSSynchronizedDwellTime.h"
#include "beamline/AMBeamlineActionItem.h"

/*!
  The following is the model for the experiment configuration for VESPERS.  It handles enabling some of the key components that users need to worry about when setting up
  an experiment.  Like many other classes, the ability to set an experiment configuration can be done directly or using actions.

  For VESPERS, the synchronized dwell time is required and therefore is passed into the constructor.  The other controls are created internally in this class.
  */
class VESPERSExperimentConfiguration : public QObject
{
	Q_OBJECT
public:
	/// Constructor.  Builds the configuration from the synchronized dwell time and some internal controls.
	explicit VESPERSExperimentConfiguration(CLSSynchronizedDwellTime *dwellTime, QObject *parent = 0);

signals:
	/// Notifier that the beamline is ready for to take experiments.
	void experimentReady(bool);

public slots:

protected slots:
	/// Determines whether the state of the experiment ready status.
	void determineExperimentStatus();
	/// Enables/Disables the POE status from the experiment ready status.
	void usePOEStatus(bool use)
	{
		usePOE_ = use;

		if (poeBeamStatusEnable_->isConnected())
			poeBeamStatusEnable_->move(use == true ? 0.0 : 1.0);

		determineExperimentStatus();
	}
	/// Enables/Disables the SOE status from the experiment ready status.
	void useSOEStatus(bool use)
	{
		useSOE_ = use;

		if (soeBeamStatusEnable_->isConnected())
			soeBeamStatusEnable_->move(use == true ? 0.0 : 1.0);

		determineExperimentStatus();
	}
	/// Enables/Disables the fast shutter from experiment ready status.
	void useFastShutterStatus(bool use)
	{
		useFastShutter_ = use;
		determineExperimentStatus();
	}
	/// Enables/Disables the CCD from the experiment ready status.
	void useCCDStatus(bool use)
	{
		useCCD_ = use;
		determineExperimentStatus();

		if (synchronizedDwellTime()->isConnected())
			synchronizedDwellTime_->elementAt(2)->setEnabled(use);
	}
	/// Enables/Disables the sample stage from the experiment ready status.
	void useSampleStageStatus(bool use)
	{
		useSampleStage_ = use;
		determineExperimentStatus();
	}
	/// Enables/Disables the single element vortex detector from the experiment ready status.
	void useSingleElementVortex(bool use)
	{
		useSingleEl_ = use;
		determineExperimentStatus();

		if (synchronizedDwellTime()->isConnected())
			synchronizedDwellTime_->elementAt(1)->setEnabled(use);
	}
	/// Enables/Disables the four element vortex detector from the experiment ready status.
	void useFourElementVortex(bool use)
	{
		useFourEl_ = use;
		determineExperimentStatus();

		if (synchronizedDwellTime()->isConnected())
			synchronizedDwellTime_->elementAt(4)->setEnabled(use);
	}

protected:
	/// The pointer to the dwell time.
	CLSSynchronizedDwellTime *dwellTime_;

	// Experiment Ready controls.
	/// The POE beam status control.
	AMControl *poeBeamStatus_;
	/// The POE beam status enable.
	AMControl *poeBeamStatusEnable_;
	/// The SOE beam status control.
	AMControl *soeBeamStatus_;
	/// The SOE beam status enable.
	AMControl *soeBeamStatusEnable_;
	/// The fast shutter control.
	AMControl *fastShutterReady_;
	/// The CCD status control.
	AMControl *ccdStatus_;

	/// Flag to keep track of whether or not the POE is enabled.
	bool usePOE_;
	/// Flag to keep track of whether or not the SOE is enabled.
	bool useSOE_;
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
