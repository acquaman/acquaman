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


#ifndef AMDEFERREDFUNCTIONCALL_H
#define AMDEFERREDFUNCTIONCALL_H

#include <QObject>
#include <QTimer>
#include <QEvent>
#include <QCoreApplication>

/// This class provides a convenient way to schedule something to run after all currently-running functions return... ie: when flow control returns to the Qt event loop. It also provides the handy optimization of combining several repeated function calls into one call.
/*! (For example, if you need to do an expensive computation whenever any one of a set of values is changed, and the user might be setting multiple values in one function call, it might be beneficial to wait for all the values to change, and then do your computation once.)

There are two ways to use this class.  You can inherit it, and re-implement execute() to do your dirty business. Alternatively, you can simply connect the executed() signal to your desired slots.

- Call schedule() one or more times to schedule the function call(s) to execute.  Multiple calls to schedule() within one run loop will only result in one execution.

OR

- Call runLater() with a maximum delay in ms.  The function will be executed at the end of the delay.  Multiple calls to runLater() within the delay period will only result in one execution.


When using schedule(): Both execute() and the executed() signal will be triggered on every run loop, if you've called schedule() one or more times in the previous run loop.*/
class AMDeferredFunctionCall : public QObject {
	Q_OBJECT

public:
 	virtual ~AMDeferredFunctionCall();
	AMDeferredFunctionCall(QObject* parent = 0);

	/// Returns true if the function call has been scheduled and will execute on the next run loop cycle.
	bool isScheduled() const { return scheduled_; }
	/// Returns true if the function call has been scheduled to run after a delay
	bool willRunLater() const { return delayTimer_.isActive(); }

public slots:
	/// Schedule the function call(s) for execution.
	void schedule();
	/// Cancel any (possible) previous calls to schedule().  The function won't be executed until you call schedule() again.
	void unschedule() { scheduled_ = false; }

	/// Schedule the function call(s) for execution after \c delayMs milliseconds.  If runLater() has already been called but not yet finished executing, this will do nothing.
	void runLater(int delayMs);
	/// Cancel any (possible) previous calls to runLater().  The function won't be executed until you call runlater() again.
	void cancelRunLater() { delayTimer_.stop(); }


	/// Run the function call(s). The base class version does nothing. You can re-implement this to do work required in a custom class (or alternatively, connect the executed() signal to your desired slots.)
	virtual void execute() {
	}

signals:
	/// Emitted when control returns to the Qt event loop and schedule() has been called at least once in the past run loop, or when runLater() has been called and the delay has expired.
	void executed();

protected slots:
	/// executes the deferred function following a delay
	void onDelayTimerTimeout();

protected:
	bool scheduled_;
	static QEvent::Type eventType_;
	/// This timer is used for deferred function calls with a delay
	QTimer delayTimer_;

	/// events are posted to run the deferred function as soon as control goes back to the event loop
	bool event(QEvent* e);

};

#endif // AMDEFERREDFUNCTIONCALL_H
