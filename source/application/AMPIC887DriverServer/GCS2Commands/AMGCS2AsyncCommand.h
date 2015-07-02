#ifndef AMGCS2ASYNCCOMMAND_H
#define AMGCS2ASYNCCOMMAND_H

#define TIMER_CHECK_INTERVAL_MS 50

#include <QObject>
#include <QTime>
#include "AMGCS2Command.h"
/*!
  * Addition to the GCS2Command which allows for an observer to be tied to the
  * completed and failed signals which indicate the command has been fully completed
  * (i.e. not just started correctly).
  */
class AMGCS2AsyncCommand : public QObject, public AMGCS2Command
{
    Q_OBJECT
public:

	enum RunningState {
		NotStarted,
		Running,
		Succeeded,
		Failed
	};

	/*!
	  * Creates an instance of an asynchronous command which will emit the completed
	  * or failed signals when the command comes to a stop. The timer will automatically
	  * emit the failed signal and cease its checking mechanism when timeoutMs milliseconds
	  * have elapsed.
	  */
	explicit AMGCS2AsyncCommand(double timeoutMs = 20);

	/*!
	  * Overrides the synchronous GCS2Command implementation of run. Begins the
	  * mechanism for checking if the command has done.
	  */
	virtual void run();
signals:
	/*!
	  * Signal indicating that this command has been successfully started.
	  */
	void started(AMGCS2AsyncCommand* command);

	/*!
	  * Signal indicating that this command has successfully finished
	  */
	void succeeded(AMGCS2AsyncCommand* command);

	/*!
	  * Signal indicating that this command has finished in a failed state.
	  */
	void failed(AMGCS2AsyncCommand* command);

public slots:

protected:

	/*!
	  * Runs the implementation specific isFinishedImplemetation function ,emits
	  * the relevant signal indicated by the implementations return value (if required)
	  * and ceases the mechanism for checking if the command has done.
	  */
	void timerEvent(QTimerEvent *);

	/*!
	  * Class specific implementation which checks for the current status of a
	  * running command. Sets the current RunningState of the command if a change
	  * has been detected, and sets the last error message if an error has been
	  * detected.
	  */
	virtual void isFinishedImplementation() = 0;

	QTime startTime_;
	int timerId_;
	double timeoutMs_;
	RunningState runningState_;
};

#endif // AMGCS2ASYNCCOMMAND_H
