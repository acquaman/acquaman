#ifndef AMTIMER_H
#define AMTIMER_H

#include <QTimer>
#include <QTime>
/*!
  * A class which provides a wrapper of a QTimer, adding the ability to pause/
  * unpause, with setPaused(bool) and to find out how long time timer has been
  * running with elapsedTime().
  */
class AMTimer : public QObject
{
    Q_OBJECT
public:
	/*!
	  * Creates an instance of an AMTimer with the default interval of 0 msecs
	  * and the provided QObject parent.
	  * \param parent ( default = 0 ) ~ The parent QObject of the created AMTimer
	  */
    explicit AMTimer(QObject *parent = 0);
	/*!
	  * Destroys the AMTimer
	  */
	virtual ~AMTimer() {}

	/*!
	  * The timeout interval in milliseconds. The default value for the interval
	  * is 0. An AMTimer with a timeout interval of 0 will time out as soon as
	  * all the events in the window system's event queue have been processed.
	  * Setting the interval of an active timer changes its timerId().
	  */
	int interval() const;

	/*!
	  * Whether the timer is active, ie running or paused
	  * \returns True if the timer is running or paused, false otherwise.
	  */
	bool isActive() const;

	/*!
	  * Whether the timer is currently running.
	  * NOTE: Paused timers are not considered to be running. If you wish to find
	  * out if a timer is running or paused, see isActive();
	  */
	bool isRunning() const;

	/*!
	  * Whether the timer is a single-shot timer. A single-shot timer fires only
	  * once, non-single-shot timers fire every interval milliseconds.
	  */
	bool isSingleShot() const;

	/*!
	  * Whether or not the timer is currently active, but paused.
	  * \returns True if the timer is active and paused, false otherwise.
	  */
	bool isPaused() const;

	/*!
	  * The number of milliseconds that the timer has been active for (ie. active
	  * or paused). Restarting the timer sets this value to 0.
	  */
	int timeSinceStarted() const;

	/*!
	  * The current number of milliseconds this interval of the timer has been
	  * running for (ie. active and not paused). Restarting the timer sets this
	  * value to 0.
	  */
	int timeSinceTimeout() const;

	/*!
	  * The number of milliseconds that the timer has been running for (ie.
	  * active and not paused). Restarting the timer sets this value to 0.
	  */
	int timeRunning() const;

	/*!
	  * The current number of milliseconds that the timer has spent paused. Restarting
	  * the timer sets this value to 0.
	  */
	int timePaused() const;

	/*!
	  * Sets the timeout interval in milliseconds.
	  * \param msec ~ The new timeout interval in milliseconds.
	  */
	void setInterval(int msec);

	/*!
	  * Sets whether the timer is a single-shot timer. A single-shot timer fires
	  * only once, non-single-shot timers fire every interval milliseconds.
	  * \param singleShot ~ Whether the timer should become single-shot or
	  * non-single-shot.
	  */
	void setSingleShot(bool singleShot);

signals:
	/*!
	  * Signal indicating that the timer has timed out.
	  */
	void timeout();

	/*!
	  * Signal indicating that the timer has transitioned from one pause state to
	  * another (ie. from paused to unpaused or from unpaused to paused).
	  */
	void pauseStateChanged(bool pauseState);
public slots:
	/*!
	  * Starts or restarts the timer with a timeout interval of msec milliseconds.
	  * If the timer is already running, it will be stopped and restarted.
	  */
	void start(int msec);

	/*!
	  * This function overloads start(). Starts or restarts the timer with the
	  * timeout specified in interval. If the timer is already running, it will
	  * be stopped and restarted.
	  */
	void start();

	/*!
	  * Stops the timer.
	  */
	void stop();

	/*!
	  * Pauses the timer.
	  */
	void pause();
	/*!
	  * resume the timer.
	  */
	void resume();

protected slots:
	/// slot to handle the timeout event of the QTimer
	void onTimerTimeout();

protected:
	// Instance lifetime member variables
	QTimer timer_;
	int timerInterval_;
	bool isPaused_;

	// Run lifetime member variables (ie reset on stop)
	QTime startTime_;
	int timeWhilePaused_;
	QTime timeLastPaused_;
	QTime timeLastResumed_;

	// Interval lifetime member variables (ie reset on timeout)
	QTime intervalStartTime_;
	int intervalRemaining_;
	bool resetTimerIntervalNextTimeout_;
};

#endif // AMTIMER_H
