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


#include "CLSVariableIntegrationTime.h"

#include "actions3/AMListAction3.h"
#include "actions3/actions/AMControlMoveAction3.h"

 CLSVariableIntegrationTime::~CLSVariableIntegrationTime(){}
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
	a0_ = new AMReadOnlyPVControl("a0", baseName + ":a0", this);
	a1_ = new AMReadOnlyPVControl("a1", baseName + ":a1", this);
	a2_ = new AMReadOnlyPVControl("a2", baseName + ":a2", this);

	connect(mode_, SIGNAL(valueChanged(double)), this, SLOT(onModeChanged()));
	connect(defaultTime_, SIGNAL(valueChanged(double)), this, SLOT(onDefaultTimeChanged(double)));
	connect(threshold_, SIGNAL(valueChanged(double)), this, SIGNAL(thresholdChanged(double)));
	connect(function_, SIGNAL(valueChanged(double)), this, SLOT(onFunctionChagned()));
	connect(lowVal_, SIGNAL(valueChanged(double)), this, SIGNAL(lowValueChanged(double)));
	connect(highVal_, SIGNAL(valueChanged(double)), this, SIGNAL(highValueChanged(double)));
	connect(maxTime_, SIGNAL(valueChanged(double)), this, SLOT(onMaximumTimeChanged(double)));
	connect(a0_, SIGNAL(valueChanged(double)), this, SIGNAL(a0Changed(double)));
	connect(a1_, SIGNAL(valueChanged(double)), this, SIGNAL(a1Changed(double)));
	connect(a2_, SIGNAL(valueChanged(double)), this, SIGNAL(a2Changed(double)));
}

AMAction3 *CLSVariableIntegrationTime::createModeAction(Mode mode)
{
	if (!mode_->isConnected())
		return 0;

	AMControlInfo setpoint = mode_->toInfo();
	setpoint.setValue(double(mode));
	AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
	AMAction3 *action = new AMControlMoveAction3(actionInfo, mode_);

	return action;
}

AMAction3 *CLSVariableIntegrationTime::createDefaultTimeAction(double time)
{
	if (!defaultTime_->isConnected())
		return 0;

	AMControlInfo setpoint = defaultTime_->toInfo();
	setpoint.setValue(time);
	AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
	AMAction3 *action = new AMControlMoveAction3(actionInfo, defaultTime_);

	return action;
}

AMAction3 *CLSVariableIntegrationTime::createThresholdAction(double threshold)
{
	if (!threshold_->isConnected())
		return 0;

	AMControlInfo setpoint = threshold_->toInfo();
	setpoint.setValue(threshold);
	AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
	AMAction3 *action = new AMControlMoveAction3(actionInfo, threshold_);

	return action;
}

AMAction3 *CLSVariableIntegrationTime::createFunctionAction(Function function)
{
	if (!function_->isConnected())
		return 0;

	AMControlInfo setpoint = function_->toInfo();
	setpoint.setValue(double(function));
	AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
	AMAction3 *action = new AMControlMoveAction3(actionInfo, function_);

	return action;
}

AMAction3 *CLSVariableIntegrationTime::createLowValueAction(double low)
{
	if (!lowVal_->isConnected())
		return 0;

	AMControlInfo setpoint = lowVal_->toInfo();
	setpoint.setValue(low);
	AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
	AMAction3 *action = new AMControlMoveAction3(actionInfo, lowVal_);

	return action;
}

AMAction3 *CLSVariableIntegrationTime::createHighValueAction(double high)
{
	if (!highVal_->isConnected())
		return 0;

	AMControlInfo setpoint = highVal_->toInfo();
	setpoint.setValue(high);
	AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
	AMAction3 *action = new AMControlMoveAction3(actionInfo, highVal_);

	return action;
}

AMAction3 *CLSVariableIntegrationTime::createMaximumTimeAction(double time)
{
	if (!maxTime_->isConnected())
		return 0;

	AMControlInfo setpoint = maxTime_->toInfo();
	setpoint.setValue(time);
	AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
	AMAction3 *action = new AMControlMoveAction3(actionInfo, maxTime_);

	return action;
}

AMAction3 *CLSVariableIntegrationTime::createComputeAction()
{
	if (!compute_->isConnected())
		return 0;

	AMControlInfo setpoint = compute_->toInfo();
	setpoint.setValue(1.0);
	AMControlMoveActionInfo3 *actionInfo = new AMControlMoveActionInfo3(setpoint);
	AMAction3 *action = new AMControlMoveAction3(actionInfo, compute_);

	return action;
}

