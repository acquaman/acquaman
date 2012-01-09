#include "CLSVariableIntegrationTime.h"
#include "actions/AMBeamlineControlMoveAction.h"
#include "actions/AMBeamlineParallelActionsList.h"
#include "actions/AMBeamlineListAction.h"

CLSVariableIntegrationTime::CLSVariableIntegrationTime(const QString &baseName, QObject *parent)
	: QObject(parent)
{
	mode_ = new AMSinglePVControl("Mode", baseName + ":enable", this, 0.1);
	defaultTime_ = new AMSinglePVControl("Default Time", baseName + ":default", this, 0.1);
	threshold_ = new AMSinglePVControl("Threshold", baseName + ":trigger:thresh", this, 0.1);
	function_ = new AMSinglePVControl("Function", baseName + ":method", this, 0.1);
	lowVal_ = new AMSinglePVControl("Low Value", baseName + ":xmin", this, 0.1);
	highVal_ = new AMSinglePVControl("High Value", baseName + ":xmax", this, 0.1);
	maxTime_ = new AMSinglePVControl("Maximum Time", baseName + ":ymax", this, 0.1);
	compute_ = new AMSinglePVControl("Compute", baseName + ":aN:calc:run", this, 0.1);

	connect(mode_, SIGNAL(valueChanged(double)), this, SLOT(onModeChanged()));
	connect(defaultTime_, SIGNAL(valueChanged(double)), this, SLOT(onDefaultTimeChanged(double)));
	connect(threshold_, SIGNAL(valueChanged(double)), this, SIGNAL(thresholdChanged(double)));
	connect(function_, SIGNAL(valueChanged(double)), this, SLOT(onFunctionChagned()));
	connect(lowVal_, SIGNAL(valueChanged(double)), this, SIGNAL(lowValueChanged(double)));
	connect(highVal_, SIGNAL(valueChanged(double)), this, SIGNAL(highValueChanged(double)));
	connect(maxTime_, SIGNAL(valueChanged(double)), this, SLOT(onMaximumTimeChanged(double)));
}

AMBeamlineActionItem *CLSVariableIntegrationTime::createModeAction(Mode mode)
{
	if (!mode_->isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(mode_);
	action->setSetpoint(mode);

	return action;
}

AMBeamlineActionItem *CLSVariableIntegrationTime::createDefaultTimeAction(double time)
{
	if (!defaultTime_->isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(defaultTime_);
	action->setSetpoint(time*1000);

	return action;
}

AMBeamlineActionItem *CLSVariableIntegrationTime::createThresholdAction(double threshold)
{
	if (!threshold_->isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(threshold_);
	action->setSetpoint(threshold);

	return action;
}

AMBeamlineActionItem *CLSVariableIntegrationTime::createFunctionAction(Function function)
{
	if (!function_->isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(function_);
	action->setSetpoint(function);

	return action;
}

AMBeamlineActionItem *CLSVariableIntegrationTime::createLowValueAction(double low)
{
	if (!lowVal_->isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(lowVal_);
	action->setSetpoint(low);

	return action;
}

AMBeamlineActionItem *CLSVariableIntegrationTime::createHighValueAction(double high)
{
	if (!highVal_->isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(highVal_);
	action->setSetpoint(high);

	return action;
}

AMBeamlineActionItem *CLSVariableIntegrationTime::createMaximumTimeAction(double time)
{
	if (!maxTime_->isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(maxTime_);
	action->setSetpoint(time*1000);

	return action;
}

AMBeamlineActionItem *CLSVariableIntegrationTime::createComputeAction()
{
	if (!compute_->isConnected())
		return 0;

	AMBeamlineControlMoveAction *action = new AMBeamlineControlMoveAction(compute_);
	action->setSetpoint(1);

	return action;
}

AMBeamlineActionItem *CLSVariableIntegrationTime::createSetupAction(Mode mode, double defaultTime, double threshold, Function function, double low, double high, double maximumTime)
{
	if (!(mode_->isConnected()
		  && defaultTime_->isConnected()
		  && threshold_->isConnected()
		  && function_->isConnected()
		  && lowVal_->isConnected()
		  && highVal_->isConnected()
		  && maxTime_->isConnected()
		  && compute_->isConnected()))
		return 0;

	// This is a two stage setup.  The first stage is setting all of the appropriate values to all of the fields.  Once those are all done, then the app needs to compute the new values.
	AMBeamlineParallelActionsList *setupActionsList = new AMBeamlineParallelActionsList;
	AMBeamlineListAction *setupAction = new AMBeamlineListAction(setupActionsList);

	setupActionsList->appendStage(new QList<AMBeamlineActionItem*>());
	setupActionsList->appendAction(0, createModeAction(mode));
	setupActionsList->appendAction(0, createDefaultTimeAction(defaultTime));
	setupActionsList->appendAction(0, createThresholdAction(threshold));
	setupActionsList->appendAction(0, createFunctionAction(function));
	setupActionsList->appendAction(0, createLowValueAction(low));
	setupActionsList->appendAction(0, createHighValueAction(high));
	setupActionsList->appendAction(0, createMaximumTimeAction(maximumTime));

	setupActionsList->appendStage(new QList<AMBeamlineActionItem *>());
	setupActionsList->appendAction(1, createComputeAction());

	return setupAction;
}
