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


#ifndef CLSVARIABLEINTEGRATIONTIME_H
#define CLSVARIABLEINTEGRATIONTIME_H

#include <QObject>

#include "beamline/AMPVControl.h"
#include "actions3/AMAction3.h"

/*!
  The following is meant to be an abstraction to the Variable Integration time app used at the CLS.  Currently, only knowledge of how it works is based on
  VESPERS, however my understanding is that all of the apps were made by the same person (Ru) and therefore will likely be very similar, if not identical,
  to each other in naming conventions and other aspects.  To use the class, the idea will be to pass the base name and the constructor will build the
  necessary controls.  The functionality of the class will be similar to the new trend for AM classes to have getters, setters, and action getters.  Getters
  and setters for immediate actions on the beamline, and action getters that return an action that handles all the necessary setup and can be
  started at any time and behave according the Acquaman actions framework (useful for intialization or clean up procedures that need to wait for other actions
  to complete before starting).  One final thing, although most things use seconds as the default unit of time, the CLS app uses milliseconds.  Therefore,
  I have mapped all of the values from milliseconds to seconds for consistency within AM.
  */
class CLSVariableIntegrationTime : public QObject
{
	Q_OBJECT
public:
	/// Enum for the various modes of the app.
	enum Mode { Disabled = 0, EnabledwThreshold, Enabled };
	/// Enum for the various functions that can be used for the app.
	enum Function { Default = 0, Geometric, Exponential, Linear, Quadratic, SmoothStep, Logarithmic };

	/// Constructor.  Takes in the base name and parent and builds a variable integration time object.
 	virtual ~CLSVariableIntegrationTime();
	explicit CLSVariableIntegrationTime(const QString &baseName, QObject *parent = 0);

	/// Returns the current mode of the variable integration time app.
	Mode mode() const
	{
		Mode temp = Disabled;

		switch ((int)mode_->value()){

		case 0:
			temp = Disabled;
			break;
		case 1:
			temp = EnabledwThreshold;
			break;
		case 2:
			temp = Enabled;
			break;
		}

		return temp;
	}
	/// Returns the current default time (in seconds) of the variable integration app.
	double defautTime() const { return 1000*defaultTime_->value(); }
	/// Returns the threshold value (in k-space).
	double threshold() const { return threshold_->value(); }
	/// Returns the function being used to compute the various times.
	/*!
		0) Default
		1) Geometric
		2) Exponential
		3) Linear
		4) Quadratic
		5) Smooth Step
		6) Logarithmic
	  */
	Function function() const
	{
		Function temp = Default;

		switch ((int)function_->value()){

		case 0:
			temp = Default;
			break;
		case 1:
			temp = Geometric;
			break;
		case 2:
			temp = Exponential;
			break;
		case 3:
			temp = Linear;
			break;
		case 4:
			temp = Quadratic;
			break;
		case 5:
			temp = SmoothStep;
			break;
		case 6:
			temp = Logarithmic;
			break;
		}

		return temp;
	}
	/// Returns the low value of the region.  This should be set to the same as threshold for proper configuration.
	double lowValue() const { return lowVal_->value(); }
	/// Returns the high value of the region.  This should be set to the same as the end point of the region.
	double highValue() const { return highVal_->value(); }
	/// Returns the maximum time (in seconds) that should be used while computing the various dwell times.
	double maximumTime() const { return 1000*maxTime_->value(); }
	/// Returns the current value of a0.
	double a0() const { return a0_->value(); }
	/// Returns the current value of a1.
	double a1() const { return a1_->value(); }
	/// Returns the current value of a2.
	double a2() const { return a2_->value(); }

	/// Compute the time it will take to go through all of the points and return the value in seconds.  Uses the current values of the a0, a1, a2, and the current function.
	double totalTime(double delta) const;