AMAction3 *CLSVariableIntegrationTime::createSetupAction(Mode mode, double defaultTime, double threshold, Function function, double low, double high, double maximumTime)
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
	AMListAction3 *valueSetupAction = new AMListAction3(new AMListActionInfo3("Handles all the settings", "Handles setting all the setup values in a parallel list."), AMListAction3::Parallel);
	valueSetupAction->addSubAction(createModeAction(mode));
	valueSetupAction->addSubAction(createDefaultTimeAction(defaultTime));
	valueSetupAction->addSubAction(createThresholdAction(threshold));
	valueSetupAction->addSubAction(createFunctionAction(function));
	valueSetupAction->addSubAction(createLowValueAction(low));
	valueSetupAction->addSubAction(createHighValueAction(high));
	valueSetupAction->addSubAction(createMaximumTimeAction(maximumTime));

	AMListAction3 *setupAction = new AMListAction3(new AMListActionInfo3("Full variable integration time setup", "This action does all the setup for the variable integration time."), AMListAction3::Sequential);
	setupAction->addSubAction(valueSetupAction);
	setupAction->addSubAction(createComputeAction());

	return setupAction;
}

double CLSVariableIntegrationTime::totalTime(double delta) const
{
	int n = int(round((highValue() - lowValue())/delta));
	double time = 0;

	// The results of all of the functions will be in milliseconds and therefore need to be divided by 1000 to get into seconds.
	switch(function()){

	case Default:
		time = totalTimeDefault(delta, n)/1000;
		break;

	case Geometric:
		time = totalTimeGeometric(delta, n)/1000;
		break;

	case Exponential:
		time = totalTimeExponential(delta, n)/1000;
		break;

	case Linear:
		time = totalTimeLinear(delta, n)/1000;
		break;

	case Quadratic:
		time = totalTimeQuadratic(delta, n)/1000;
		break;

	case SmoothStep:
		time = totalTimeSmoothStep(delta, n)/1000;
		break;

	case Logarithmic:
		time = totalTimeLogarithmic(delta, n)/1000;
		break;
	}

	return time;
}

double CLSVariableIntegrationTime::totalTimeDefault(double delta, int n) const
{
	Q_UNUSED(delta)

	return a0()*n;
}

double CLSVariableIntegrationTime::totalTimeGeometric(double delta, int n) const
{
	double time = 0;
	double a = a0();
	double b = a1();
	double c = a2();
	double k0 = lowValue();

	for (int i = 0; i < n; i++)
		time += a + b*pow(k0 + i*delta, c);

	time += a + b*pow(highValue(), c);

	return time;
}

double CLSVariableIntegrationTime::totalTimeExponential(double delta, int n) const
{
	double time = 0;
	double a = a0();
	double b = a1();
	double c = a2();
	double k0 = lowValue();

	for (int i = 0; i < n; i++)
		time += a + b*exp(c*(k0 + i*delta));

	time += a + b*exp(c*(highValue()));

	return time;
}

double CLSVariableIntegrationTime::totalTimeLinear(double delta, int n) const
{
	double time = 0;
	double a = a0();
	double b = a1();
	double k0 = lowValue();

	for (int i = 0; i < n; i++)
		time += a + b*(k0 + i*delta);

	time += a + b*(highValue());

	return time;
}

double CLSVariableIntegrationTime::totalTimeQuadratic(double delta, int n) const
{
	double time = 0;
	double a = a0();
	double b = a1();
	double k0 = lowValue();

	for (int i = 0; i < n; i++)
		time += a + b*pow(k0 + i*delta, 2);

	time += a + b*pow(highValue(), 2);

	return time;
}

double CLSVariableIntegrationTime::totalTimeSmoothStep(double delta, int n) const
{
	double time = 0;
	double a = a0();
	double b = a1();
	double c = a2();
	double k0 = lowValue();

	for (int i = 0; i < n; i++)
		time += a + b/(1 + exp(-c*(k0 + i*delta)));

	time += a + b/(1 + exp(-c*(highValue())));

	return time;
}

double CLSVariableIntegrationTime::totalTimeLogarithmic(double delta, int n) const
{
	double time = 0;
	double a = a0();
	double b = a1();
	double c = a2();
	double k0 = lowValue();

	for (int i = 0; i < n; i++)
		time += a + b*log(fabs(k0 + i*delta + c));

	time += a + b*log(fabs(highValue() + c));

	return time;
}
