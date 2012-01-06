#ifndef CLSVARIABLEINTEGRATIONTIME_H
#define CLSVARIABLEINTEGRATIONTIME_H

#include <QObject>

#include "beamline/AMControl.h"
#include "actions/AMBeamlineActionItem.h"

/*!
  The following is meant to be an abstraction to the Variable Integration time app used at the CLS.  Currently, only knowledge of how it works is based on
  VESPERS, however my understanding is that all of the apps were made by the same person (Ru) and therefore will likely be very similar, if not identical,
  to each other in naming conventions and other aspects.  To use the class, the idea will be to pass the base name and the constructor will build the
  necessary controls.  The functionality of the class will be similar to the new trend for AM classes to have getters, setters, and action getters.  Getters
  and setters for immediate actions on the beamline, and action getters that return an AMBeamlineActionItem that handles all the necessary setup and can be
  started at any time and behave according the Acquaman actions framework (useful for intialization or clean up procedures that need to wait for other actions
  to complete before starting).
  */
class CLSVariableIntegrationTime : public QObject
{
	Q_OBJECT
public:
	/// Constructor.  Takes in the base name and parent and builds a variable integration time object.
	explicit CLSVariableIntegrationTime(const QString &baseName, QObject *parent = 0);

	/// Returns the current mode of the variable integration time app.  0 is disabled, 1 is enabled w/ threshold, 2 is enabled.
	int mode() const { return (int)mode_->value(); }
	/// Returns the current default time of the variable integration app.
	double defautTime() const { return defaultTime_->value(); }
	/// Returns the threshold value (in k-space).
	double threshold() const { return threshold_->value(); }
	/// Returns the index of which function is being used to compute the various times.
	/*!
		0) Default
		1) Geometric
		2) Exponential
		3) Linear
		4) Quadratic
		5) Smooth Step
		6) Logarithmic
	  */
	int function() const { return (int)function_->value(); }
	/// Returns the low value of the region.  This should be set to the same as threshold for proper configuration.
	double lowValue() const { return lowVal_->value(); }
	/// Returns the high value of the region.  This should be set to the same as the end point of the region.
	double highValue() const { return highVal_->value(); }
	/// Returns the maximum time that should be used while computing the various dwell times.
	double maximumTime() const { return maxTime_->value(); }

signals:
	/// Notifier that the mode has changed.  Passes the new mode.
	void modeChanged(int);
	/// Notifier that the default time changed.  Passes the new time.
	void defautTimeChanged(double);
	/// Notifier that the threshold has changed.  Passes the new threshold.
	void thresholdChanged(double);
	/// Notifier that the function used to compute the various times has changed.  Passes the index of the new function.
	void functionChanged(int);
	/// Notifier that the low value of the region has changed.  Passes the new value.
	void lowValueChanged(double);
	/// Notifier that the high value of the region has changed.  Passes the new value.
	void highValueChanged(double);
	/// Notifier that the maximum time has changed.  Passes the new value.
	void maximumTimeChanged(double);

public slots:
	/// Sets the mode for the variable integration time app.
	void setMode(int mode) { mode_->move(mode); }
	/// Sets the default time used for the variable integration time app.
	void setDefaultTime(double time) { defaultTime_->move(time); }
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
	void setFunction(int function) { function_->move(function); }
	/// Sets the low value of the region that is used for computing the proper time values.  This SHOULD be the same as the threshold for expected performance.
	void setLowValue(double low) { lowVal_->move(low); }
	/// Sets the high value of the region that is used for computing the proper time values.  This should be the same as the last value in the regions.
	void setHighValue(double high) { highVal_->move(high); }
	/// Sets the maximum time that should be spent on any particular point of a scan.
	void setMaximumTime(double maxTime) { maxTime_->move(maxTime); }
	/// This tells the app to compute a new set of values for the dwell time.
	void compute() { compute_->move(1); }

protected slots:
	/// Helper slot to emit the mode signal.
	void onModeChanged(double mode) { emit modeChanged((int)mode); }
	/// Helper slot to emit the function signal.
	void onFunctionChagned(double function) { emit functionChanged((int)function); }

protected:
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
};

#endif // CLSVARIABLEINTEGRATIONTIME_H