	// Action getters.  Returns an action equivalent of a setter.
	/// Returns an action that changes the mode to \param mode.
	AMAction3 *createModeAction(Mode mode);
	/// Returns an action that changes the default time to \param time (in seconds).
	AMAction3 *createDefaultTimeAction(double time);
	/// Returns an action that changes the threshold to \param threshold.
	AMAction3 *createThresholdAction(double threshold);
	/// Returns an action that changes the function to \param function.
	AMAction3 *createFunctionAction(Function function);
	/// Returns an action that changes the low value to \param low.
	AMAction3 *createLowValueAction(double low);
	/// Returns an action that changes the high value to \param high.
	AMAction3 *createHighValueAction(double high);
	/// Returns an action that changes the maximum time to \param time (in seconds).
	AMAction3 *createMaximumTimeAction(double time);
	/// Returns an action that intiates a compute action.
	AMAction3 *createComputeAction();
	/// Returns an action that sets up the entire variable dwell time app.
	AMAction3 *createSetupAction(Mode mode, double defaultTime, double threshold, Function function, double low, double high, double maximumTime);

signals:
	/// Notifier that the mode has changed.  Passes the new mode.
	void modeChanged(Mode);
	/// Notifier that the default time changed.  Passes the new time.
	void defautTimeChanged(double);
	/// Notifier that the threshold has changed.  Passes the new threshold.
	void thresholdChanged(double);
	/// Notifier that the function used to compute the various times has changed.  Passes the index of the new function.
	void functionChanged(Function);
	/// Notifier that the low value of the region has changed.  Passes the new value.
	void lowValueChanged(double);
	/// Notifier that the high value of the region has changed.  Passes the new value.
	void highValueChanged(double);
	/// Notifier that the maximum time has changed.  Passes the new value.
	void maximumTimeChanged(double);
	/// Notifier that the value of a0 has changed.  Passes the new value.
	void a0Changed(double);
	/// Notifier that the value of a1 has changed.  Passes the new value.
	void a1Changed(double);
	/// Notifier that the value of a2 has changed.  Passes the new value.
	void a2Changed(double);

public slots:
	/// Sets the mode for the variable integration time app.
	void setMode(Mode mode) { mode_->move((int)mode); }
	/// Overload.  Sets the mode for the variable integration time app.
	void setMode(int mode) { mode_->move(mode); }
	/// Sets the default time (in seconds) used for the variable integration time app.
	void setDefaultTime(double time) { defaultTime_->move(time*1000); }
	/// Sets the threshold value (in k-space).
	void setThreshold(double value) { threshold_->move(value); }
	/// Sets the function that will be used for computing the various times.
	/*!
		0) Default
		1) Geometric
		2) Exponential
		3) Linear
		4) Quadratic
		5) Smooth Step
		6) Logarithmic
	  */
	void setFunction(Function function) { function_->move((int)function); }
	/// Overload.  Sets the function that will be used for computing the various times.
	void setFunction(int function) { function_->move(function); }
	/// Sets the low value of the region that is used for computing the proper time values.  This SHOULD be the same as the threshold for expected performance.
	void setLowValue(double low) { lowVal_->move(low); }
	/// Sets the high value of the region that is used for computing the proper time values.  This should be the same as the last value in the regions.
	void setHighValue(double high) { highVal_->move(high); }
	/// Sets the maximum time (in seconds) that should be spent on any particular point of a scan.
	void setMaximumTime(double maxTime) { maxTime_->move(maxTime*1000); }
	/// This tells the app to compute a new set of values for the dwell time.
	void compute() { compute_->move(1); compute_->move(0); }

protected slots:
	/// Helper slot to emit the default time changed to ensure that the value is in seconds.
	void onDefaultTimeChanged(double time) { emit defautTimeChanged(1000*time); }
	/// Helper slot to emit the maximum time changed to ensure that the value is in seconds.
	void onMaximumTimeChanged(double time) { emit maximumTimeChanged(1000*time); }
	/// Helper slot to emit the mode signal.
	void onModeChanged() { emit modeChanged(mode()); }
	/// Helper slot to emit the function signal.
	void onFunctionChagned() { emit functionChanged(function()); }

protected:
	/// Method that returns the total time using the default function.
	double totalTimeDefault(double delta, int n) const;
	/// Method that returns the total time using the geometric function.
	double totalTimeGeometric(double delta, int n) const;
	/// Method that returns the total time using the exponential function.
	double totalTimeExponential(double delta, int n) const;
	/// Method that returns the total time using the linear function.
	double totalTimeLinear(double delta, int n) const;
	/// Method that returns the total time using the quadratic function.
	double totalTimeQuadratic(double delta, int n) const;
	/// Method that returns the total time using the smooth step function.
	double totalTimeSmoothStep(double delta, int n) const;
	/// Method that returns the total time using the logaritmic function.
	double totalTimeLogarithmic(double delta, int n) const;

	/// Control that sets the mode of the integration time.
	AMControl *mode_;
	/// Control that sets the default time.
	AMControl *defaultTime_;
	/// Control that sets the threshold value before the app starts taking over.
	AMControl *threshold_;
	/// Control that sets the function used to compute the various integration times.
	AMControl *function_;
	/// Control that sets the low value of the region.  Needs to be the same as the threshold.
	AMControl *lowVal_;
	/// Control that sets the high value of the region.  Needs to be the same as the end point of the region.
	AMControl *highVal_;
	/// Control that sets the maximum amount of time to dwell when collecting data for a single point.
	AMControl *maxTime_;
	/// Control that computes the appropriate values for the various dwell times.  This should be used after every change to the parameters of this class, or at the end of a complete setup.
	AMControl *compute_;
	/// Control that holds the current value of a0.
	AMControl *a0_;
	/// Control that holds the current value of a1.
	AMControl *a1_;
	/// Control that holds the current value of a2.
	AMControl *a2_;
};

#endif // CLSVARIABLEINTEGRATIONTIME_H
